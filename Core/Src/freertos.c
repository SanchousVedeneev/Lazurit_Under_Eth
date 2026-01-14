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
int32_t cbckModbusTcpServer(Eth_t_SocTcp* socket);

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/*---------------------MODBUS------------------------*/
//ModbusCoreTypedef modbusCore;
#define MODBUS_TCP_SERVER_SOC_CNT (4)
Eth_t_SocTcp tcpModbusSocket[MODBUS_TCP_SERVER_SOC_CNT];
uint8_t modbusBufRxTxTCP[MODBUS_SS_BUF_CNT];
uint8_t modbusBufRxTxRtu485[MODBUS_SS_BUF_CNT];
#define MODBUS_TABLE_HOLDING_QUANT (MBP_DI_STATE - MBP_AI1_X + 1)
uint16_t modbusHoldingBuf[MODBUS_TABLE_HOLDING_QUANT];

//--ModbusSS
ModbusSS_table_t modbusTableHolding1 = {
  .buf = (uint8_t*)modbusHoldingBuf,
  .quantity = MODBUS_TABLE_HOLDING_QUANT,
  .regNo = MBP_AI1_X,
  .type = ModbusSS_Holding
};

ModbusSS_table_t* modbusTables[1] = {&modbusTableHolding1};

void modbusHoldingReq(ModbusSS_table_t* table, uint16_t reg, uint16_t quantity);

ModbusSS_t modbusSS_tcp = {
    .bufRxTx = modbusBufRxTxTCP,
    .cbHoldingRequest = modbusHoldingReq,
    .rtuTcp = MODBUS_SS_TCP,
    .slaveId = 1,
    .tables = modbusTables,
    .tablesCount = 1};

ModbusSS_t modbusSS_rtu_rs485 = {
    .bufRxTx = modbusBufRxTxRtu485,
    .cbHoldingRequest = modbusHoldingReq,
    .rtuTcp = MODBUS_SS_RTU,
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
osThreadId_t taskLedHandle;
const osThreadAttr_t taskLed_attributes = {
  .name = "taskLed",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

osThreadId_t taskNetHandle;
const osThreadAttr_t taskNet_attributes = {
  .name = "taskNet",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* USER CODE END Variables */
/* Definitions for defaultTask */
osThreadId_t defaultTaskHandle;
const osThreadAttr_t defaultTask_attributes = {
  .name = "defaultTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void taskLedEntry(void *argument);
void taskNetEntry(void *argument);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void *argument);

extern void MX_USB_DEVICE_Init(void);
void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
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
  taskLedHandle = osThreadNew(taskLedEntry, NULL, &taskLed_attributes);
  taskNetHandle = osThreadNew(taskNetEntry, NULL, &taskNet_attributes);

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
  /* init code for USB_DEVICE */
  MX_USB_DEVICE_Init();
  /* USER CODE BEGIN StartDefaultTask */

  asm("NOP");

  // uint16_t state_DI = 0;
  // uint16_t state_AI[2];

  HAL_UART_Receive_IT(&huart1, modbusBufRxTxRtu485,10);


  /* Infinite loop */
  for(;;)
  {
    for (int i = BSP_DI1; i < BSP_DI_COUNT; i++)
    {
        bsp_di_get(i);
    }
    asm("NOP");
    bsp_ai_read();

    // LL_GPIO_TogglePin(O_D_Led_GPIO_Port, O_D_Led_Pin);
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

void taskLedEntry(void *argument)
{
  /* USER CODE BEGIN taskLedEntry */
  BSP_LED1_RG_TIM_START();
 
  /* Infinite loop */
  for(;;)
  {
    bsp_led1_rg_incDecInv(1);
    osDelay(1);
  }
  /* USER CODE END taskLedEntry */
}

//---------------------------------- NETWORK 
void taskNetEntry(void *argument)
{
  /* USER CODE BEGIN taskLedEntry */

  if(Eth_f_Init() != SET){
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
    //tcpModbusSocket[i].echoMode = SET;
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

  BSP_LED2_G_TOGGLE();

  osDelay(1);
}
/* USER CODE END taskLedEntry */
}

int32_t cbckModbusTcpServer(Eth_t_SocTcp* socket){
  socket->exchangeState = state_res_inProc;
  socket->txFrameSize = ModbusSS_ParseRxData(&modbusSS_tcp);
  return 0;
}

void modbusHoldingReq(ModbusSS_table_t* table, uint16_t reg, uint16_t quantity){

  if (table == &modbusTableHolding1)
  {
    for (int r = reg; r < reg + quantity; r++)
    {
      asm("NOP");
      switch (r)
      {
      case MBP_AI1_X:
        ModbusSS_SetWord(table, r, bsp_ai_read_cache(BSP_AI1));
        asm("NOP");
        break;
      case MBP_AI2_Y:
        ModbusSS_SetWord(table, r, bsp_ai_read_cache(BSP_AI2));
        asm("NOP");
        break;
      case MBP_DI_STATE:
        ModbusSS_SetWord(table, r, bsp_di_get_cache_pack16());
        asm("NOP");
        break;
      default:
        break;
      }
    }
  }
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart){

  for (int i = 0; i < 10; i++)
  {
    modbusBufRxTxRtu485[i] = i;
  }
  HAL_UART_Transmit_IT(huart,modbusBufRxTxRtu485,10);

  HAL_UART_Receive_IT(&huart1, modbusBufRxTxRtu485,10);
  asm("NOP");

}

/* USER CODE END Application */

