#ifndef __OLED_I2C_H
#define	__OLED_I2C_H

#include "stm32f10x.h"
#define OLED_ADDRESS	0x78 //通过调整0R电阻,屏可以0x78和0x7A两个地址 -- 默认0x78

/* STM32 I2C 快速模式 */
#define I2C_Speed              400000

/*I2C接口*/
#define OLED_I2C                          I2C1
#define OLED_I2C_CLK                      RCC_APB1Periph_I2C1
#define OLED_I2C_CLK_INIT								  RCC_APB1PeriphClockCmd

#define OLED_I2C_SCL_PIN                  GPIO_Pin_6                 
#define OLED_I2C_SCL_GPIO_PORT            GPIOB                       
#define OLED_I2C_SCL_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define OLED_I2C_SCL_SOURCE               GPIO_PinSource6
#define OLED_I2C_SCL_AF                   GPIO_AF_I2C1

#define OLED_I2C_SDA_PIN                  GPIO_Pin_7                  
#define OLED_I2C_SDA_GPIO_PORT            GPIOB                       
#define OLED_I2C_SDA_GPIO_CLK             RCC_AHB1Periph_GPIOB
#define OLED_I2C_SDA_SOURCE               GPIO_PinSource7
#define OLED_I2C_SDA_AF                   GPIO_AF_I2C1

void I2C_Configuration(void);

void I2C_WriteByte(uint8_t addr,uint8_t data);
void WriteCmd(unsigned char I2C_Command);
void WriteDat(unsigned char I2C_Data);

void OLED_Init(void);
void OLED_SetPos(unsigned char x, unsigned char y);
void OLED_Fill(unsigned char fill_Data);
void OLED_CLS(void);
void OLED_ON(void);
void OLED_OFF(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);
void OLED_ShowCN(unsigned char x, unsigned char y, unsigned char N);
void OLED_ShowCN2(unsigned char x, unsigned char y, unsigned char N);
void OLED_ShowCN3(unsigned char x, unsigned char y, unsigned char N);
void OLED_Show_num(uint8_t x,uint8_t y,float num,uint8_t size);
#endif
