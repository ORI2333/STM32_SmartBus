/*********************************************************************************
 
 * ��ʱ����ʼ���ļ�
 
**********************************************************************************/	


#include "bsp_tim.h"
#include "bsp_usart2.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"


//����TIM4�Ŀ���
//sta:0���ر�;   1,����;
void TIM4_Set(uint8_t sta) {
	
	if(sta) {
       
		TIM_SetCounter(TIM4, 0);  //���������
		TIM_Cmd(TIM4, ENABLE);   //ʹ��TIMx	
		
	} else TIM_Cmd(TIM4, DISABLE);//�رն�ʱ��4
	
}

//����TIM4Ԥװ������ֵ
void TIM4_SetARR(uint16_t period) {
	
   TIM_SetCounter(TIM4,0);   // ���������
	 TIM4->ARR &= 0x00;        // ����Ԥװ������ֵΪ0
	 TIM4->ARR |= period;      // ����Ԥװ������ֵ
}

/*------------------------ ���� USARTx �ж����ȼ� -----------------------*/
static void NVIC_Configuration(void){
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);          // �����ж����ȼ�����
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ; //��ռ���ȼ�1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		   //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			     //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);
	
}

//ͨ�ö�ʱ���жϳ�ʼ��
//����ʼ��ѡ��ΪAPB1��2������APB1Ϊ36M
//arr���Զ���װֵ��
//psc��ʱ��Ԥ��Ƶ��		 
void TIM4_Init(uint16_t arr,uint16_t psc) {
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //ʱ��ʹ��//TIM4ʱ��ʹ��    
	
	//��ʱ��TIM4��ʼ��
	TIM_TimeBaseStructure.TIM_Period = arr;    //��������һ�������¼�װ�����Զ���װ�ؼĴ������ڵ�ֵ	
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //����������ΪTIMxʱ��Ƶ�ʳ�����Ԥ��Ƶֵ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //����ʱ�ӷָ�:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM���ϼ���ģʽ
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);              //����ָ���Ĳ�����ʼ��TIMx��ʱ�������λ
 
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //ʹ��ָ����TIM4�ж�,��������ж�

	NVIC_Configuration();

	
}
