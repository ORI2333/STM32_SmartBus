/*********************************************************************************
 
 * ����2DMA�����ļ�
 
**********************************************************************************/	

#include "bsp_dma.h"
#include "bsp_usart2.h"

//	UART_DMA_Config(DMA1_Channel7,(u32)&USART2->DR,(u32)USART2_TX_BUF);//DMA1ͨ��7,����Ϊ����2,�洢��ΪUSART2_TX_BUF 
void DMA_USARTx_TX_Config(void){
	
	DMA_InitTypeDef  DMA_InitStructure;
	
	RCC_AHBPeriphClockCmd(USART_TX_DMA_CLK, ENABLE);                          // ����DMAʱ��
	DMA_DeInit(USART_TX_DMAy_CHANNELx);                                       // ��DMA��ͨ��1�Ĵ�������Ϊȱʡֵ
	
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)USART_DR_ADDRESS;    // Ŀ���ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)USART2_TX_BUF;           // Դ���ݵ�ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                        // ���򣺴洢��������
	
	DMA_InitStructure.DMA_BufferSize = 0;                                     // �����С	
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;          // �����ַ������
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;                   // �ڴ��ַ����
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;   // �������ݵ�λ
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;           // �ڴ����ݵ�λ
	
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                             // DMAģʽ��һ��ģʽ
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;                       // ���ȼ�����	
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                              // ��ʹ���ڴ浽�ڴ�Ĵ���
	
	DMA_Init(USART_TX_DMAy_CHANNELx, &DMA_InitStructure);                     // ����DMAͨ��
	
	DMA_ClearFlag(USART_TX_DMA_FLAG_TC);                                      // ���DMA������������ɱ�־λ
	
	DMA_Cmd(USART_TX_DMAy_CHANNELx, ENABLE);                                  // ʹ��DMA
	
}

//����һ��DMA����
void UART_DMA_Enable(DMA_Channel_TypeDef* DMA_CHx, uint16_t len) {
	
	DMA_Cmd(DMA_CHx, DISABLE );          // �ر� ָʾ��ͨ��   
	
	DMA_SetCurrDataCounter(DMA_CHx,len); // DMAͨ����DMA����Ĵ�С	
	
	DMA_Cmd(DMA_CHx, ENABLE);            // ����DMA����
}	

