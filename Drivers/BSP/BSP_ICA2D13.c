
#include "BSP_ICA2D13.h"

// #include "adc.h"
#include "usart.h"


void bsp_init()
{
//   USART1->RTOR |= 10 << USART_RTOR_RTO_Pos;
//   huart3.Instance->CR1 |= USART_CR1_RTOIE;
//   huart3.Instance->CR2 |= USART_CR2_RTOEN;
//   HAL_DMA_Start(huart3.hdmarx, (uint32_t)&huart3.Instance->RDR, (uint32_t)modbusStruct._BuferRxTx, MODBUS_BUF_CNT);
//   huart3.Instance->CR3 |= USART_CR3_DMAR;
}

__INLINE void bsp_led1_rg_incDec(int32_t inc)
{
    static int8_t dir = 1;
    BSP_LED1_G_TIM_CHANNEL_CCR += dir*inc;
    BSP_LED1_R_TIM_CHANNEL_CCR = BSP_LED1_G_TIM_CHANNEL_CCR;

    if(BSP_LED1_G_TIM_CHANNEL_CCR >= BSP_LED1_RG_TIM_PERIOD){
        BSP_LED1_G_TIM_CHANNEL_CCR = BSP_LED1_RG_TIM_PERIOD;
        dir*=-1;
    }else if ((BSP_LED1_G_TIM_CHANNEL_CCR == 0) || (BSP_LED1_G_TIM_CHANNEL_CCR > BSP_LED1_RG_TIM_PERIOD*2) ){
                dir*=-1;
        BSP_LED1_G_TIM_CHANNEL_CCR = 0;
    }
}

__INLINE void bsp_led1_rg_incDecInv(int32_t inc)
{
    static int8_t dir = 1;
    if (BSP_LED1_R_TIM_CHANNEL_CCR == 0)
    {
        dir *= -1;
    }
    else if (BSP_LED1_R_TIM_CHANNEL_CCR == BSP_LED1_RG_TIM_PERIOD)
    {
        dir *= -1;
    }
    int32_t ccr = BSP_LED1_R_TIM_CHANNEL_CCR + dir * inc;
    bsp_set_led1_rg(ccr, BSP_LED1_RG_TIM_PERIOD - ccr);
}
