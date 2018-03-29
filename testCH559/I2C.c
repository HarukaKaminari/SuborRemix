#include <INTRINS.h>
#include <stdio.h>
#include "I2C.h"
#include <CH559.h>
#include "GPIO.h"


/*
	I2C.c
	标准80C51单片机模拟I2C总线的主机程序
	本程序仅供学习参考，不提供任何可靠性方面的担保；请勿用于商业目的
说明：
	这是一个用C51模拟I2C总线协议的精简版本
	只考虑一主多从模式，不考虑多主模式
	不考虑I2C总线协议中的时钟同步问题
	要想更清楚地了解本程序的细节，请参考Philips公司的相关协议标准
用法：
	1. 将文件I2C.h和I2C.c一起复制到您的工程文件夹下
	2. 根据实际单片机的型号，更换SFR的头文件<reg52.h>
	3. 根据实际电路，修改I2C总线两根信号线SCL和SDA的定义
	4. 通过宏定义I2C_DELAY_VALUE调整I2C总线的速度使其符合实际需要
	5. 把文件I2C.c添加进工程中，在需要的地方包含头文件I2C.h
	6. 在main()函数的开始处，要调用一次初始化函数I2C_Init()
	7. I2C_Puts()和I2C_Gets()是I2C总线综合读写函数，请看清注释后再使用
	8. 所有全局性质的标识符都以I2C_开头，可有效避免命名冲突问题
	9. 注意：从机地址采用7位纯地址表示，不含读写位，即最高位无效
*/

// 定义I2C总线时钟的延时值，要根据实际情况修改，取值1～255
// 1T单片机@30MHz主频下，只要这个值超过33，那么延时时间就超过了4.7uS
// 考虑到1T单片机IO口可能来不及反应变量的改变，以及器件电容和分布电容造成的波形边缘不陡峭，这个值应该略大于计算值
#define I2C_DELAY_VALUE		64

// 定义I2C总线停止后在下一次开始之前的等待时间，取值1～65535
// 等待时间为(I2C_STOP_WAIT_VALUE*5)/主频uS
// 对于多数器件取值为1即可；但对于某些器件来说，较长的延时是必须的
#define I2C_STOP_WAIT_VALUE		1

// 定义I2C主机等待从机响应的超时时间，取值1~65535
#define I2C_WAIT_ACK_TIMEOUT	255


// 模拟I2C总线的引脚定义
sbit I2C_SCL = P0^2;
sbit I2C_SDA = P0^3;


/*
函数：I2C_Delay()
功能：延时，模拟I2C总线专用
说明：这个函数延时时间为((6+2+I2C_DELAY_VALUE*4+4)/主频)uS
*/
static void I2C_Delay() small{
	u8 I2C_Delay_t = (I2C_DELAY_VALUE);
	while(--I2C_Delay_t != 0);
}

/*
函数：I2C_Init()
功能：I2C总线初始化，使总线处于空闲状态
说明：在main()函数的开始处，通常应当要执行一次本函数
*/
void I2C_Init() small{
	// 将I2C_SCL和I2C_SDA设置成开漏
	GPIO_SelMode(0, 5, 2);
	GPIO_SelMode(0, 5, 3);
	
	I2C_SCL = 1;
	I2C_Delay();
	I2C_SDA = 1;
	I2C_Delay();
}

/*
函数：I2C_Start()
功能：产生I2C总线的起始状态
说明：
	SCL处于高电平期间，当SDA出现下降沿时启动I2C总线
	不论SDA和SCL处于什么电平状态，本函数总能正确产生起始状态
	本函数也可以用来产生重复起始状态
	本函数执行后，I2C总线处于忙状态
*/
void I2C_Start() small{
	I2C_SDA = 1;
	I2C_Delay();
	I2C_SCL = 1;
	I2C_Delay();
	I2C_SDA = 0;
	I2C_Delay();
	I2C_SCL = 0;
	I2C_Delay();
}

/*
函数：I2C_Write()
功能：向I2C总线写1个字节的数据
参数：
	dat：要写到总线上的数据
*/ 
void I2C_Write(u8 dat) small{
	u8 t = 8;
	do{
		I2C_SDA = (bit)(dat & 0x80);
		I2C_Delay();
		I2C_SCL = 1;
		I2C_Delay();
		I2C_SCL = 0;
		I2C_Delay();
		dat <<= 1;
	}while(--t != 0);
}

/*
函数：I2C_Read()
功能：从从机读取1个字节的数据
返回：读取的一个字节数据
*/
u8 I2C_Read() small{
	u8 dat;
	u8 t = 8;
	I2C_SDA = 1;	// 在读取数据之前，要把SDA拉高
	I2C_Delay();
	do{
		I2C_SCL = 1;		// 主机释放SCL线，等待从机
		I2C_Delay();
		while(!I2C_SCL);	// 等待从机释放SCL。从机释放SCL代表从机已经将数据在SDA线上准备好，可以立刻读取
		dat <<= 1;
		dat |= I2C_SDA;
		I2C_SCL = 0;
		I2C_Delay();
	}while(--t != 0);
	return dat;
}

/*
函数：I2C_GetAck()
功能：读取从机应答位
返回：
	0：从机应答
	1：从机非应答
说明：
	从机在收到总线上的地址后，如果和自己的地址相同，则产生应答位
	从机在收到每个字节的数据后，要产生应答位
	从机在收到最后1个字节的数据后，一般要产生非应答位
*/
bit I2C_GetAck() small{
	u16 timeout = I2C_WAIT_ACK_TIMEOUT;
	I2C_SDA = 1;
	I2C_Delay();
	I2C_SCL = 1;
	I2C_Delay();
	while(I2C_SDA){
		timeout--;
		if(timeout==0){
			//printf("Not Acknowledged.\r\n");
			return 1;
		}
	}
	I2C_SCL = 0;
	I2C_Delay();
	return 0;
}

/*
函数：I2C_PutAck()
功能：主机产生应答位或非应答位
参数：
	ack=0：主机产生应答位
	ack=1：主机产生非应答位
说明：
	主机在接收完每一个字节的数据后，都应当产生应答位
	主机在接收完最后一个字节的数据后，应当产生非应答位
*/
void I2C_PutAck(bit ack) small{
	I2C_SDA = ack;
	I2C_Delay();
	I2C_SCL = 1;
	I2C_Delay();
	I2C_SCL = 0;
	I2C_Delay();
}

/*
函数：I2C_Stop()
功能：产生I2C总线的停止状态
说明：
	SCL处于高电平期间，当SDA出现上升沿时停止I2C总线
	不论SDA和SCL处于什么电平状态，本函数总能正确产生停止状态
	本函数执行后，I2C总线处于空闲状态
*/
void I2C_Stop() small{
	u16 t = I2C_STOP_WAIT_VALUE;
	I2C_SDA = 0;
	I2C_Delay();
	I2C_SCL = 1;
	I2C_Delay();
	I2C_SDA = 1;
	I2C_Delay();
	while(--t != 0);	// 在下一次产生Start之前，要加一定的延时
}

/*
函数：CheckI2CSlaveValidity()
功能：检查总线上所有的I2C器件，输出合法的器件地址
说明：
	主机企图向器件写地址，如果器件产生应答，则说明该器件合法
*/
void CheckI2CSlaveValidity() small{
	u8 i = 0;
	printf("Start checking slaves!\r\n");
	I2C_Init();
	while(1){
		bit result;
		I2C_Start();
		I2C_Write(i);
		result = I2C_GetAck();
		if(result == 0){
			printf("Slave %d is valid.\r\n", (int)i);
		}
		I2C_Stop();
		i++;
		if(i == 0)break;
	}
	printf("Finished checking slaves!\r\n");
}

/*
函数：I2C_Puts()
功能：I2C总线综合发送函数，向从机发送多个字节的数据
参数：
	SlaveAddr：从机地址（7位纯地址，不含读写位）
	SubAddr：从机的子地址
	SubMod：子地址模式，0－无子地址，1－单字节子地址，2－双字节子地址
	*dat：要发送的数据
	Size：数据的字节数
返回：
	0：发送成功
	1：在发送地址过程中出现异常
	-1：在发送数据过程中出现异常
说明：
	本函数能够很好地适应所有常见的I2C器件，不论其是否有子地址
	当从机没有子地址时，参数SubAddr任意，而SubMod应当为0
*/
s8 I2C_Puts(u8 SlaveAddr, u16 SubAddr, u8 SubMod, u8* dat, u16 Size) small{
	// 定义临时变量
	u8 i;
	u8 a[3];
	// 检查长度
	if(Size == 0)return 0;
	// 准备从机地址
	a[0] = (SlaveAddr << 1);
	// 检查子地址模式
	if(SubMod > 2)SubMod = 2;
	// 确定子地址
	switch(SubMod){
	case 0:
		break;
	case 1:
		a[1] = (u8)(SubAddr & 0xFF);
		break;
	case 2:
		a[1] = (u8)((SubAddr >> 8) & 0xFF);
		a[2] = (u8)(SubAddr & 0xFF);
		break;
	default:
		break;
	}
	// 发送从机地址，接着发送子地址（如果有子地址的话）
	SubMod++;
	I2C_Start();
	for(i=0;i<SubMod;++i){
		I2C_Write(a[i]);
		if(I2C_GetAck()){
			I2C_Stop();
			return 1;
		}
	}
	// 发送数据
	do{
		I2C_Write(*dat++);
		if(I2C_GetAck())break;
	}while(--Size != 0);
	// 发送完毕，停止I2C总线，并返回结果
	I2C_Stop();
	if(Size == 0){ 
		return 0;
	}else{
		return -1;
	}
}

/*
函数：I2C_Gets()
功能：I2C总线综合接收函数，从从机接收多个字节的数据
参数：
	SlaveAddr：从机地址（7位纯地址，不含读写位）
	SubAddr：从机的子地址
	SubMod：子地址模式，0－无子地址，1－单字节子地址，2－双字节子地址
	*dat：保存接收到的数据
	Size：数据的字节数
返回：
	0：接收成功
	1：在接收过程的发送地址时出现异常
说明：
	本函数能够很好地适应所有常见的I2C器件，不论其是否有子地址
	当从机没有子地址时，参数SubAddr任意，而SubMod应当为0
*/
s8 I2C_Gets(u8 SlaveAddr, u16 SubAddr, u8 SubMod, u8* dat, u16 Size) small{
	// 定义临时变量
	u8 i;
	u8 a[3];
	// 检查长度
	if(Size == 0)return 0;
	// 准备从机地址
	a[0] = (SlaveAddr << 1);
	// 检查子地址模式
	if(SubMod > 2)SubMod = 2;
	// 如果是有子地址的从机，则要先发送从机地址和子地址
	if(SubMod != 0){
		//确定子地址
		if(SubMod == 1){
			a[1] = (u8)(SubAddr & 0xFF);
		}else{
			a[1] = (u8)((SubAddr >> 8) & 0xFF);
			a[2] = (u8)(SubAddr & 0xFF);
		}
		// 发送从机地址，接着发送子地址
		SubMod++;
		I2C_Start();
		for(i=0;i<SubMod;++i){
			I2C_Write(a[i]);
			if(I2C_GetAck()){
				I2C_Stop();
				return 1;
			}
		}
	}
	// 这里的I2C_Start()对于有子地址的从机是重复起始状态
	// 对于无子地址的从机则是正常的起始状态
	I2C_Start();
	// 发送从机地址
	I2C_Write(a[0] + 1);
	if(I2C_GetAck()){
		I2C_Stop();
		return 1;
	}
	// 接收数据
	for(;;){
		*dat++ = I2C_Read();
		if(--Size == 0){
			I2C_PutAck(1);
			break;
		}
		I2C_PutAck(0);
	}
	// 接收完毕，停止I2C总线，并返回结果
	I2C_Stop();
	return 0;
}
