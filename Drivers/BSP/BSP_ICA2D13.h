#ifndef _BSP_ICA2D13_H_
#define _BSP_ICA2D13_H_

#include "main.h"

void bsp_init();

//----------------------- LED1 Red
#define BSP_LED1_ON()                         (LL_GPIO_SetOutputPin(LED1_GPIO_Port, LED1_Pin))
#define BSP_LED1_OFF()                        (LL_GPIO_ResetOutputPin(LED1_GPIO_Port, LED1_Pin))
#define BSP_LED1_TOGGLE()                     (LL_GPIO_TogglePin(LED1_GPIO_Port, LED1_Pin))
//----------------------- LED1 Red End

//----------------------- LED3 Red
#define BSP_LED3_ON()                         (LL_GPIO_SetOutputPin(LED3_GPIO_Port, LED3_Pin))
#define BSP_LED3_OFF()                        (LL_GPIO_ResetOutputPin(LED3_GPIO_Port, LED3_Pin))
#define BSP_LED3_TOGGLE()                     (LL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin))
//----------------------- LED3 Red End

#endif