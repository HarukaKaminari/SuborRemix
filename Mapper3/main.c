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
bit isReadyToHalt = 1;	// ͣ����־���ϵ�����ͣ��


static void GPIO_Init(){
	// P3������Ϊ׼˫��Ĭ������ߵ�ƽ��׼˫���д1���ܶ��ⲿ״̬��
	P3M0 = 0;
	P3M1 = 0;
	P3 = 0xFF;
	// P5������Ϊ׼˫��Ĭ������͵�ƽ
	P5M0 = 0;
	P5M1 = 0;
	P5 = 0;
}

static void Interrupt_Init(){
	// INT0�жϴ�������Ϊ�½��ش���
	IT0 = 1;
	// ����INT0�ж�
	EX0 = 1;
	// ����ȫ���ж�
	EA = 1;
}

static void INT0_Isr() interrupt 0{
	// �ⲿ�ж�0������˵��R//W����д�ź�
	// ���/ROMSELΪ�͵�ƽ��˵����ʱ����дROM����mapper��bank����
	if(ROMSEL == 0){
		// ���̽�D0��D1ֵ�����A13��A14
		P5 = P3 << 4;
	}
	// ���̽���ͣ��ģʽ������ͣ��ģʽ֮��IO�ڱ���ͣ��֮ǰ��״̬�����Զ�����
	isReadyToHalt = 1;
}

void main(){
	GPIO_Init();
	Interrupt_Init();
	while(1){
		// ��⵽ͣ����־
		if(isReadyToHalt == 1){
			// ����ͣ��ģʽ
			PCON |= 0x02;
			_nop_();
			_nop_();
			_nop_();
			_nop_();
			// ���ѣ����־λ
			isReadyToHalt = 0;
		}
	}
}
