/*>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
 *±¾ÎÄ¼şÖĞº¯ÊıÓÃÓÚÊı¾İ´¦Àí£¬·¢ËÍµÈ
 <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<*/
 
 #include "data.h"
 
 
#include "xbee.h"

u8 dma[8] = {0x00,0x13,0xA2,0x00,0x40,0x70,0x1B,0xE8};

u8 period_block_data[250];  //?ü?úD??éêy?Y

u8 payload_data[300]; //?o??êy?Y

u8 send_data[300]; //òa·??íµ?êy?Y???'3?è?xbee×??oµ?°ü??ê???íaµ?êy?Y

u8 package[300];

void prepare_com(u8* data, u8* dest_a, u8 len);

void xbee_Prepare(u16 data[],u16 _length,u8 channel)
{
	u8 i = 0;
	u8 j = 0;
//	u16 length = 0;
	u8 sequenceNum = 1;
	u8 data_8b_index = 0;
	u16 period_data_index = 0;
	u16 payload_data_index = 0;
	u16 send_data_index = 0;
	u8 data_8b[200] = {0};

//°?16??µ?êy×é2e3é8??	
	for( i = data_8b_index; i < 100; i++) {
		u8 top,low;
		top = (u8)(data[i]>>8);
		low = (u8)data[i];
		data_8b[j++] = top;
		data_8b[j++] = low; 
	}

//??o?3é?ü?úD??éêy?Y	
	period_block_data[period_data_index++] = (u8)(_length>>8);
	period_block_data[period_data_index++] = (u8)_length;  //?°á??? ×ü3¤?è
	period_block_data[period_data_index++] = sequenceNum; //Dòo?
	period_block_data[period_data_index++] = channel; //í¨µào?	
	for( i = 0; i < 200; i++){  //?óè?????íê3éµ?8??êy?Y
		period_block_data[period_data_index++] = data_8b[i];
	}
	sequenceNum++;

//??o?3é?o??êy?Y???óè??üá?ààDíoí?úµ?µ??·	
	payload_data[payload_data_index++] = 0x02; //?üá?ààDí???ü?úD??éêy?Y
	payload_data[payload_data_index++] = 0;
	payload_data[payload_data_index++] = 0x01; //?úµ?µ??·??0x01(í?1?µ??·)
	for( i = 0;i < period_data_index; i++){ // ?óè??ü?úD??éêy?Y
		payload_data[payload_data_index++] = period_block_data[i];
	}

//??o?3éòa·??íµ?êy?Y???'3yè?xbee×?éí°üí·??íaµ?êy?Y2?·?	
	send_data[send_data_index++] = 0x7E; //?eê?×?·?
	send_data[send_data_index++] = (u8)((payload_data_index + 8)>>8); //
	send_data[send_data_index++] = (u8)(payload_data_index + 8); //3¤?è
	for( i = 0; i < payload_data_index; i++){ //?óè??o??
		send_data[send_data_index++] = payload_data[i];
	}
	
	prepare_com(send_data,dma,send_data_index -1);
}

void prepare_com(u8* data, u8* dest_a, u8 len)
{   
		u8 i = 0;
    u16 length;
    u8  cs = 0; 
    
    u8  index= 0;
    
    package[index++] = 0x7E;
    //len
    length= len+14;
    package[index++] = (uint8_t)(length>>8);
    package[index++] = (uint8_t)(length&0x00FF);
    
    package[index++] = 0x10;
    package[index++] = 0x00;
    //sink 64??
    //for(int i = 0;i<8;i++){
    //        package[index++] = dma[i];
    //} 
		for( i = 0; i < 8; i++)
			package[index++] = dma[i];
/*    package[index++] = 0x00;
    package[index++] = 0x00;
    package[index++] = 0x00;
    package[index++] = 0x00;
    package[index++] = 0x00;
    package[index++] = 0x00;
    package[index++] = 0xFF;
    package[index++] = 0xFF;
*/    
    //sink 16???
    package[index++] = 0xFF;
    package[index++] = 0xFE;
    
    //Radius,Options
    package[index++] = 0x00;
    package[index++] = 0x00;
    
    //app data
    for( i = 0;i < len;i++){
      package[index++] = data[i];
    }
    //??
    for( i = 3;i < index;i++){
    cs += package[i];
    }
    cs = 0xFF - cs;
    package[index++] = cs;
//    halUartXbeeWrite(package, index);
		UART1_Send_Array(package,index);
   // halUartWrite(package, index);
}
