#include "uart.h"
void uart_init(u8 baud)
{
	SCON=0X50;			//设置为工作方式1
	TMOD|=0X20;			//设置计数器工作方式2
	PCON=0X80;			//波特率加倍
	TH1=0XF3;				//计数器初始值设置，注意波特率是4800的
	TL1=0XF3;
	ES=1;						//打开接收中断
	EA=1;						//打开总中断
	TR1=1;					//打开计数器
	
}

void send8bit(u8 dat)
 {
	SBUF=dat;
	while(!TI);
	TI=0;
 }
 void send(u8 *str,u16 length)
 {		
	 int i=0;
   for(i=0;i<length;i++)
   {	   
   send8bit(*(str+i));
   }
}