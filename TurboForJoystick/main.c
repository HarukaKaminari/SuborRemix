#include <INTRINS.H>
#include "STC15W.h"


/*
 --------------------------
|1 (OUT)	\__/   (INT1) 8|
|						   |
|2 (VCC)		   (INT0) 7|
|		 STC15W204S		   |
|3 (LED)	 		(TXD) 6|
|						   |
|4 (GND)	 		(RXD) 5|
 --------------------------

LED ---|<|---[===]--- VCC
	   Red	  330

Fosc = 6MHz
*/


// 脉冲输出脚
sbit OUT = P5^4;
// LED引脚
sbit LED = P5^5;

// 停机标志位
bit isReadyToHalt = 0;

// 方波次数累计
static volatile unsigned char count = 0;


static void GPIO_Init(){
	P3M0 = 0;
	P3M1 = 0;
	P3 = 0;
	P5M0 = 0;
	P5M1 = 0;
	P5 = 0;
}

static void Timer0_Init(){
	// 33毫秒@6.000MHz
	AUXR &= 0x7F;	// 定时器时钟12T模式
	TMOD &= 0xF0;	// 设置定时器模式
	TL0 = 0x8C;		// 设置定时初值
	TH0 = 0xBF;		// 设置定时初值
	TF0 = 0;		// 清除TF0标志
	TR0 = 1;		// 定时器0开始计时
}

static void Timer2_Init(){
	// 125毫秒@6.000MHz
	AUXR &= 0xFB;	// 定时器时钟12T模式
	T2L = 0xDC;		// 设置定时初值
	T2H = 0x0B;		// 设置定时初值
	AUXR &= 0xEF;	// 定时器2暂时不计时
}

static void Interrupt_Init(){
	// INT0中断触发类型为下降沿
	IT0 = 1;
	// 允许INT0中断
	EX0 = 1;
	// 允许定时器0中断
	ET0 = 1;
	// INT1中断触发类型为下降沿
	IT1 = 1;
	// 允许INT1中断
	EX1 = 1;
	// 允许定时器2中断
	IE2 |= 0x08;//ET2 = 1;
	// 允许全局中断
	EA = 1;
}

static void INT0_Isr() interrupt 0{
	// 方波次数累计归零
	count = 0;
}

static void Timer0_Isr() interrupt 1{
	// 输出方波
	OUT = ~OUT;
	// 累计方波次数
	count++;
	// 超过250个方波，准备停机
	if(count > 250){
		count = 0;
		isReadyToHalt = 1;
	}
}

static void INT1_Isr() interrupt 2{
	// 方波次数累计归零
	count = 0;
}

static void Timer2_Isr() interrupt 12{
	// 输出方波
	LED = ~LED;
}

void main(){
	// 初始化P3和P5口
	GPIO_Init();
	// 初始化定时器0
	Timer0_Init();
	// 初始化定时器2
	Timer2_Init();
	// 初始化中断
	Interrupt_Init();
	
	while(1){
		// 检测电量
		if(PCON & (1 << 5)){
			// 如果电量低，允许LED闪烁
			AUXR |= 0x10;
			// 清电量检测硬件标志位
			PCON &= (1 << 5);
		}else{
			// 电量正常，停止LED闪烁并且LED灭
			AUXR &= 0xEF;
			LED = 1;
		}
		
		// 检测到停机标志
		if(isReadyToHalt == 1){
			// 暂时关闭定时器0中断
			ET0 = 0;
			// 停机之前先把脉冲输出脚置低电平，以方便INT0和INT1唤醒CPU
			OUT = 0;
			// 进入停机模式
			PCON |= 0x02;
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			// 唤醒，清标志位
			isReadyToHalt = 0;
			// 重开定时器0中断
			ET0 = 1;
		}
	}
}
