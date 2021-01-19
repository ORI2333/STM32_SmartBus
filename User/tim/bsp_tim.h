#ifndef _BSP_TIM_H
#define _BSP_TIM_H

#include "stm32f10x.h"

void TIM4_Set(uint8_t sta);
void TIM4_SetARR(uint16_t period);
void TIM4_Init(uint16_t arr,uint16_t psc);


#endif /* _BSP_TIM_H */
