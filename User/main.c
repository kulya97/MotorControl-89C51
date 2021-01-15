#include <reg52.h>
#include "commond.h"
#include "uart.h"

sbit dir=P2^0;
sbit pul =P2^1;

#define MAX_BUFF_value 18
u8 mbuf[MAX_BUFF_value];
u8 buf[MAX_BUFF_value];
/****************************************/
u16 uart_idle_count;//空闲计数
u8 uart_readbuf_count;//读取字节数计数
/****************************************/
u32 MotorStep = 50000;//电机步长
u32 pwm_count = 0;//pwm计数
u32 pos = 0;//电机当前位置
u16 motor_speed=40;//电机速度
bit motor_dir;//电机方向
bit motor_move;//电机运动状态
u8 status_data[20];
/****************************************/

void delay(u32 i)
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

int main()
{
	Timer0Init();  //定时器0初始化
	uart_init(9600);
	dir=0;
	pul=0;
	delay(5000);
	while(1)
	{
   if(motor_move)
	 {
		  delay(motor_speed);
		pul=~pul;
		 pos++;
		 if((++pwm_count)>=MotorStep)
		 {
			
			 pwm_count=0;
			 motor_move=0;
		 }
		 
	 }
	}
}

void Timer0() interrupt 1
{
	TH0=0XFC;	//给定时器赋初值，定时1ms
	TL0=0X18;
if(uart_readbuf_count!=0)
	{
		if(++uart_idle_count>20)
		{
			uart_idle_count=0;
			uart_readbuf_count=0;
		 if (mbuf[0] != 0x30 || mbuf[1] != 0xff) {
			return;
		}
		if (mbuf[2] == 0x00) { //设置移动步数
			MotorStep = mbuf[3];
			return;
		} else if (mbuf[2] == 0x01) { //移动
			motor_dir=(mbuf[3]==0x00)?0:1;
			
			motor_speed=(mbuf[4]&0xff)<<8|(mbuf[5]&0xff);
			motor_speed=(motor_speed>80)?80:motor_speed;
			motor_speed=(motor_speed<2)?2:motor_speed;
		  MotorStep=((mbuf[6]&0xff)<<8|(mbuf[7]&0xff)<<16)|(mbuf[8]&0xff)<<8|(mbuf[9]&0xff);
			MotorStep=(MotorStep<0)?0:MotorStep;
			
			dir=motor_dir;
			motor_move=1;
			//send8bit(0x30);
			return;
		} else if (mbuf[2] == 0x02) { //停止
			motor_move=0;
			return;
		} else if (mbuf[2] == 0x03) { //查询
			
			sprintf(status_data, "%d,%d,%d\n", pos,MotorStep,motor_dir);
			send(status_data, sizeof(status_data));
			return;
		}
			
		}
	}
}


void Usart() interrupt 4
{
    if(RI == 1) {  //如果收到.
      RI = 0;      //清除标志.
			uart_idle_count=0;
			if(uart_readbuf_count<MAX_BUFF_value)
			{ 
		   	mbuf[uart_readbuf_count++]=SBUF;
			  uart_idle_count=0;
			}
    }
}