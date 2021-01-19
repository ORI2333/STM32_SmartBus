/*********************************************************************************
 
 * ����3��ʼ���ļ�
 * ZXIAT��ʼ���ļ�
 
**********************************************************************************/	

#include "./gps/gps_config.h"
#include "nmea/nmea.h"
#include "bsp_usart1.h"


/* DMA���ջ���  */
uint8_t gps_rbuff[GPS_RBUFF_SIZE];

/* DMA���������־ */
__IO uint8_t GPS_TransferEnd = 0, GPS_HalfTransferEnd = 0;


/*-------------------------- USART ��ز������� --------------------------*/
/*
 * ��������GPS_USART_Config
 * ����  ��USART GPIO ����,����ģʽ����
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
 */
static void GPS_USART_Config(void)
{
	/* ��ʼ���ṹ�� */
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	/*** ��ʼ��������Ҫ�õ���GPIO ***/
	
	/* �򿪴���GPIO��ʱ�� */
	RCC_APB2PeriphClockCmd(GPS_USART_RX_GPIO_CLK|GPS_USART_TX_GPIO_CLK, ENABLE);
	GPS_RCC_PeriphClockCmd_Fun(GPS_USART_CLK, ENABLE);

	/* ��USART Tx��GPIO����Ϊ���츴��ģʽ */
  GPIO_InitStructure.GPIO_Pin = GPS_USART_TX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPS_USART_TX_GPIO_PORT, &GPIO_InitStructure);
	    
  /* ��USART Rx��GPIO����Ϊ��������ģʽ */
  GPIO_InitStructure.GPIO_Pin = GPS_USART_RX_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPS_USART_TX_GPIO_PORT, &GPIO_InitStructure);
	  
	/*** ���� USART �Ĺ������� ***/
	
	/* �򿪴��������ʱ�� */
	USART_InitStructure.USART_BaudRate = GPS_USART_BAUDRATE;            						 // ���ò�����
	USART_InitStructure.USART_WordLength = USART_WordLength_8b; 										 // ����һ֡�����ֳ�
	USART_InitStructure.USART_StopBits = USART_StopBits_1; 													 // ����ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No ; 														 // ����У��λ
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;   						   // ���ù���ģʽ���շ�һ��
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;	 // ����Ӳ��������
	
	USART_Init(GPS_USART, &USART_InitStructure); 			// ��ɴ��ڵĳ�ʼ������
	
	// �����ж����ȼ�����
	USART_Cmd(GPS_USART, ENABLE);	 // ʹ�ܴ��ڽ����ж�
	
}


/*------------------------ ���� USARTx �ж����ȼ� -----------------------*/
/**
  * @brief  GPS_Interrupt_Config ����GPSʹ�õ�DMA�ж� 
  * @param  None.
  * @retval None.
  */
static void GPS_Interrupt_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);			// �����ж����ȼ�����

	NVIC_InitStructure.NVIC_IRQChannel = GPS_DMA_IRQn;		// �����ж�Դ 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // ������ռ���ȼ��������ȼ���
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;  // ���������ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;  // ʹ���ж�
	NVIC_Init(&NVIC_InitStructure);

}



/**
  * @brief  GPS_DMA_Config gps dma��������
  * @param  ��
  * @retval ��
  */
static void GPS_DMA_Config(void)
{
   DMA_InitTypeDef DMA_InitStructure;
	
		/*����DMAʱ��*/
		RCC_AHBPeriphClockCmd(GPS_DMA_CLK, ENABLE);

		/*����DMAԴ���������ݼĴ�����ַ*/
		DMA_InitStructure.DMA_PeripheralBaseAddr = GPS_DATA_ADDR;	   

		/*�ڴ��ַ(Ҫ����ı�����ָ��)*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)gps_rbuff;

		/*���򣺴��ڴ浽����*/		
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	

		/*�����СDMA_BufferSize=SENDBUFF_SIZE*/	
		DMA_InitStructure.DMA_BufferSize = GPS_RBUFF_SIZE;

		/*�����ַ����*/	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 

		/*�ڴ��ַ����*/
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	

		/*�������ݵ�λ*/	
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

		/*�ڴ����ݵ�λ 8bit*/
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 

		/*DMAģʽ������ѭ��*/
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 

		/*���ȼ�����*/	
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  

		/*��ֹ�ڴ浽�ڴ�Ĵ���	*/
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

		/*����DMA��ͨ��*/		   
		DMA_Init(GPS_DMA_CHANNEL, &DMA_InitStructure); 	   
    
    GPS_Interrupt_Config();
		
    DMA_ITConfig(GPS_DMA_CHANNEL,DMA_IT_HT|DMA_IT_TC,ENABLE);  //����DMA������ɺ�����ж�

		/*ʹ��DMA*/
		DMA_Cmd (GPS_DMA_CHANNEL,ENABLE);		
    
    /* ���ô��� �� DMA����TX���� */
		USART_DMACmd(GPS_USART, USART_DMAReq_Rx, ENABLE);
}


/**
  * @brief  GPS_Config gps ��ʼ��
  * @param  ��
  * @retval ��
  */
void GPS_Config(void)
{
  GPS_USART_Config();
  GPS_DMA_Config();  
  
}



/**
  * @brief  trace �ڽ���ʱ��������GPS���
  * @param  str: Ҫ������ַ�����str_size:���ݳ���
  * @retval ��
  */
void trace(const char *str, int str_size)
{
  #ifdef __GPS_DEBUG    //��gps_config.h�ļ���������꣬�Ƿ����������Ϣ
    uint16_t i;
    printf("\r\nTrace: ");
    for(i=0;i<str_size;i++)
      printf("%c",*(str+i));
  
    printf("\n");
  #endif
}

/**
  * @brief  error �ڽ������ʱ�����ʾ��Ϣ
  * @param  str: Ҫ������ַ�����str_size:���ݳ���
  * @retval ��
  */
void error(const char *str, int str_size)
{
    #ifdef __GPS_DEBUG   //��gps_config.h�ļ���������꣬�Ƿ����������Ϣ

    uint16_t i;
    printf("\r\nError: ");
    for(i=0;i<str_size;i++)
      printf("%c",*(str+i));
    printf("\n");
    #endif
}

/**
  * @brief  error �ڽ������ʱ�����ʾ��Ϣ
  * @param  str: Ҫ������ַ�����str_size:���ݳ���
  * @retval ��
  */
void gps_info(const char *str, int str_size)
{

    uint16_t i;
    printf("\r\nInfo: ");
    for(i=0;i<str_size;i++)
      printf("%c",*(str+i));
    printf("\n");
}



/******************************************************************************************************** 
**     ��������:            bit        IsLeapYear(uint8_t    iYear) 
**    ��������:            �ж�����(�������2000�Ժ�����) 
**    ��ڲ�����            iYear    ��λ���� 
**    ���ڲ���:            uint8_t        1:Ϊ����    0:Ϊƽ�� 
********************************************************************************************************/ 
static uint8_t IsLeapYear(uint8_t iYear) 
{ 
    uint16_t    Year; 
    Year    =    2000+iYear; 
    if((Year&3)==0) 
    { 
        return ((Year%400==0) || (Year%100!=0)); 
    } 
     return 0; 
} 

/******************************************************************************************************** 
**     ��������:            void    GMTconvert(uint8_t *DT,uint8_t GMT,uint8_t AREA) 
**    ��������:            ��������ʱ�任�������ʱ��ʱ�� 
**    ��ڲ�����            *DT:    ��ʾ����ʱ������� ��ʽ YY,MM,DD,HH,MM,SS 
**                        GMT:    ʱ���� 
**                        AREA:    1(+)���� W0(-)���� 
********************************************************************************************************/ 
void    GMTconvert(nmeaTIME *SourceTime, nmeaTIME *ConvertTime, uint8_t GMT,uint8_t AREA) 
{ 
    uint32_t    YY,MM,DD,hh,mm,ss;        //������ʱ�����ݴ���� 
     
    if(GMT==0)    return;                //�������0ʱ��ֱ�ӷ��� 
    if(GMT>12)    return;                //ʱ�����Ϊ12 �����򷵻�         

    YY    =    SourceTime->year;                //��ȡ�� 
    MM    =    SourceTime->mon;                 //��ȡ�� 
    DD    =    SourceTime->day;                 //��ȡ�� 
    hh    =    SourceTime->hour;                //��ȡʱ 
    mm    =    SourceTime->min;                 //��ȡ�� 
    ss    =    SourceTime->sec;                 //��ȡ�� 

    if(AREA)                        //��(+)ʱ������ 
    { 
        if(hh+GMT<24)    hh    +=    GMT;//������������ʱ�䴦��ͬһ�������Сʱ���� 
        else                        //����Ѿ����ڸ�������ʱ��1����������ڴ��� 
        { 
            hh    =    hh+GMT-24;        //�ȵó�ʱ�� 
            if(MM==1 || MM==3 || MM==5 || MM==7 || MM==8 || MM==10)    //���·�(12�µ�������) 
            { 
                if(DD<31)    DD++; 
                else 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
            } 
            else if(MM==4 || MM==6 || MM==9 || MM==11)                //С�·�2�µ�������) 
            { 
                if(DD<30)    DD++; 
                else 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
            } 
            else if(MM==2)    //����2�·� 
            { 
                if((DD==29) || (DD==28 && IsLeapYear(YY)==0))        //��������������2��29�� ���߲�����������2��28�� 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
                else    DD++; 
            } 
            else if(MM==12)    //����12�·� 
            { 
                if(DD<31)    DD++; 
                else        //�������һ�� 
                {               
                    DD    =    1; 
                    MM    =    1; 
                    YY    ++; 
                } 
            } 
        } 
    } 
    else 
    {     
        if(hh>=GMT)    hh    -=    GMT;    //������������ʱ�䴦��ͬһ�������Сʱ���� 
        else                        //����Ѿ����ڸ�������ʱ��1����������ڴ��� 
        { 
            hh    =    hh+24-GMT;        //�ȵó�ʱ�� 
            if(MM==2 || MM==4 || MM==6 || MM==8 || MM==9 || MM==11)    //�����Ǵ��·�(1�µ�������) 
            { 
                if(DD>1)    DD--; 
                else 
                { 
                    DD    =    31; 
                    MM    --; 
                } 
            } 
            else if(MM==5 || MM==7 || MM==10 || MM==12)                //������С�·�2�µ�������) 
            { 
                if(DD>1)    DD--; 
                else 
                { 
                    DD    =    30; 
                    MM    --; 
                } 
            } 
            else if(MM==3)    //�����ϸ�����2�·� 
            { 
                if((DD==1) && IsLeapYear(YY)==0)                    //�������� 
                { 
                    DD    =    28; 
                    MM    --; 
                } 
                else    DD--; 
            } 
            else if(MM==1)    //����1�·� 
            { 
                if(DD>1)    DD--; 
                else        //�����һ�� 
                {               
                    DD    =    31; 
                    MM    =    12; 
                    YY    --; 
                } 
            } 
        } 
    }         

    ConvertTime->year   =    YY;                //������ 
    ConvertTime->mon    =    MM;                //������ 
    ConvertTime->day    =    DD;                //������ 
    ConvertTime->hour   =    hh;                //����ʱ 
    ConvertTime->min    =    mm;                //���·� 
    ConvertTime->sec    =    ss;                //������ 
}  


