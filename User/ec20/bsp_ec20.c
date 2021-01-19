/*********************************************************************************
 
 * EC20初始化文件
 
**********************************************************************************/	

#include "bsp_ec20.h"
#include "bsp_dht11.h"
#include "nmea/nmea.h"
#include "nema_decode_test.h"
#include "stm32f10x_it.h"	


#define geographical_position 0

char GPSBuf[BUFLEN];  //GPS打印缓存器

uint8_t EC20CSQ[BUFLEN];                //当前信号值
char AtStrBuf[BUFLEN];   								//打印缓存器

/* 地理位置信息定义及声明 */
		enum CoordinateSystem {WGS_84=1,GCJ_02};  // 坐标系统
		extern	double deg_lat;										//纬度
		extern	double deg_lon;										//经度
		extern	double deg_elv;                   //高度
		extern  double deg_speed;
		
		extern  int man;
typedef struct {
	float longitude;  // 经度
	float latitude;   // 纬度
	float altitude;   // 高度
	enum CoordinateSystem xOyS;  // 坐标系统
		
} GPS_TypeDef;

/* 温湿度数据定义及声明 */
		DHT11_Data_TypeDef DHT11_Data;
		double temp, humi;


		
/*
 * EC20发送命令后,检测接收到的应答
 * str:期待的应答结果
 * 返回值：0，没有得到期待的应答结果；
 * 其他，期待应答结果的位置(str的位置)
 */
uint8_t* EC20_check_cmd(uint8_t *str)
{
    char *strx=0;
	
    if(USART2_RX_STA&0X8000)																				//接收到一次数据了
    {
        USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;											//添加结束符
        strx=strstr((const char*)USART2_RX_BUF,(const char*)str);
        printf("ATCMD<-\r\n %s\r\n",USART2_RX_BUF);									//发送命令
    }
    return (uint8_t*)strx;
}

void check_cmd(void)
{
    if(USART2_RX_STA&0X8000)																				//接收到一次数据了
    {
        USART2_RX_BUF[USART2_RX_STA&0X7FFF]=0;											//添加结束符
    }
}




/* 向EC20发送命令
 * cmd:发送的命令字符串(不需要添加回车了),当cmd<0XFF的时候,发送数字(比如发送0X1A),大于的时候发送字符串.
 * ack:期待的应答结果,如果为空,则表示不需要等待应答
 * waittime:等待时间(单位:10ms)
 * 返回值：0, 发送成功(得到了期待的应答结果)
 *         1, 发送失败
 */ 
uint8_t EC20_send_cmd(uint8_t *cmd,uint8_t *ack,uint16_t waittime)
{
    uint8_t res=0;
    USART2_RX_STA=0;
    if((uint32_t)cmd <= 0XFF)
    {
        while(DMA1_Channel7->CNDTR!=0);	//等待通道7传输完成
        USART2->DR=(uint32_t)cmd;
    }else u2_printf("%s\r\n",cmd);      //发送命令
		
    printf("ATCMD->\r\n %s\r\n",cmd);   //发送命令的调试信息
		
    if(ack&&waittime)		  //需要等待应答
    {
        while(--waittime)	//等待倒计时
        {
            Delay_ms(10);
            if(USART2_RX_STA&0X8000)          //接收到期待的应答结果
            {
                if(EC20_check_cmd(ack)) break;//得到有效数据
							
                USART2_RX_STA=0;
            }
        }
        if(waittime==0) res=1;
    }
    USART2_RX_STA=0;
    return res;
}

/* 
	* 测试EC20是否就位
	* 返回值：SIM_OK（成功）
	*         
*/ 
uint8_t EC20_work_test(void)
{
	 
    memset(EC20CSQ,0,BUFLEN); 
	
    if(EC20_send_cmd((uint8_t *)"ATE1",(uint8_t *)"OK",100))
    {
        if(EC20_send_cmd((uint8_t *)"ATE1",(uint8_t *)"OK",100))		return SIM_COMMUNTION_ERR;	//通信不上
    }
    if(EC20_send_cmd((uint8_t *)"AT+CPIN?",(uint8_t *)"READY",400))	return SIM_CPIN_ERR;			//没有SIM卡
	
		
    if(EC20_send_cmd((uint8_t *)"AT+CREG?",(uint8_t *)"0,1",400))														//检测0，1 或者 0，5
    {
        if(strstr((const char*)USART2_RX_BUF,"0,5")==NULL)
        {
            if(!EC20_send_cmd((uint8_t *)"AT+CSQ",(uint8_t *)"OK",200))
            {
                memcpy(EC20CSQ,USART2_RX_BUF+15,2); 
            }
            return SIM_CREG_FAIL;		//等待附着到网络
        }
    }
    return SIM_OK;
		
}


/*
 * 描述  ：检测各个EC20各项功能是否到位
 * 输入  ：无
 * 输出  : res
 * 调用  ：外部调用
 */
uint8_t EC20_INIT(void)
{
    uint8_t res;
    res=EC20_work_test();
    switch(res)
    {
    case SIM_OK:
        printf("4G模块自检成功\r\n");
        break;
    case SIM_COMMUNTION_ERR:
        printf("正在连接到4G模块,请稍等...\r\n");
        break;
    case SIM_CPIN_ERR:
        printf("正在检测到SIM卡,请稍等..\r\n");
        break;
    case SIM_CREG_FAIL:
        printf("注册网络中...\r\n");
        printf("当前信号值：%s",EC20CSQ);
        break;
		
    default:
        break;
    }
    return res;
}


/*
 * 描述  ：配置连入阿里云平台相关信息，订阅topic等
 * 输入  ：PRODUCTKEY DEVICENAME DEVICESECRET （设备证书信息）
 * 输出  : 
 * 调用  ：外部调用
 */
uint8_t EC20_CONNECT_MQTT_SERVER(uint8_t *PRODUCTKEY,uint8_t *DEVICENAME,uint8_t *DEVICESECRET)
{
    if(EC20_send_cmd((uint8_t *)"AT+CGATT?", (uint8_t *)": 1", 100))	   return 1;  // 检测激活PDP
    if(EC20_send_cmd((uint8_t *)"AT+QIACT?", (uint8_t *)"OK", 100))      return 2;  // 检测激活ACT
    if(EC20_send_cmd((uint8_t *)"AT+QIDEACT=1", (uint8_t *)"OK", 100))	 return 3;  // 关闭当前连接
    if(EC20_send_cmd((uint8_t *)"AT+QMTCLOSE=0", NULL, 0))	 return 4;  						// 关闭MQTT客户端
    if(EC20_send_cmd((uint8_t *)"AT+QMTDISC=0", NULL, 0))	   return 5;  					  // 关闭和MQTT服务器的所有连接

    //配置进入阿里云
    memset(AtStrBuf,0,BUFLEN);
		sprintf(AtStrBuf, "AT+QMTCFG=\"ALIAUTH\",0,\"%s\",\"%s\",\"%s\"\r\n",PRODUCTKEY,DEVICENAME,DEVICESECRET);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, (uint8_t *)"OK", 1000))	 return 6;
    
		/*** 打开阿里云的连接，需要比较久的时间 ***/
		/*  当前使用的阿里云服务器地址为华东二区，端口号为1883  */
    if(EC20_send_cmd((uint8_t *)"AT+QMTOPEN=0,\"iot-as-mqtt.cn-shanghai.aliyuncs.com\",1883", (uint8_t *)"+QMTOPEN: 0,0", 3000)) return 7;

    //连接到阿里云设备
    memset(AtStrBuf,0,BUFLEN);
    sprintf(AtStrBuf, "AT+QMTCONN=0,\"%s\"\r\n",DEVICENAME);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, (uint8_t *)"+QMTCONN: 0,0,0", 1000))	 return 8;

    /*** 订阅到阿里云  ***/
		/*  当前订阅的Topic为 /%s/%s/user/get  */
    memset(AtStrBuf,0,BUFLEN);
    sprintf(AtStrBuf, "AT+QMTSUB=0,1,\"/%s/%s/user/get\",0 \r\n",PRODUCTKEY,DEVICENAME);  
    if(EC20_send_cmd((uint8_t *)AtStrBuf, (uint8_t *)"+QMTSUB: 0,1,0,1", 1000))	 return 9;

    printf("设备已经连接到阿里云,准备发送数据 [..]\r\n");
    return 0;
}



uint8_t EC20_CONNECT_SERVER_CFG_INFOR(uint8_t *PRODUCTKEY,uint8_t *DEVICENAME,uint8_t *DEVICESECRET)
{
    uint8_t res;
    res=EC20_CONNECT_MQTT_SERVER(PRODUCTKEY,DEVICENAME,DEVICESECRET);
    return res;
}


/*
 * 描述  ：向服务器发送数据
 * 输入  ：PRODUCTKEY DEVICENAME （设备证书信息）
 * 输出  : 
 * 调用  ：外部调用
 */
uint8_t EC20_MQTT_SEND_AUTO(uint8_t *PRODUCTKEY,uint8_t *DEVICENAME)
{
		
			
		/*----------------------------------------- 发送温湿度 --------------------------------------------------- */
		if( DHT11_Read_TempAndHumidity ( & DHT11_Data ) == SUCCESS)
		{
			printf("\r\n读取DHT11成功!\r\n\r\n湿度为%d.%d ％RH ，温度为 %d.%d℃ \r\n",\
			DHT11_Data.humi_int,DHT11_Data.humi_deci,DHT11_Data.temp_int,DHT11_Data.temp_deci);
		}		
		/* 发送温度 */
    memset(AtStrBuf,0,BUFLEN); //发送数据命令
    sprintf(AtStrBuf, "AT+QMTPUB=0,0,0,0,\"/sys/%s/%s/thing/event/property/post\"\r\n",PRODUCTKEY,DEVICENAME);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, (uint8_t *)">", 100)) return 1;

    temp = DHT11_Data.temp_int*1.0+DHT11_Data.temp_deci*0.1; //温度数值
    memset(AtStrBuf,0,BUFLEN); //发送数据命令
    sprintf(AtStrBuf, "{params:{CurrentTemperature:%.2f}}",temp);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, NULL, 0)) return 2;
    if(EC20_send_cmd((uint8_t *)0x1A, (uint8_t *)"OK", 1500)) return 3;
			
		/* 发送湿度 */
		memset(AtStrBuf,0,BUFLEN); //发送数据命令
    sprintf(AtStrBuf, "AT+QMTPUB=0,0,0,0,\"/sys/%s/%s/thing/event/property/post\"\r\n",PRODUCTKEY,DEVICENAME);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, (uint8_t *)">", 100)) return 1;
			
		humi = DHT11_Data.humi_int*1.0 + DHT11_Data.humi_deci*0.1;//湿度数值
		memset(AtStrBuf,0,BUFLEN); //发送数据命令
    sprintf(AtStrBuf, "{params:{RelativeHumidity:%.2f}}",humi);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, NULL, 0)) return 2;
    if(EC20_send_cmd((uint8_t *)0x1A, (uint8_t *)"OK", 1500)) return 3;
		
		/*--------------------------------------- 发送地理位置信息 -------------------------------------------------*/

		GPS_TypeDef GPS_Structuer;          //地理位置数据结构体
		GPS_Structuer.longitude = deg_lon;  //纬度
		GPS_Structuer.latitude = deg_lat;		//经度
		GPS_Structuer.altitude = deg_elv;		//高度
		GPS_Structuer.xOyS = WGS_84;
		
		memset(AtStrBuf,0,BUFLEN); //发送数据命令
    sprintf(AtStrBuf, "AT+QMTPUB=0,0,0,0,\"/sys/%s/%s/thing/event/property/post\"\r\n",PRODUCTKEY,DEVICENAME);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, (uint8_t *)">", 100))return 1;
		
		memset(AtStrBuf,0,BUFLEN); //发送数据命令
    sprintf(AtStrBuf, "{params:{GeoLocation:{\"altitude\":%.8f,\"CoordinateSystem\":%u,\"latitude\":%.8f,\"longitude\":%.8f}}}",\
		GPS_Structuer.altitude ,GPS_Structuer.xOyS,GPS_Structuer.latitude,GPS_Structuer.longitude);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, NULL, 0))return 2;
    if(EC20_send_cmd((uint8_t *)0x1A, (uint8_t *)"OK", 1500))	return 3;
		
			/*--------------------------------------- 发送当前速度 -------------------------------------------------*/
		/* 发送速度 */
		memset(AtStrBuf,0,BUFLEN); //发送数据命令
    sprintf(AtStrBuf, "AT+QMTPUB=0,0,0,0,\"/sys/%s/%s/thing/event/property/post\"\r\n",PRODUCTKEY,DEVICENAME);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, (uint8_t *)">", 100)) return 1;
			
		
		memset(AtStrBuf,0,BUFLEN); //发送数据命令
    sprintf(AtStrBuf, "{params:{VehSpeed:%.3f}}",deg_speed);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, NULL, 0)) return 2;
    if(EC20_send_cmd((uint8_t *)0x1A, (uint8_t *)"OK", 1500)) return 3;
		
		/*--------------------------------------- 发送当前车内人数 -------------------------------------------------*/
		/* 发送速度 */
		memset(AtStrBuf,0,BUFLEN); //发送数据命令
    sprintf(AtStrBuf, "AT+QMTPUB=0,0,0,0,\"/sys/%s/%s/thing/event/property/post\"\r\n",PRODUCTKEY,DEVICENAME);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, (uint8_t *)">", 100)) return 1;
			
		
		memset(AtStrBuf,0,BUFLEN); //发送数据命令
    sprintf(AtStrBuf, "{params:{mannumber:%d}}",man);
    if(EC20_send_cmd((uint8_t *)AtStrBuf, NULL, 0)) return 2;
    if(EC20_send_cmd((uint8_t *)0x1A, (uint8_t *)"OK", 1500)) return 3;
	
		/*------------------------------------------------END-----------------------------------------------------------*/
    printf("系统数据发送成功  [OK]\r\n");
		
		Delay_ms(100);   //延时3秒
    return 0;
}



