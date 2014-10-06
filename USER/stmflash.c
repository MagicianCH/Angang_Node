#include "stmflash.h"
#include "delay.h"
#include "usart.h"
#include "ad7606_spi.h"
 
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEKս��STM32������
//STM32 FLASH ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/13
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved									  
//////////////////////////////////////////////////////////////////////////////////
 
//��ȡָ����ַ�İ���(16λ����)
//faddr:����ַ(�˵�ַ����Ϊ2�ı���!!)
//����ֵ:��Ӧ����.
u16 STMFLASH_ReadHalfWord(u32 faddr)
{
	return *(vu16*)faddr; 
}
#if STM32_FLASH_WREN	//���ʹ����д   
//������д��
//WriteAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��   
void STMFLASH_Write_NoCheck(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)   
{ 			 		 
	u16 i;
	for(i=0;i<NumToWrite;i++)
	{
		FLASH_ProgramHalfWord(WriteAddr,pBuffer[i]);
	    WriteAddr+=2;//��ַ����2.
	}  
} 
//��ָ����ַ��ʼд��ָ�����ȵ�����
//WriteAddr:��ʼ��ַ(�˵�ַ����Ϊ2�ı���!!)
//pBuffer:����ָ��
//NumToWrite:����(16λ)��(����Ҫд���16λ���ݵĸ���.)
#if STM32_FLASH_SIZE<256
#define STM_SECTOR_SIZE 1024 //�ֽ�
#else 
#define STM_SECTOR_SIZE	2048
#endif		 
u16 STMFLASH_BUF[STM_SECTOR_SIZE/2];//�����2K�ֽ�
void STMFLASH_Write(u32 WriteAddr,u16 *pBuffer,u16 NumToWrite)	
{
	u32 secpos;	   //������ַ
	u16 secoff;	   //������ƫ�Ƶ�ַ(16λ�ּ���)
	u16 secremain; //������ʣ���ַ(16λ�ּ���)	   
 	u16 i;    
	u32 offaddr;   //ȥ��0X08000000��ĵ�ַ
	if(WriteAddr<STM32_FLASH_BASE||(WriteAddr>=(STM32_FLASH_BASE+1024*STM32_FLASH_SIZE)))return;//�Ƿ���ַ
	FLASH_Unlock();						//����
	offaddr=WriteAddr-STM32_FLASH_BASE;		//ʵ��ƫ�Ƶ�ַ.
	secpos=offaddr/STM_SECTOR_SIZE;			//������ַ  0~127 for STM32F103RBT6
	secoff=(offaddr%STM_SECTOR_SIZE)/2;		//�������ڵ�ƫ��(2���ֽ�Ϊ������λ.)
	secremain=STM_SECTOR_SIZE/2-secoff;		//����ʣ��ռ��С   
	if(NumToWrite<=secremain)secremain=NumToWrite;//�����ڸ�������Χ
	while(1) 
	{	
		STMFLASH_Read(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//������������������
		for(i=0;i<secremain;i++)//У������
		{
			if(STMFLASH_BUF[secoff+i]!=0XFFFF)break;//��Ҫ����  	  
		}
		if(i<secremain)//��Ҫ����
		{
			FLASH_ErasePage(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE);//�����������
			for(i=0;i<secremain;i++)//����
			{
				STMFLASH_BUF[i+secoff]=pBuffer[i];	  
			}
			STMFLASH_Write_NoCheck(secpos*STM_SECTOR_SIZE+STM32_FLASH_BASE,STMFLASH_BUF,STM_SECTOR_SIZE/2);//д����������  
		}else STMFLASH_Write_NoCheck(WriteAddr,pBuffer,secremain);//д�Ѿ������˵�,ֱ��д������ʣ������. 				   
		if(NumToWrite==secremain)break;//д�������
		else//д��δ����
		{
			secpos++;				//������ַ��1
			secoff=0;				//ƫ��λ��Ϊ0 	 
		   	pBuffer+=secremain;  	//ָ��ƫ��
			WriteAddr+=secremain;	//д��ַƫ��	   
		   	NumToWrite-=secremain;	//�ֽ�(16λ)���ݼ�
			if(NumToWrite>(STM_SECTOR_SIZE/2))secremain=STM_SECTOR_SIZE/2;//��һ����������д����
			else secremain=NumToWrite;//��һ����������д����
		}	 
	};	
	FLASH_Lock();//����
}
#endif

//��ָ����ַ��ʼ����ָ�����ȵ�����
//ReadAddr:��ʼ��ַ
//pBuffer:����ָ��
//NumToWrite:����(16λ)��
void STMFLASH_Read(u32 ReadAddr,u16 *pBuffer,u16 NumToRead)   	
{
	u16 i;
	for(i=0;i<NumToRead;i++)
	{
		pBuffer[i]=STMFLASH_ReadHalfWord(ReadAddr);//��ȡ2���ֽ�.
		ReadAddr+=2;//ƫ��2���ֽ�.	
	}
}

//////////////////////////////////////////////////////////////////////////////////////////////////////
//WriteAddr:��ʼ��ַ
//WriteData:Ҫд�������
void Test_Write(u32 WriteAddr,u16 WriteData)   	
{
	STMFLASH_Write(WriteAddr,&WriteData,1);//д��һ���� 
}

void Test_Flash(void)
{
	u16 SAMPLING_LENGTH1_PRE = 1000;  //��һͨ���������ȣ�����
	u16 SAMPLING_LENGTH2_PRE = 1000;  //�ڶ���ͨ����������
	u16 SAMPLING_RATE1_PRE = 10000; //��һͨ��������
	u16 SAMPLING_RATE2_PRE = 10000;  //�ڶ���ͨ��
	u16 SAMPLING_PERIOD_PRE = 5000;
	
	u16 d_l1,d_l2,d_r1,d_r2,d_p;

	 //��һ�ε�ʱ�������дһ����ȥ
	Test_Write(SAMPLING_LENGTH1_ADDR,500);
	Test_Write(SAMPLING_LENGTH2_ADDR,500);
	Test_Write(SAMPLING_RATE1_ADDR,5000);
	Test_Write(SAMPLING_RATE2_ADDR,5000);
	Test_Write(SAMPLING_PERIOD_ADDR,7000);

	
	d_l1 = STMFLASH_ReadHalfWord(SAMPLING_LENGTH1_ADDR);
	d_l2 = STMFLASH_ReadHalfWord(SAMPLING_LENGTH2_ADDR);
	d_r1 = STMFLASH_ReadHalfWord(SAMPLING_RATE1_ADDR);
	d_r2 = STMFLASH_ReadHalfWord(SAMPLING_RATE2_ADDR);
	d_p = STMFLASH_ReadHalfWord(SAMPLING_PERIOD_ADDR);
	
	if(((d_l1<400)||(d_l1>=1500)) || ((d_l2<400)&&(d_l2>=1500)) || ((d_r1<=100)||(d_r1>=65535)) 
		  || ((d_r2<=100)||(d_r2>=65535)) || ((d_p<=1000)||(d_p>65535))) {
	
		Test_Write(SAMPLING_LENGTH1_ADDR,SAMPLING_LENGTH1_PRE);
		Test_Write(SAMPLING_LENGTH2_ADDR,SAMPLING_LENGTH2_PRE);
		Test_Write(SAMPLING_RATE1_ADDR,SAMPLING_RATE1_PRE);
		Test_Write(SAMPLING_RATE2_ADDR,SAMPLING_RATE2_PRE);
		Test_Write(SAMPLING_PERIOD_ADDR,SAMPLING_PERIOD_PRE);
	}
		SAMPLING_LENGTH1 = STMFLASH_ReadHalfWord(SAMPLING_LENGTH1_ADDR);
		SAMPLING_LENGTH2 = STMFLASH_ReadHalfWord(SAMPLING_LENGTH2_ADDR);
		SAMPLING_RATE1 = STMFLASH_ReadHalfWord(SAMPLING_RATE1_ADDR);
		SAMPLING_RATE2 = STMFLASH_ReadHalfWord(SAMPLING_RATE2_ADDR);
		SAMPLING_PERIOD = STMFLASH_ReadHalfWord(SAMPLING_PERIOD_ADDR);
}















