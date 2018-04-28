#include "STC15W.h"
#include <INTRINS.H>


/**
 --------------------------
|1 (A13)	\__/(/ROMSEL) 8|
|						   |
|2 (VCC)		   (R//W) 7|
|		 STC15W204S		   |
|3 (A14)	 	 (TXD/D1) 6|
|						   |
|4 (GND)	 	 (RXD/D0) 5|
 --------------------------
 Fosc = 12MHz
*/


sbit ROMSEL	= P3^3;
bit isReadyToHalt = 1;	// 停机标志。上电立刻停机


static void GPIO_Init(){
	// P3口设置为准双向，默认输出高电平（准双向口写1才能读外部状态）
	P3M0 = 0;
	P3M1 = 0;
	P3 = 0xFF;
	// P5口设置为准双向，默认输出低电平
	P5M0 = 0;
	P5M1 = 0;
	P5 = 0;
}

static void Interrupt_Init(){
	// INT0中断触发类型为下降沿触发
	IT0 = 1;
	// 允许INT0中断
	EX0 = 1;
	// 允许全局中断
	EA = 1;
}

static void INT0_Isr() interrupt 0{
	// 外部中断0到来，说明R//W出现写信号
	// 如果/ROMSEL为低电平，说明此时正在写ROM，即mapper切bank操作
	if(ROMSEL == 0){
		// 立刻将D0和D1值输出到A13和A14
		P5 = P3 << 4;
	}
	// 立刻进入停机模式。进入停机模式之后，IO口保持停机之前的状态，即自动锁存
	isReadyToHalt = 1;
}

void main(){
	GPIO_Init();
	Interrupt_Init();
	while(1){
		// 检测到停机标志
		if(isReadyToHalt == 1){
			// 进入停机模式
			PCON |= 0x02;
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			// 唤醒，清标志位
			isReadyToHalt = 0;
		}
	}
}
