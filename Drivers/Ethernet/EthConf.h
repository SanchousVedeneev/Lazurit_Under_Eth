
#ifndef __ETH_CONF__H__
#define __ETH_CONF__H__


//#define ETH_USE_DHCP
//#define ETH_USE_UDP_FINDER

#define ETH_SOC_DHCP (0)
#define ETH_SOC_UDP (1)
#define ETH_SOC_TCP (2)


#define ETH_UDP_PORT (6666)
#define ETH_UDP_BUF_RX_SIZE (256)
#define ETH_UDP_BUF_TX_SIZE (256)
#define ETH_UDP_MSG_SEND_ID (0x0102)


#define ETH_IP_BROADCAST   \
    {                      \
        255, 255, 255, 255 \
    }

#define ETH_IP           \
    {                    \
        192, 168, 0, 155 \
    }
#define ETH_MASK         \
    {                    \
        255, 255, 255, 0 \
    }
#define ETH_GTW        \
    {                  \
        192, 168, 0, 1 \
    }

#define ETH_SPI     (hspi2)
#define ETH_CS_PORT W5500_CS_GPIO_Port
#define ETH_CS_PIN W5500_CS_Pin













#endif