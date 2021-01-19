/*********************************************************************************
 
 * ZXIAT��ʼ�����������ݷ��ͽ��ܺ���
 
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



#define GPS_PRINTF  1     //�Ƿ񴮿ڴ�ӡ��λ��Ϣ


	double deg_lat;//γ��
	double deg_lon;//����
	double deg_elv;//ת����ĸ߶�
	double deg_speed;
	float body_temperature;
		
		unsigned char m;


/** ------------------ ���������ڴ�һֱִ��------------------- **/
/**
  * @brief  ����GPSģ����Ϣ�����ݷ��ͽ��ܺ���
  * @param  ��
  * @retval ��
  */
void nmea_decode_test(void)
{

    nmeaINFO info;          //GPS�����õ�����Ϣ
    nmeaPARSER parser;      //����ʱʹ�õ����ݽṹ  
    uint8_t new_parse=0;    //�Ƿ����µĽ������ݱ�־
  
    nmeaTIME beiJingTime;    //����ʱ�� 

    /* �����������������Ϣ�ĺ��� */
    nmea_property()->trace_func = &trace;
    nmea_property()->error_func = &error;
    nmea_property()->info_func = &gps_info;

    /* ��ʼ��GPS���ݽṹ */
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);

   while(1)
   {
			/*---------------------ƹ���򻺴�------------------------*/
      if(GPS_HalfTransferEnd)     /* ���յ�GPS_RBUFF_SIZEһ������� */
      {
        /* ����nmea��ʽ���� */
        nmea_parse(&parser, (const char*)&gps_rbuff[0], HALF_GPS_RBUFF_SIZE, &info);
        
        GPS_HalfTransferEnd = 0;   //��ձ�־λ
        new_parse = 1;             //���ý�����Ϣ��־ 
      }
      else if(GPS_TransferEnd)    /* ���յ���һ������ */
      {
        nmea_parse(&parser, (const char*)&gps_rbuff[HALF_GPS_RBUFF_SIZE], HALF_GPS_RBUFF_SIZE, &info);
       
        GPS_TransferEnd = 0;
        new_parse =1;
      }
      
			/*---------------------------��λ����----------------------------------*/
      if(new_parse )                //���µĽ�����Ϣ   
      {   
        /* �Խ�����ʱ�����ת����ת���ɱ���ʱ�� */
        GMTconvert(&info.utc,&beiJingTime,8,1);
        
				deg_lat = nmea_ndeg2degree(info.lat);
				deg_lon = nmea_ndeg2degree(info.lon);
				deg_elv = nmea_ndeg2degree(info.elv);
				deg_speed = nmea_ndeg2degree(info.speed);
				
				/* ���ڴ�ӡ����λ����Ϣ  */
				#if GPS_PRINTF					
				printf("\r\nʱ��%d-%02d-%02d,%d:%d:%d\r\n", beiJingTime.year+1900, beiJingTime.mon,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
				printf("\r\nγ�ȣ�%f,����%f\r\n",deg_lat,deg_lon);
        printf("\r\n���θ߶ȣ�%f �� ", deg_elv);
        printf("\r\n�ٶȣ�%f km/h ", info.speed);
        printf("\r\n����%f ��", info.direction);				
				printf("\r\n����ʹ�õ�GPS���ǣ�%d,�ɼ�GPS���ǣ�%d",info.satinfo.inuse,info.satinfo.inview);
				printf("\r\n����ʹ�õı������ǣ�%d,�ɼ��������ǣ�%d",info.BDsatinfo.inuse,info.BDsatinfo.inview);
				printf("\r\nPDOP��%f,HDOP��%f��VDOP��%f",info.PDOP,info.HDOP,info.VDOP);
        #endif
				
				
				new_parse = 0;  //������Ϣ����
				
			}
			
			/* -------------------------------��������-------------------------------- */
			if(EC20_MQTT_SEND_AUTO((uint8_t *)PRODUCTKEY,(uint8_t *)DEVICENAME)==0)
      {
         Delay_ms(1000);
         check_cmd();    //���洢�����������������
         memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
         USART2_RX_STA=0;
					
         Delay_ms(100);
			}
			
			/* -------------------------------��������-------------------------------- */		
			if(USART2_RX_STA&0X8000)		
      {
         printf("����2:�յ��������·�����");
         printf((const char*)USART2_RX_BUF,"\r\n");
         check_cmd();
         memset(USART2_RX_BUF, 0, sizeof(USART2_RX_BUF));
         USART2_RX_STA=0;				
			}	
			
			
			body_temperature = SMBus_ReadTemp();
			printf("�¶� = %f��\r\n",body_temperature);
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
			OLED_ShowCN3(0+m*16,5,m);									//������ʾ����
		}
	}
		
	}

    /* �ͷ�GPS���ݽṹ */
    // nmea_parser_destroy(&parser);
		//  return 0;
}







