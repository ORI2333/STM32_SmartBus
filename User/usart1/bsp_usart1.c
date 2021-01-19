/*********************************************************************************
 
 * ����1��ʼ���ļ�
 
**********************************************************************************/	


#include "bsp_usart1.h"


uint8_t USART1_RX_BUF[USART1_REC_LEN];     //���ջ���,���USART_REC_LEN���ֽ�.
uint16_t USART1_RX_STA=0;      						 //����״̬���	 


/*------------------------ ���� USARTx �ж����ȼ� -----------------------*/
static void NVIC_Configuration(void){
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);           // �����ж����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;     // �����ж�Դ  
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // ������ռ���ȼ��������ȼ���
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 6;        // ���������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           // ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);
	
}


/*-------------------------- USART ��ز������� --------------------------*/
void USART1_Config(void){
	
	GPIO_InitTypeDef   GPIO_InitStructure;
	USART_InitTypeDef  USART_InitStructure;
	
	/*** ��ʼ��������Ҫ�õ���GPIO ***/
	
	/* �򿪴���GPIO��ʱ�� */
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);  
	
	/* ��USART Tx��GPIO����Ϊ���츴��ģʽ */
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);
	
	/* ��USART Rx��GPIO����Ϊ��������ģʽ */
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	/*** ���� USART �Ĺ������� ***/
	
	/* �򿪴��������ʱ�� */
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);  
	
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;   		 // ���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;  		 // ����һ֡�����ֳ�
	USART_InitStructure.USART_StopBits = USART_StopBits_1;      		 // ����ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;        	     // ����У��λ
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;  // ���ù���ģʽ���շ�һ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // ����Ӳ��������
	USART_Init(DEBUG_USARTx, &USART_InitStructure);   	             // ��ɴ��ڵĳ�ʼ������
	
	/*** �ж����� ***/
	
	NVIC_Configuration();  // �����ж����ȼ�����
	USART_ITConfig(DEBUG_USARTx, USART_IT_RXNE, ENABLE);   // ʹ�ܴ��ڽ����ж�
	
	/*** ʹ�ܴ��� ***/
	USART_Cmd(DEBUG_USARTx, ENABLE);
	
}



/* �ض���c�⺯��printf�����ڣ��ض�����ʹ��printf��putchar���� */
int fputc(int ch, FILE *f){
	
	USART_SendData(DEBUG_USARTx, (uint8_t)ch);  // ����һ���ֽ����ݵ�USART 
	while( USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_TXE) == RESET );  // �ȴ��������ݼĴ���Ϊ��
	
	return (ch);
	
}

/* �ض���c�⺯��scanf�����ڣ���д����ʹ��scanf��getchar�Ⱥ��� */
int fgetc(FILE *f){
	  
		while (USART_GetFlagStatus(DEBUG_USARTx, USART_FLAG_RXNE) == RESET); // �ȴ������������� 

		return (int)USART_ReceiveData(DEBUG_USARTx);
}


