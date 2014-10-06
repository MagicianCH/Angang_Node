#ifndef _XBEE_H
#define _XBEE_H

#include "stm32f10x.h"
#include "ad7606_spi.h"

extern u8 cpy_address[17];
extern u8 cpy_channel;
extern u8* cpy_p_data;
extern u16 cpy_length;

void data_Process(u8 _flag,u8 _zip_data_flag);
void UART1_Send_Array(unsigned char send_array[],unsigned char num);
void UART1_Send_CharArray(char send_array[],unsigned char num);
void UART1_Send_Array16(u16 array[],u16 num);
void DataSendXbee(u8 cpy_channel, u16 cpy_length, u8* cpy_p_data, u16 cpy_all_length, u8 sequence_num,u8 _zip_flag);
void ZipDataSendXbee(u8 cpy_channel, u16 cpy_length, char* cpy_p_data, u16 cpy_all_length, u8 sequence_num,u8 _zip_flag);

#endif
