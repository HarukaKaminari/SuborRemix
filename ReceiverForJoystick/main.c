#include <INTRINS.H>
#include "STC15W.h"


/*

*---------------------------*
|1 (GPIO2)	 \__/  (GPIO1)16|
|							|
|2 (GPIO3)		   (GPIO0)15|
|							|
|3 (GPIO4)				  14|
|							|
|4 (GPIO5)			(DATA)13|
|		  STC15W204S		|
|5 (GPIO6)		   (LATCH)12|
|							|
|6 (VCC)			 (CLK)11|
|		  					|
|7 (GPIO7)			 (TXD)10|
|							|
|8 (GND)			 (RXD) 9|
*---------------------------*
Fosc = 24MHz

P1.0~P1.7 -> 上 左 下 右 SEL STA B A
*/


sbit CLK	= P3^2;
sbit LATCH	= P3^3;
sbit DATA	= P3^6;


bit isReady = 0;
static unsigned char key = 0;
static const unsigned char data mask[] = {0x80, 0x40, 0x10, 0x20, 0x01, 0x04, 0x02, 0x08};	// A B SELECT START UP DOWN LEFT RIGHT。为提升速度，这个表放到RAM中
static unsigned char idx = 0;


static void GPIO_Init(){
	// P1口用于接收并行信号，全部初始化为准双向，依靠内部弱上拉输出高电平
	P1M0 = 0;
	P1M1 = 0;
	P1= 0xFF;
	// P3口初始化为准双向
	P3M0 = 0;
	P3M1 = 0;
	P3 = 0xFF;
}

static void Interrupt_Init(){
	// INT0中断（CLK）触发类型为上升沿+下降沿
	IT0 = 0;
	// 允许INT0中断
	EX0 = 1;
	// INT1中断（LATCH）触发类型为下降沿
	IT1 = 1;
	// 允许INT1中断
	EX1 = 1;
	// 开启全局中断
	EA = 1;
}

static void INT0_Isr() interrupt 0{
	// 只有已经成功锁存才允许CLK移位
	if(isReady == 0)
		return;
	// 读当前引脚电平，如果是低电平则说明是下降沿，此时直接忽略该中断
	if(CLK == 0)
		return;
	// CLK上升沿到来的时候，取锁存值的下一位输出
	idx++;
	DATA = key & mask[idx];
}

static void INT1_Isr() interrupt 2{
	// 只有收到LATCH下降沿的时候才允许后续的逻辑
	isReady = 1;
	// 当LATCH下降沿到来的时候，锁存P1口的状态，同时把键值A输出到DATA
	key = P1;
	idx = 0;
	DATA = key & mask[idx];
}

void main(){
	GPIO_Init();
	Interrupt_Init();
	
	while(1){
		PCON |= 0x02;	// 进入掉电模式
		_nop_();
		_nop_();
		_nop_();
		_nop_();
	}
}
