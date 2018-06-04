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

P1.0~P1.7 -> �� �� �� �� SEL STA B A
*/


sbit CLK	= P3^2;
sbit LATCH	= P3^3;
sbit DATA	= P3^6;


bit isReady = 0;
static unsigned char key = 0;
static const unsigned char data mask[] = {0x80, 0x40, 0x10, 0x20, 0x01, 0x04, 0x02, 0x08};	// A B SELECT START UP DOWN LEFT RIGHT��Ϊ�����ٶȣ������ŵ�RAM��
static unsigned char idx = 0;


static void GPIO_Init(){
	// P1�����ڽ��ղ����źţ�ȫ����ʼ��Ϊ׼˫�������ڲ�����������ߵ�ƽ
	P1M0 = 0;
	P1M1 = 0;
	P1= 0xFF;
	// P3�ڳ�ʼ��Ϊ׼˫��
	P3M0 = 0;
	P3M1 = 0;
	P3 = 0xFF;
}

static void Interrupt_Init(){
	// INT0�жϣ�CLK����������Ϊ������+�½���
	IT0 = 0;
	// ����INT0�ж�
	EX0 = 1;
	// INT1�жϣ�LATCH����������Ϊ�½���
	IT1 = 1;
	// ����INT1�ж�
	EX1 = 1;
	// ����ȫ���ж�
	EA = 1;
}

static void INT0_Isr() interrupt 0{
	// ֻ���Ѿ��ɹ����������CLK��λ
	if(isReady == 0)
		return;
	// ����ǰ���ŵ�ƽ������ǵ͵�ƽ��˵�����½��أ���ʱֱ�Ӻ��Ը��ж�
	if(CLK == 0)
		return;
	// CLK�����ص�����ʱ��ȡ����ֵ����һλ���
	idx++;
	DATA = key & mask[idx];
}

static void INT1_Isr() interrupt 2{
	// ֻ���յ�LATCH�½��ص�ʱ�������������߼�
	isReady = 1;
	// ��LATCH�½��ص�����ʱ������P1�ڵ�״̬��ͬʱ�Ѽ�ֵA�����DATA
	key = P1;
	idx = 0;
	DATA = key & mask[idx];
}

void main(){
	GPIO_Init();
	Interrupt_Init();
	
	while(1){
		PCON |= 0x02;	// �������ģʽ
		_nop_();
		_nop_();
		_nop_();
		_nop_();
	}
}
