#ifndef _BSP_DMA_H
#define _BSP_DMA_H

#include "stm32f10x.h"

#define SENDBUFF_SIZE           5000
#define USART_DR_ADDRESS        (USART2_BASE+0x04)

#define USART_TX_DMA_CLK        RCC_AHBPeriph_DMA1
#define USART_TX_DMAy_CHANNELx  DMA1_Channel7
#define USART_TX_DMA_FLAG_TC    DMA1_FLAG_TC7

void DMA_USARTx_TX_Config(void);
void UART_DMA_Enable(DMA_Channel_TypeDef*DMA_CHx,u16 len);


#endif /* _BSP_DMA_H */

