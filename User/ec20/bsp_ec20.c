/*********************************************************************************
 
 * EC20��ʼ���ļ�
 
**********************************************************************************/	

#include "bsp_ec20.h"
#include "bsp_dht11.h"
#include "nmea/nmea.h"
#include "nema_decode_test.h"
#include "stm32f10x_it.h"	


#define geographical_position 0

char GPSBuf[BUFLEN];  //GPS��ӡ������

uint8_t EC20CSQ[BUFLEN];                //��ǰ�ź�ֵ
char AtStrBuf[BUFLEN];   								//��ӡ������

/* ����λ����Ϣ���弰���� */
		enum CoordinateSystem {WGS_84=1,GCJ_02};  // ����ϵͳ
		extern	double deg_lat;										//γ��
		extern	double deg_lon;										//����
		extern	double deg_elv;                   //�߶�
		extern  double deg_speed;
		
		extern  int man;
typedef struct {
	float longitude;  // ����
	float latitude;   // γ��
	float altitude;   // �߶�
	enum CoordinateSystem xOyS;  // ����ϵͳ
		
} GPS_TypeDef;

/* ��ʪ�����ݶ��弰���� */
		DHT11_Data_TypeDef DHT11_Data;
		double temp, humi;


		
/*
 * EC20���������,�����յ���Ӧ��
 * str:�ڴ���Ӧ����
 * ����ֵ��0��û�еõ��ڴ���Ӧ������
 * �������ڴ�Ӧ������λ��(str��λ��)
 */
uint8_t* EC20_check_cmd(uint8_t *str)
{
    char *strx=0;
	
    if(USART2_RX_STA&0X8000)																				//���յ�һ��������
    {
        USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;											//��ӽ�����
        strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
        printf("ATCMD<-\r\n %s\r\n",USART2_RX_BUF);									//��������
    }
    return (uint8_t*)strx;
}

void check_cmd(void)
{
    if(USART2_RX_STA&0X8000)																				//���յ�һ��������
    {
        USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;											//��ӽ�����
    }
}




/* ��EC20��������
 * cmd:���͵������ַ���(����Ҫ��ӻس���),��cmd<0XFF��ʱ��,��������(���緢��0X1A),���ڵ�ʱ�����ַ���.
 * ack:�ڴ���Ӧ����,���Ϊ��,���ʾ����Ҫ�ȴ�Ӧ��
 * waittime:�ȴ�ʱ��(��λ:10ms)
 * ����ֵ��0, ���ͳɹ�(�õ����ڴ���Ӧ����)
 *         1, ����ʧ��
 */ 
uint8_t EC20_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime)
{
    uint8_t res=0;
    USART2_RX_STA=0;
    if((uint32_t)cmd <= 0XFF)
    {
        while(DMA1_Channel7->CNDTR!=0);	//�ȴ�ͨ��7�������
        USART2->DR=(uint32_t)cmd;
    }else u2_printf("%s\r\n",cmd);      //��������
		
    printf("ATCMD->\r\n %s\r\n",cmd);   //��������ĵ�����Ϣ
		
    if(ack&&waittime)		  //��Ҫ�ȴ�Ӧ��
    {
        while(--waittime)	//�ȴ�����ʱ
        {
            Delay_ms(10);
            if(USART2_RX_STA&0X8000)          //���յ��ڴ���Ӧ����
            {
                if(EC20_check_cmd(ack)) break;//�õ���Ч����
							
                USART2_RX_STA=0;
            }
        }
        if(waittime==0) res=1;
    }
    USART2_RX_STA=0;
    return res;
}

/* 
	* ����EC20�Ƿ��λ
	* ����ֵ��SIM_OK���ɹ���
	*         
*/ 
uint8_t EC20_work_test(void)
{
	 
    memset(EC20CSQ,0,BUFLEN); 
	
    if(EC20_send_cmd((uint8_t *)"ATE1",(uint8_t *)"OK",100))
    {
        if(EC20_send_cmd((uint8_t *)"ATE1",(uint8_t *)"OK",100))		return SIM_COMMUNTION_ERR;	//ͨ�Ų���
    }
    if(EC20_send_cmd((uint8_t *)"AT+CPIN?",(uint8_t *)"READY",400))	return SIM_CPIN_ERR;			//û��SIM��
	
		
    if(EC20_send_cmd((uint8_t *)"AT+CREG?",(uint8_t *)"0,1",400))														//���0��1 ���� 0��5
    {
        if(strstr((const char*)USART2_RX_BUF,"0,5")==NULL)
        {
            if(!EC20_send_cmd((uint8_t *)"AT+CSQ",(uint8_t *)"OK",200))
            {
                memcpy(EC20CSQ,USART2_RX_BUF+15,2); 
            }
            return SIM_CREG_FAIL;		//�ȴ����ŵ�����
        }
    }
    return SIM_OK;
		
}


/*
 * ����  ��������EC20������Ƿ�λ
 * ����  ����
 * ���  : res
 * ����  ���ⲿ����
 */
uint8_t EC20_INIT(void)
{
    uint8_t res;
    res=EC20_work_test();
    switch(res)
    {
    case SIM_OK:
        printf("4Gģ���Լ�ɹ�\r\n");
        break;
    case SIM_COMMUNTION_ERR:
        printf("�������ӵ�4Gģ��,���Ե�...\r\n");
        break;
    case SIM_CPIN_ERR:
        printf("���ڼ�⵽SIM��,���Ե�..\r\n");
        break;
    case SIM_CREG_FAIL:
        printf("ע��������...\r\n");
        printf("��ǰ�ź�ֵ��%s",EC20CSQ);
        break;
		
    default:
        break;
    }
    return res;
}


/*
 * ����  ���������밢����ƽ̨�����Ϣ������topic��
 * ����  ��PRODUCTKEY DEVICENAME DEVICESECRET ���豸֤����Ϣ��
 * ���  : 
 * ����  ���ⲿ����
 */
uint8_t EC20_CONNECT_MQTT_SERVER(uint8_t *PRODUCTKEY,uint8_t *DEVICENAME,uint8_t *DEVICESECRET)
{
    if(EC20_send_cmd((uint8_t *)"AT+CGATT?", (uint8_t *)": 1", 100))	   return 1;  // ��⼤��PDP
    if(EC20_send_cmd((uint8_t *)"AT+QIACT?", (uint8_t *)"OK", 100))      return 2;  // ��⼤��ACT
    if(EC20_send_cmd((uint8_t *)"AT+QIDEACT=1", (uint8_t *)"OK", 100))	 return 3;  // �رյ�ǰ����
    if(EC20_send_cmd((uint8_t *)"AT+QMTCLOSE=0", NULL, 0))	 return 4;  						// �ر�MQTT�ͻ���
    if(EC20_send_cmd((uint8_t *)"AT+QMTDISC=0", NULL, 0))	   return 5;  					  // �رպ�MQTT����������������

    //���ý��밢����
    memset(AtStrBuf,0,BUFLEN);
		sprintf(AtStrBuf, "AT+QMTCFG=\"ALIAUTH\",0,\"%s\",\"%s\",\"%s\"\r\n",PRODUCTKEY,DEVICENAME,DEVICESECRET);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, (uint8_t *)"OK", 1000))	 return 6;
    
		/*** �򿪰����Ƶ����ӣ���Ҫ�ȽϾõ�ʱ�� ***/
		/*  ��ǰʹ�õİ����Ʒ�������ַΪ�����������˿ں�Ϊ1883  */
    if(EC20_send_cmd((uint8_t *)"AT+QMTOPEN=0,\"iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883", (uint8_t *)"+QMTOPEN: 0,0", 3000)) return 7;

    //���ӵ��������豸
    memset(AtStrBuf,0,BUFLEN);
    sprintf(AtStrBuf, "AT+QMTCONN=0,\"%s\"\r\n",DEVICENAME);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, (uint8_t *)"+QMTCONN: 0,0,0", 1000))	 return 8;

    /*** ���ĵ�������  ***/
		/*  ��ǰ���ĵ�TopicΪ /%s/%s/user/get  */
    memset(AtStrBuf,0,BUFLEN);
    sprintf(AtStrBuf, "AT+QMTSUB=0,1,\"/%s/%s/user/get\",0 \r\n",PRODUCTKEY,DEVICENAME);  
    if(EC20_send_cmd((uint8_t *)AtStrBuf, (uint8_t *)"+QMTSUB: 0,1,0,1", 1000))	 return 9;

    printf("�豸�Ѿ����ӵ�������,׼���������� [..]\r\n");
    return 0;
}



uint8_t EC20_CONNECT_SERVER_CFG_INFOR(uint8_t *PRODUCTKEY,uint8_t *DEVICENAME,uint8_t *DEVICESECRET)
{
    uint8_t res;
    res=EC20_CONNECT_MQTT_SERVER(PRODUCTKEY,DEVICENAME,DEVICESECRET);
    return res;
}


/*
 * ����  �����������������
 * ����  ��PRODUCTKEY DEVICENAME ���豸֤����Ϣ��
 * ���  : 
 * ����  ���ⲿ����
 */
uint8_t EC20_MQTT_SEND_AUTO(uint8_t *PRODUCTKEY,uint8_t *DEVICENAME)
{
		
			
		/*----------------------------------------- ������ʪ�� --------------------------------------------------- */
		if( DHT11_Read_TempAndHumidity ( & DHT11_Data ) == SUCCESS)
		{
			printf("\r\n��ȡDHT11�ɹ�!\r\n\r\nʪ��Ϊ%d.%d ��RH ���¶�Ϊ %d.%d�� \r\n",\
			DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
		}		
		/* �����¶� */
    memset(AtStrBuf,0,BUFLEN); //������������
    sprintf(AtStrBuf, "AT+QMTPUB=0,0,0,0,\"/sys/%s/%s/thing/event/property/post\"\r\n",PRODUCTKEY,DEVICENAME);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, (uint8_t *)">", 100)) return 1;

    temp = DHT11_Data.temp_int*1.0+DHT11_Data.temp_deci*0.1; //�¶���ֵ
    memset(AtStrBuf,0,BUFLEN); //������������
    sprintf(AtStrBuf, "{params:{CurrentTemperature:%.2f}}",temp);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, NULL, 0)) return 2;
    if(EC20_send_cmd((uint8_t *)0x1A, (uint8_t *)"OK", 1500)) return 3;
			
		/* ����ʪ�� */
		memset(AtStrBuf,0,BUFLEN); //������������
    sprintf(AtStrBuf, "AT+QMTPUB=0,0,0,0,\"/sys/%s/%s/thing/event/property/post\"\r\n",PRODUCTKEY,DEVICENAME);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, (uint8_t *)">", 100)) return 1;
			
		humi = DHT11_Data.humi_int*1.0 + DHT11_Data.humi_deci*0.1;//ʪ����ֵ
		memset(AtStrBuf,0,BUFLEN); //������������
    sprintf(AtStrBuf, "{params:{RelativeHumidity:%.2f}}",humi);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, NULL, 0)) return 2;
    if(EC20_send_cmd((uint8_t *)0x1A, (uint8_t *)"OK", 1500)) return 3;
		
		/*--------------------------------------- ���͵���λ����Ϣ -------------------------------------------------*/

		GPS_TypeDef GPS_Structuer;          //����λ�����ݽṹ��
		GPS_Structuer.longitude = deg_lon;  //γ��
		GPS_Structuer.latitude = deg_lat;		//����
		GPS_Structuer.altitude = deg_elv;		//�߶�
		GPS_Structuer.xOyS = WGS_84;
		
		memset(AtStrBuf,0,BUFLEN); //������������
    sprintf(AtStrBuf, "AT+QMTPUB=0,0,0,0,\"/sys/%s/%s/thing/event/property/post\"\r\n",PRODUCTKEY,DEVICENAME);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, (uint8_t *)">", 100))return 1;
		
		memset(AtStrBuf,0,BUFLEN); //������������
    sprintf(AtStrBuf, "{params:{GeoLocation:{\"altitude\":%.8f,\"CoordinateSystem\":%u,\"latitude\":%.8f,\"longitude\":%.8f}}}",\
		GPS_Structuer.altitude ,GPS_Structuer.xOyS,GPS_Structuer.latitude,GPS_Structuer.longitude);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, NULL, 0))return 2;
    if(EC20_send_cmd((uint8_t *)0x1A, (uint8_t *)"OK", 1500))	return 3;
		
			/*--------------------------------------- ���͵�ǰ�ٶ� -------------------------------------------------*/
		/* �����ٶ� */
		memset(AtStrBuf,0,BUFLEN); //������������
    sprintf(AtStrBuf, "AT+QMTPUB=0,0,0,0,\"/sys/%s/%s/thing/event/property/post\"\r\n",PRODUCTKEY,DEVICENAME);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, (uint8_t *)">", 100)) return 1;
			
		
		memset(AtStrBuf,0,BUFLEN); //������������
    sprintf(AtStrBuf, "{params:{VehSpeed:%.3f}}",deg_speed);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, NULL, 0)) return 2;
    if(EC20_send_cmd((uint8_t *)0x1A, (uint8_t *)"OK", 1500)) return 3;
		
		/*--------------------------------------- ���͵�ǰ�������� -------------------------------------------------*/
		/* �����ٶ� */
		memset(AtStrBuf,0,BUFLEN); //������������
    sprintf(AtStrBuf, "AT+QMTPUB=0,0,0,0,\"/sys/%s/%s/thing/event/property/post\"\r\n",PRODUCTKEY,DEVICENAME);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, (uint8_t *)">", 100)) return 1;
			
		
		memset(AtStrBuf,0,BUFLEN); //������������
    sprintf(AtStrBuf, "{params:{mannumber:%d}}",man);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, NULL, 0)) return 2;
    if(EC20_send_cmd((uint8_t *)0x1A, (uint8_t *)"OK", 1500)) return 3;
	
		/*------------------------------------------------END-----------------------------------------------------------*/
    printf("ϵͳ���ݷ��ͳɹ�  [OK]\r\n");
		
		Delay_ms(100);   //��ʱ3��
    return 0;
}



