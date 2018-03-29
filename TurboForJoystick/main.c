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


// 矩形波输出脚
sbit OUT = P5^4;
// 低压指示灯引脚
sbit LED = P5^5;

// 停机标志位
bit isReadyToHalt = 0;

// 矩形波次数累计
static volatile unsigned char count = 0;


static void GPIO_Init(){
	// P3口设置为准双向，默认输出高电平
	P3M0 = 0;
	P3M1 = 0;
	P3 = 0xFF;
	// P5口设置为准双向，默认输出高电平
	P5M0 = 0;
	P5M1 = 0;
	P5 = 0xFF;
}

static void Timer0_Init(){
	// 16毫秒@6.000MHz
	AUXR &= 0x7F;	// 定时器时钟12T模式
	TMOD &= 0xF0;	// 设置定时器模式
	TL0 = 0xC0;		// 设置定时初值
	TH0 = 0xE0;		// 设置定时初值
	TF0 = 0;		// 清除TF0标志
	TR0 = 1;		// 定时器0开始计时
}

static void Interrupt_Init(){
	// INT0中断触发类型为下降沿触发
	IT0 = 1;
	// 允许INT0中断
	EX0 = 1;
	// INT1中断触发类型为下降沿触发
	IT1 = 1;
	// 允许INT1中断
	EX1 = 1;
	// 允许定时器0中断
	ET0 = 1;
	// 允许全局中断
	EA = 1;
}

static void INT0_Isr() interrupt 0{
	// 矩形波次数累计归零
	count = 0;
	// 重开定时器0中断
	ET0 = 1;
}

static void Timer0_Isr() interrupt 1{
	// 输出矩形波
	OUT = ~OUT;
	// 累计矩形波次数
	count++;
	// 超过250个矩形波，准备停机
	if(count > 250){
		count = 0;
		isReadyToHalt = 1;
	}
}

static void INT1_Isr() interrupt 2{
	// 矩形波次数累计归零
	count = 0;
	// 重开定时器0中断
	ET0 = 1;
}


void main(){
	// 初始化P3和P5口
	GPIO_Init();
	// 初始化定时器0
	Timer0_Init();
	// 初始化中断
	Interrupt_Init();
	
	while(1){
		// 检测电量
		if(PCON & (1 << 5)){
			// 如果电量低，低压指示灯亮
			LED = 0;
			// 清电量检测硬件标志位
			PCON &= ~(1 << 5);
		}else{
			// 电量正常，低压指示灯灭
			LED = 1;
		}

		// 检测到停机标志
		if(isReadyToHalt == 1){
			// 暂时关闭定时器0中断
			ET0 = 0;
			// 停机之前先把矩形波输出脚置低电平，以方便INT0和INT1唤醒CPU
			OUT = 0;
			// 停机之前先把低压指示灯灭掉，以省电
			LED = 1;
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
