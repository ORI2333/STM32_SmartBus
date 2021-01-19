/*********************************************************************************
 
 * ����2��ʼ���ļ�
 
**********************************************************************************/	

#include "bsp_usart2.h"
#include "bsp_dma.h"
#include "stdarg.h"	 	  	 
#include "string.h"
#include "bsp_tim.h"


/* ���ڷ��ͻ����� 	*/
uint8_t USART2_TX_BUF[USART2_MAX_SEND_LEN]; 	      //���ͻ���,���USART2_MAX_SEND_LEN�ֽ� 

/* ���ڽ��ջ����� 	*/
uint8_t USART2_RX_BUF[USART2_MAX_RECV_LEN]; 				//���ջ���,���USART2_MAX_RECV_LEN���ֽ�.

	uint8_t Scan_Wtime;
	u16 USART2_RX_STA=0; 

/*------------------------ ���� USARTx �ж����ȼ� -----------------------*/
static void NVIC_Configuration(void){
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);           // �����ж����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART2_IRQ;    // �����ж�Դ 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // ������ռ���ȼ��������ȼ���
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 5;        // ���������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);
	
}

/*-------------------------- USART ��ز������� --------------------------*/
void USART2_Config(void){
	
	/* ��ʼ���ṹ�� */
	GPIO_InitTypeDef   GPIO_InitStructure;
	USART_InitTypeDef  USART_InitStructure;
	
	/*** ��ʼ��������Ҫ�õ���GPIO ***/
	
	/* �򿪴���GPIO��ʱ�� */
	DEBUG_USART2_GPIO_APBxClkCmd(DEBUG_USART2_GPIO_CLK, ENABLE);  
	
	/* ��USART Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART2_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART2_TX_GPIO_PORT, &GPIO_InitStructure);
	
	/* ��USART Rx��GPIO����Ϊ��������ģʽ */
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART2_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART2_RX_GPIO_PORT, &GPIO_InitStructure);
	
	/*** ���� USART �Ĺ������� ***/
	
	/* �򿪴��������ʱ�� */
	DEBUG_USART2_APBxClkCmd(DEBUG_USART2_CLK, ENABLE);  
	
	USART_InitStructure.USART_BaudRate = DEBUG_USART2_BAUDRATE;   	 // ���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  		 // ����һ֡�����ֳ�
	USART_InitStructure.USART_StopBits = USART_StopBits_1;      		 // ����ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;        	     // ����У��λ
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  // ���ù���ģʽ���շ�һ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // ����Ӳ��������
	USART_Init(USART2, &USART_InitStructure);   	             // ��ɴ��ڵĳ�ʼ������
	
	/*** �ж����� ***/
	
	NVIC_Configuration();  // �����ж����ȼ�����
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);   // ʹ�ܴ��ڽ����ж�
	
	/*** ʹ�ܴ��� ***/
	
	USART_Cmd(USART2, ENABLE);
	
	/* ͨ����ʼ�� */
	DMA_USARTx_TX_Config();
	
	/* ��ͨ�� */
	USART_DMACmd(USART2,USART_DMAReq_Tx,ENABLE);
	
	TIM4_Init(99,7199);	 // 10ms�ж� 
	USART2_RX_STA=0;		 // ����
	TIM4_Set(0);			   // �رն�ʱ��4 
}

/*-------------------------- ����2 printf ���� --------------------------*/

// ע�⣺ȷ��һ�η������ݲ�����USART2_MAX_SEND_LEN�ֽ�
/*
	* va_start(ap,fmt) ���ҳ����������ջ�����е�һ�Ѳ�������ʼ��ַ��Ȼ��ֱ�����ջ�в�����
	* ���� vsprintf() ʵ�ָ�ʽ���ַ����Ķ�ȡ��
	* ��� vs_end(ap) �ͷ�ap�������ͷ�ָ��һ����
	* void va_start(va_list ap, last_arg);
	* ap -- ����һ�� va_list ���͵Ķ����������洢ͨ�� va_arg ��ȡ�������ʱ���������Ϣ��
  * last_arg -- ���һ�����ݸ���������֪�Ĺ̶�������
*/
void u2_printf(char* fmt,...)  {  
	
	va_list ap;
	va_start(ap,fmt);
	vsprintf((char*)USART2_TX_BUF, fmt, ap);
	va_end(ap);
	
	while(DMA1_Channel7->CNDTR!=0);	//�ȴ�ͨ��7�������   
	UART_DMA_Enable(DMA1_Channel7,strlen((const char*)USART2_TX_BUF)); 	//ͨ��dma���ͳ�ȥ   
}
//����2���ͺ���
void Uart2_SendStr(char*SendBuf)
{
    while(*SendBuf)
    {
        while((USART2->SR&0X40)==0);//�ȴ��������
        USART2->DR = (u8) *SendBuf;
        SendBuf++;
    }
}


