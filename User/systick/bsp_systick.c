/*********************************************************************************
 
 * ϵͳʱ�ӳ�ʼ���ļ�
 
**********************************************************************************/	

#include "bsp_systick.h"
#include "bsp_led.h"

/**************   ��ѯ�ķ���ʵ����ʱ   **************/

void SysTick_Delay_us(uint32_t us){
	
	uint32_t i;
	SysTick_Config(72); // �ж�����
	
	/* һ�β�ѯ�ɹ��� 1 ΢���ѯ us �� */
	for(i=0;i<us;i++){ 
		/* �ȴ� COUNTFLAG ��1����һ�β�ѯ���Ҷ�ȡ��λ��ֵ����0 */
		while( !(SysTick->CTRL & (1<<16)) ); 
	}
	
	/* �ر��ж� */
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

void SysTick_Delay_ms(uint32_t ms){
	
	uint32_t i;
	SysTick_Config(72000); // �ж�����
	
	/* һ�β�ѯ�ɹ��� 1 ΢���ѯ us �� */
	for(i=0;i<ms;i++){
		/* �ȴ� COUNTFLAG ��1����һ�β�ѯ���Ҷ�ȡ��λ��ֵ����0 */
		while( !(SysTick->CTRL & (1<<16)) );
	}
	
	/* �ر��ж� */
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/*********              END               *********/

/*------------------------------------------------*/

/**************   �жϵķ���ʵ����ʱ   **************/

static uint32_t TimingDelay; // ����һ��ȫ�־�̬������ʵ����ʱ

/* ����ϵͳ�δ�ʱ�� SysTick */
void SysTick_Init(void){
	
	/* �������1��������ʧ�ܣ�������ѭ�� */ 
	/* @return  1 = failed, 0 = successful*/
	if(SysTick_Config(72000)){
		while(1); 
	}
}

/* ms����ʱ���� */
void Delay_ms(uint32_t ms){
	
	TimingDelay = ms;
	
	SysTick_Init(); // ����ϵͳ�δ�ʱ�� SysTick
	
	/* �ȴ��ж���ʱ���� */
	while(TimingDelay != 0);
	
	/* �ر��ж� */
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/* �ú��������� SysTick �жϺ��� SysTick_Handler() �е���
 * ʵ���ж�һ�� TimingDelay ��ֵ��һ
*/
void TimingDelay_Decrement(void){
	/**/
	if(TimingDelay != 0){
		TimingDelay--;
	}
}

/*********              END               *********/
