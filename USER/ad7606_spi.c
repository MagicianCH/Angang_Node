/*
*********************************************************************************************************
*	                                  
*	ģ������ : AD7606����ģ��
*	�ļ����� : bsp_spi_ad7606.c
*	��    �� : V1.0
*	˵    �� : ����AD7606 ADCת���� SPI�ӿ�
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2011-06-18 armfly  ����
*
*	Copyright (C), 2010-2011, ���������� www.armfly.com
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

/* ����ȫ���жϵĺ� */
#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ��ȫ���ж� */
#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹȫ���ж� */

//����������ȣ��������ڣ��������ȣ���Щ��������ʱ��flash�ж�ȡ
u16 SAMPLING_LENGTH1;  //��һͨ���������ȣ�����
u16 SAMPLING_LENGTH2;  //�ڶ���ͨ����������

u16 SAMPLING_RATE1; //��һͨ��������
u16 SAMPLING_RATE2;  //�ڶ���ͨ��

u16 SAMPLING_PERIOD;

//����ȫ�ֱ���
u8 flag = 0;  //��־λ 
u8 sampling_flag1;  //channel 1����ͨ����־λ
u8 sampling_flag2;  //channel 2,3

u16 data_length1 = 0;  //��һͨ���Ѳɼ��������ݳ���
u16 data_length2 = 0; //channel 2 3
u16 tim2_counter = 0;

//uint16_t usReadValue[3][1000]={0,0};// ����ͨ���ɼ���������
u16 usReadValue1[2000] = {0};
u16 usReadValue2[2000] = {0};
u16 usReadValue3[2000] = {0};
u8 xbeeReceieveData[100] = {0};   //xbee�������ݻ�����
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
*	�� �� ��: bsp_InitAD7606
*	����˵��: ��ʼ��AD7606 SPI����
*	��    �Σ���
*	�� �� ֵ: ��
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
	


//ʹ��SPI_1ʱ��
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
     
  //���� SPI1 : SCK, MISO and MOSI */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  
  //����Ƭѡ��IO��PA4
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

	/* ����CONVST GPIO */
	GPIO_InitStructure.GPIO_Pin = AD_CONVST_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(AD_CONVST_GPIO_PORT, &GPIO_InitStructure);
	PAout(8)=1;

	/* ����OS0-2 GPIO */
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
	
	/* ����������GPIO */
    
	 //test by wyd
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
	  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0  | GPIO_Pin_10 | GPIO_Pin_11 | GPIO_Pin_12  | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	

   // RCC_APB2Periph_GPIOB added by wyd in following statement

	/* ʹ��GPIOʱ�� */
	RCC_APB2PeriphClockCmd(AD_RESET_GPIO_CLK | AD_CONVST_GPIO_CLK | AD_OS0_GPIO_CLK 
		| AD_OS1_GPIO_CLK | AD_OS2_GPIO_CLK |RCC_APB2Periph_GPIOA, ENABLE);

	/* ����RESET GPIO


	GPIO_InitStructure.GPIO_Pin = AD_RESET_PIN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(AD_RESET_GPIO_PORT, &GPIO_InitStructure);
 */	
//	PBout(6) = 0;
	
  

	/* ���ù�����ģʽ */
//	ad7606_SetOS(0);
	
	//��������
	ad7606_SetInputRange(1);    // -10~10

	/* ����GPIO�ĳ�ʼ״̬ */
	ad7606_Reset();				/* Ӳ����λ��AD7606 */
	
	AD_CONVST_HIGH();			/* CONVST������Ϊ�ߵ�ƽ */	
    ad7606_StartConv();
	bsp_TIM2_Configuration();	/* ����TIM2��ʱ�ж� ��ռ���ȼ�1����Ӧ���ȼ�0*/	
  sampling_Priod_TIM3_Config();	 //����TIM3��ʱ�жϣ����Ʋ������ڣ���ռ���ȼ�0����Ӧ���ȼ�0

}

/*
*********************************************************************************************************
*	�� �� ��: ad7606_Reset
*	����˵��: Ӳ����λAD7606
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ad7606_Reset(void)
{
	/* AD7606�Ǹߵ�ƽ��λ��Ҫ����С����50ns */
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
*	�� �� ��: ad7606_SetOS
*	����˵��: ���ù�����ģʽ�������˲���Ӳ����ƽ��ֵ)
*	��    �Σ�_ucMode : 0-6  0��ʾ�޹�������1��ʾ2����2��ʾ4����3��ʾ8����4��ʾ16��
*				5��ʾ32����6��ʾ64��
*	�� �� ֵ: ��
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
	else	/* ��0���� */
	{
		AD_OS2_0();
		AD_OS1_0();
		AD_OS0_0();
	}
}

/*
*********************************************************************************************************
*	�� �� ��: ad7606_StartConv
*	����˵��: ����AD7606��ADCת��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ad7606_StartConv(void)
{
	/* �����ؿ�ʼת�����͵�ƽ����ʱ������25ns  */
	AD_CONVST_LOW();
	AD_CONVST_LOW();
	AD_CONVST_LOW();	/* ����ִ��2�Σ��͵�ƽԼ50ns */
	AD_CONVST_LOW();
	AD_CONVST_LOW();
//	delay_ms(5);
	AD_CONVST_HIGH();
}


// ���ò������� timer3 
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

//���ò������ڣ��������ڽ���flash�ض�ȡ����λΪms
void sampling_Period_Set(u16 _ms)
{
	u16 ms;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	
	ms = 2 * _ms;
	
	TIM_DeInit(TIM3);	/* ��λTIM3��ʱ�� */

	TIM_TimeBaseStructure.TIM_Period = ms - 1; 		/* �������� */
	TIM_TimeBaseStructure.TIM_Prescaler = 35999;	/* ��Ƶϵ�� */
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1; 		/* */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //�����������ϼ���
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	/* Clear TIM2 update pending flag[���TIM2����жϱ�־] */
	TIM_ClearFlag(TIM3, TIM_FLAG_Update);

	TIM_SetCounter(TIM3, 0);

	/* Enable TIM2 Update interrupt [TIM2����ж�����]*/
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);  
		
	TIM_Cmd(TIM3,ENABLE); //ʹ��TIM3
	
}

/*
*********************************************************************************************************
*	�� �� ��: start_TIM3
*	����˵��: ��ʼ�ɼ����ڼ�ʱ
*	��    �Σ�u16 _ms,�ɼ����ڣ���_msΪ��λ    <><><><><><><><><><<><><><><><><><><><><><
*	�� �� ֵ: ��                                   
*********************************************************************************************************
*/
void start_TIM3(u16 _ms)
{
	sampling_Priod_TIM3_Config();  //���� TIM3��ʱ�ж�
	sampling_Period_Set(_ms);  //

}

/*
*********************************************************************************************************
*	�� �� ��: stop_TIM3
*	����˵��: ֹͣ�������ڼ�ʱ�����ڵ��յ�������Ϣʱ��ֹͣ���������ò���
*	��    �Σ���         <><><><><><><><><><<><><><><><><><><><><><
*	�� �� ֵ: ��                                   
*********************************************************************************************************
*/
void stop_TIM3(void)
{
	TIM_Cmd(TIM3,DISABLE);

}

/*
*********************************************************************************************************
*	�� �� ��: bsp_TIM2_Configuration
*	����˵��: ����TIM2��ʱ��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_TIM2_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;
	
	/* TIM2 clock enable */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);  

	/* Configure the NVIC Preemption Priority Bits[�������ȼ���] */  
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);

	/* Enable the TIM2 gloabal Interrupt [����TIM2ȫ���ж�]*/
	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;  
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}
	
/*
*********************************************************************************************************
*	�� �� ��: bsp_SET_TIM2_FREQ
*	����˵��: ����TIM2��ʱ��Ƶ��
*	��    �Σ�_ulFreq : ����Ƶ�ʣ���λHz��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_SET_TIM2_FREQ(uint32_t _ulFreq)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
	uint16_t usPrescaler;
	uint16_t usPeriod;

	TIM_DeInit(TIM2);	/* ��λTIM2��ʱ�� */

	/* TIM2 configuration 
		TIM2CLK = 36 MHz	
	*/
	if (_ulFreq == 0)
	{
		return;		/* ����Ƶ��Ϊ0��ֹͣ���� */
	}
	else if (_ulFreq <= 100)   /* ����Ƶ��С��100Hz */
	{
		usPrescaler = 36000;		/* TM2CLK = 72 000 000/36000 = 2000 */
		usPeriod = 2000 / _ulFreq;			 	
	}
	else if (_ulFreq <= 200000)	/* ����Ƶ�� ��100Hz - 200kHz */
	{
		usPrescaler = 36 - 1;		/* TM2CLK = 36 000 000/36 = 2 000 000 */
		usPeriod = 2000000 / _ulFreq;
	}	
	else	/* ����Ƶ�ʴ��� 200kHz */
	{
		return;
	}
	
	TIM_TimeBaseStructure.TIM_Period = usPeriod - 1; 		/* �������� */
	TIM_TimeBaseStructure.TIM_Prescaler = usPrescaler;	/* ��Ƶϵ�� */
	TIM_TimeBaseStructure.TIM_ClockDivision = 0x0; 		/* */
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  //�����������ϼ���
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

	/* Clear TIM2 update pending flag[���TIM2����жϱ�־] */
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	TIM_SetCounter(TIM2, 0);

	/* Enable TIM2 Update interrupt [TIM2����ж�����]*/
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);  

	/* TIM2 enable counter [����tim2����]*/
	TIM_Cmd(TIM2, ENABLE);
}  

/*
*********************************************************************************************************
*	�� �� ��: AD7606_SetInputRange
*	����˵��: ����AD7606ģ���ź��������̡�
*	��    ��: _ucRange : 0 ��ʾ����5V   1��ʾ����10V
*	�� �� ֵ: ��
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
*	�� �� ��: ad7606_ReadBytes
*	����˵��: ��ȡAD7606�Ĳ������
*	��    �Σ�
*	�� �� ֵ: ��
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
*	�� �� ��: ad7606_IRQSrc
*	����˵��: ��ʱ���ñ����������ڶ�ȡADת��������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ad7606_IRQSrc(void)
{

	uint16_t i;
	
	u8 times = SAMPLING_RATE1/SAMPLING_RATE2;

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);

	// return ;
	/* ��ȡ���� 
	ʾ������⣬CS�͵�ƽ����ʱ�� 35us 
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
	//��ӡ�����ڲ���
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
		sampling_flag1 = 1;  //channel 1 ��������
//		PCout(13) = ~PCout(13);
//		data_length1 = SAMPLING_LENGTH1 + 2 ; //֮��ɼ��������ݴ�����һ��λ��������
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
	ad7606_StartConv(); //������һ��ת��
}


/*
*********************************************************************************************************
*	�� �� ��: ad7606_StartRecord
*	����˵��: ��ʼ�ɼ�
*	��    �Σ�_ulFreq : ����Ƶ��, ��λ HZ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ad7606_StartRecord(uint32_t _ulFreq)
{
	//ad7606_Reset();	/* ��λӲ�� */

	ad7606_StartConv();				/* ���������������1������ȫ0������ */

//	g_tAD.usRead = 0;				/* �����ڿ���TIM2֮ǰ��0 */
//	g_tAD.usWrite = 0;

	bsp_TIM2_Configuration();		/* ����TIM2��ʱ�ж� */		

	bsp_SET_TIM2_FREQ(_ulFreq);		/* ���ò���Ƶ��, ��ʹ��TIM2��ʱ�����ж� */
}

/*
*********************************************************************************************************
*	�� �� ��: ad7606_StopRecord
*	����˵��: ֹͣ�ɼ�
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ad7606_StopRecord(void)
{
	/* TIM2 enable counter [����tim2����]*/
	TIM_Cmd(TIM2, DISABLE);
	
}

/*
*********************************************************************************************************
*	�� �� ��: TIM2_IRQHandler
*	����˵��: ��ʱ�жϴ�����
*	��    �Σ���
*	�� �� ֵ: ��
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

