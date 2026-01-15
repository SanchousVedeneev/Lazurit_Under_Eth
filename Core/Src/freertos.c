/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * File Name          : freertos.c
 * Description        : Code for freertos applications
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file
 * in the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "Eth.h"
#include "BSP_ICA2D13.h"

#include "tim.h"
#include "usart.h"

#include "ModbusSS.h"
#include "ModbusConf.h"
#include "ModbusProtocol.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
int32_t cbckModbusTcpServer(Eth_t_SocTcp *socket);

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/*---------------------MODBUS------------------------*/
// ModbusCoreTypedef modbusCore;
#define MODBUS_TCP_SERVER_SOC_CNT (4)
Eth_t_SocTcp tcpModbusSocket[MODBUS_TCP_SERVER_SOC_CNT];
uint8_t modbusBufRxTxTCP[MODBUS_SS_BUF_CNT];
#define MODBUS_TABLE_HOLDING_QUANT (MBP_reg_7 - MBP_reg_1 + 1)
uint16_t modbusHoldingBuf[MODBUS_TABLE_HOLDING_QUANT];


#define MDB_RTU_TX_SLACE_ID (1)
#define MDB_RTU_TX_FUNCTION (4)
#define MDB_RTU_TX_REG_ADRES (1)
#define MDB_RTU_TX_REG_QANT (5)
#define MDB_RTU_TX_COUNT_BYTE (8)

#define MDB_RTU_RX_COUNT_BYTE (5 + (MDB_RTU_TX_REG_QANT * 2))

uint8_t modbusBufTxRtu485[30];
uint8_t modbusBufRxRtu485[30];
uint8_t mdb_wait_rx_flag = 0;

uint16_t mdb_reg_buf[MDB_RTU_TX_REG_QANT] = {0};
uint16_t mdb_err_cnt = 0;
uint8_t  mdb_err_code = 0;


//--ModbusSS
ModbusSS_table_t modbusTableHolding1 = {
    .buf = (uint8_t *)modbusHoldingBuf,
    .quantity = MODBUS_TABLE_HOLDING_QUANT,
    .regNo = MBP_reg_1,
    .type = ModbusSS_Holding};

ModbusSS_table_t *modbusTables[1] = {&modbusTableHolding1};

void modbusHoldingReq(ModbusSS_table_t *table, uint16_t reg, uint16_t quantity);

ModbusSS_t modbusSS_tcp = {
    .bufRxTx = modbusBufRxTxTCP,
    .cbHoldingRequest = modbusHoldingReq,
    .rtuTcp = MODBUS_SS_TCP,
    .slaveId = 1,
    .tables = modbusTables,
    .tablesCount = 1};

/*---------------------MODBUS END------------------------*/

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
osThreadId_t taskNetHandle;
const osThreadAttr_t taskNet_attributes = {
    .name = "taskNet",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

osThreadId_t taskMdbHandle;
const osThreadAttr_t taskMdb_attributes = {
    .name = "taskMdb",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
    .name = "defaultTask",
    .stack_size = 256 * 4,
    .priority = (osPriority_t)osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void taskNetEntry(void *argument);
void taskMdbEntry(void *argument);

__STATIC_INLINE uint16_t _Crc16_mdb(uint8_t *pcBlock, uint16_t len);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
 * @brief  FreeRTOS initialization
 * @param  None
 * @retval None
 */
void MX_FREERTOS_Init(void)
{
  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of defaultTask */
  defaultTaskHandle = osThreadNew(StartDefaultTask, NULL, &defaultTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  taskNetHandle = osThreadNew(taskNetEntry, NULL, &taskNet_attributes);
  taskMdbHandle = osThreadNew(taskMdbEntry, NULL, &taskMdb_attributes);

  asm("NOP");
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */
}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
 * @brief  Function implementing the defaultTask thread.
 * @param  argument: Not used
 * @retval None
 */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void *argument)
{
  /* USER CODE BEGIN StartDefaultTask */

  asm("NOP");

  // uint16_t state_DI = 0;
  // uint16_t state_AI[2];

  // HAL_UART_Receive_IT(&huart1, modbusBufRxTxRtu485,10);

  /* Infinite loop */
  for (;;)
  {
    // for (int i = BSP_DI1; i < BSP_DI_COUNT; i++)
    // {
    //     bsp_di_get(i);
    // }
    // asm("NOP");
    // bsp_ai_read();

    // LL_GPIO_TogglePin(O_D_Led_GPIO_Port, O_D_Led_Pin);
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

//---------------------------------- NETWORK
void taskNetEntry(void *argument)
{
  /* USER CODE BEGIN taskLedEntry */

  if (Eth_f_Init() != SET)
  {
    Error_Handler();
  };

  /*---------------------MODBUS------------------------*/
  for (int i = 0; i < MODBUS_TCP_SERVER_SOC_CNT; i++)
  {
    tcpModbusSocket[i].hwSocNum = i;
    tcpModbusSocket[i].port = 502;
    tcpModbusSocket[i].bufRx = modbusBufRxTxTCP;
    tcpModbusSocket[i].bufTx = modbusBufRxTxTCP;
    tcpModbusSocket[i].bufRxLen = MODBUS_SS_BUF_CNT;
    tcpModbusSocket[i].bufTxLen = MODBUS_SS_BUF_CNT;
    tcpModbusSocket[i].cbfunc_pendingData = cbckModbusTcpServer;
    // tcpModbusSocket[i].echoMode = SET;
  }
  /*---------------------MODBUS END------------------------*/

  /* Infinite loop */
  for (;;)
  {

    for (int i = 0; i < MODBUS_TCP_SERVER_SOC_CNT; i++)
    {
      if (Eth_f_runTcpSrv(&tcpModbusSocket[i]) != SET)
      {
        Error_Handler();
      }
    }

    // BSP_LED2_G_TOGGLE();

    osDelay(1);
  }
  /* USER CODE END taskLedEntry */
}

int32_t cbckModbusTcpServer(Eth_t_SocTcp *socket)
{
  socket->exchangeState = state_res_inProc;
  socket->txFrameSize = ModbusSS_ParseRxData(&modbusSS_tcp);
  return 0;
}

void modbusHoldingReq(ModbusSS_table_t *table, uint16_t reg, uint16_t quantity)
{

  BSP_LED1_TOGGLE();
  if (table == &modbusTableHolding1)
  {
    for (int r = reg; r < reg + quantity; r++)
    {
      asm("NOP");
      switch (r)
      {
      case MBP_reg_1:
        ModbusSS_SetWord(table, r, mdb_reg_buf[0]);
        asm("NOP");
        break;
      case MBP_reg_2:
        ModbusSS_SetWord(table, r, mdb_reg_buf[1]);
        asm("NOP");
        break;
      case MBP_reg_3:
        ModbusSS_SetWord(table, r, mdb_reg_buf[2]);
        asm("NOP");
        break;
      case MBP_reg_4:
        ModbusSS_SetWord(table, r, mdb_reg_buf[3]);
        asm("NOP");
        break;
      case MBP_reg_5:
        ModbusSS_SetWord(table, r, mdb_reg_buf[4]);
        asm("NOP");
        break;
      case MBP_reg_6:
        ModbusSS_SetWord(table, r, mdb_err_cnt);
        asm("NOP");
        break;
      case MBP_reg_7:
        ModbusSS_SetWord(table, r, mdb_err_code);
        asm("NOP");
        break;
      default:
        break;
      }
    }
  }
}
//---------------------------------- NETWORK END

//---------------------------------- MODBUS RTU


#define MDB_RTU_TX_SCAN_RATE (250)
#define MDB_RTU_RX_RESPONS_TIMEOUT (2000)

enum userFlag
{
  flagReset = 0,
  flagSet = 1
};

enum Mdb_RTU_TX_buf_index
{
  adress_TX   = 0,
  function_TX = 1,
  adress_first_reg_HB_TX = 2,
  adress_first_reg_LB_TX = 3,
  count_reg_HB_TX = 4,
  count_reg_LB_TX = 5,
  CRC_LB_TX = 6,
  CRC_HB_TX = 7,
};

enum Mdb_RTU_err_name
{
  noErr   = 0,
  crc_no_corret = 3,
  timeout = 11
};

void taskMdbEntry(void *argument)
{
  /* USER CODE BEGIN taskMdbEntry */

  /* Infinite loop */

  for (;;)
  {
    static uint16_t mdb_req_time = 0;
    static uint16_t mdb_respons_timeout = 0;
    static uint16_t crc = 0;
    osDelay(1);

    if (mdb_wait_rx_flag == flagSet)
    {
      if (mdb_respons_timeout++ > MDB_RTU_RX_RESPONS_TIMEOUT)
      {
        HAL_UART_AbortReceive_IT(&huart1);
        mdb_wait_rx_flag = flagReset;
        mdb_respons_timeout = 0;
        mdb_err_cnt++;
        mdb_err_code = timeout;
      }
    }
    else if (mdb_req_time++ > MDB_RTU_TX_SCAN_RATE)
    {

      modbusBufTxRtu485[adress_TX] = MDB_RTU_TX_SLACE_ID;
      modbusBufTxRtu485[function_TX] = MDB_RTU_TX_FUNCTION;
      modbusBufTxRtu485[adress_first_reg_HB_TX] = 0x00;
      modbusBufTxRtu485[adress_first_reg_LB_TX] = MDB_RTU_TX_REG_ADRES;
      modbusBufTxRtu485[count_reg_HB_TX] = 0x00;
      modbusBufTxRtu485[count_reg_LB_TX] = MDB_RTU_TX_REG_QANT;
      crc = _Crc16_mdb(modbusBufTxRtu485, 6);
      modbusBufTxRtu485[CRC_LB_TX] = (uint8_t)crc;
      modbusBufTxRtu485[CRC_HB_TX] = (uint8_t)(crc >> 8);

      mdb_wait_rx_flag = flagSet;
      mdb_respons_timeout = 0;
      if (HAL_UART_Transmit(&huart1, modbusBufTxRtu485, MDB_RTU_TX_COUNT_BYTE, 2) == HAL_OK)
      {
        HAL_UART_Receive_IT(&huart1, modbusBufRxRtu485, MDB_RTU_RX_COUNT_BYTE);
      }
      mdb_req_time = 0;
    }
  }
  /* USER CODE END taskMdbEntry */
}

enum Mdb_RTU_RX_buf_index
{
  adress_RX   = 0,
  function_RX = 1,
  count_byte_RX = 2,

/*
reg data
*/

  CRC_LB_RX = count_byte_RX+1+MDB_RTU_TX_REG_QANT*2,
  CRC_HB_RX = CRC_LB_RX+1,
};


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  uint8_t mdb_err = noErr;
  uint16_t calculated_CRC = 0;
  uint16_t resive_CRC = 0;

  calculated_CRC = _Crc16_mdb(modbusBufRxRtu485, (MDB_RTU_RX_COUNT_BYTE-2));

  resive_CRC |= (uint16_t)(modbusBufRxRtu485[CRC_LB_RX]);
  resive_CRC |= (uint16_t)(modbusBufRxRtu485[CRC_HB_RX] << 8);

  if (calculated_CRC != resive_CRC)
  {
    mdb_err = crc_no_corret;
  }


  if (mdb_err_code == 0)
  {
    for (uint8_t i = 0; i < MDB_RTU_TX_REG_QANT; i++)
    {
      mdb_reg_buf[i] = 0;
    }

    for (uint8_t ind = (count_byte_RX+1), i = 0; i < MDB_RTU_TX_REG_QANT ; ind+=2, i++)
    {
      mdb_reg_buf[i] |= (uint16_t)(modbusBufRxRtu485[ind] << 8);
      mdb_reg_buf[i] |= (uint16_t)(modbusBufRxRtu485[ind+1]);
    }
  }

  mdb_wait_rx_flag = flagReset;
  BSP_LED3_TOGGLE();
  mdb_err_code = mdb_err;
}

__STATIC_INLINE uint16_t _Crc16_mdb(uint8_t *pcBlock, uint16_t len)
{
  uint16_t w = 0;
  uint8_t shift_cnt;

  w = 0xffffU;
  for (; len > 0; len--)
  {
    w = (uint16_t)((w / 256U) * 256U + ((w % 256U) ^ (*pcBlock++)));
    for (shift_cnt = 0; shift_cnt < 8; shift_cnt++)
    {
      if ((w & 0x1) == 1)
        w = (uint16_t)((w >> 1) ^ 0xa001U);
      else
        w >>= 1;
    }
  }
  return w;
}

/* USER CODE END Application */
