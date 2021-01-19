#ifndef _BSP_USART2_H
#define _BSP_USART2_H

#include "stm32f10x.h"
#include "stdio.h"


/* USART2 */

#define DEBUG_USART2_CLK              RCC_APB1Periph_USART2
#define DEBUG_USART2_APBxClkCmd       RCC_APB1PeriphClockCmd
#define DEBUG_USART2_BAUDRATE         115200

#define  DEBUG_USART2_IRQ             USART2_IRQn
#define  DEBUG_USART2_IRQHandler      USART2_IRQHandler

/* USART GPIO 引脚宏定义 */
#define DEBUG_USART2_GPIO_CLK         RCC_APB2Periph_GPIOA
#define DEBUG_USART2_GPIO_APBxClkCmd  RCC_APB2PeriphClockCmd

#define DEBUG_USART2_TX_GPIO_PORT     GPIOA
#define DEBUG_USART2_TX_GPIO_PIN      GPIO_Pin_2
#define DEBUG_USART2_RX_GPIO_PORT     GPIOA
#define DEBUG_USART2_RX_GPIO_PIN      GPIO_Pin_3


#define USART2_MAX_RECV_LEN		1024				//最大接收缓存字节数
#define USART2_MAX_SEND_LEN		1024				//最大发送缓存字节数


extern uint8_t  USART2_RX_BUF[USART2_MAX_RECV_LEN]; 		//接收缓冲,最大USART2_MAX_RECV_LEN字节
extern uint8_t  USART2_TX_BUF[USART2_MAX_SEND_LEN]; 		//发送缓冲,最大USART2_MAX_SEND_LEN字节
extern uint16_t USART2_RX_STA;   					            	//接收数据状态
extern uint8_t Scan_Wtime;

void USART2_Config(void);
void u2_printf(char* fmt,...) ;
void Uart2_SendStr(char*SendBuf);

#endif /* _BSP_USART2_H */
