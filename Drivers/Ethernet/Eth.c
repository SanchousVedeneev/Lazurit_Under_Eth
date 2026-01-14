
#include "Eth.h"

#include "spi.h"
#include "wizchip_conf.h"
#include "socket.h"

#include "string.h"




#ifdef ETH_USE_DHCP
#include "dhcp.h"
#endif

//-----------------------------------  FUNC
static void cs_sel();
static void cs_desel();
static uint8_t spi_rb(void);
static void spi_wb(uint8_t b);
static void spi_rb_burst(uint8_t *buf, uint16_t len);
static void spi_wb_burst(uint8_t *buf, uint16_t len);
void eth_dhcp_ip_assign();
void eth_dhcp_ip_conflict();


//-----------------------------------  FUNC END


wiz_NetInfo netInfo = {
    .ip = ETH_IP,
    .sn = ETH_MASK,
    .gw = ETH_GTW
};

#ifdef ETH_USE_DHCP
#define ETH_DHCP_TASK_MS (100)
#define ETH_DHCP_TIME_MS (1000)
uint8_t dhcpBuf[1024];
#endif

#ifdef ETH_USE_UDP_FINDER
uint8_t udpBufRx[ETH_UDP_BUF_RX_SIZE];
uint8_t udpBufTx[ETH_UDP_BUF_TX_SIZE];
#endif

// Init
uint8_t Eth_f_Init()
{
    wiz_NetInfo netInfoCheck;
    
    reg_wizchip_cs_cbfunc(cs_sel, cs_desel);
    reg_wizchip_spi_cbfunc(spi_rb, spi_wb);
    reg_wizchip_spiburst_cbfunc(spi_rb_burst, spi_wb_burst);

    uint8_t bufSize[] = {2, 2, 2, 2, 2, 2, 2, 2};
    wizchip_init(bufSize, bufSize);

    wizchip_setinterruptmask(IK_SOCK_ALL);

    Eth_f_GetMacFromUID(netInfo.mac);

    setSHAR(netInfo.mac);
    getSHAR(netInfoCheck.mac);

    // check if MAC is set
    for (uint8_t i = 0; i < 6; i++)
    {
        if (*(netInfo.mac + i) != *(netInfoCheck.mac + i))
            return RESET; //
    }

    setRTR(2000); // 200 мсек
    setRCR(3);  // 3 повтора

#ifdef ETH_USE_DHCP
    reg_dhcp_cbfunc(
        eth_dhcp_ip_assign,
        eth_dhcp_ip_assign,
        eth_dhcp_ip_conflict);
    DHCP_init(ETH_SOC_DHCP, dhcpBuf);
#else
    setGAR(netInfo.gw);
    setSUBR(netInfo.sn);
    setSIPR(netInfo.ip);
#endif

    //int8_t errCode = SOCK_OK;

#ifdef ETH_USE_UDP_FINDER
    errCode = socket(ETH_SOC_UDP, Sn_MR_UDP, ETH_UDP_PORT, SF_IO_NONBLOCK);
    if (errCode != SOCK_OK)
    {
        return RESET;
    }
#endif

    // // SF_TCP_NODELAY
    // errCode = socket(ETH_SOC_TCP, Sn_MR_TCP, ETH_TCP_PORT, SF_IO_NONBLOCK);
    // if (errCode != SOCK_BUSY)
    // {
    //     return RESET;
    // }
    // errCode = listen(ETH_SOC_TCP);
    // if(errCode != SOCK_OK){
    //    return RESET; 
    // }

#ifdef ETH_USE_UDP_FINDER
    udpBufTx[0] = (uint8_t)ETH_UDP_MSG_SEND_ID;
    udpBufTx[1] = (uint8_t)(ETH_UDP_MSG_SEND_ID >> 8);
    memcpy(udpBufTx + 2, (uint8_t *)UID_BASE, 12);
    asm("NOP");
    uint8_t destIp[4] = ETH_IP_BROADCAST;
    if (sendto(ETH_SOC_UDP, udpBufTx, 14, destIp, ETH_UDP_PORT) < SOCK_OK)
    {
        Error_Handler();
    }
#endif

    return SET;
}


__STATIC_INLINE int32_t Eth_f_recv(Eth_t_SocTcp *socketObj)
{
    __IO int32_t ret = 0;
    __IO uint16_t size = 0;
    // get the size of recv data in recv buffer
    if ((size = getSn_RX_RSR(socketObj->hwSocNum)) == 0) return 0;
    
    // recv data
    ret = recv(socketObj->hwSocNum, socketObj->bufRx, socketObj->bufRxLen);
    if ((ret <= 0) || (ret != size))
    {
        // printf("%d:recv fail\r\n", sn);
        if ((ret = disconnect(socketObj->hwSocNum)) != SOCK_OK)
        {
            // printf("%d:disconnect fail\r\n", sn);
            asm("NOP");
            return ret;
        }
        asm("NOP");
        return 0;
    }

    return ret;
}

__STATIC_INLINE int32_t Eth_f_send(Eth_t_SocTcp *socketObj/*, uint16_t size*/)
{
    __IO int32_t ret = 0;
    if(socketObj->txFrameSize > socketObj->bufTxLen) return 0;


    //wait until 
    while(getSn_TX_FSR(socketObj->hwSocNum) < socketObj->txFrameSize){
        asm("NOP");
    }

    ret = send(socketObj->hwSocNum, socketObj->bufTx, socketObj->txFrameSize);
    if (ret <= 0)
    {
        // printf("%d:send fail\r\n", sn);
        // close the socket
        if ((ret = disconnect(socketObj->hwSocNum)) != SOCK_OK){
            // printf("%d:disconnect fail\r\n", sn);
            asm("NOP");
            return ret;
        }
        asm("NOP");
        return 0;
    }

    if(ret != socketObj->txFrameSize){
        return 0;
    }

    if (socketObj->txTotalSize >= socketObj->txFrameSize)
    {
        socketObj->txTotalSize -= socketObj->txFrameSize;
    }
    else
    {
        socketObj->txTotalSize = 0;
    }
    socketObj->txFrameSize = 0;
    return ret;
}

//------------------------------ HttpServer
int32_t Eth_f_runTcpSrv(Eth_t_SocTcp *socketObj)
{
    __IO int32_t ret;
    // uint8_t dest_ip[4];
    // uint16_t dest_port;
    __IO uint16_t size = 0;

    // Socket Status Transitions
    // Check the W5500 Socket n status register (Sn_SR, The 'Sn_SR' controlled by Sn_CR command or Packet send/recv status)
    switch (getSn_SR(socketObj->hwSocNum))
    {

    case SOCK_CLOSED:
    {
        /* open socket */
        // printf("TCP server start\r\n");
        if ((ret = socket(socketObj->hwSocNum, Sn_MR_TCP, socketObj->port, 0x00)) != socketObj->hwSocNum)
        {
            // printf("socket %d open fail\r\n", sn);
            asm("NOP");
            return ret;
        }
        // printf("socket %d open success\r\n", sn);
    }
    break;

    case SOCK_INIT:
    {
        /* waiting for a client to connect */
        // printf("listen %d port...\r\n", port);
        if ((ret = listen(socketObj->hwSocNum)) != SOCK_OK)
        {
            // printf("%d:listen fail\r\n", sn);
            asm("NOP");
            return ret;
        }
        // printf("%d:listen success\r\n", sn);
    }
    break;

    case SOCK_ESTABLISHED:
    {
        /* socket has been established */
        if (getSn_IR(socketObj->hwSocNum) & Sn_IR_CON)
        {
            // getSn_DIPR(ethServer->socNum, dest_ip);
            // dest_port = getSn_DPORT(ethServer->socNum);
            // printf("%d:a client connect success, %d.%d.%d.%d:%d\r\n", sn, dest_ip[0], dest_ip[1], dest_ip[2], dest_ip[3], dest_port);
            setSn_IR(socketObj->hwSocNum, Sn_IR_CON);
        }

        switch (socketObj->exchangeState)
        {
        case state_idle:
        {
            // get the size of recv data in recv buffer
            if ((size = Eth_f_recv(socketObj)) > 0)
            {
                if (socketObj->echoMode)
                {
                    memcpy(socketObj->bufTx, socketObj->bufRx, size);
                    socketObj->txFrameSize = size;
                    if (Eth_f_send(socketObj) == size)
                    {
                        
                    }
                    break;
                }

                if (socketObj->cbfunc_pendingData != NULL)
                {
                    size = socketObj->cbfunc_pendingData(socketObj);
                }

                if ((socketObj->exchangeState == state_res_inProc))
                {
                    // if (socketObj->cbfunc_sendData == NULL)
                    // {
                    //     break;
                    // }

                    ///*size = */socketObj->cbfunc_sendData(socketObj);
                    // socketObj->txFrameSize = size;
                    size = socketObj->txFrameSize;
                    if (Eth_f_send(socketObj) == size)
                    {
                        if (socketObj->txTotalSize == 0)
                            socketObj->exchangeState = state_idle;
                    }
                }
            }
        }
        break;
        case state_res_inProc:
        {
            if (socketObj->cbfunc_sendData == NULL)
            {
                break;
            }

            /*size = */socketObj->cbfunc_sendData(socketObj);
            size = socketObj->txFrameSize;
            if (Eth_f_send(socketObj) == size)
            {
                if (socketObj->txTotalSize == 0)
                    socketObj->exchangeState = state_idle;
            }
        }
        break;
        case state_close:
        {
            if ((ret = disconnect(socketObj->hwSocNum)) != SOCK_OK)
            {
                asm("NOP");
                return ret;
            }
            socketObj->exchangeState = state_idle;
        }
            /* code */
            break;
        default:
            break;
        }
        // send resp data ?
    }
    break;

    case SOCK_CLOSE_WAIT:
        /* closing the socket */
        if ((ret = disconnect(socketObj->hwSocNum)) != SOCK_OK)
        {
            // printf("%d:disconnect fail\r\n", sn);
            asm("NOP");
            return ret;
        }
        // printf("%d: socket is closed\r\n", sn);
        break;
    break;
    default:
        break;
    }
    asm("NOP");
    return SOCK_OK;
}
//------------------------------ HttpServer END




#ifdef ETH_USE_DHCP
void eth_dhcp_ip_assign(){
  LL_GPIO_TogglePin(O_D_Led_GPIO_Port, O_D_Led_Pin);

    
}

void eth_dhcp_ip_conflict(){
    Error_Handler();
}
#endif


uint8_t Eth_f_GetMacFromUID(uint8_t *mac)
{
    *(mac + 0) = 0x00;
    *(mac + 1) = 0x80;
    *(mac + 2) = 0xE1;
    *(mac + 3) = (uint8_t)(HAL_GetUIDw0());  // byte 0
    *(mac + 4) = (uint8_t)(HAL_GetUIDw0()>>16);  // byte 2
    *(mac + 5) = (uint8_t)(HAL_GetUIDw1());  // byte 4
    return SET;
}



//------------------------------SPI FUNC-----------------------------------
static void cs_sel()
{
    LL_GPIO_ResetOutputPin(W5500_CS_GPIO_Port, W5500_CS_Pin); // CS Н�?ЗК�?Й
}

static void cs_desel()
{
    LL_GPIO_SetOutputPin(W5500_CS_GPIO_Port, W5500_CS_Pin); // CS ВЫСОК�?Й
}

static uint8_t spi_rb(void)
{
    uint8_t rbuf;

    if (HAL_SPI_Receive(&ETH_SPI, &rbuf, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        Error_Handler();
    }
    return rbuf;
}

static void spi_wb(uint8_t b)
{
    if (HAL_SPI_Transmit(&ETH_SPI, &b, 1, HAL_MAX_DELAY) != HAL_OK)
    {
        Error_Handler();
    }
}


#define USE_DMA
static void spi_rb_burst(uint8_t *buf, uint16_t len)
{
#ifdef USE_DMA
    if (HAL_SPI_Receive_DMA(&ETH_SPI, buf, len) != HAL_OK)
    {
        Error_Handler();
    }
    while (HAL_SPI_GetState(&ETH_SPI) == HAL_SPI_STATE_BUSY_RX)
        ;
#else
    HAL_SPI_Receive(&ETH_SPI, buf, len, HAL_MAX_DELAY);
#endif
}

static void spi_wb_burst(uint8_t *buf, uint16_t len)
{
#ifdef USE_DMA
    if (HAL_SPI_Transmit_DMA(&ETH_SPI, buf, len) != HAL_OK)
    {
        Error_Handler();
    }

    while (HAL_SPI_GetState(&ETH_SPI) == HAL_SPI_STATE_BUSY_TX)
        ;
#else
    HAL_SPI_Transmit(&ETH_SPI, buf, len, HAL_MAX_DELAY);
#endif
}
//------------------------------SPI FUNC END-----------------------------------



//-----------------------  ---------------------------
// uint8_t EthTask()
// {
//     static uint32_t tick = 0;
//     tick = MAIN_DBG_MCU_GET();

//     //LL_GPIO_TogglePin(O_D_Led_GPIO_Port, O_D_Led_Pin);


// #ifdef ETH_USE_DHCP
//     static uint32_t tim_DHCP_task = 0;
//     static uint32_t tim_DHCP_time = 0;
//     uint8_t dhcp_state = 0;

//     if (++tim_DHCP_task % ETH_DHCP_TASK_MS == 0)
//         dhcp_state = DHCP_run();
//     if (++tim_DHCP_time % ETH_DHCP_TIME_MS == 0)
//         DHCP_time_handler();
// #endif

// #ifdef ETH_USE_UDP_FINDER
//     uint8_t ip[4];
//     uint16_t port;
//     int32_t resLen = recvfrom(ETH_SOC_UDP, udpBufRx, ETH_UDP_BUF_RX_SIZE, ip, &port);
//     switch (resLen)
//     {
//     case SOCKERR_DATALEN:
//         /* code */
//         asm("NOP");
//         break;
//     case SOCKERR_SOCKMODE:
//         /* code */
//         asm("NOP");
//         break;
//     case SOCK_BUSY:
//         /* code */
//         asm("NOP");
//         break;
//     default:
//         if (resLen > 0)
//         {
//             tick = MAIN_DBG_MCU_GET() - tick;
//             // LL_GPIO_TogglePin(O_D_Led_GPIO_Port, O_D_Led_Pin);
//         }
//         break;
//     }
// #endif


//     // if(EthTcpServer() != SOCK_OK){
//     //     Error_Handler();
//     // }


//     tick = MAIN_DBG_MCU_GET() - tick;
//     //LL_GPIO_TogglePin(O_D_Led_GPIO_Port, O_D_Led_Pin);
//     asm("NOP");
//     return 0;
// }