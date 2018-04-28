#include <INTRINS.H>
#include "STC15W.h"


/*

*---------------------------*
|1 (GPIO2)	 \__/  (GPIO1)16|
|							|
|2 (GPIO3)		   (GPIO0)15|
|							|
|3 (GPIO4)		   (LATCH)14|
|							|
|4 (GPIO5)			(DATA)13|
|		  STC15W204S		|
|5 (GPIO6)				  12|
|							|
|6 (VCC)			 (CLK)11|
|		  					|
|7 (GPIO7)			 (TXD)10|
|							|
|8 (GND)			 (RXD) 9|
*---------------------------*

*/


sbit CLK	= P3^2;
sbit DATA	= P3^6;
sbit LATCH	= P3^7;


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
	// INT0中断触发类型为上升沿
	
}

void main(){
	GPIO_Init();
	Interrupt_Init();
	
	while(1){
		
	}
}
