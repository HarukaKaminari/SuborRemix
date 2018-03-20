#include "EXPINTERFACE.h"
#include "GPIO.h"
#include "KEYBOARD.h"


// 根据NO$NES文档的说明，老版FC的手柄是不可取下的，所以第三方手柄只能插到扩展口。
// 但是扩展口缺少J1D0信号，所以第三方手柄只能作为2P手柄，或者3P/4P手柄。
// 引用原文：the 15pin connector lacks the joypad 1 signal, so at best, they could be shortcut with the joypad 2 signal, or wired as joypad 3/4; aside from 3/4-player games, many 1/2-player games also support that kind of input
// 注意关键词，外接的第三方手柄，是可以作为2P手柄的！
// 再考虑到新版FC的两个手柄都是可以拔下来的，可以得出如下结论：
// 1. J1D0和J2D0这两个端口允许悬空，且悬空等效于高电平（无按键按下）。
// 可见这两个端口内部有上拉电阻。
// 2. 手柄的DATA输出端是开漏输出结构，且有上拉电阻。否则当两个2P手柄输出不同电平的时候就会烧芯片。
// 因为只有开漏才可以实现“线与”，其他输出结构的GPIO不能直接把两个设备并联在一起。
// 基于以上结论，J1D0和J2D0这两个端口可以不用上拉电阻。
// 再根据FC电路图，验证了我的推断。J1D0~J1D1、J2D0~J2D4都有上拉电阻，因此这里可以不使用上拉。
/*
OUT0	Port -> MCU		高阻输入				P2.5
OUT1	Port -> MCU		高阻输入				P2.6
OUT2	Port -> MCU		高阻输入				P2.7

J1CLK	Port -> MCU		高阻输入				P4.6（不可位寻址）
J1D0	MCU -> Port		开漏输出，无上拉		P3.2
J1D1	MCU -> Port		开漏输出，无上拉		P3.4
J1D2	NC
J1D3	NC
J1D4	NC

J2CLK	Port -> MCU		高阻输入				P4.7（不可位寻址）
J2D0	MCU -> Port		开漏输出，无上拉		P1.2
J2D1	MCU -> Port		开漏输出，无上拉		P1.4
J2D2	MCU -> Port		开漏输出，无上拉		P1.5
J2D3	MCU -> Port		开漏输出，无上拉		P1.6
J2D4	MCU -> Port		开漏输出，无上拉		P1.7
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

static u8 data Register_1P = 0;	// 1P手柄寄存器
static u8 data Register_2P = 0;	// 2P手柄寄存器
static u8 data Register_3P = 0;	// 3P手柄寄存器
static u8 data Register_4P = 0;	// 4P手柄寄存器

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

bit isReady = 0;	// 是否检测到了锁存信号


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

// 读键码缓冲区，设置1P按键状态
static void EXPINTERFACE_Set1PKeyStatus() small{
	u8 i = 0;
	// A、B、Start、Select、Up、Down、Left、Right
	static const u8 code p[] = {0x07, 0x04, 0x16, 0x1A, 0x2C, 0x28, 0x0D, 0x0E};
	u8* keyCodeBuf = KEYBOARD_GetBuf();
	keyValue_1P = 0;
	for(i=0;i<8;++i){
		keyValue_1P |= keyCodeBuf[p[i]];
		keyValue_1P <<= 1;
	}
}

// 读键码缓冲区，设置2P按键状态
static void EXPINTERFACE_Set2PKeyStatus() small{
	u8 i = 0;
	// A、B、Start、Select、Up、Down、Left、Right
	static const u8 code p[] = {0x4F, 0x50, 0x51, 0x52, 0x62, 0x58, 0x59, 0x5A};
	u8* keyCodeBuf = KEYBOARD_GetBuf();
	keyValue_2P = 0;
	for(i=0;i<8;++i){
		keyValue_2P |= keyCodeBuf[p[i]];
		keyValue_2P <<= 1;
	}
}

// 读键码缓冲区，设置3P按键状态
static void EXPINTERFACE_Set3PKeyStatus() small{
	u8 i = 0;
	// A、B、Start、Select、Up、Down、Left、Right
	static const u8 code p[] = {0x07, 0x04, 0x16, 0x1A, 0x2C, 0x28, 0x0D, 0x0E};
	u8* keyCodeBuf = KEYBOARD_GetBuf();
	keyValue_3P = 0;
	for(i=0;i<8;++i){
		keyValue_3P |= keyCodeBuf[p[i]];
		keyValue_3P <<= 1;
	}
}

// 读键码缓冲区，设置4P按键状态
static void EXPINTERFACE_Set4PKeyStatus() small{
	u8 i = 0;
	// A、B、Start、Select、Up、Down、Left、Right
	static const u8 code p[] = {0x4F, 0x50, 0x51, 0x52, 0x62, 0x58, 0x59, 0x5A};
	u8* keyCodeBuf = KEYBOARD_GetBuf();
	keyValue_4P = 0;
	for(i=0;i<8;++i){
		keyValue_4P |= keyCodeBuf[p[i]];
		keyValue_4P <<= 1;
	}
}

// 初始化所有GPIO。参数代表是否启用该设备
void EXPINTERFACE_Init(bit isEnable) small{
	u8 i = 0;
	if(isEnable){
		// P1口设置成开漏输出，无上拉
		for(i=0;i<8;++i)GPIO_SelMode(1, 3, i);
		// P2口设置成高阻输入，无上拉
		for(i=0;i<8;++i)GPIO_SelMode(2, 0, i);
		// P3口设置成开漏输出，无上拉
		for(i=0;i<8;++i)GPIO_SelMode(3, 3, i);
		// P4口设置成高阻输入，无上拉
		for(i=0;i<8;++i)GPIO_SelP4Mode(i, 0, 0);
	}else{
		// P1口设置成高阻输入，无上拉
		for(i=0;i<8;++i)GPIO_SelMode(1, 0, i);
		// P2口设置成高阻输入，无上拉
		for(i=0;i<8;++i)GPIO_SelMode(2, 0, i);
		// P3口设置成高阻输入，无上拉
		for(i=0;i<8;++i)GPIO_SelMode(3, 0, i);
		// P4口设置成高阻输入，无上拉
		for(i=0;i<8;++i)GPIO_SelP4Mode(i, 0, 0);
	}
	// P1~P4口驱动能力为5mA
	GPIO_DrivCap(1, 0);
	GPIO_DrivCap(2, 0);
	GPIO_DrivCap(3, 0);
	GPIO_DrivCap(4, 0);
}

// 初始化Mode0。在这种模式下，USB功能完全被屏蔽，所有端口均为高阻
void EXPINTERFACE_InitMode0() small{
	EXPINTERFACE_Init(0);
}

void EXPINTERFACE_UpdateMode0() small{
	
}

// 初始化Mode1。在这种模式下，USB键盘被视为1P和2P手柄。
// 注意，如果真实1P手柄和键盘虚拟1P手柄（或真实2P手柄和键盘虚拟2P手柄）共存的话，
// 只要有其中一个手柄输出低电平键值（键按下），则另一个手柄不管是什么电平，都会被拉低。
// 这就导致了两个手柄的同键值按键是“或”关系，即只要有一个手柄按键按下，那个按键就会被视为按下。
// 由此可见，虚拟1P2P手柄可以和真实手柄共存，同时工作。
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
	// 获得1P和2P按键状态
	EXPINTERFACE_Set1PKeyStatus();
	EXPINTERFACE_Set2PKeyStatus();
	// 读OUT0（锁存信号）
	EXPINTERFACE_RefreshOUT0();
	// 读J1CLK（1P时钟）
	EXPINTERFACE_RefreshJ1CLK();
	// 读J2CLK（2P时钟）
	EXPINTERFACE_RefreshJ2CLK();
	
	// 检测是否出现锁存信号
	if(EXPINTERFACE_IsRisingEdgeOfOUT0()){
		// 出现锁存信号！允许后续逻辑
		isReady = 1;
	}
	if(isReady){
		// 检测是否结束锁存
		if(EXPINTERFACE_IsFallingEdgeOfOUT0()){
			// 锁存信号结束！把按键状态锁存到两个手柄寄存器中
			Register_1P = keyValue_1P;
			Register_2P = keyValue_2P;
			// 锁存的键值立刻出现在串行数据端
			J1D0 = Register_1P & 1;
			J2D0 = Register_2P & 1;
			// 进入下一个状态。在这之后，按键状态不再影响已被锁存的按键寄存器
		}
		// 检测是否出现1P时钟上升沿
		if(EXPINTERFACE_IsRisingEdgeOfJ1CLK()){
			// 1P手柄寄存器移位
			Register_1P <<= 1;
			// 锁存的键值立刻出现在串行数据端
			J1D0 = Register_1P & 1;
			// 进入下一个状态
		}
		// 检测是否出现1P时钟下降沿
		if(EXPINTERFACE_IsFallingEdgeOfJ1CLK()){
			// 这个状态下没什么可做的
		}
		// 检测是否出现2P时钟上升沿
		if(EXPINTERFACE_IsRisingEdgeOfJ2CLK()){
			// 2P手柄寄存器移位
			Register_2P <<= 1;
			// 锁存的键值立刻出现在串行数据端
			J2D0 = Register_2P & 1;
			// 进入下一个状态
		}
		// 检测是否出现2P时钟下降沿
		if(EXPINTERFACE_IsFallingEdgeOfJ2CLK()){
			// 这个状态下没什么可做的
		}
	}
	
	// 更新锁存信号状态
	EXPINTERFACE_UpdateOUT0();
	// 更新1P时钟信号状态
	EXPINTERFACE_UpdateJ1CLK();
	// 更新2P时钟信号状态
	EXPINTERFACE_UpdateJ2CLK();
}

// 初始化Mode2。在这种模式下，USB键盘被视为3P和4P手柄。
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
	// 获得3P和4P按键状态
	EXPINTERFACE_Set3PKeyStatus();
	EXPINTERFACE_Set4PKeyStatus();
	// 读OUT0（锁存信号）
	EXPINTERFACE_RefreshOUT0();
	// 读J1CLK（3P时钟）
	EXPINTERFACE_RefreshJ1CLK();
	// 读J2CLK（4P时钟）
	EXPINTERFACE_RefreshJ2CLK();
	
	// 检测是否出现锁存信号
	if(EXPINTERFACE_IsRisingEdgeOfOUT0()){
		// 出现锁存信号！允许后续逻辑
		isReady = 1;
	}
	if(isReady){
		// 检测是否结束锁存
		if(EXPINTERFACE_IsFallingEdgeOfOUT0()){
			// 锁存信号结束！把按键状态锁存到两个手柄寄存器中
			Register_3P = keyValue_3P;
			Register_4P = keyValue_4P;
			// 锁存的键值立刻出现在串行数据端
			J1D1 = Register_3P & 1;
			J2D1 = Register_4P & 1;
			// 进入下一个状态。在这之后，按键状态不再影响已被锁存的按键寄存器
		}
		// 检测是否出现3P时钟上升沿
		if(EXPINTERFACE_IsRisingEdgeOfJ1CLK()){
			// 3P手柄寄存器移位
			Register_3P <<= 1;
			// 锁存的键值立刻出现在串行数据端
			J1D1 = Register_3P & 1;
			// 进入下一个状态
		}
		// 检测是否出现3P时钟下降沿
		if(EXPINTERFACE_IsFallingEdgeOfJ1CLK()){
			// 这个状态下没什么可做的
		}
		// 检测是否出现4P时钟上升沿
		if(EXPINTERFACE_IsRisingEdgeOfJ2CLK()){
			// 4P手柄寄存器移位
			Register_4P <<= 1;
			// 锁存的键值立刻出现在串行数据端
			J2D1 = Register_4P & 1;
			// 进入下一个状态
		}
		// 检测是否出现4P时钟下降沿
		if(EXPINTERFACE_IsFallingEdgeOfJ2CLK()){
			// 这个状态下没什么可做的
		}
	}
	
	// 更新锁存信号状态
	EXPINTERFACE_UpdateOUT0();
	// 更新3P时钟信号状态
	EXPINTERFACE_UpdateJ1CLK();
	// 更新4P时钟信号状态
	EXPINTERFACE_UpdateJ2CLK();
}
