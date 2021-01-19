/*********************************************************************************
 
 * �жϷ������ļ�
 
**********************************************************************************/	


/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"	
#include "bsp_usart1.h"
#include "bsp_systick.h"
#include "bsp_usart2.h"
#include "bsp_tim.h"
#include "./gps/gps_config.h"
#include "bsp_exti.h"
#include "bsp_led.h"


int man = 0;
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

void SysTick_Handler(void)
{
	TimingDelay_Decrement();
}

/* ���ڽ����ж� */
void DEBUG_USART1_IRQHandler(void){
	
	uint8_t Res;
	
	/* �ٴ�ȷ���Ƿ�����жϣ����������ж� */
	if(USART_GetFlagStatus(DEBUG_USARTx, USART_IT_RXNE) != RESET) { //�����ж�(���յ������ݱ�����0x0d 0x0a��β)
		
		Res = USART_ReceiveData(DEBUG_USARTx); // �����յ������ݸ�ֵ�� res	
		
		if((USART1_RX_STA&0x8000)==0){ //����δ��� 
			
			if(USART1_RX_STA&0x4000){ //���յ���0x0d 
				
				if(Res!=0x0a) USART1_RX_STA=0; //���մ���,���¿�ʼ
				
				else USART1_RX_STA |= 0x8000;	//��������� 
			}
			
			else {  //��û�յ�0X0D
				
				if(Res==0x0d) USART1_RX_STA|=0x4000;
				
				else{
					
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res;
					USART1_RX_STA++;
					
					if(USART1_RX_STA>(USART1_REC_LEN-1)) USART1_RX_STA=0; //�������ݴ���,���¿�ʼ����	  
				}		 
			}
		}  
		
	}
}

/* ���ڶ��жϺ��� */
void DEBUG_USART2_IRQHandler(void) 
{
	uint8_t res;	    
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){ // �ٴ�ȷ���Ƿ�����жϣ����������ж�
			 
		res =USART_ReceiveData(USART2);	        // ��������
		
		if(USART2_RX_STA<USART2_MAX_RECV_LEN){	// �����Խ�������
		
			TIM_SetCounter(TIM4,0);               // ��������� 
			
			if(USART2_RX_STA==0) TIM4_Set(1);	   	// ʹ�ܶ�ʱ��4���ж�  
			
			USART2_RX_BUF[USART2_RX_STA++]=res;		// ��¼���յ���ֵ	 
			
		}else {
		
			USART2_RX_STA|=1<<15;					// ǿ�Ʊ�ǽ������
		} 
	}  											 
}   

/* ��ʱ��4�жϷ������ */		    
void TIM4_IRQHandler(void)
{ 
	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET){  // �ٴ�ȷ���Ƿ�����жϣ����������ж�		 			   

		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);        // ���TIMx�����жϱ�־    

	  USART2_RX_STA|=1<<15;  // ��ǽ������
		
		TIM4_Set(0);		       // �ر�TIM4  
	  
	}   
}
/******* GPS_ProcessDMAIRQ GPS DMA�жϷ����� *******/
/**
  * @brief  GPS_ProcessDMAIRQ GPS DMA�жϷ�����
  * @param  None.
  * @retval None.
  */
void GPS_ProcessDMAIRQ(void)
{  
  if(DMA_GetITStatus(GPS_DMA_IT_HT) )         /* DMA �봫����� */
  {
    GPS_HalfTransferEnd = 1;                //���ð봫����ɱ�־λ
    DMA_ClearFlag(GPS_DMA_FLAG_HT);
  }
  else if(DMA_GetITStatus(GPS_DMA_IT_TC))     /* DMA ������� */
  {
    GPS_TransferEnd = 1;                    //���ô�����ɱ�־λ
    DMA_ClearFlag(GPS_DMA_FLAG_TC);

   }
}

void KEY1_IRQHandler(void)
{
  //ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{
		// LED1 ȡ��		
		LED1_TOGGLE;
		man++;
		printf("��ǰ����������%d\n",man);
    //����жϱ�־λ
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);     
	}  
}

void KEY2_IRQHandler(void)
{
  //ȷ���Ƿ������EXTI Line�ж�
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
		// LED2 ȡ��		
		LED2_TOGGLE;
		man--;
		printf("��ǰ����������%d\n",man);
    //����жϱ�־λ
		EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);     
	}  
}

/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

void GPS_DMA_IRQHANDLER(void)
{
  
  GPS_ProcessDMAIRQ();

}

/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
