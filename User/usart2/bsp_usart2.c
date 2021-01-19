/*********************************************************************************
 
 * 串口2初始化文件
 
**********************************************************************************/	

#include "bsp_usart2.h"
#include "bsp_dma.h"
#include "stdarg.h"	 	  	 
#include "string.h"
#include "bsp_tim.h"


/* 串口发送缓存区 	*/
uint8_t USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	      //发送缓冲,最大USART2_MAX_SEND_LEN字节 

/* 串口接收缓存区 	*/
uint8_t USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//接收缓冲,最大USART2_MAX_RECV_LEN个字节.

	uint8_t Scan_Wtime;
	u16 USART2_RX_STA=0; 

/*------------------------ 配置 USARTx 中断优先级 -----------------------*/
static void NVIC_Configuration(void){
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);           // 配置中断优先级分组
	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART2_IRQ;    // 配置中断源 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 配置抢占优先级（主优先级）
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;        // 配置子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能中断
	NVIC_Init(&NVIC_InitStructure);
	
}

/*-------------------------- USART 相关参数配置 --------------------------*/
void USART2_Config(void){
	
	/* 初始化结构体 */
	GPIO_InitTypeDef   GPIO_InitStructure;
	USART_InitTypeDef  USART_InitStructure;
	
	/*** 初始化串口需要用到的GPIO ***/
	
	/* 打开串口GPIO的时钟 */
	DEBUG_USART2_GPIO_APBxClkCmd(DEBUG_USART2_GPIO_CLK, ENABLE);  
	
	/* 将USART Tx的GPIO配置为推挽复用模式 */
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART2_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART2_TX_GPIO_PORT, &GPIO_InitStructure);
	
	/* 将USART Rx的GPIO配置为浮空输入模式 */
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART2_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART2_RX_GPIO_PORT, &GPIO_InitStructure);
	
	/*** 配置 USART 的工作参数 ***/
	
	/* 打开串口外设的时钟 */
	DEBUG_USART2_APBxClkCmd(DEBUG_USART2_CLK, ENABLE);  
	
	USART_InitStructure.USART_BaudRate = DEBUG_USART2_BAUDRATE;   	 // 配置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  		 // 配置一帧数据字长
	USART_InitStructure.USART_StopBits = USART_StopBits_1;      		 // 配置停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;        	     // 配置校验位
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  // 配置工作模式，收发一起
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // 配置硬件流控制
	USART_Init(USART2, &USART_InitStructure);   	             // 完成串口的初始化配置
	
	/*** 中断配置 ***/
	
	NVIC_Configuration();  // 串口中断优先级配置
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);   // 使能串口接收中断
	
	/*** 使能串口 ***/
	
	USART_Cmd(USART2, ENABLE);
	
	/* 通道初始化 */
	DMA_USARTx_TX_Config();
	
	/* 打开通道 */
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);
	
	TIM4_Init(99,7199);	 // 10ms中断 
	USART2_RX_STA=0;		 // 清零
	TIM4_Set(0);			   // 关闭定时器4 
}

/*-------------------------- 串口2 printf 函数 --------------------------*/

// 注意：确保一次发送数据不超过USART2_MAX_SEND_LEN字节
/*
	* va_start(ap,fmt) 就找出这个函数在栈中排列的一堆参数的起始地址，然后直接浏览栈中参数，
	* 并用 vsprintf() 实现格式化字符串的读取，
	* 最后 vs_end(ap) 释放ap，就像释放指针一样。
	* void va_start(va_list ap, last_arg);
	* ap -- 这是一个 va_list 类型的对象，它用来存储通过 va_arg 获取额外参数时所必需的信息。
  * last_arg -- 最后一个传递给函数的已知的固定参数。
*/
void u2_printf(char* fmt,...)  {  
	
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF, fmt, ap);
	va_end(ap);
	
	while(DMA1_Channel7->CNDTR!=0);	//等待通道7传输完成   
	UART_DMA_Enable(DMA1_Channel7,strlen((const char*)USART2_TX_BUF)); 	//通过dma发送出去   
}
//串口2发送函数
void Uart2_SendStr(char*SendBuf)
{
    while(*SendBuf)
    {
        while((USART2->SR&0X40)==0);//等待发送完成
        USART2->DR = (u8) *SendBuf;
        SendBuf++;
    }
}


