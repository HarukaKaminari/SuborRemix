#include "STDIO.h"
#include "SYSTEM.h"
#include <CH559.h>


void STDIO_Init() small{
	UINT32 x;
    UINT8 x2; 

    SM0 = 0;
    SM1 = 1;
    SM2 = 0;                                                                   //����0ʹ��ģʽ1
                                                                               //ʹ��Timer1��Ϊ�����ʷ�����
    RCLK = 0;                                                                  //UART0����ʱ��
    TCLK = 0;                                                                  //UART0����ʱ��
    PCON |= SMOD;
    x = 10 * FREQ_SYS / 57600 / 16;                                            //���������Ƶ��ע��x��ֵ��Ҫ���                            
    x2 = x % 10;
    x /= 10;
    if ( x2 >= 5 ) x ++;                                                       //��������

    TMOD = TMOD & ~ bT1_GATE & ~ bT1_CT & ~ MASK_T1_MOD | bT1_M1;              //0X20��Timer1��Ϊ8λ�Զ����ض�ʱ��
    T2MOD = T2MOD | bTMR_CLK | bT1_CLK;                                        //Timer1ʱ��ѡ��
    TH1 = 0-x;                                                                 //12MHz����,buad/12Ϊʵ�������ò�����
    TR1 = 1;                                                                   //������ʱ��1
    TI = 1;
    REN = 1;                                                                   //����0����ʹ��
}

void STDIO_SetUART0ToP0() small{
	PORT_CFG |= bP0_OC;
    P0_DIR |= bTXD_;
    P0_PU |= bTXD_ | bRXD_;
    PIN_FUNC |= bUART0_PIN_X;	// ����ӳ�䵽P0.2��P0.3
}