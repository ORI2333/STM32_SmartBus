/*********************************************************************************
 * ��ʾ���ƣ��ǻ۹�����Ӳ���㣩
 * ����    ��������λ����Ϣ����ʪ�ȵ���Ϣͨ��MQTTЭ���Ϸ��������� ��
						�����Խ���Qos�ȼ�Ϊ 0 ��MQTT��Ϣ������ع��ܣ�
						
 * �汾��V3.4
 * ʵ��ƽ̨��STM32F103ZE(�����壩/STM32F103C8T6�����İ壩
 *
 * ����    ��ORI��zzuli_zh)
 * ��ϵ��ʽ��Q1327408725  ����ori_zh@outlook.com
 * ʱ��    ��2020-11 
**********************************************************************************/	

#include "bsp_led.h"
#include "bsp_usart1.h"	
#include "string.h"    
#include "bsp_usart2.h" 
#include "bsp_ec20.h"
#include "bsp_systick.h"
#include "bsp_dma.h"
#include "bsp_tim.h"
#include <stdio.h>
#include <stdlib.h>
#include "./gps/gps_config.h"
#include "nema_decode_test.h"
#include "./dht11/bsp_dht11.h"
#include "bsp_exti.h"
#include "stm32f10x_it.h"	
#include "mlx90614.h"
#include "OLED_I2C.h"

/*�������豸֤����Ϣ

*/
/******************************************************************************/
#if 1
#define PRODUCTKEY     "a1ut8TU6WN1"
#define DEVICENAME     "test3"
#define DEVICESECRET   "78b98e24256a46f6957349b1376eacc8"


#else 

#define PRODUCTKEY     "a1lVSYuMzme"
#define DEVICENAME     "MQTT_test_device01"
#define DEVICESECRET   "848ed9953d79478b48c9c84b47d03b73"
#endif
/******************************************************************************/

int errcont = 0;

int main(void) {
	
    uint8_t res=1;
    errcont = 0;
		unsigned char i;
		unsigned char m;
	
		/*** ��ʼ������ ***/
		SysTick_Init();   			// ��ʼ���ӳٺ���
		USART1_Config();        // ��ʼ��USART1������λ�������ӣ�������
    USART2_Config();        // ��ʼ��USART2����EC20ͨ��
		GPS_Config();     			// ��ʼ��USART3����ZXIATģ�������ӣ���ʼ����λģ���������
    DHT11_Init ();					// ��ʼ��DHT11
		EXTI_Key_Config();			// ��ʼ���������
		LED_GPIO_Config();			// ��ʼ��LED��
		SMBus_Init();						// ��ʼ��MXL90614
		I2C_Configuration();		// ����CPU��Ӳ��I2C
		OLED_Init();    				// ��ʼ��OLED
		
	
	
		printf("\r\n ############    Demo   ############\r\n ");
		printf("\r\n ʵ��ǰ�����Ӻø�ģ�� \r\n ");
		
		
    while(1)
    {
			OLED_Fill(0xFF);//ȫ������
			Delay_ms(1000);		// 2s		
			OLED_Fill(0x00);//ȫ����
			Delay_ms(1000);		// 2s
				for(i=0;i<4;i++)
				{
					OLED_ShowCN(30+i*16,0,i);									//������ʾ����
				}
				
				OLED_ShowStr(0,2,(unsigned char*)"--------------------",1);	
				
				for(m=0;m<8;m++)
				{
					OLED_ShowCN2(0+m*16,3,m);									//������ʾ����
				}
				
		
        while(res)
        {
            res=EC20_INIT();      		//�ȴ�EC20��ʼ�����
            Delay_ms(2000);
            errcont++;
            printf("���ڳ�������: %d��\r\n",errcont);
            if(errcont > 50)
            {
								printf("����:���ӳ�ʱ�����и�λ����\r\n");
                NVIC_SystemReset();		//��ʱ����,���ø�λ����������ǿ�Ƹ�λ
            }
        }
        errcont = 0;
        res=1;
        while(1)
        {
            while(res)
            {
                res=EC20_CONNECT_SERVER_CFG_INFOR((u8 *)PRODUCTKEY,(u8 *)DEVICENAME,(u8 *)DEVICESECRET);   //���밢����
                errcont++;
                printf("���ڳ�������: %d��\r\n",errcont);
                if(errcont > 50)
                {
                    NVIC_SystemReset();	//��ʱ����
                }
            }
            printf("�����Ӱ����ƽӷ�������׼����Ϣ����\r\n");
            while(1)
            {
								nmea_decode_test();      //������Ϣ����״̬

						}
				}

		}
		
}	

