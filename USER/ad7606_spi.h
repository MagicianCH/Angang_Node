/*
*********************************************************************************************************
*	                                  
*	ģ������ : AD7606����ģ�� 
*	�ļ����� : bsp_ad7606.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v0.1    2009-12-27 armfly  �������ļ���ST�̼���汾ΪV3.1.2
*		v1.0    2011-01-11 armfly  ST�̼���������V3.4.0�汾��
*
*	Copyright (C), 2010-2011, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef _AD7606_SPI_H
#define _AD7606_SPI_H

/* ÿ������2�ֽڣ��ɼ�ͨ�� */
#define CH_NUM			3				/* �ɼ�3��ͨ�� */
//#define FIFO_SIZE		2*1024*2		/* ��С��Ҫ����48K (CPU�ڲ�RAM ֻ��64K) */

/* ����AD7606��SPI GPIO */
#define AD_SPI                           SPI1
#define AD_SPI_CLK                       RCC_APB2Periph_SPI1

#define AD_SPI_SCK_PIN                   GPIO_Pin_5				
#define AD_SPI_SCK_GPIO_PORT             GPIOA						
#define AD_SPI_SCK_GPIO_CLK              RCC_APB2Periph_GPIOA

#define AD_SPI_MISO_PIN                  GPIO_Pin_6				
#define AD_SPI_MISO_GPIO_PORT            GPIOA						
#define AD_SPI_MISO_GPIO_CLK             RCC_APB2Periph_GPIOA

#define AD_CS_PIN                        GPIO_Pin_4   //cs PA4
#define AD_CS_GPIO_PORT                  GPIOA				
#define AD_CS_GPIO_CLK                   RCC_APB2Periph_GPIOA

/*������AD7606������GPIO */
#define AD_RESET_PIN                     GPIO_Pin_4
#define AD_RESET_GPIO_PORT               GPIOB				
#define AD_RESET_GPIO_CLK                RCC_APB2Periph_GPIOB

#define AD_CONVST_PIN                    GPIO_Pin_8
#define AD_CONVST_GPIO_PORT              GPIOA		
#define AD_CONVST_GPIO_CLK               RCC_APB2Periph_GPIOA

#define AD_RANGE_PIN                     GPIO_Pin_5
#define AD_RAGEE_GPIO_PORT               GPIOB		
#define AD_RAGEE_GPIO_CLK                RCC_APB2Periph_GPIOB

#define AD_OS0_PIN                     GPIO_Pin_0
#define AD_OS0_GPIO_PORT               GPIOB		
#define AD_OS0_GPIO_CLK                RCC_APB2Periph_GPIOB

#define AD_OS1_PIN                     GPIO_Pin_1
#define AD_OS1_GPIO_PORT               GPIOB	
#define AD_OS1_GPIO_CLK                RCC_APB2Periph_GPIOB

#define AD_OS2_PIN                     GPIO_Pin_2
#define AD_OS2_GPIO_PORT               GPIOB		
#define AD_OS2_GPIO_CLK                RCC_APB2Periph_GPIOB

// ����LED GPIO
#define LED_PIN                        GPIO_Pin_13
#define LED_GPIO_PORT                  GPIOC
#define LED_GPIO_CLK                   RCC_APB2Periph_GPIOC

#define AD_CS_LOW()     				AD_CS_GPIO_PORT->BRR = AD_CS_PIN
#define AD_CS_HIGH()     				AD_CS_GPIO_PORT->BSRR = AD_CS_PIN

#define AD_RESET_LOW()					AD_RESET_GPIO_PORT->BRR = AD_RESET_PIN
#define AD_RESET_HIGH()					AD_RESET_GPIO_PORT->BSRR = AD_RESET_PIN
	
#define AD_CONVST_LOW()					AD_CONVST_GPIO_PORT->BRR = AD_CONVST_PIN
#define AD_CONVST_HIGH()				AD_CONVST_GPIO_PORT->BSRR = AD_CONVST_PIN

#define AD_RANGE_5V()					AD_RANGE_GPIO_PORT->BRR = AD_RANGE_PIN
#define AD_RANGE_10V()					AD_RANGE_GPIO_PORT->BSRR = AD_RANGE_PIN

#define AD_OS0_0()						AD_OS0_GPIO_PORT->BRR = AD_OS0_PIN
#define AD_OS0_1()						AD_OS0_GPIO_PORT->BSRR = AD_OS0_PIN

#define AD_OS1_0()						AD_OS1_GPIO_PORT->BRR = AD_OS1_PIN
#define AD_OS1_1()						AD_OS1_GPIO_PORT->BSRR = AD_OS1_PIN

#define AD_OS2_0()						AD_OS2_GPIO_PORT->BRR = AD_OS2_PIN
#define AD_OS2_1()						AD_OS2_GPIO_PORT->BSRR = AD_OS2_PIN

/* AD���ݲɼ������� */
/*typedef struct
{
	uint16_t usRead;
	uint16_t usWrite;
	uint16_t usCount;
	uint16_t usBuf[FIFO_SIZE];
}FIFO_T;
*/

extern u8 flag;
extern u8 sampling_flag1;
extern u8 sampling_flag2;

extern u16 usReadValue1[2000];
extern u16 usReadValue2[2000];
extern u16 usReadValue3[2000];
extern u8 xbeeReceieveData[100];  //xbee ���յ�������
extern char zip_data[1500]; //ѹ�������ݻ�����
extern char *p_zip_data;

extern u16 SAMPLING_LENGTH1;  //channel 1 data length
extern u16 SAMPLING_LENGTH2;   //channel 2 3 data length

extern u16 SAMPLING_RATE1;    //channel 1
extern u16 SAMPLING_RATE2;   //channel 2 3

extern u16 SAMPLING_PERIOD;

/* ���ⲿ���õĺ������� */
void ad7606_Reset(void);
void ad7606_SetOS(uint8_t _ucMode);
void bsp_SET_TIM2_FREQ(uint32_t _ulFreq);
void ad7606_init(void);
void ad7606_StartRecord(uint32_t _ulFreq);
void ad7606_StopRecord(void);
//uint8_t GetAdcFormFifo(uint16_t *_usReadAdc);
void ad7606_SetInputRange(uint8_t _ucRange);
void ad7606_StartConv(void);

//void sampling_Period_Set(u16 _ms);
void start_TIM3(u16 _ms);
void stop_TIM3(void);


void UART1_Send_Array(unsigned char send_array[],unsigned char num);
extern void USART1_Send_Array16(u16 array[],u16 num);
//void data_Send();


//extern FIFO_T  g_tAD;

#endif


