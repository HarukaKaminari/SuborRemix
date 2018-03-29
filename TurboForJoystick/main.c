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


// ���β������
sbit OUT = P5^4;
// ��ѹָʾ������
sbit LED = P5^5;

// ͣ����־λ
bit isReadyToHalt = 0;

// ���β������ۼ�
static volatile unsigned char count = 0;


static void GPIO_Init(){
	// P3������Ϊ׼˫��Ĭ������ߵ�ƽ
	P3M0 = 0;
	P3M1 = 0;
	P3 = 0xFF;
	// P5������Ϊ׼˫��Ĭ������ߵ�ƽ
	P5M0 = 0;
	P5M1 = 0;
	P5 = 0xFF;
}

static void Timer0_Init(){
	// 16����@6.000MHz
	AUXR &= 0x7F;	// ��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;	// ���ö�ʱ��ģʽ
	TL0 = 0xC0;		// ���ö�ʱ��ֵ
	TH0 = 0xE0;		// ���ö�ʱ��ֵ
	TF0 = 0;		// ���TF0��־
	TR0 = 1;		// ��ʱ��0��ʼ��ʱ
}

static void Interrupt_Init(){
	// INT0�жϴ�������Ϊ�½��ش���
	IT0 = 1;
	// ����INT0�ж�
	EX0 = 1;
	// INT1�жϴ�������Ϊ�½��ش���
	IT1 = 1;
	// ����INT1�ж�
	EX1 = 1;
	// ����ʱ��0�ж�
	ET0 = 1;
	// ����ȫ���ж�
	EA = 1;
}

static void INT0_Isr() interrupt 0{
	// ���β������ۼƹ���
	count = 0;
	// �ؿ���ʱ��0�ж�
	ET0 = 1;
}

static void Timer0_Isr() interrupt 1{
	// ������β�
	OUT = ~OUT;
	// �ۼƾ��β�����
	count++;
	// ����250�����β���׼��ͣ��
	if(count > 250){
		count = 0;
		isReadyToHalt = 1;
	}
}

static void INT1_Isr() interrupt 2{
	// ���β������ۼƹ���
	count = 0;
	// �ؿ���ʱ��0�ж�
	ET0 = 1;
}


void main(){
	// ��ʼ��P3��P5��
	GPIO_Init();
	// ��ʼ����ʱ��0
	Timer0_Init();
	// ��ʼ���ж�
	Interrupt_Init();
	
	while(1){
		// ������
		if(PCON & (1 << 5)){
			// ��������ͣ���ѹָʾ����
			LED = 0;
			// ��������Ӳ����־λ
			PCON &= ~(1 << 5);
		}else{
			// ������������ѹָʾ����
			LED = 1;
		}

		// ��⵽ͣ����־
		if(isReadyToHalt == 1){
			// ��ʱ�رն�ʱ��0�ж�
			ET0 = 0;
			// ͣ��֮ǰ�ȰѾ��β�������õ͵�ƽ���Է���INT0��INT1����CPU
			OUT = 0;
			// ͣ��֮ǰ�Ȱѵ�ѹָʾ���������ʡ��
			LED = 1;
			// ����ͣ��ģʽ
			PCON |= 0x02;
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			// ���ѣ����־λ
			isReadyToHalt = 0;
			// �ؿ���ʱ��0�ж�
			ET0 = 1;
		}
	}
}
