# STM32_SmartBus
Realize data interaction between hardware and Alibaba Cloud platform through STM32, 4G, and MQTT protocols
通过STM32、4G和MQTT协议实现硬件与阿里云平台之间的数据交互

# 具体Specific
硬件分为主控、数据采集、下位机数据显示和数据传输四大部分。其开发平台为MDK KEIL 5。裸机开发，无RTOS系统。
1. 主控芯片选择为STM32F103ZET6。
2. 数据采集中的定位功能选择外接ZX1612模块（郑州信大先进技术研究所自研），通过USART协议来连接主控，其解码采用NAME库，发送采用乒乓球式缓存技术；温湿度检测DHT11温湿度传感器，主控通过读取DATA口时序一次解析40bit数据；其中车内人数的测量通过在车辆的前后门处，各安装一对红外对管对射式光电开关传感器实现，主控通过EXTI外设控制器，捕获传感器输出信号的下降沿来获取是否有人通过；乘客间距需要在座位下安装压力传感器采集获得，主控获取方式同上；乘客体温数据则需要在车门处安装MXL90614GYBAA传感器来获得，通过SMBUS协议通信。
3. 下位机数据显示方面，通过OLED屏显示人体温度，若温度过高会显示警告，通过IIC协议与主控连接。同时门口有人上下车时有LED灯反馈。
4. 数据传输方面选用移远EC20，与主控通过USART协议连接，同时EC20通过MQTT协议将数据发送到阿里云物联网平台，服务质量为QoS 0，而且也能够接受来自云平台的数据。    

The hardware is divided into four parts: main control, data acquisition, lower computer data display and data transmission. Its development platform is MDK KEIL 5. Bare metal development, no RTOS system.
1. The main control chip is selected as STM32F103ZET6.
2. The positioning function in data collection selects an external ZX1612 module (self-developed by Zhengzhou Xinda Advanced Technology Institute), connects to the main control through the USART protocol, uses NAME library for decoding, and uses table tennis cache technology for transmission; temperature and humidity detection DHT11 Temperature and humidity sensor, the main control analyzes 40bit data at a time by reading the DATA port time sequence; the number of people in the car is measured by installing a pair of infrared photoelectric switch sensors at the front and rear doors of the vehicle, and the main control is through EXTI The peripheral controller captures the falling edge of the sensor output signal to obtain whether someone has passed; the passenger distance needs to be acquired by installing a pressure sensor under the seat, and the main control acquisition method is the same as above; the passenger body temperature data needs to be obtained by installing the MXL90614GYBAA sensor at the door. Communication through SMBUS protocol.
3. In terms of data display of the lower computer, the human body temperature is displayed on the OLED screen, and a warning will be displayed if the temperature is too high, and it is connected to the main control through the IIC protocol. At the same time, there will be LED light feedback when someone gets on and off at the door.
4. Quectel EC20 is selected for data transmission, which is connected with the main control through the USART protocol. At the same time, the EC20 sends data to the Alibaba Cloud IoT platform through the MQTT protocol. The service quality is QoS 0, and it can also accept data from the cloud platform.


# 最后Last
我知道这个系统还存在很多缺陷，比如裸机开发，代码逻辑不够清晰，而且繁琐。而且MQTT质量低，容易丢包。还有一些硬件选材不够合适，造成资源浪费。
恳请各位大佬提出意见，甚至可以一起完善这个项目，谢谢！  
I know that this system still has many flaws, such as bare metal development, the code logic is not clear enough, and cumbersome. Moreover, the quality of MQTT is low and packets are easily lost. In addition, some hardware materials are not suitable enough, causing a waste of resources.
I sincerely ask you all for your opinions, and you can even improve this project together, thank you!

**该项目仅供学习用**
**This project is for learning purposes only**
