#include "GPIO.h"


/*******************************************************************************
* Description    : �˿�0��1��2��3��4������������
* Input          : UINT8 Port�˿�ѡ��(0��1��2��3��4)
                   bit Cap��������ѡ��((0)5mA��(1)20mA(ע��:P1����10mA))
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_DrivCap(UINT8 Port, bit Cap){
	if(Port > 4){
		return;
	}
	if(Cap == 0){
		if(Port == 4)
			P4_CFG &= ~(1 << bP4_DRV);
		else
			PORT_CFG &= ~(bP0_DRV << Port);	// �����������5mA
	}else{
		if(Port == 4)
			P4_CFG |= 1 << bP4_DRV;
		else
			PORT_CFG |= (bP0_DRV << Port);	// �����������20mA��P1����10mA
	}
}

/*******************************************************************************
* Description    : �˿�0��1��2��3����ģʽ����
* Input          : UINT8 Port�˿�ѡ��(0��1��2��3)
                   UINT8 Cap������ʽѡ��(bPn_OC & Pn_DIR & Pn_PU)
                   0(000)�������룬��������
                   1(001)�������룬��������
                   2(01x)������������ߵ͵�ƽǿ������
                   3(100)����©�������������֧�����룻
                   4(110)����©�����������,��ת������ɵ͵���ʱ��������2��ʱ�ӵĸߵ�ƽ
                   5(101)��׼˫��(��׼51ģʽ)����©�����������
                   6(111)��׼˫��(��׼51ģʽ)����©���������������ת������ɵ͵���ʱ��������2��ʱ�ӵĸߵ�ƽ
                   UINT8 PinNum(����ѡ��0-7)
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_SelMode(UINT8 Port, UINT8 Mode, UINT8 PinNum){
	UINT8 Pn_DIR, Pn_PU;
	if(Port >= 4){
		return;
	}
	switch(Mode){
		case 0:								// �����룬������
			PORT_CFG &= ~(bP0_OC << Port);
			Pn_DIR &= ~(1 << PinNum);
			Pn_PU &= ~(1 << PinNum);
		break;
		case 1:								// �����룬������
			PORT_CFG &= ~(bP0_OC << Port);
			Pn_DIR &= ~(1 << PinNum);
			Pn_PU |= 1 << PinNum;
		break;
		case 2:								// ����������ߵ͵�ƽǿ����
			PORT_CFG &= ~(bP0_OC << Port);
			Pn_DIR |= ~(1 << PinNum);
		break;
		case 3:								// ��©�������������֧������
			PORT_CFG |= (bP0_OC << Port);
			Pn_DIR &= ~(1 << PinNum);
			Pn_PU &= ~(1 << PinNum);
		break;
		case 4:								// ��©�����������,��ת������ɵ͵���ʱ��������2��ʱ�ӵĸߵ�ƽ
			PORT_CFG |= (bP0_OC << Port);
			Pn_DIR |= 1 << PinNum;
			Pn_PU &= ~(1 << PinNum);
		break;
		case 5:								// ��׼˫��(��׼51ģʽ)����©�����������
			PORT_CFG |= (bP0_OC << Port);
			Pn_DIR &= ~(1 << PinNum);
			Pn_PU |= 1 << PinNum;
		break;
		case 6:								// ׼˫��(��׼51ģʽ)����©���������������ת������ɵ͵���ʱ��������2��ʱ�ӵĸߵ�ƽ
			PORT_CFG |= (bP0_OC << Port);
			Pn_DIR |= 1 << PinNum;
			Pn_PU |= 1 << PinNum;
		break;
		default:
			
		break;
	}
	if(Port == 0){
		P0_DIR = Pn_DIR;
		P0_PU = Pn_PU;
	}else if(Port == 1){
		P1_DIR = Pn_DIR;
		P1_PU = Pn_PU;
	}else if(Port == 2){
		P2_DIR = Pn_DIR;
		P2_PU = Pn_PU;
	}else if(Port == 3){
		P3_DIR = Pn_DIR;
		P3_PU = Pn_PU;
	}
}

/*******************************************************************************
* Description    : CH559��P4�˿ڳ�ʼ����P4Ĭ���������
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_SelP4Mode(UINT8 PinNum, bit isOutput, bit isPullup){
	// ����
	if(isOutput)
		P4_DIR |= 1 << PinNum;
	else
		P4_DIR &= ~(1 << PinNum);
	// �ڲ�����
	if(isPullup)
		P4_PU |= 1 << PinNum;
	else
		P4_PU &= ~(1 << PinNum);
}
