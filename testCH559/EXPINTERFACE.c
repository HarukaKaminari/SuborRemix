#include "EXPINTERFACE.h"
#include "GPIO.h"
#include "KEYBOARD.h"


// ����NO$NES�ĵ���˵�����ϰ�FC���ֱ��ǲ���ȡ�µģ����Ե������ֱ�ֻ�ܲ嵽��չ�ڡ�
// ������չ��ȱ��J1D0�źţ����Ե������ֱ�ֻ����Ϊ2P�ֱ�������3P/4P�ֱ���
// ����ԭ�ģ�the 15pin connector lacks the joypad 1 signal, so at best, they could be shortcut with the joypad 2 signal, or wired as joypad 3/4; aside from 3/4-player games, many 1/2-player games also support that kind of input
// ע��ؼ��ʣ���ӵĵ������ֱ����ǿ�����Ϊ2P�ֱ��ģ�
// �ٿ��ǵ��°�FC�������ֱ����ǿ��԰������ģ����Եó����½��ۣ�
// 1. J1D0��J2D0�������˿��������գ������յ�Ч�ڸߵ�ƽ���ް������£���
// �ɼ��������˿��ڲ����������衣
// 2. �ֱ���DATA������ǿ�©����ṹ�������������衣��������2P�ֱ������ͬ��ƽ��ʱ��ͻ���оƬ��
// ��Ϊֻ�п�©�ſ���ʵ�֡����롱����������ṹ��GPIO����ֱ�Ӱ������豸������һ��
// �������Ͻ��ۣ�J1D0��J2D0�������˿ڿ��Բ����������衣
// �ٸ���FC��·ͼ����֤���ҵ��ƶϡ�J1D0~J1D1��J2D0~J2D4�����������裬���������Բ�ʹ��������
/*
OUT0	Port -> MCU		��������				P2.5
OUT1	Port -> MCU		��������				P2.6
OUT2	Port -> MCU		��������				P2.7

J1CLK	Port -> MCU		��������				P4.6������λѰַ��
J1D0	MCU -> Port		��©�����������		P3.2
J1D1	MCU -> Port		��©�����������		P3.4
J1D2	NC
J1D3	NC
J1D4	NC

J2CLK	Port -> MCU		��������				P4.7������λѰַ��
J2D0	MCU -> Port		��©�����������		P1.2
J2D1	MCU -> Port		��©�����������		P1.4
J2D2	MCU -> Port		��©�����������		P1.5
J2D3	MCU -> Port		��©�����������		P1.6
J2D4	MCU -> Port		��©�����������		P1.7
*/
sbit OUT0 = P2^5;
sbit OUT1 = P2^6;
sbit OUT2 = P2^7;
sbit J1D0 = P3^2;
sbit J1D1 = P3^4;
sbit J2D0 = P1^2;
sbit J2D1 = P1^4;
sbit J2D2 = P1^5;
sbit J2D3 = P1^6;
sbit J2D4 = P1^7;
#define JOYCLK	P4_IN

static u8 data keyValue_1P;
static u8 data keyValue_2P;
static u8 data keyValue_3P;
static u8 data keyValue_4P;
static u8 data keyValue_Keyboard;

static u8 data Register_1P = 0;	// 1P�ֱ��Ĵ���
static u8 data Register_2P = 0;	// 2P�ֱ��Ĵ���
static u8 data Register_3P = 0;	// 3P�ֱ��Ĵ���
static u8 data Register_4P = 0;	// 4P�ֱ��Ĵ���

bit OUT0_curr = 0;
bit OUT1_curr = 0;
bit OUT2_curr = 0;
bit J1CLK_curr = 0;
bit J2CLK_curr = 0;
bit OUT0_prev = 0;
bit OUT1_prev = 0;
bit OUT2_prev = 0;
bit J1CLK_prev = 0;
bit J2CLK_prev = 0;

bit isReady = 0;	// �Ƿ��⵽�������ź�


static void EXPINTERFACE_RefreshOUT0() small{
	OUT0_curr = OUT0;
}
static bit EXPINTERFACE_IsRisingEdgeOfOUT0() small{
	return OUT0_curr == 1 && OUT0_prev == 0;
}
static bit EXPINTERFACE_IsFallingEdgeOfOUT0() small{
	return OUT0_curr == 0 && OUT0_prev == 1;
}
static void EXPINTERFACE_UpdateOUT0() small{
	OUT0_prev = OUT0_curr;
}

static void EXPINTERFACE_RefreshJ1CLK() small{
	J1CLK_curr = (JOYCLK >> 6) & 1;
}
static bit EXPINTERFACE_IsRisingEdgeOfJ1CLK() small{
	return J1CLK_curr == 1 && J1CLK_prev == 0;
}
static bit EXPINTERFACE_IsFallingEdgeOfJ1CLK() small{
	return J1CLK_curr == 0 && J1CLK_prev == 1;
}
static void EXPINTERFACE_UpdateJ1CLK() small{
	J1CLK_prev = J1CLK_curr;
}

static void EXPINTERFACE_RefreshJ2CLK() small{
	J2CLK_curr = (JOYCLK >> 7) & 1;
}
static bit EXPINTERFACE_IsRisingEdgeOfJ2CLK() small{
	return J2CLK_curr == 1 && J2CLK_prev == 0;
}
static bit EXPINTERFACE_IsFallingEdgeOfJ2CLK() small{
	return J2CLK_curr == 0 && J2CLK_prev == 1;
}
static void EXPINTERFACE_UpdateJ2CLK() small{
	J2CLK_prev = J2CLK_curr;
}

// �����뻺����������1P����״̬
static void EXPINTERFACE_Set1PKeyStatus() small{
	u8 i = 0;
	// A��B��Start��Select��Up��Down��Left��Right
	static const u8 code p[] = {0x07, 0x04, 0x16, 0x1A, 0x2C, 0x28, 0x0D, 0x0E};
	u8* keyCodeBuf = KEYBOARD_GetBuf();
	keyValue_1P = 0;
	for(i=0;i<8;++i){
		keyValue_1P |= keyCodeBuf[p[i]];
		keyValue_1P <<= 1;
	}
}

// �����뻺����������2P����״̬
static void EXPINTERFACE_Set2PKeyStatus() small{
	u8 i = 0;
	// A��B��Start��Select��Up��Down��Left��Right
	static const u8 code p[] = {0x4F, 0x50, 0x51, 0x52, 0x62, 0x58, 0x59, 0x5A};
	u8* keyCodeBuf = KEYBOARD_GetBuf();
	keyValue_2P = 0;
	for(i=0;i<8;++i){
		keyValue_2P |= keyCodeBuf[p[i]];
		keyValue_2P <<= 1;
	}
}

// �����뻺����������3P����״̬
static void EXPINTERFACE_Set3PKeyStatus() small{
	u8 i = 0;
	// A��B��Start��Select��Up��Down��Left��Right
	static const u8 code p[] = {0x07, 0x04, 0x16, 0x1A, 0x2C, 0x28, 0x0D, 0x0E};
	u8* keyCodeBuf = KEYBOARD_GetBuf();
	keyValue_3P = 0;
	for(i=0;i<8;++i){
		keyValue_3P |= keyCodeBuf[p[i]];
		keyValue_3P <<= 1;
	}
}

// �����뻺����������4P����״̬
static void EXPINTERFACE_Set4PKeyStatus() small{
	u8 i = 0;
	// A��B��Start��Select��Up��Down��Left��Right
	static const u8 code p[] = {0x4F, 0x50, 0x51, 0x52, 0x62, 0x58, 0x59, 0x5A};
	u8* keyCodeBuf = KEYBOARD_GetBuf();
	keyValue_4P = 0;
	for(i=0;i<8;++i){
		keyValue_4P |= keyCodeBuf[p[i]];
		keyValue_4P <<= 1;
	}
}

// ��ʼ������GPIO�����������Ƿ����ø��豸
void EXPINTERFACE_Init(bit isEnable) small{
	u8 i = 0;
	if(isEnable){
		// P1�����óɿ�©�����������
		for(i=0;i<8;++i)GPIO_SelMode(1, 3, i);
		// P2�����óɸ������룬������
		for(i=0;i<8;++i)GPIO_SelMode(2, 0, i);
		// P3�����óɿ�©�����������
		for(i=0;i<8;++i)GPIO_SelMode(3, 3, i);
		// P4�����óɸ������룬������
		for(i=0;i<8;++i)GPIO_SelP4Mode(i, 0, 0);
	}else{
		// P1�����óɸ������룬������
		for(i=0;i<8;++i)GPIO_SelMode(1, 0, i);
		// P2�����óɸ������룬������
		for(i=0;i<8;++i)GPIO_SelMode(2, 0, i);
		// P3�����óɸ������룬������
		for(i=0;i<8;++i)GPIO_SelMode(3, 0, i);
		// P4�����óɸ������룬������
		for(i=0;i<8;++i)GPIO_SelP4Mode(i, 0, 0);
	}
	// P1~P4����������Ϊ5mA
	GPIO_DrivCap(1, 0);
	GPIO_DrivCap(2, 0);
	GPIO_DrivCap(3, 0);
	GPIO_DrivCap(4, 0);
}

// ��ʼ��Mode0��������ģʽ�£�USB������ȫ�����Σ����ж˿ھ�Ϊ����
void EXPINTERFACE_InitMode0() small{
	EXPINTERFACE_Init(0);
}

void EXPINTERFACE_UpdateMode0() small{
	
}

// ��ʼ��Mode1��������ģʽ�£�USB���̱���Ϊ1P��2P�ֱ���
// ע�⣬�����ʵ1P�ֱ��ͼ�������1P�ֱ�������ʵ2P�ֱ��ͼ�������2P�ֱ�������Ļ���
// ֻҪ������һ���ֱ�����͵�ƽ��ֵ�������£�������һ���ֱ�������ʲô��ƽ�����ᱻ���͡�
// ��͵����������ֱ���ͬ��ֵ�����ǡ��򡱹�ϵ����ֻҪ��һ���ֱ��������£��Ǹ������ͻᱻ��Ϊ���¡�
// �ɴ˿ɼ�������1P2P�ֱ����Ժ���ʵ�ֱ����棬ͬʱ������
void EXPINTERFACE_InitMode1() small{
	EXPINTERFACE_Init(1);
	Register_1P = 0;
	Register_2P = 0;
	OUT0_curr = 0;
	J1CLK_curr = 0;
	J2CLK_curr = 0;
	OUT0_prev = 0;
	J1CLK_prev = 0;
	J2CLK_prev = 0;
	
	isReady = 0;
}

void EXPINTERFACE_UpdateMode1() small{
	// ���1P��2P����״̬
	EXPINTERFACE_Set1PKeyStatus();
	EXPINTERFACE_Set2PKeyStatus();
	// ��OUT0�������źţ�
	EXPINTERFACE_RefreshOUT0();
	// ��J1CLK��1Pʱ�ӣ�
	EXPINTERFACE_RefreshJ1CLK();
	// ��J2CLK��2Pʱ�ӣ�
	EXPINTERFACE_RefreshJ2CLK();
	
	// ����Ƿ���������ź�
	if(EXPINTERFACE_IsRisingEdgeOfOUT0()){
		// ���������źţ����������߼�
		isReady = 1;
	}
	if(isReady){
		// ����Ƿ��������
		if(EXPINTERFACE_IsFallingEdgeOfOUT0()){
			// �����źŽ������Ѱ���״̬���浽�����ֱ��Ĵ�����
			Register_1P = keyValue_1P;
			Register_2P = keyValue_2P;
			// ����ļ�ֵ���̳����ڴ������ݶ�
			J1D0 = Register_1P & 1;
			J2D0 = Register_2P & 1;
			// ������һ��״̬������֮�󣬰���״̬����Ӱ���ѱ�����İ����Ĵ���
		}
		// ����Ƿ����1Pʱ��������
		if(EXPINTERFACE_IsRisingEdgeOfJ1CLK()){
			// 1P�ֱ��Ĵ�����λ
			Register_1P <<= 1;
			// ����ļ�ֵ���̳����ڴ������ݶ�
			J1D0 = Register_1P & 1;
			// ������һ��״̬
		}
		// ����Ƿ����1Pʱ���½���
		if(EXPINTERFACE_IsFallingEdgeOfJ1CLK()){
			// ���״̬��ûʲô������
		}
		// ����Ƿ����2Pʱ��������
		if(EXPINTERFACE_IsRisingEdgeOfJ2CLK()){
			// 2P�ֱ��Ĵ�����λ
			Register_2P <<= 1;
			// ����ļ�ֵ���̳����ڴ������ݶ�
			J2D0 = Register_2P & 1;
			// ������һ��״̬
		}
		// ����Ƿ����2Pʱ���½���
		if(EXPINTERFACE_IsFallingEdgeOfJ2CLK()){
			// ���״̬��ûʲô������
		}
	}
	
	// ���������ź�״̬
	EXPINTERFACE_UpdateOUT0();
	// ����1Pʱ���ź�״̬
	EXPINTERFACE_UpdateJ1CLK();
	// ����2Pʱ���ź�״̬
	EXPINTERFACE_UpdateJ2CLK();
}

// ��ʼ��Mode2��������ģʽ�£�USB���̱���Ϊ3P��4P�ֱ���
void EXPINTERFACE_InitMode2() small{
	EXPINTERFACE_Init(1);
	Register_3P = 0;
	Register_4P = 0;
	OUT0_curr = 0;
	J1CLK_curr = 0;
	J2CLK_curr = 0;
	OUT0_prev = 0;
	J1CLK_prev = 0;
	J2CLK_prev = 0;
	
	isReady = 0;
}

void EXPINTERFACE_UpdateMode2() small{
	// ���3P��4P����״̬
	EXPINTERFACE_Set3PKeyStatus();
	EXPINTERFACE_Set4PKeyStatus();
	// ��OUT0�������źţ�
	EXPINTERFACE_RefreshOUT0();
	// ��J1CLK��3Pʱ�ӣ�
	EXPINTERFACE_RefreshJ1CLK();
	// ��J2CLK��4Pʱ�ӣ�
	EXPINTERFACE_RefreshJ2CLK();
	
	// ����Ƿ���������ź�
	if(EXPINTERFACE_IsRisingEdgeOfOUT0()){
		// ���������źţ����������߼�
		isReady = 1;
	}
	if(isReady){
		// ����Ƿ��������
		if(EXPINTERFACE_IsFallingEdgeOfOUT0()){
			// �����źŽ������Ѱ���״̬���浽�����ֱ��Ĵ�����
			Register_3P = keyValue_3P;
			Register_4P = keyValue_4P;
			// ����ļ�ֵ���̳����ڴ������ݶ�
			J1D1 = Register_3P & 1;
			J2D1 = Register_4P & 1;
			// ������һ��״̬������֮�󣬰���״̬����Ӱ���ѱ�����İ����Ĵ���
		}
		// ����Ƿ����3Pʱ��������
		if(EXPINTERFACE_IsRisingEdgeOfJ1CLK()){
			// 3P�ֱ��Ĵ�����λ
			Register_3P <<= 1;
			// ����ļ�ֵ���̳����ڴ������ݶ�
			J1D1 = Register_3P & 1;
			// ������һ��״̬
		}
		// ����Ƿ����3Pʱ���½���
		if(EXPINTERFACE_IsFallingEdgeOfJ1CLK()){
			// ���״̬��ûʲô������
		}
		// ����Ƿ����4Pʱ��������
		if(EXPINTERFACE_IsRisingEdgeOfJ2CLK()){
			// 4P�ֱ��Ĵ�����λ
			Register_4P <<= 1;
			// ����ļ�ֵ���̳����ڴ������ݶ�
			J2D1 = Register_4P & 1;
			// ������һ��״̬
		}
		// ����Ƿ����4Pʱ���½���
		if(EXPINTERFACE_IsFallingEdgeOfJ2CLK()){
			// ���״̬��ûʲô������
		}
	}
	
	// ���������ź�״̬
	EXPINTERFACE_UpdateOUT0();
	// ����3Pʱ���ź�״̬
	EXPINTERFACE_UpdateJ1CLK();
	// ����4Pʱ���ź�״̬
	EXPINTERFACE_UpdateJ2CLK();
}