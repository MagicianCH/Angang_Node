#ifndef _DATA_H
#define _DATA_H

#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "sys.h"
#include <stdio.h>


//����������ȣ��������ڣ��������ȣ���Щ��������ʱ��flash�ж�ȡ
u16 DATA_LENGTH = 1000;
u16 SAMPLING_PERIOD;
u16 SAMPLING_RATE;

//����ȫ�ֱ���
u8 flag = 0;  //��־λ 

uint16_t data_length=0;  //�Ѳɼ��������ݳ���
//int16_t usReadValue[3][1000]={0,0};// ����ͨ���ɼ���������



#endif
