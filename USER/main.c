
//头文件
#include "stm32f10x.h"
#include "usart.h"
#include "ad7606_spi.h"
#include "stdio.h"
#include "delay.h"
#include "sys.h"
//#include "data.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "xbee.h"
#include "stmflash.h"
//extern u8 flag;


//=============================================================================
//文件名称：main
//功能概要：主函数
//参数说明：无
//函数返回：int
//=============================================================================
int main(void)
{	
//	u8 ch_data[2] = {0x41,0x42};
	SystemInit();
	delay_init(72);
	NVIC_Configuration();
	uart_init(115200);	
	ad7606_init();

	Test_Flash();
	
	start_TIM3(SAMPLING_PERIOD);
	ad7606_StartRecord(SAMPLING_RATE1);
		
			while(1) 
	{
		
/*		if(SAMPLING_PERIOD == 5000 && SAMPLING_LENGTH1 == 1000 && SAMPLING_RATE1 == 10000) {
			PCout(13) = ~PCout(13);
			delay_ms(700);
		}
*/		
		
		if(flag == 1) {     //
			ad7606_StopRecord();
			PCout(13) = ~PCout(13);
			flag = 0;
		}
		
		if(sampling_flag1 == 1){  //采样结束
//			PCout(13) = ~PCout(13);
			data_Process(sampling_flag1,1);
//			DataSendXbee(0,2,ch_data,1000,1,0);
			sampling_flag1 = 0;
//			PCout(13) = ~PCout(13);
		}
		if(sampling_flag2 == 2) {  //第二通道采样结束
//			PCout(13) = ~PCout(13);
			data_Process(sampling_flag2,1);
			data_Process(sampling_flag2 + 1,1);
			sampling_flag2 = 0;
//			flag = 1;
		}
	
		if(flag == 2) {  //收到上位机发来的命令包
			stop_TIM3();  //停止采集周期记时
			ad7606_StopRecord();  //停止采样
	//		PCout(13) = ~PCout(13);
			
			switch(xbeeReceieveData[30]){  //xebeeReceieveData[30]是命令号，31是通道号
			 case 0x03:  //修改采样率
				 if(xbeeReceieveData[31] == 0x01) {    //channel 1
					 SAMPLING_RATE1 = ((u16)xbeeReceieveData[32]<<8) + (u16)xbeeReceieveData[33]; // 修改第一通道采样率，32、33是设置的参数值
					 Test_Write(SAMPLING_RATE1_ADDR,SAMPLING_RATE1);
				 }
				 else if(xbeeReceieveData[31] == 0x02 || xbeeReceieveData[31] == 0x03) {
					 SAMPLING_RATE2 = ((u16)xbeeReceieveData[32]<<8) + (u16)xbeeReceieveData[33];  // 修改第二通道采样率
					 Test_Write(SAMPLING_RATE2_ADDR,SAMPLING_RATE2);
				 }
				else
					 break;
				 break;
			 case 0x04:  //修改采样长度
//				 PCout(13) = ~PCout(13);
				 if(xbeeReceieveData[31] == 0x01) {   // 
				   SAMPLING_LENGTH1 = ((u16)xbeeReceieveData[32]<<8) + (u16)xbeeReceieveData[33];
					 Test_Write(SAMPLING_LENGTH1_ADDR,SAMPLING_LENGTH1);
				 }
				 else if(xbeeReceieveData[31] == 0x02 || xbeeReceieveData[31] == 0x03) {
					 SAMPLING_LENGTH2 = ((u16)xbeeReceieveData[32]<<8) + (u16)xbeeReceieveData[33];  // 修改第二通道采样长度
					 Test_Write(SAMPLING_LENGTH2_ADDR,SAMPLING_LENGTH2);
				 }
				 else
					 break;
			   break;
				 
			 case 0x05:  //修改采样周期
				 SAMPLING_PERIOD = ((u16)xbeeReceieveData[32]<<8) + (u16)xbeeReceieveData[33];  //认为三个通道的采样周期都是一样的，所以要改一块改
				 Test_Write(SAMPLING_PERIOD_ADDR,SAMPLING_PERIOD);
//				 PCout(13) = ~PCout(13);
			   break;
			}
			
		flag = 0;	
		start_TIM3(SAMPLING_PERIOD);
		ad7606_StartRecord(SAMPLING_RATE1); 
		}		
	}
}
