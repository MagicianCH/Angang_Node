#include "xbee.h"
#include "delay.h"

u8 cpy_address_out[17] = {0x7E, 0x00, 0x00, 0x10, 0x00, 0x00, 0x13, 0xA2, 0x00, 0x40, 0x8B, 0xC7, 0x3A, 0xFF, 0xFE, 0x00, 0x00};
                         //xbee包格式中data之前的部分

//u8 cpy_address_in[15] = {0x7E, 0x00, 0x00, 0x02, 0x00, 0x13, 0xA2, 0x00, 0x40, 0x70, 0x1C, 0x05,0x02,0xFF,0x01};
                         //数据包格式中负载前边的部分
	
u8 cpy_address_in[17] = {0x7E, 0x00, 0x00, 0x02, 0x00, 0x13, 0xA2, 0x00, 0x40, 0x70, 0x1C, 0x05,
		                       0x00, //本包中负载长度，一会填充
		                       0x01,0x01,0x02,0x00}; //下位机id，簇头逻辑地址，命令号，压缩标志位（发送时填充）
		                       

u8 cpy_channel;  //通道号
//u8 zip_flag;
u8* cpy_p_data0 = (u8*)usReadValue1;
u8 *cpy_p_data1 = (u8*)usReadValue2;
u8* cpy_p_data2 = (u8*)usReadValue3;
u16 cpy_length = 0;

char* data_zip(u16* _data,u8 _len);


//USART1发送一个数组
void UART1_Send_Array(unsigned char send_array[],unsigned char num) //???? ????(???) ??????1-255 
{
	unsigned char i=0;  //????????  ?? ????? ++??

	while(i<num)   //i????num ???  ???
  {
		USART_SendData(USART1,send_array[i]);        //?????  ????
		while(!(USART1->SR & USART_FLAG_TXE))
			;
	
    i++;  //? ??         
  }       
}

void UART1_Send_CharArray(char send_array[],unsigned char num) //???? ????(???) ??????1-255 
{
	unsigned char i=0;  //????????  ?? ????? ++??

	while(i<num)   //i????num ???  ???
  {
		USART_SendData(USART1,send_array[i]);        //?????  ????
		while(!(USART1->SR & USART_FLAG_TXE))
			;
	
    i++;  //? ??         
  }       
}

void UART1_Send_Array16(u16 array[],u16 num)
{
	u16 i = 0;
	u8 top;
	u8 low;
	
	while(i < num)
	{
		top = (u8)(array[i] >> 8);
		low = (u8)array[i];
		
		USART_SendData(USART1,top);
		while(!(USART1->SR & USART_FLAG_TXE));
		USART_SendData(USART1,low);
		while(!(USART1->SR & USART_FLAG_TXE));
		
		i++;
		
	}

}

//处理数据
void data_Process(u8 _flag,u8 _zip_data_flag)
{
//	u16 test[5] = {0,1,2,3,4};
//	xbee_Prepare(test,5,1);
//	u16 *x = usReadValue[1];
	//xbee_Prepare(usReadValue,1000,1);
//	PCout(13) = ~PCout(13);
//	u16 j = 0;
//				for( j = 0; j < 1000; j++)
//				usReadValue[i][j]=0;
//					printf("%d\n",usReadValue[1][j]);
//	printf("*******************************************\n");
	
	u8 i = 0;
	u8 counter;
	
	if(_zip_data_flag == 0) {       //不压缩
		if(_flag == 1) {
			if((SAMPLING_LENGTH1%50) == 0 ) {
				counter = SAMPLING_LENGTH1*2/50;
				for(i = 0; i < counter; i++)
				{
			//	DataSendXbee(0, 100, cpy_p_data+100*i, counter*100, i);
					DataSendXbee(1, 50, cpy_p_data0+50*i, SAMPLING_LENGTH1, i,0);
					delay_ms(100);
				}
			}
			else {
				counter = SAMPLING_LENGTH1*2/100;
				for(i = 0; i < counter; i++)
				{
			//	DataSendXbee(0, 100, cpy_p_data+100*i, counter*100, i);
					DataSendXbee(1, 100, cpy_p_data0+100*i, SAMPLING_LENGTH1, i,0);
					delay_ms(100);
				}
				DataSendXbee(1,((SAMPLING_LENGTH1*2)%100),cpy_p_data0+100*i, SAMPLING_LENGTH1, i,0);
				delay_ms(100);
			}
		}
		
		else if(_flag == 2) {
			if((SAMPLING_LENGTH2%100) == 0 ) {
				counter = SAMPLING_LENGTH2*2/100;
				for(i = 0; i < counter; i++)
				{
			//	DataSendXbee(0, 100, cpy_p_data+100*i, counter*100, i);
					DataSendXbee(2, 100, cpy_p_data1+100*i, SAMPLING_LENGTH2, i,0);
					delay_ms(100);
				}
			}
			else {
				counter = SAMPLING_LENGTH2*2/100;
				for(i = 0; i < counter; i++)
				{ 
			//	DataSendXbee(0, 100, cpy_p_data+100*i, counter*100, i);
					DataSendXbee(2, 100, cpy_p_data1+100*i, SAMPLING_LENGTH2, i,0);
					delay_ms(100);
				}
				DataSendXbee(2,((SAMPLING_LENGTH2*2)%100),cpy_p_data1+100*i, SAMPLING_LENGTH2, i,0);
				delay_ms(100);
			}
		}
		
		else if(_flag == 3) {
			if((SAMPLING_LENGTH2%100) == 0) {
				counter = SAMPLING_LENGTH2*2/100;
				for(i = 0; i < counter; i++)
				{
			//	DataSendXbee(0, 100, cpy_p_data+100*i, counter*100, i);
					DataSendXbee(3, 100, cpy_p_data2+100*i, SAMPLING_LENGTH2, i,0);
					delay_ms(100);
				}
			}
			else {
				counter = SAMPLING_LENGTH2*2/100;
				for(i = 0; i < counter; i++)
				{ 
			//	DataSendXbee(0, 100, cpy_p_data+100*i, counter*100, i);
					DataSendXbee(3, 100, cpy_p_data2+100*i, SAMPLING_LENGTH2, i,0);
					delay_ms(100);
				}
				DataSendXbee(3,((SAMPLING_LENGTH2*2)%100),cpy_p_data2+100*i, SAMPLING_LENGTH2, i,0);
				delay_ms(100);
			}	
		}
	}
	
	else if(_zip_data_flag == 1) {      //压缩后的数据
		if(_flag == 1) {
		//	data_zip(usReadValue1,SAMPLING_LENGTH1);
			if(SAMPLING_LENGTH1%50 == 0 ) {
				counter = SAMPLING_LENGTH1/50;
				for(i = 0; i < counter; i++)
				{
			//	DataSendXbee(0, 100, cpy_p_data+100*i, counter*100, i);
					data_zip(usReadValue1+50*i,50);
					ZipDataSendXbee(1, 51, p_zip_data, SAMPLING_LENGTH1, i,1);  ///here here
					delay_ms(100);
				}
			}
			else {
				counter = SAMPLING_LENGTH1/50;
				for(i = 0; i < counter; i++)
				{
			//	DataSendXbee(0, 100, cpy_p_data+100*i, counter*100, i);
					data_zip(usReadValue1+50*i,50);
					ZipDataSendXbee(1, 51, p_zip_data, SAMPLING_LENGTH1, i,1);
					delay_ms(100);
				}
				data_zip(usReadValue1+50*i,(SAMPLING_LENGTH1%50));
				ZipDataSendXbee(1,(SAMPLING_LENGTH1%50+1),p_zip_data, SAMPLING_LENGTH1, i,1);
				delay_ms(100);
			}
		}
		
		else if(_flag == 2) {
	//		data_zip(usReadValue2,SAMPLING_LENGTH2);
			if(SAMPLING_LENGTH2%50 == 0 ) {
				counter = SAMPLING_LENGTH2/50;
				for(i = 0; i < counter; i++)
				{
			//	DataSendXbee(0, 100, cpy_p_data+100*i, counter*100, i);
					data_zip(usReadValue2+50*i,50);
					ZipDataSendXbee(2, 51, p_zip_data, SAMPLING_LENGTH2, i,1);
					delay_ms(100);
				}
			}
			else {
				counter = SAMPLING_LENGTH2/50;
				for(i = 0; i < counter; i++)
				{ 
			//	DataSendXbee(0, 100, cpy_p_data+100*i, counter*100, i);
					data_zip(usReadValue2+50*i,50);
					ZipDataSendXbee(2, 51, p_zip_data, SAMPLING_LENGTH2, i,1);
					delay_ms(100);
				}
				data_zip(usReadValue2+50*i,SAMPLING_LENGTH2%50);
				ZipDataSendXbee(2,(SAMPLING_LENGTH2%50+1),p_zip_data, SAMPLING_LENGTH2, i,1);
				delay_ms(100);
			}
		}
		
		else if(_flag == 3) {
//			delay_ms(100);
//			data_zip(usReadValue3,SAMPLING_LENGTH2);
			if(SAMPLING_LENGTH2%50 == 0) {
				counter = SAMPLING_LENGTH2/50;
				for(i = 0; i < counter; i++)
				{
			//	DataSendXbee(0, 100, cpy_p_data+100*i, counter*100, i);
					data_zip(usReadValue3+50*i,50);
					ZipDataSendXbee(3, 51, p_zip_data, SAMPLING_LENGTH2, i,1);
					delay_ms(100);
				}
			}
			else {
				counter = SAMPLING_LENGTH2/50;
				for(i = 0; i < counter; i++)
				{ 
			//	DataSendXbee(0, 100, cpy_p_data+100*i, counter*100, i);
					data_zip(usReadValue3+50*i,50);
					ZipDataSendXbee(3, 51, p_zip_data, SAMPLING_LENGTH2, i,1);
					delay_ms(100);
				}
				data_zip(usReadValue2+50*i,SAMPLING_LENGTH2%50);
				ZipDataSendXbee(3,(SAMPLING_LENGTH2%50+1),p_zip_data, SAMPLING_LENGTH2, i,1);
				delay_ms(100);
			}	
		}
	}
}



void DataSendXbee(u8 cpy_channel, u16 cpy_length, u8* cpy_p_data, u16 cpy_all_length, u8 sequence_num,u8 _zip_flag)
{
	u16 length, i = 0;
	u8 d_length_low, d_length_high, length_low, length_high;  //d_length_ 总数据长度
	u8 payload_length;    //后来加的那个负载长度，其实完全可以由length_highlength_low算出来，就是n+8+1那个，-8-1就可以了
	u8 thispackage_length_high,thispackage_length_low;
	u8 cs = 0;
	
	d_length_low = (u8)cpy_all_length;  //采样数据总长度，这个是个全局变量，从flash中读取或由上层设置
	d_length_high = (u8)(cpy_all_length>>8);
		
	payload_length = cpy_length + 9;
	
	thispackage_length_high = (u8)((payload_length + 8 + 1) >> 8);
	thispackage_length_low = (u8)(payload_length + 8 + 1);
	
//	length = cpy_length + 4 + 13 + 14;
	length = cpy_length + 14 + 15 + 4 + 3;  //length和FCS之间的数据长度
	length_low = (u8)length;                //14:xbee格式中length到data之间有14个字节；13：一次发送的包除负载外有13字节
	length_high = (u8)(length>>8);          //3：命令类型，节点地址  4:采样长度，序号，通道 
	
	cpy_address_out[1] = length_high;  //填充整个xbee包中那个长度
	cpy_address_out[2] = length_low;
	cpy_address_in[1] = thispackage_length_high;    //填充本包长度 n+8+1 那个
	cpy_address_in[2] = thispackage_length_low;
	cpy_address_in[12] = payload_length;  //填充负载长度，后加的我觉得没必要那个
	cpy_address_in[16] = _zip_flag;
	
	UART1_Send_Array(cpy_address_out, 17);
	UART1_Send_Array(cpy_address_in, 17);
	
	USART_SendData(USART1,d_length_high);     //总采样长度
	while(!(USART1->SR & USART_FLAG_TXE));
			
	USART_SendData(USART1,d_length_low);     //同上
	while(!(USART1->SR & USART_FLAG_TXE));
	
	USART_SendData(USART1, sequence_num);  //序号
	while(!(USART1->SR & USART_FLAG_TXE));
	
	USART_SendData(USART1,cpy_channel);   //通道号
	while(!(USART1->SR & USART_FLAG_TXE));
	
	UART1_Send_Array(cpy_p_data, cpy_length);  //本包的唯一数据，即原始数据
	
	USART_SendData(USART1, 0xEE);  //FCS,暂时不计算，用EE填充
	while(!(USART1->SR & USART_FLAG_TXE));
	
	for(i = 3; i < 17; i++)
	{
		cs += cpy_address_out[i];
	}
	for(i = 0; i < 17; i++)
	{
		cs += cpy_address_in[i];
	}
	cs+=d_length_high;
	cs+=d_length_low;
	cs+=sequence_num;
	cs+=cpy_channel;	
	
	for(i = 0; i < cpy_length; i++)
	{
		cs += cpy_p_data[i];
	}
	cs+=0xEE;  //FCS
	cs = 0xFF - cs;
	USART_SendData(USART1, cs);
	while(!(USART1->SR & USART_FLAG_TXE));
}

void ZipDataSendXbee(u8 cpy_channel, u16 cpy_length, char* cpy_p_data, u16 cpy_all_length, u8 sequence_num,u8 _zip_flag)
{
	u16 length, i = 0;
	u8 d_length_low, d_length_high, length_low, length_high;  //d_length_ 总数据长度
	u8 payload_length;    //后来加的那个负载长度，其实完全可以由length_highlength_low算出来，就是n+8+1那个，-8-1就可以了
	u8 thispackage_length_high,thispackage_length_low;
	u8 cs = 0;
	
	d_length_low = (u8)cpy_all_length;  //采样数据总长度，这个是个全局变量，从flash中读取或由上层设置
	d_length_high = (u8)(cpy_all_length>>8);
		
	payload_length = cpy_length + 9;
	
	thispackage_length_high = (u8)((payload_length + 8 + 1) >> 8);
	thispackage_length_low = (u8)(payload_length + 8 + 1);
	
//	length = cpy_length + 4 + 13 + 14;
	length = cpy_length + 14 + 15 + 4 + 3;  //length和FCS之间的数据长度
	length_low = (u8)length;                //14:xbee格式中length到data之间有14个字节；13：一次发送的包除负载外有13字节
	length_high = (u8)(length>>8);          //3：命令类型，节点地址  4:采样长度，序号，通道 
	
	cpy_address_out[1] = length_high;  //填充整个xbee包中那个长度
	cpy_address_out[2] = length_low;
	cpy_address_in[1] = thispackage_length_high;    //填充本包长度 n+8+1 那个
	cpy_address_in[2] = thispackage_length_low;
	cpy_address_in[12] = payload_length;  //填充负载长度，后加的我觉得没必要那个
	cpy_address_in[16] = _zip_flag;
	
	UART1_Send_Array(cpy_address_out, 17);
	UART1_Send_Array(cpy_address_in, 17);
	
	USART_SendData(USART1,d_length_high);     //总采样长度
	while(!(USART1->SR & USART_FLAG_TXE));
			
	USART_SendData(USART1,d_length_low);     //同上
	while(!(USART1->SR & USART_FLAG_TXE));
	
	USART_SendData(USART1, sequence_num);  //序号
	while(!(USART1->SR & USART_FLAG_TXE));
	
	USART_SendData(USART1,cpy_channel);   //通道号
	while(!(USART1->SR & USART_FLAG_TXE));
	
	UART1_Send_CharArray(cpy_p_data, cpy_length);  //本包的唯一数据，即原始数据
	
	USART_SendData(USART1, 0xEE);  //FCS,暂时不计算，用EE填充
	while(!(USART1->SR & USART_FLAG_TXE));
	
	for(i = 3; i < 17; i++)
	{
		cs += cpy_address_out[i];
	}
	for(i = 0; i < 17; i++)
	{
		cs += cpy_address_in[i];
	}
	cs+=d_length_high;
	cs+=d_length_low;
	cs+=sequence_num;
	cs+=cpy_channel;	
	
	for(i = 0; i < cpy_length; i++)
	{
		cs += cpy_p_data[i];
	}
	cs+=0xEE;  //FCS
	cs = 0xFF - cs;
	USART_SendData(USART1, cs);
	while(!(USART1->SR & USART_FLAG_TXE));
}


//数据压缩算法
char* data_zip(u16* _data,u8 _len)  //为了避免溢出，每个数据只取前14位
{
	u16 i;
	
	zip_data[0] = (char)(_data[0] >> 8);
	zip_data[1] = (char)_data[0] >> 2;
	for(i = 1; i < _len; i++) {
		zip_data[i+1] = (char)(_data[i] >> 2) - (_data[i-1] >> 2);
	}
	return zip_data;
}
