/*********************************************************************************
 
 * ZXIAT初始化函数及数据发送接受函数
 
**********************************************************************************/	


#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "./gps/gps_config.h"
#include "nmea/nmea.h"
#include "nema_decode_test.h"
#include "bsp_ec20.h"

#include "bsp_led.h"  
#include "mlx90614.h"
#include "OLED_I2C.h"

/******************************************************************************/
#if 1
#define PRODUCTKEY     "a1ut8TU6WN1"
#define DEVICENAME     "test3"
#define DEVICESECRET   "78b98e24256a46f6957349b1376eacc8"
#endif

#if 0
#define PRODUCTKEY     "a1lVSYuMzme"
#define DEVICENAME     "MQTT_test_device01"
#define DEVICESECRET   "848ed9953d79478b48c9c84b47d03b73"
#endif
/******************************************************************************/



#define GPS_PRINTF  1     //是否串口打印定位信息


	double deg_lat;//纬度
	double deg_lon;//经度
	double deg_elv;//转换后的高度
	double deg_speed;
	float body_temperature;
		
		unsigned char m;


/** ------------------ 程序最终在此一直执行------------------- **/
/**
  * @brief  解码GPS模块信息，数据发送接受函数
  * @param  无
  * @retval 无
  */
void nmea_decode_test(void)
{

    nmeaINFO info;          //GPS解码后得到的信息
    nmeaPARSER parser;      //解码时使用的数据结构  
    uint8_t new_parse=0;    //是否有新的解码数据标志
  
    nmeaTIME beiJingTime;    //北京时间 

    /* 设置用于输出调试信息的函数 */
    nmea_property()->trace_func = &trace;
    nmea_property()->error_func = &error;
    nmea_property()->info_func = &gps_info;

    /* 初始化GPS数据结构 */
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);

   while(1)
   {
			/*---------------------乒乓球缓存------------------------*/
      if(GPS_HalfTransferEnd)     /* 接收到GPS_RBUFF_SIZE一半的数据 */
      {
        /* 进行nmea格式解码 */
        nmea_parse(&parser, (const char*)&gps_rbuff[0], HALF_GPS_RBUFF_SIZE, &info);
        
        GPS_HalfTransferEnd = 0;   //清空标志位
        new_parse = 1;             //设置解码消息标志 
      }
      else if(GPS_TransferEnd)    /* 接收到另一半数据 */
      {
        nmea_parse(&parser, (const char*)&gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &info);
       
        GPS_TransferEnd = 0;
        new_parse =1;
      }
      
			/*---------------------------定位数据----------------------------------*/
      if(new_parse )                //有新的解码消息   
      {   
        /* 对解码后的时间进行转换，转换成北京时间 */
        GMTconvert(&info.utc,&beiJingTime,8,1);
        
				deg_lat = nmea_ndeg2degree(info.lat);
				deg_lon = nmea_ndeg2degree(info.lon);
				deg_elv = nmea_ndeg2degree(info.elv);
				deg_speed = nmea_ndeg2degree(info.speed);
				
				/* 串口打印地理位置信息  */
				#if GPS_PRINTF					
				printf("\r\n时间%d-%02d-%02d,%d:%d:%d\r\n", beiJingTime.year+1900, beiJingTime.mon,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
				printf("\r\n纬度：%f,经度%f\r\n",deg_lat,deg_lon);
        printf("\r\n海拔高度：%f 米 ", deg_elv);
        printf("\r\n速度：%f km/h ", info.speed);
        printf("\r\n航向：%f 度", info.direction);				
				printf("\r\n正在使用的GPS卫星：%d,可见GPS卫星：%d",info.satinfo.inuse,info.satinfo.inview);
				printf("\r\n正在使用的北斗卫星：%d,可见北斗卫星：%d",info.BDsatinfo.inuse,info.BDsatinfo.inview);
				printf("\r\nPDOP：%f,HDOP：%f，VDOP：%f",info.PDOP,info.HDOP,info.VDOP);
        #endif
				
				
				new_parse = 0;  //解码消息清零
				
			}
			
			/* -------------------------------发送数据-------------------------------- */
			if(EC20_MQTT_SEND_AUTO((uint8_t *)PRODUCTKEY,(uint8_t *)DEVICENAME)==0)
      {
         Delay_ms(1000);
         check_cmd();    //不存储到串口数组清空数据
         memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
         USART2_RX_STA=0;
					
         Delay_ms(100);
			}
			
			/* -------------------------------接收数据-------------------------------- */		
			if(USART2_RX_STA&0X8000)		
      {
         printf("串口2:收到服务器下发数据");
         printf((const char*)USART2_RX_BUF,"\r\n");
         check_cmd();
         memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
         USART2_RX_STA=0;				
			}	
			
			
			body_temperature = SMBus_ReadTemp();
			printf("温度 = %f℃\r\n",body_temperature);
			if(body_temperature>30){
			OLED_ShowStr(0,5,(unsigned char*)"                    ",2);	
		OLED_Show_num(35,5,body_temperature,2);
		if(body_temperature>37){
		//OLED_Show_num(35,5,man,2);
			OLED_ShowStr(35,7,(unsigned char*)"warning",1);	
		}
		else
				OLED_ShowStr(35,7,(unsigned char*)"       ",1);	
		}
		else{
		for(m=0;m<7;m++)
		{
			OLED_ShowCN3(0+m*16,5,m);									//测试显示中文
		}
	}
		
	}

    /* 释放GPS数据结构 */
    // nmea_parser_destroy(&parser);
		//  return 0;
}







