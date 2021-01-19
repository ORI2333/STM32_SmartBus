/*********************************************************************************
 
 * 定时器初始化文件
 
**********************************************************************************/	


#include "bsp_tim.h"
#include "bsp_usart2.h"
#include "stdarg.h"	 	 
#include "stdio.h"	 	 
#include "string.h"


//设置TIM4的开关
//sta:0，关闭;   1,开启;
void TIM4_Set(uint8_t sta) {
	
	if(sta) {
       
		TIM_SetCounter(TIM4, 0);  //计数器清空
		TIM_Cmd(TIM4, ENABLE);   //使能TIMx	
		
	} else TIM_Cmd(TIM4, DISABLE);//关闭定时器4
	
}

//配置TIM4预装载周期值
void TIM4_SetARR(uint16_t period) {
	
   TIM_SetCounter(TIM4,0);   // 计数器清空
	 TIM4->ARR &= 0x00;        // 先清预装载周期值为0
	 TIM4->ARR |= period;      // 更新预装载周期值
}

/*------------------------ 配置 USARTx 中断优先级 -----------------------*/
static void NVIC_Configuration(void){
	
	NVIC_InitTypeDef NVIC_InitStructure;
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);          // 配置中断优先级分组
	NVIC_InitStructure.NVIC_IRQChannel = TIM4_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1 ; //抢占优先级1
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;		   //子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			     //IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);
	
}

//通用定时器中断初始化
//这里始终选择为APB1的2倍，而APB1为36M
//arr：自动重装值。
//psc：时钟预分频数		 
void TIM4_Init(uint16_t arr,uint16_t psc) {
	
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //时钟使能//TIM4时钟使能    
	
	//定时器TIM4初始化
	TIM_TimeBaseStructure.TIM_Period = arr;    //设置在下一个更新事件装入活动的自动重装载寄存器周期的值	
	TIM_TimeBaseStructure.TIM_Prescaler = psc; //设置用来作为TIMx时钟频率除数的预分频值
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;      //设置时钟分割:TDTS = Tck_tim
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //TIM向上计数模式
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);              //根据指定的参数初始化TIMx的时间基数单位
 
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //使能指定的TIM4中断,允许更新中断

	NVIC_Configuration();

	
}
