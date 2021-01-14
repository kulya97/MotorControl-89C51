#include <reg52.h>
#include "commond.h"
#include "uart.h"
#define led P2
sbit dir=P2^0;
sbit pul =P2^1;
#define MAX_BUFF_value 20
u8 mbuf[MAX_BUFF_value];
u8 buf[MAX_BUFF_value];

u16 uart_idle_count;
u8 uart_readbuf_count;
u8 uart_read_ready_flag;

u16 MotorStep = 50000;
u16 pwm_count = 0;
u16 pos = 0;

u8 motor_dir;
u8 motor_move;

u8 status_data[8];
void delay_ms(u16 ms)
{
  u8 i,j;
	for(j=0;j<ms;j++)
 	 for(i=0;i<120;i++);
}
void delay(u16 i)
{
	while(i--);	
}
void Timer0Init()
{
	TMOD|=0X01;//选择为定时器0模式，工作方式1，仅用TR0打开启动。

	TH0=0XFC;	//给定时器赋初值，定时1ms
	TL0=0X18;	
	ET0=1;//打开定时器0中断允许
	EA=1;//打开总中断
	TR0=1;//打开定时器			
}
void Timer1Init()
{
	TMOD|=0X10;//选择为定时器1模式，工作方式1，仅用TR1打开启动。

	TH1=0XFC;	//给定时器赋初值，定时1ms
	TL1=0X18;	
	ET1=1;//打开定时器1中断允许
	EA=1;//打开总中断
	TR1=1;//打开定时器			
}

int main()
{
	Timer0Init();  //定时器0初始化
	uart_init(9600);
	//led=0xfe;
	dir=0;
	pul=0;
	delay(5000);
	while(1)
	{
		//pul=~pul;
   if(motor_move)
	 {
		  pul=~pul;
		 pos++;
		 if((++pwm_count)>=MotorStep)
		 {
			 pwm_count=0;
			 motor_move=0;
		 }
		 delay(1);
	 }
	}
}
void Timer0() interrupt 1
{
	static u16 i;
	TH0=0XFC;	//给定时器赋初值，定时1ms
	TL0=0X18;
if(uart_readbuf_count!=0)
	{
		if(++uart_idle_count>10)
		{
			//send(mbuf,uart_readbuf_count);
			uart_idle_count=0;
			uart_readbuf_count=0;
			uart_read_ready_flag=1;
			if (mbuf[0] == 0x30 && mbuf[1] == 0xff) {
//			HAL_UART_Transmit(&huart1, result_ok, sizeof(result_ok), 50);
		} else {
			return;
		}
		if (mbuf[2] == 0x00) { //设置移动步数
			MotorStep = mbuf[3];
			return;
		} else if (mbuf[2] == 0x01) { //移动
			if(mbuf[3] == 0x00)
			{
				motor_dir=0;
			}
			else if(mbuf[3] == 0x01)
			{
				motor_dir=1;
			}
			dir=motor_dir;
			delay(2);
			motor_move=1;
			return;
		} else if (mbuf[2] == 0x02) { //停止
			motor_move=0;
			return;
		} else if (mbuf[2] == 0x03) { //查询
			
			sprintf(status_data, "%d\n", pos);
			send( status_data, sizeof(status_data));
			return;
			//HAL_UART_Transmit(&huart1, "\n", sizeof("\n"), 50);
		}
			
		}
	}
}


void Usart() interrupt 4
{
	  u8 mbuff;
    if(RI == 1) {  //如果收到.
      RI = 0;      //清除标志.
			uart_idle_count=0;
			if(uart_readbuf_count<MAX_BUFF_value)
			{ mbuff = SBUF;  //接收.
		   	mbuf[uart_readbuf_count++]=SBUF;
			  uart_idle_count=0;
			}
    }
}