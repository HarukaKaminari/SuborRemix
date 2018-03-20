#include "GPIO.h"


/*******************************************************************************
* Description    : 端口0、1、2、3、4驱动能力设置
* Input          : UINT8 Port端口选择(0、1、2、3、4)
                   bit Cap驱动能力选择((0)5mA、(1)20mA(注意:P1口是10mA))
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
			PORT_CFG &= ~(bP0_DRV << Port);	// 驱动电流最大5mA
	}else{
		if(Port == 4)
			P4_CFG |= 1 << bP4_DRV;
		else
			PORT_CFG |= (bP0_DRV << Port);	// 驱动电流最大20mA，P1口是10mA
	}
}

/*******************************************************************************
* Description    : 端口0、1、2、3引脚模式设置
* Input          : UINT8 Port端口选择(0、1、2、3)
                   UINT8 Cap驱动方式选择(bPn_OC & Pn_DIR & Pn_PU)
                   0(000)：仅输入，无上拉；
                   1(001)：仅输入，带上拉；
                   2(01x)：推挽输出，高低电平强驱动；
                   3(100)：开漏输出，无上拉，支持输入；
                   4(110)：开漏输出，无上拉,当转变输出由低到高时，仅驱动2个时钟的高电平
                   5(101)：准双向(标准51模式)，开漏输出，带上拉
                   6(111)：准双向(标准51模式)，开漏输出，带上拉，当转变输出由低到高时，仅驱动2个时钟的高电平
                   UINT8 PinNum(引脚选择0-7)
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_SelMode(UINT8 Port, UINT8 Mode, UINT8 PinNum){
	UINT8 Pn_DIR, Pn_PU;
	if(Port >= 4){
		return;
	}
	switch(Mode){
		case 0:								// 仅输入，无上拉
			PORT_CFG &= ~(bP0_OC << Port);
			Pn_DIR &= ~(1 << PinNum);
			Pn_PU &= ~(1 << PinNum);
		break;
		case 1:								// 仅输入，带上拉
			PORT_CFG &= ~(bP0_OC << Port);
			Pn_DIR &= ~(1 << PinNum);
			Pn_PU |= 1 << PinNum;
		break;
		case 2:								// 推挽输出，高低电平强驱动
			PORT_CFG &= ~(bP0_OC << Port);
			Pn_DIR |= ~(1 << PinNum);
		break;
		case 3:								// 开漏输出，无上拉，支持输入
			PORT_CFG |= (bP0_OC << Port);
			Pn_DIR &= ~(1 << PinNum);
			Pn_PU &= ~(1 << PinNum);
		break;
		case 4:								// 开漏输出，无上拉,当转变输出由低到高时，仅驱动2个时钟的高电平
			PORT_CFG |= (bP0_OC << Port);
			Pn_DIR |= 1 << PinNum;
			Pn_PU &= ~(1 << PinNum);
		break;
		case 5:								// 弱准双向(标准51模式)，开漏输出，带上拉
			PORT_CFG |= (bP0_OC << Port);
			Pn_DIR &= ~(1 << PinNum);
			Pn_PU |= 1 << PinNum;
		break;
		case 6:								// 准双向(标准51模式)，开漏输出，带上拉，当转变输出由低到高时，仅驱动2个时钟的高电平
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
* Description    : CH559的P4端口初始化，P4默认是输入口
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void GPIO_SelP4Mode(UINT8 PinNum, bit isOutput, bit isPullup){
	// 方向
	if(isOutput)
		P4_DIR |= 1 << PinNum;
	else
		P4_DIR &= ~(1 << PinNum);
	// 内部上拉
	if(isPullup)
		P4_PU |= 1 << PinNum;
	else
		P4_PU &= ~(1 << PinNum);
}
