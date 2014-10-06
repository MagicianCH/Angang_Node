/*
*********************************************************************************************************
*	                                  
*	模块名称 : AD7606驱动模块
*	文件名称 : bsp_spi_ad7606.c
*	版    本 : V1.0
*	说    明 : 驱动AD7606 ADC转换器 SPI接口
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2011-06-18 armfly  创建
*
*	Copyright (C), 2010-2011, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "stm32f10x.h"
#include <stdio.h>
#include "ad7606_spi.h"
#include "usart.h"
#include "sys.h"
#include "ad7606_spi.h"
#include "xbee.h"
#include "delay.h"
//#include "data.h"
//#include "timer.h"

/* 开关全局中断的宏 */
#define ENABLE_INT()	__set_PRIMASK(0)	/* 使能全局中断 */
#define DISABLE_INT()	__set_PRIMASK(1)	/* 禁止全局中断 */

//定义采样长度，采样周期，采样长度，这些参数开机时从flash中读取
u16 SAMPLING_LENGTH1;  //第一通道采样长度，轴向
u16 SAMPLING_LENGTH2;  //第二三通道采样长度

u16 SAMPLING_RATE1; //第一通道采样率
u16 SAMPLING_RATE2;  //第二三通道

u16 SAMPLING_PERIOD;

//定义全局变量
u8 flag = 0;  //标志位 
u8 sampling_flag1;  //channel 1采样通道标志位
u8 sampling_flag2;  //channel 2,3

u16 data_length1 = 0;  //第一通道已采集到的数据长度
u16 data_length2 = 0; //channel 2 3
u16 tim2_counter = 0;

//uint16_t usReadValue[3][1000]={0,0};// 三个通道采集到的数据
u16 usReadValue1[2000] = {0};
u16 usReadValue2[2000] = {0};
u16 usReadValue3[2000] = {0};
u8 xbeeReceieveData[100] = {0};   //xbee接收数据缓冲区
char zip_data[1500] = {0};
char *p_zip_data = zip_data;

void bsp_TIM2_Configuration(void);
void sampling_Priod_TIM3_Config(void);






void ad_reset_config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB , ENABLE); 						 
//=============================================================================
//LED -> PC13
//=============================================================================			 
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}




/*
*********************************************************************************************************
*	函 数 名: bsp_InitAD7606
*	功能说明: 初始化AD7606 SPI口线
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void ad7606_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
	SPI_InitTypeDef   SPI_InitStructure;
	
	/* AD_SPI_CS_GPIO, AD_SPI_MOSI_GPIO, AD_SPI_MISO_GPIO, AD_SPI_DETECT_GPIO 
	   and AD_SPI_SCK_GPIO Periph clock enable */
	RCC_APB2PeriphClockCmd(AD_CS_GPIO_CLK | AD_SPI_MISO_GPIO_CLK | AD_SPI_SCK_GPIO_CLK | RCC_APB2Periph_GPIOB | LED_GPIO_CLK 
			, ENABLE);
	


//使能SPI_1时钟
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
     
  //定义 SPI1 : SCK, MISO and MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  //定义片选线IO口PA4
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  PAout(4)=1;
  PAout(5)=1;
/*
	RCC_APB2PeriphClockCmd(AD_SPI_CLK, ENABLE); 
	
	GPIO_InitStructure.GPIO_Pin = AD_SPI_SCK_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(AD_SPI_SCK_GPIO_PORT, &GPIO_InitStructure);
	
	
	GPIO_InitStructure.GPIO_Pin = AD_SPI_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;  
	GPIO_Init(AD_SPI_MISO_GPIO_PORT, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = AD_CS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(AD_CS_GPIO_PORT, &GPIO_InitStructure);
*/	

	   ad_reset_config();

	/* 配置CONVST GPIO */
	GPIO_InitStructure.GPIO_Pin = AD_CONVST_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(AD_CONVST_GPIO_PORT, &GPIO_InitStructure);
	PAout(8)=1;

	/* 配置OS0-2 GPIO */
/*
	GPIO_InitStructure.GPIO_Pin = AD_OS0_PIN;
	GPIO_Init(AD_OS0_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = AD_OS1_PIN;
	GPIO_Init(AD_OS1_GPIO_PORT, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = AD_OS2_PIN;
	GPIO_Init(AD_OS2_GPIO_PORT, &GPIO_InitStructure);
  */

	//PC13 led
	GPIO_InitStructure.GPIO_Pin = LED_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(LED_GPIO_PORT, &GPIO_InitStructure);
	
	//PB5 RANGE   
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	PBout(5)=1;
	//busy
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/* AD_SPI Config */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	//SPI_InitStructure.SPI_Direction = SPI_Direction_1Line_Rx;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(AD_SPI, &SPI_InitStructure);
	
	SPI_Cmd(AD_SPI, ENABLE); /* AD_SPI enable */
	SPI_I2S_SendData(AD_SPI, 0xFFFF);
	
	/* 配置其它的GPIO */
    
	 //test by wyd
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12  | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

   // RCC_APB2Periph_GPIOB added by wyd in following statement

	/* 使能GPIO时钟 */
	RCC_APB2PeriphClockCmd(AD_RESET_GPIO_CLK | AD_CONVST_GPIO_CLK | AD_OS0_GPIO_CLK 
		| AD_OS1_GPIO_CLK | AD_OS2_GPIO_CLK |RCC_APB2Periph_GPIOA, ENABLE);

	/* 配置RESET GPIO


	GPIO_InitStructure.GPIO_Pin = AD_RESET_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(AD_RESET_GPIO_PORT, &GPIO_InitStructure);
 */	
//	PBout(6) = 0;
	
  

	/* 设置过采样模式 */
//	ad7606_SetOS(0);
	
	//设置量程
	ad7606_SetInputRange(1);    // -10~10

	/* 设置GPIO的初始状态 */
	ad7606_Reset();				/* 硬件复位复AD7606 */
	
	AD_CONVST_HIGH();			/* CONVST脚设置为高电平 */	
    ad7606_StartConv();
	bsp_TIM2_Configuration();	/* 配置TIM2定时中断 抢占优先级1，响应优先级0*/	
  sampling_Priod_TIM3_Config();	 //配置TIM3定时中断，控制采样周期，抢占优先级0，响应优先级0

}

/*
*********************************************************************************************************
*	函 数 名: ad7606_Reset
*	功能说明: 硬件复位AD7606
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void ad7606_Reset(void)
{
	/* AD7606是高电平复位，要求最小脉宽50ns */
	int i;
	PBout(8)=0;
	PBout(8)=1;
	/*
	AD_RESET_LOW();
	
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	AD_RESET_HIGH();
	
	AD_RESET_LOW();
	*/
	//__asm("nop");
	for(i=0;i<50;i++) ;
	PBout(8)=0;
}

/*
*********************************************************************************************************
*	函 数 名: ad7606_SetOS
*	功能说明: 设置过采样模式（数字滤波，硬件求平均值)
*	形    参：_ucMode : 0-6  0表示无过采样，1表示2倍，2表示4倍，3表示8倍，4表示16倍
*				5表示32倍，6表示64倍
*	返 回 值: 无
*********************************************************************************************************
*/
void ad7606_SetOS(uint8_t _ucMode)
{
	if (_ucMode == 1)
	{
		AD_OS2_0();
		AD_OS1_0();
		AD_OS0_1();
	}
	else if (_ucMode == 2)
	{
		AD_OS2_0();
		AD_OS1_1();
		AD_OS0_0();
	}
	else if (_ucMode == 3)
	{
		AD_OS2_0();
		AD_OS1_1();
		AD_OS0_1();
	}
	else if (_ucMode == 4)
	{
		AD_OS2_1();
		AD_OS1_0();
		AD_OS0_0();
	}
	else if (_ucMode == 5)
	{
		AD_OS2_1();
		AD_OS1_0();
		AD_OS0_1();
	}
	else if (_ucMode == 6)
	{
		AD_OS2_1();
		AD_OS1_1();
		AD_OS0_0();
	}
	else	/* 按0处理 */
	{
		AD_OS2_0();
		AD_OS1_0();
		AD_OS0_0();
	}
}

/*
*********************************************************************************************************
*	函 数 名: ad7606_StartConv
*	功能说明: 启动AD7606的ADC转换
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void ad7606_StartConv(void)
{
	/* 上升沿开始转换，低电平持续时间至少25ns  */
	AD_CONVST_LOW();
	AD_CONVST_LOW();
	AD_CONVST_LOW();	/* 连续执行2次，低电平约50ns */
	AD_CONVST_LOW();
	AD_CONVST_LOW();
//	delay_ms(5);
	AD_CONVST_HIGH();
}


// 配置采样周期 timer3 
void sampling_Priod_TIM3_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

}

//设置采样周期，采用周期将从flash重读取，单位为ms
void sampling_Period_Set(u16 _ms)
{
	u16 ms;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	ms = 2 * _ms;
	
	TIM_DeInit(TIM3);	/* 复位TIM3定时器 */

	TIM_TimeBaseStructure.TIM_Period = ms - 1; 		/* 计数周期 */
	TIM_TimeBaseStructure.TIM_Prescaler = 35999;	/* 分频系数 */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 		/* */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向向上计数
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/* Clear TIM2 update pending flag[清除TIM2溢出中断标志] */
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);

	TIM_SetCounter(TIM3, 0);

	/* Enable TIM2 Update interrupt [TIM2溢出中断允许]*/
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);  
		
	TIM_Cmd(TIM3,ENABLE); //使能TIM3
	
}

/*
*********************************************************************************************************
*	函 数 名: start_TIM3
*	功能说明: 开始采集周期记时
*	形    参：u16 _ms,采集周期，以_ms为单位    <><><><><><><><><><<><><><><><><><><><><><
*	返 回 值: 无                                   
*********************************************************************************************************
*/
void start_TIM3(u16 _ms)
{
	sampling_Priod_TIM3_Config();  //配置 TIM3定时中断
	sampling_Period_Set(_ms);  //

}

/*
*********************************************************************************************************
*	函 数 名: stop_TIM3
*	功能说明: 停止采样周期记时，用于当收到设置信息时，停止并重新设置参数
*	形    参：无         <><><><><><><><><><<><><><><><><><><><><><
*	返 回 值: 无                                   
*********************************************************************************************************
*/
void stop_TIM3(void)
{
	TIM_Cmd(TIM3,DISABLE);

}

/*
*********************************************************************************************************
*	函 数 名: bsp_TIM2_Configuration
*	功能说明: 配置TIM2定时器
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_TIM2_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  

	/* Configure the NVIC Preemption Priority Bits[配置优先级组] */  
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	/* Enable the TIM2 gloabal Interrupt [允许TIM2全局中断]*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
	
/*
*********************************************************************************************************
*	函 数 名: bsp_SET_TIM2_FREQ
*	功能说明: 设置TIM2定时器频率
*	形    参：_ulFreq : 采样频率，单位Hz，
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_SET_TIM2_FREQ(uint32_t _ulFreq)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	uint16_t usPrescaler;
	uint16_t usPeriod;

	TIM_DeInit(TIM2);	/* 复位TIM2定时器 */

	/* TIM2 configuration 
		TIM2CLK = 36 MHz	
	*/
	if (_ulFreq == 0)
	{
		return;		/* 采样频率为0，停止采样 */
	}
	else if (_ulFreq <= 100)   /* 采样频率小于100Hz */
	{
		usPrescaler = 36000;		/* TM2CLK = 72 000 000/36000 = 2000 */
		usPeriod = 2000 / _ulFreq;			 	
	}
	else if (_ulFreq <= 200000)	/* 采样频率 ：100Hz - 200kHz */
	{
		usPrescaler = 36 - 1;		/* TM2CLK = 36 000 000/36 = 2 000 000 */
		usPeriod = 2000000 / _ulFreq;
	}	
	else	/* 采样频率大于 200kHz */
	{
		return;
	}
	
	TIM_TimeBaseStructure.TIM_Period = usPeriod - 1; 		/* 计数周期 */
	TIM_TimeBaseStructure.TIM_Prescaler = usPrescaler;	/* 分频系数 */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; 		/* */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //计数方向向上计数
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/* Clear TIM2 update pending flag[清除TIM2溢出中断标志] */
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	TIM_SetCounter(TIM2, 0);

	/* Enable TIM2 Update interrupt [TIM2溢出中断允许]*/
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);  

	/* TIM2 enable counter [允许tim2计数]*/
	TIM_Cmd(TIM2, ENABLE);
}  

/*
*********************************************************************************************************
*	函 数 名: AD7606_SetInputRange
*	功能说明: 配置AD7606模拟信号输入量程。
*	形    参: _ucRange : 0 表示正负5V   1表示正负10V
*	返 回 值: 无
*********************************************************************************************************
*/
void ad7606_SetInputRange(uint8_t _ucRange)
{
	if (_ucRange == 0)
	{
		PBout(5) = 0;
	}
	else
	{
		PBout(5) = 1;
	}
}

/*
*********************************************************************************************************
*	函 数 名: ad7606_ReadBytes
*	功能说明: 读取AD7606的采样结果
*	形    参：
*	返 回 值: 无
*********************************************************************************************************
*/
uint16_t ad7606_ReadBytes(void)
{
  uint16_t usData = 0;
  
  /* Wait until the transmit buffer is empty */
  while (SPI_I2S_GetFlagStatus(AD_SPI, SPI_I2S_FLAG_TXE) == RESET)
  {
  }
  
  /* Send the byte */
  SPI_I2S_SendData(AD_SPI, 0xFFFF);

  /* Wait until a data is received */
  while (SPI_I2S_GetFlagStatus(AD_SPI, SPI_I2S_FLAG_RXNE) == RESET)
  {
  }
  
  /* Get the received data */
  usData = SPI_I2S_ReceiveData(AD_SPI);

  /* Return the shifted data */
  return usData;
}


/*
*********************************************************************************************************
*	函 数 名: ad7606_IRQSrc
*	功能说明: 定时调用本函数，用于读取AD转换器数据
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void ad7606_IRQSrc(void)
{

	uint16_t i;
	
	u8 times = SAMPLING_RATE1/SAMPLING_RATE2;

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	// return ;
	/* 读取数据 
	示波器监测，CS低电平持续时间 35us 
	*/
	AD_CS_LOW();

	for (i = 0; i < 2; i++)  // <CH_NUM
	{
	//  usReadValue[i][data_length] = ad7606_ReadBytes();
		usReadValue1[data_length1] = ad7606_ReadBytes();
	} 
	
//	usReadValue1[data_length1] = ad7606_ReadBytes();
	usReadValue2[data_length2] = ad7606_ReadBytes();
	usReadValue3[data_length2] = ad7606_ReadBytes();
	//打印，用于测试
/*	for(i = 0; i < CH_NUM; i++)
	{
		printf("%d: %d\n ",i,usReadValue[i][data_length]);
	}
*/
printf("%d : %d \n",data_length2,usReadValue2[data_length2]);
	tim2_counter++;
	if(data_length1 <=SAMPLING_LENGTH1)
		data_length1++ ;
	if(tim2_counter%times == 0)
		data_length2++;

	if(data_length1 > SAMPLING_LENGTH1)
	{
//		for(i = 0; i < 3; i++)
//			for(j = 0; j < 1000; j++)
//				usReadValue[i][j]=0;
//					printf("%d\n",usReadValue[1][j]);
		sampling_flag1 = 1;  //channel 1 采样结束
//		PCout(13) = ~PCout(13);
//		data_length1 = SAMPLING_LENGTH1 + 2 ; //之后采集到的数据存入下一个位，不发送
//		data_length1 = 0;
	}
	if(data_length2 > SAMPLING_LENGTH2) {
		sampling_flag2 = 2;
		data_length1 = 0;
		data_length2 = 0;
		tim2_counter = 0;
		flag = 1;
//		PCout(13) = ~PCout(13);
	}
	  
	AD_CS_HIGH();	
	ad7606_StartConv(); //启动下一次转换
}


/*
*********************************************************************************************************
*	函 数 名: ad7606_StartRecord
*	功能说明: 开始采集
*	形    参：_ulFreq : 采样频率, 单位 HZ
*	返 回 值: 无
*********************************************************************************************************
*/
void ad7606_StartRecord(uint32_t _ulFreq)
{
	//ad7606_Reset();	/* 复位硬件 */

	ad7606_StartConv();				/* 启动采样，避免第1组数据全0的问题 */

//	g_tAD.usRead = 0;				/* 必须在开启TIM2之前清0 */
//	g_tAD.usWrite = 0;

	bsp_TIM2_Configuration();		/* 配置TIM2定时中断 */		

	bsp_SET_TIM2_FREQ(_ulFreq);		/* 设置采样频率, 并使能TIM2定时采样中断 */
}

/*
*********************************************************************************************************
*	函 数 名: ad7606_StopRecord
*	功能说明: 停止采集
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void ad7606_StopRecord(void)
{
	/* TIM2 enable counter [允许tim2计数]*/
	TIM_Cmd(TIM2, DISABLE);
	
}

/*
*********************************************************************************************************
*	函 数 名: TIM2_IRQHandler
*	功能说明: 定时中断处理函数
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void TIM2_IRQHandler(void)
{
	ad7606_IRQSrc();
//	printf("a");
}

void TIM3_IRQHandler(void)
{
	if(TIM_GetITStatus(TIM3,TIM_IT_Update) != RESET) {
		TIM_ClearITPendingBit(TIM3,TIM_IT_Update);
//		PCout(13) = ~PCout(13);
		//flag = 0;
		ad7606_StartRecord(SAMPLING_RATE1);
	}
}

