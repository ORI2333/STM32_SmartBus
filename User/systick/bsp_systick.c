/*********************************************************************************
 
 * 系统时钟初始化文件
 
**********************************************************************************/	

#include "bsp_systick.h"
#include "bsp_led.h"

/**************   查询的方法实现延时   **************/

void SysTick_Delay_us(uint32_t us){
	
	uint32_t i;
	SysTick_Config(72); // 中断配置
	
	/* 一次查询成功是 1 微妙，查询 us 次 */
	for(i=0;i<us;i++){ 
		/* 等待 COUNTFLAG 置1，即一次查询，且读取该位的值后清0 */
		while( !(SysTick->CTRL & (1<<16)) ); 
	}
	
	/* 关闭中断 */
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Delay_ms(uint32_t ms){
	
	uint32_t i;
	SysTick_Config(72000); // 中断配置
	
	/* 一次查询成功是 1 微妙，查询 us 次 */
	for(i=0;i<ms;i++){
		/* 等待 COUNTFLAG 置1，即一次查询，且读取该位的值后清0 */
		while( !(SysTick->CTRL & (1<<16)) );
	}
	
	/* 关闭中断 */
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/*********              END               *********/

/*------------------------------------------------*/

/**************   中断的方法实现延时   **************/

static uint32_t TimingDelay; // 定义一个全局静态变量，实现延时

/* 启动系统滴答定时器 SysTick */
void SysTick_Init(void){
	
	/* 如果返回1，即配置失败，陷入死循环 */ 
	/* @return  1 = failed, 0 = successful*/
	if(SysTick_Config(72000)){
		while(1); 
	}
}

/* ms级延时函数 */
void Delay_ms(uint32_t ms){
	
	TimingDelay = ms;
	
	SysTick_Init(); // 启动系统滴答定时器 SysTick
	
	/* 等待中断延时结束 */
	while(TimingDelay != 0);
	
	/* 关闭中断 */
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/* 该函数用于在 SysTick 中断函数 SysTick_Handler() 中调用
 * 实现中断一次 TimingDelay 数值减一
*/
void TimingDelay_Decrement(void){
	/**/
	if(TimingDelay != 0){
		TimingDelay--;
	}
}

/*********              END               *********/
