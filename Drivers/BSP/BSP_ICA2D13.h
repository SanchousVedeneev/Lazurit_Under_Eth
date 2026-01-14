#ifndef _BSP_ICA2D13_H_
#define _BSP_ICA2D13_H_

#include "main.h"

void bsp_init();


//----------------------- LED 1 Red & Green
#define BSP_LED1_G_TIM                          TIM2
#define BSP_LED1_R_TIM                          TIM3
#define BSP_LED1_G_TIM_CHANNEL                  (LL_TIM_CHANNEL_CH2)
#define BSP_LED1_R_TIM_CHANNEL                  (LL_TIM_CHANNEL_CH1)
#define BSP_LED1_G_TIM_CHANNEL_CCR              BSP_LED1_G_TIM->CCR2
#define BSP_LED1_R_TIM_CHANNEL_CCR              BSP_LED1_R_TIM->CCR1
#define BSP_LED1_RG_TIM_PERIOD                  TIM_LED_PERIOD

#define BSP_LED1_RG_TIM_START()                 LL_TIM_EnableCounter(BSP_LED1_G_TIM);\
                                                LL_TIM_EnableCounter(BSP_LED1_R_TIM);\
                                                (LL_TIM_CC_EnableChannel(BSP_LED1_G_TIM,  BSP_LED1_G_TIM_CHANNEL));\
                                                (LL_TIM_CC_EnableChannel(BSP_LED1_R_TIM,  BSP_LED1_R_TIM_CHANNEL));

#define BSP_LED1_RG_SET_PWM(X)                  LED1_G_TIM_CHANNEL_CCR(BSP_LED1_G_TIM,X);\
                                                LED1_R_TIM_CHANNEL_CCR(BSP_LED1_R_TIM,X);


__STATIC_INLINE void bsp_set_led1_rg(int32_t r, int32_t g){
    if(r > BSP_LED1_RG_TIM_PERIOD) r = BSP_LED1_RG_TIM_PERIOD;
    else if (r < 0 ) r = 0;
    BSP_LED1_R_TIM_CHANNEL_CCR = r;

    if(g > BSP_LED1_RG_TIM_PERIOD) g = BSP_LED1_RG_TIM_PERIOD;
    else if (g < 0 ) g = 0;
    BSP_LED1_G_TIM_CHANNEL_CCR = g;
}

__STATIC_INLINE void bsp_led1_rg_inc(uint32_t inc){
    if(BSP_LED1_G_TIM_CHANNEL_CCR < BSP_LED1_RG_TIM_PERIOD){
        BSP_LED1_G_TIM_CHANNEL_CCR += inc;
        BSP_LED1_R_TIM_CHANNEL_CCR += inc;
    }else{
        BSP_LED1_G_TIM_CHANNEL_CCR = 0;
        BSP_LED1_R_TIM_CHANNEL_CCR = 0;  
    }
}

void bsp_led1_rg_incDec(int32_t inc);
void bsp_led1_rg_incDecInv(int32_t inc);


//----------------------- LED 1 Red & Green End

//----------------------- LED 2 Green
#define BSP_LED2_G_ON()                         (LL_GPIO_SetOutputPin(OD_LED2G_GPIO_Port,OD_LED2G_Pin))
#define BSP_LED2_G_OFF()                        (LL_GPIO_ResetOutputPin(OD_LED2G_GPIO_Port,OD_LED2G_Pin))
#define BSP_LED2_G_TOGGLE()                     (LL_GPIO_TogglePin(OD_LED2G_GPIO_Port,OD_LED2G_Pin))
//----------------------- LED 2 Green End


// DI States
typedef enum{
    BSP_DI1 = 0,
    BSP_DI2,
    BSP_DI3,
    BSP_DI4,
    BSP_DI5,
    BSP_DI6,
    BSP_DI7,
    BSP_DI8,
    BSP_DI9,
    BSP_DI10,
    BSP_DI11,
    BSP_DI12,
    BSP_DI13
}bsp_di_enum;

#define BSP_DI_COUNT (BSP_DI13 - BSP_DI1 +1)

uint8_t bsp_di_get(bsp_di_enum di);
uint8_t bsp_di_get_cache(bsp_di_enum di);
uint16_t bsp_di_get_cache_pack16();
// DI States End


//Analog input
typedef enum{
    BSP_AI1 = 0,
    BSP_AI2
}bsp_ai_enum;

#define BSP_AI_COUNT (BSP_AI2 - BSP_AI1 + 1)

void bsp_ai_read(void);
uint16_t bsp_ai_read_cache(bsp_ai_enum ai);
//Analog input end



#endif