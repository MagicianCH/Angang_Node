#ifndef __USART_H
#define __USART_H
#include "stdio.h"

extern unsigned char USART_RX_BUF[64];     //���ջ���,���63���ֽ�.ĩ�ֽ�Ϊ���з� 
extern u8 USART_RX_STA;         //����״̬���	

extern void uart_init(u32 bound);

#endif
