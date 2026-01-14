
#include "BSP_ICA2D13.h"

#include "adc.h"
#include "usart.h"

typedef struct BSP_DI_TYPE
{
    GPIO_TypeDef* port;
    uint32_t pin;
    uint8_t state;
}bsp_di_t;

bsp_di_t diArray[] = {
    {.pin = ID_1_Pin,
     .port = ID_1_GPIO_Port,
     .state = 0},
         {.pin = ID_2_Pin,
     .port = ID_2_GPIO_Port,
     .state = 0},
         {.pin = ID_3_Pin,
     .port = ID_3_GPIO_Port,
     .state = 0},
         {.pin = ID_4_Pin,
     .port = ID_4_GPIO_Port,
     .state = 0},
         {.pin = ID_5_Pin,
     .port = ID_5_GPIO_Port,
     .state = 0},
         {.pin = ID_6_Pin,
     .port = ID_6_GPIO_Port,
     .state = 0},
         {.pin = ID_7_Pin,
     .port = ID_7_GPIO_Port,
     .state = 0},
         {.pin = ID_8_Pin,
     .port = ID_8_GPIO_Port,
     .state = 0},
         {.pin = ID_9_Pin,
     .port = ID_9_GPIO_Port,
     .state = 0},
         {.pin = ID_10_Pin,
     .port = ID_10_GPIO_Port,
     .state = 0},
         {.pin = ID_11_Pin,
     .port = ID_11_GPIO_Port,
     .state = 0},
        {.pin = ID_12_Pin,
     .port = ID_12_GPIO_Port,
     .state = 0},
        {.pin = ID_13_Pin,
     .port = ID_13_GPIO_Port,
     .state = 0}
};

typedef struct BSP_AI_TYPE
{
    ADC_HandleTypeDef* handle;
    uint32_t rank;
    uint16_t value;
}bsp_ai_t;

bsp_ai_t aiArray[] = {
{.handle = &hadc2,
.rank = ADC_INJECTED_RANK_1,
.value = 0},
{.handle = &hadc2,
.rank = ADC_INJECTED_RANK_2,
.value = 0}
};

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

uint16_t bsp_di_get_cache_pack16()
{
    uint16_t w = 0;
    for (int i = 0; i < BSP_DI_COUNT; i++)
    {
        w |= (bsp_di_get_cache(i) ? 1:0) << i;
    }
    return w;
}

__INLINE uint8_t bsp_di_get(bsp_di_enum di)
{
    bsp_di_t* __di = &diArray[di];
    __di->state = LL_GPIO_IsInputPinSet(__di->port,__di->pin);
    return __di->state;
}

__INLINE uint8_t bsp_di_get_cache(bsp_di_enum di)
{
    bsp_di_t* __di = &diArray[di];
    return __di->state;
}


void bsp_ai_read(){
    HAL_ADCEx_InjectedPollForConversion(&hadc2,500);
    HAL_ADCEx_InjectedStart(&hadc2);
    HAL_ADCEx_InjectedPollForConversion(&hadc2,500);

    for (int i = 0; i < BSP_AI_COUNT; i++)
    {
        aiArray[i].value = HAL_ADCEx_InjectedGetValue(aiArray[i].handle, aiArray[i].rank);
    }
}

uint16_t bsp_ai_read_cache(bsp_ai_enum ai){
    return aiArray[ai].value;
}