#ifndef _DATA_H
#define _DATA_H

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "sys.h"
#include <stdio.h>


//定义采样长度，采样周期，采样长度，这些参数开机时从flash中读取
u16 DATA_LENGTH = 1000;
u16 SAMPLING_PERIOD;
u16 SAMPLING_RATE;

//定义全局变量
u8 flag = 0;  //标志位 

uint16_t data_length=0;  //已采集到的数据长度
//int16_t usReadValue[3][1000]={0,0};// 三个通道采集到的数据



#endif
