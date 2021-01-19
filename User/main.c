/*********************************************************************************
 * 演示名称：智慧公交（硬件层）
 * 描述    ：将地理位置信息，温湿度等信息通过MQTT协议上发到阿里云 ；
						（可以接受Qos等级为 0 的MQTT消息，无相关功能）
						
 * 版本：V3.4
 * 实验平台：STM32F103ZE(开发板）/STM32F103C8T6（核心板）
 *
 * 作者    ：ORI（zzuli_zh)
 * 联系方式：Q1327408725  邮箱ori_zh@outlook.com
 * 时间    ：2020-11 
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

/*阿里云设备证书信息

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
	
		/*** 初始化配置 ***/
		SysTick_Init();   			// 初始化延迟函数
		USART1_Config();        // 初始化USART1，与上位机相连接，调试用
    USART2_Config();        // 初始化USART2，与EC20通信
		GPS_Config();     			// 初始化USART3，与ZXIAT模块相连接，初始化定位模块相关配置
    DHT11_Init ();					// 初始化DHT11
		EXTI_Key_Config();			// 初始化红外对射
		LED_GPIO_Config();			// 初始化LED灯
		SMBus_Init();						// 初始化MXL90614
		I2C_Configuration();		// 配置CPU的硬件I2C
		OLED_Init();    				// 初始化OLED
		
	
	
		printf("\r\n ############    Demo   ############\r\n ");
		printf("\r\n 实验前请连接好各模块 \r\n ");
		
		
    while(1)
    {
			OLED_Fill(0xFF);//全屏点亮
			Delay_ms(1000);		// 2s		
			OLED_Fill(0x00);//全屏灭
			Delay_ms(1000);		// 2s
				for(i=0;i<4;i++)
				{
					OLED_ShowCN(30+i*16,0,i);									//测试显示中文
				}
				
				OLED_ShowStr(0,2,(unsigned char*)"--------------------",1);	
				
				for(m=0;m<8;m++)
				{
					OLED_ShowCN2(0+m*16,3,m);									//测试显示中文
				}
				
		
        while(res)
        {
            res=EC20_INIT();      		//等待EC20初始化完成
            Delay_ms(2000);
            errcont++;
            printf("正在尝试连接: %d次\r\n",errcont);
            if(errcont > 50)
            {
								printf("错误:连接超时，进行复位处理\r\n");
                NVIC_SystemReset();		//超时重启,调用复位函数，进行强制复位
            }
        }
        errcont = 0;
        res=1;
        while(1)
        {
            while(res)
            {
                res=EC20_CONNECT_SERVER_CFG_INFOR((u8 *)PRODUCTKEY,(u8 *)DEVICENAME,(u8 *)DEVICESECRET);   //接入阿里云
                errcont++;
                printf("正在尝试连接: %d次\r\n",errcont);
                if(errcont > 50)
                {
                    NVIC_SystemReset();	//超时重启
                }
            }
            printf("已连接阿里云接服务器，准备消息发送\r\n");
            while(1)
            {
								nmea_decode_test();      //进入消息发送状态

						}
				}

		}
		
}	

