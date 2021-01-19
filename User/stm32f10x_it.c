/*********************************************************************************
 
 * 中断服务函数文件
 
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

/* 串口接收中断 */
void DEBUG_USART1_IRQHandler(void){
	
	uint8_t Res;
	
	/* 再次确认是否产生中断，避免错误的中断 */
	if(USART_GetFlagStatus(DEBUG_USARTx, USART_IT_RXNE) != RESET) { //接收中断(接收到的数据必须是0x0d 0x0a结尾)
		
		Res = USART_ReceiveData(DEBUG_USARTx); // 将接收到的数据赋值给 res	
		
		if((USART1_RX_STA&0x8000)==0){ //接收未完成 
			
			if(USART1_RX_STA&0x4000){ //接收到了0x0d 
				
				if(Res!=0x0a) USART1_RX_STA=0; //接收错误,重新开始
				
				else USART1_RX_STA |= 0x8000;	//接收完成了 
			}
			
			else {  //还没收到0X0D
				
				if(Res==0x0d) USART1_RX_STA|=0x4000;
				
				else{
					
					USART1_RX_BUF[USART1_RX_STA&0X3FFF]=Res;
					USART1_RX_STA++;
					
					if(USART1_RX_STA>(USART1_REC_LEN-1)) USART1_RX_STA=0; //接收数据错误,重新开始接收	  
				}		 
			}
		}  
		
	}
}

/* 串口二中断函数 */
void DEBUG_USART2_IRQHandler(void) 
{
	uint8_t res;	    
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET){ // 再次确认是否产生中断，避免错误的中断
			 
		res =USART_ReceiveData(USART2);	        // 接收数据
		
		if(USART2_RX_STA<USART2_MAX_RECV_LEN){	// 还可以接收数据
		
			TIM_SetCounter(TIM4,0);               // 计数器清空 
			
			if(USART2_RX_STA==0) TIM4_Set(1);	   	// 使能定时器4的中断  
			
			USART2_RX_BUF[USART2_RX_STA++]=res;		// 记录接收到的值	 
			
		}else {
		
			USART2_RX_STA|=1<<15;					// 强制标记接收完成
		} 
	}  											 
}   

/* 定时器4中断服务程序 */		    
void TIM4_IRQHandler(void)
{ 
	
	if (TIM_GetITStatus(TIM4, TIM_IT_Update) != RESET){  // 再次确认是否产生中断，避免错误的中断		 			   

		TIM_ClearITPendingBit(TIM4, TIM_IT_Update);        // 清除TIMx更新中断标志    

	  USART2_RX_STA|=1<<15;  // 标记接收完成
		
		TIM4_Set(0);		       // 关闭TIM4  
	  
	}   
}
/******* GPS_ProcessDMAIRQ GPS DMA中断服务函数 *******/
/**
  * @brief  GPS_ProcessDMAIRQ GPS DMA中断服务函数
  * @param  None.
  * @retval None.
  */
void GPS_ProcessDMAIRQ(void)
{  
  if(DMA_GetITStatus(GPS_DMA_IT_HT) )         /* DMA 半传输完成 */
  {
    GPS_HalfTransferEnd = 1;                //设置半传输完成标志位
    DMA_ClearFlag(GPS_DMA_FLAG_HT);
  }
  else if(DMA_GetITStatus(GPS_DMA_IT_TC))     /* DMA 传输完成 */
  {
    GPS_TransferEnd = 1;                    //设置传输完成标志位
    DMA_ClearFlag(GPS_DMA_FLAG_TC);

   }
}

void KEY1_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
	{
		// LED1 取反		
		LED1_TOGGLE;
		man++;
		printf("当前车内人数：%d\n",man);
    //清除中断标志位
		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);     
	}  
}

void KEY2_IRQHandler(void)
{
  //确保是否产生了EXTI Line中断
	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
	{
		// LED2 取反		
		LED2_TOGGLE;
		man--;
		printf("当前车内人数：%d\n",man);
    //清除中断标志位
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
