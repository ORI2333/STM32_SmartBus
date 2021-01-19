#ifndef _BSP_USART1_H
#define _BSP_USART1_H

#include "stm32f10x.h"
#include "stdio.h"


/* USART1 */
#define DEBUG_USARTx                 USART1
#define DEBUG_USART_CLK              RCC_APB2Periph_USART1
#define DEBUG_USART_APBxClkCmd       RCC_APB2PeriphClockCmd
#define DEBUG_USART_BAUDRATE         115200

#define  DEBUG_USART_IRQ             USART1_IRQn
#define  DEBUG_USART1_IRQHandler      USART1_IRQHandler

/* USART1 GPIO ���ź궨�� */
#define DEBUG_USART_GPIO_CLK         RCC_APB2Periph_GPIOA
#define DEBUG_USART_GPIO_APBxClkCmd  RCC_APB2PeriphClockCmd

#define DEBUG_USART_TX_GPIO_PORT     GPIOA
#define DEBUG_USART_TX_GPIO_PIN      GPIO_Pin_9
#define DEBUG_USART_RX_GPIO_PORT     GPIOA
#define DEBUG_USART_RX_GPIO_PIN      GPIO_Pin_10


#define USART1_REC_LEN  			200  	//�����������ֽ��� 200
#define EN_USART1_RX 			1		      //ʹ�ܣ�1��/��ֹ��0������1����
	  	
extern u8  USART1_RX_BUF[USART1_REC_LEN]; //���ջ���,���USART_REC_LEN���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u16 USART1_RX_STA;         				//����״̬���	


void USART1_Config(void);


#endif /* _BSP_USART1_H */
