/*********************************************************************************
 
 * 串口1初始化文件
 
**********************************************************************************/	


#include "bsp_usart1.h"


uint8_t USART1_RX_BUF[USART1_REC_LEN];     //接收缓冲,最大USART_REC_LEN个字节.
uint16_t USART1_RX_STA=0;      						 //接收状态标记	 


/*------------------------ 配置 USARTx 中断优先级 -----------------------*/
static void NVIC_Configuration(void){
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);           // 配置中断优先级分组
	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;     // 配置中断源  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 配置抢占优先级（主优先级）
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;        // 配置子优先级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // 使能中断
	NVIC_Init(&NVIC_InitStructure);
	
}


/*-------------------------- USART 相关参数配置 --------------------------*/
void USART1_Config(void){
	
	GPIO_InitTypeDef   GPIO_InitStructure;
	USART_InitTypeDef  USART_InitStructure;
	
	/*** 初始化串口需要用到的GPIO ***/
	
	/* 打开串口GPIO的时钟 */
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);  
	
	/* 将USART Tx的GPIO配置为推挽复用模式 */
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);
	
	/* 将USART Rx的GPIO配置为浮空输入模式 */
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	/*** 配置 USART 的工作参数 ***/
	
	/* 打开串口外设的时钟 */
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);  
	
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;   		 // 配置波特率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  		 // 配置一帧数据字长
	USART_InitStructure.USART_StopBits = USART_StopBits_1;      		 // 配置停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;        	     // 配置校验位
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  // 配置工作模式，收发一起
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // 配置硬件流控制
	USART_Init(DEBUG_USARTx, &USART_InitStructure);   	             // 完成串口的初始化配置
	
	/*** 中断配置 ***/
	
	NVIC_Configuration();  // 串口中断优先级配置
	USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);   // 使能串口接收中断
	
	/*** 使能串口 ***/
	USART_Cmd(DEBUG_USARTx, ENABLE);
	
}



/* 重定向c库函数printf到串口，重定向后可使用printf、putchar函数 */
int fputc(int ch, FILE *f){
	
	USART_SendData(DEBUG_USARTx, (uint8_t)ch);  // 发送一个字节数据到USART 
	while( USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET );  // 等待发送数据寄存器为空
	
	return (ch);
	
}

/* 重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数 */
int fgetc(FILE *f){
	  
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET); // 等待串口输入数据 

		return (int)USART_ReceiveData(DEBUG_USARTx);
}


