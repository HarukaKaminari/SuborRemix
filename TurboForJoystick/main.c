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


// ���������
sbit OUT = P5^4;
// LED����
sbit LED = P5^5;

// ͣ����־λ
bit isReadyToHalt = 0;

// ���������ۼ�
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
	// 33����@6.000MHz
	AUXR &= 0x7F;	// ��ʱ��ʱ��12Tģʽ
	TMOD &= 0xF0;	// ���ö�ʱ��ģʽ
	TL0 = 0x8C;		// ���ö�ʱ��ֵ
	TH0 = 0xBF;		// ���ö�ʱ��ֵ
	TF0 = 0;		// ���TF0��־
	TR0 = 1;		// ��ʱ��0��ʼ��ʱ
}

static void Timer2_Init(){
	// 125����@6.000MHz
	AUXR &= 0xFB;	// ��ʱ��ʱ��12Tģʽ
	T2L = 0xDC;		// ���ö�ʱ��ֵ
	T2H = 0x0B;		// ���ö�ʱ��ֵ
	AUXR &= 0xEF;	// ��ʱ��2��ʱ����ʱ
}

static void Interrupt_Init(){
	// INT0�жϴ�������Ϊ�½���
	IT0 = 1;
	// ����INT0�ж�
	EX0 = 1;
	// ����ʱ��0�ж�
	ET0 = 1;
	// INT1�жϴ�������Ϊ�½���
	IT1 = 1;
	// ����INT1�ж�
	EX1 = 1;
	// ����ʱ��2�ж�
	IE2 |= 0x08;//ET2 = 1;
	// ����ȫ���ж�
	EA = 1;
}

static void INT0_Isr() interrupt 0{
	// ���������ۼƹ���
	count = 0;
}

static void Timer0_Isr() interrupt 1{
	// �������
	OUT = ~OUT;
	// �ۼƷ�������
	count++;
	// ����250��������׼��ͣ��
	if(count > 250){
		count = 0;
		isReadyToHalt = 1;
	}
}

static void INT1_Isr() interrupt 2{
	// ���������ۼƹ���
	count = 0;
}

static void Timer2_Isr() interrupt 12{
	// �������
	LED = ~LED;
}

void main(){
	// ��ʼ��P3��P5��
	GPIO_Init();
	// ��ʼ����ʱ��0
	Timer0_Init();
	// ��ʼ����ʱ��2
	Timer2_Init();
	// ��ʼ���ж�
	Interrupt_Init();
	
	while(1){
		// ������
		if(PCON & (1 << 5)){
			// ��������ͣ�����LED��˸
			AUXR |= 0x10;
			// ��������Ӳ����־λ
			PCON &= (1 << 5);
		}else{
			// ����������ֹͣLED��˸����LED��
			AUXR &= 0xEF;
			LED = 1;
		}
		
		// ��⵽ͣ����־
		if(isReadyToHalt == 1){
			// ��ʱ�رն�ʱ��0�ж�
			ET0 = 0;
			// ͣ��֮ǰ�Ȱ�����������õ͵�ƽ���Է���INT0��INT1����CPU
			OUT = 0;
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
