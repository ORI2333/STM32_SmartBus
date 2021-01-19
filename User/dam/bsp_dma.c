/*********************************************************************************
 
 * 串口2DMA设置文件
 
**********************************************************************************/	

#include "bsp_dma.h"
#include "bsp_usart2.h"

//	UART_DMA_Config(DMA1_Channel7,(u32)&USART2->DR,(u32)USART2_TX_BUF);//DMA1通道7,外设为串口2,存储器为USART2_TX_BUF 
void DMA_USARTx_TX_Config(void){
	
	DMA_InitTypeDef  DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(USART_TX_DMA_CLK, ENABLE);                          // 开启DMA时钟
	DMA_DeInit(USART_TX_DMAy_CHANNELx);                                       // 将DMA的通道1寄存器重设为缺省值
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART_DR_ADDRESS;    // 目标地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART2_TX_BUF;           // 源数据地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                        // 方向：存储器到外设
	
	DMA_InitStructure.DMA_BufferSize = 0;                                     // 传输大小	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;          // 外设地址不递增
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                   // 内存地址递增
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;   // 外设数据单位
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;           // 内存数据单位
	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                             // DMA模式，一次模式
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;                       // 优先级：高	
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                              // 不使能内存到内存的传输
	
	DMA_Init(USART_TX_DMAy_CHANNELx, &DMA_InitStructure);                     // 配置DMA通道
	
	DMA_ClearFlag(USART_TX_DMA_FLAG_TC);                                      // 清除DMA数据流传输完成标志位
	
	DMA_Cmd(USART_TX_DMAy_CHANNELx, ENABLE);                                  // 使能DMA
	
}

//开启一次DMA传输
void UART_DMA_Enable(DMA_Channel_TypeDef* DMA_CHx, uint16_t len) {
	
	DMA_Cmd(DMA_CHx, DISABLE );          // 关闭 指示的通道   
	
	DMA_SetCurrDataCounter(DMA_CHx,len); // DMA通道的DMA缓存的大小	
	
	DMA_Cmd(DMA_CHx, ENABLE);            // 开启DMA传输
}	

