#include "KEYBOARD.h"
#include <STRING.h>


#define KEY_CODE_BUF_SIZE	(32 + 1)
// 256+8个键码缓冲区，用于指示256个按键+8个控制键（左右Ctrl、左右Shift、左右Alt、左右Win）的状态。每个按键占1bit
// 在这里，8个控制键的键值分别是：左Ctrl 0x100，左Shift 0x101，左Alt 0x102，左Win 0x103，右Ctrl 0x104，右Shift 0x105，右Alt 0x106，右Win 0x107
static u8 xdata keyCodeState[KEY_CODE_BUF_SIZE];
// 上一帧的键码缓冲区
static u8 xdata keyCodePrevState[KEY_CODE_BUF_SIZE];


// 解析键盘数据包，刷新键码缓冲区
void KEYBOARD_Refresh(u8* buf) small{
	u8 i = 0;
	// 先把上一帧的按键状态备份
	memcpy(keyCodePrevState, keyCodeState, KEY_CODE_BUF_SIZE * sizeof(u8));
	// 数据包共8字节，第0字节为8个控制键的状态
	keyCodeState[KEY_CODE_BUF_SIZE - 1] = buf[0];
	// 所有键码复位
	memset(keyCodeState, 0, KEY_CODE_BUF_SIZE * sizeof(u8));
	// 第2到第7字节为键值
	for(i = 2;i < 8;++i){
		if(buf[i] != 0){
			u8 bank = buf[i] >> 3;
			u8 offs = buf[i] & 7;
			keyCodeState[bank] |= (1 << offs);
		}
	}
	// 小霸王键盘在扫描的时候，在表现效果上需要有一个“始终按下”的键。目前把这个键安排在最后一位0xFF。也就是说，键码为0xFF的键，是处于始终按下的状态
	keyCodeState[KEY_CODE_BUF_SIZE - 2] |= (1 << 7);
}

// 某键是否按下（上一帧未按，当前帧按下）
bit KEYBOARD_IsKeyDown(u8 keyCode) small{
	u8 bank = keyCode >> 3;
	u8 offs = keyCode & 7;
	return ~keyCodePrevState[bank] & keyCodeState[bank] & (1 << offs);
}

// 某键是否松开（上一帧按下，当前帧未按下）
bit KEYBOARD_IsKeyUp(u8 keyCode) small{
	u8 bank = keyCode >> 3;
	u8 offs = keyCode & 7;
	return keyCodePrevState[bank] & ~keyCodeState[bank] & (1 << offs);
}

// 某键是否按住
bit KEYBOARD_IsKeyHold(u8 keyCode) small{
	u8 bank = keyCode >> 3;
	u8 offs = keyCode & 7;
	return keyCodeState[bank] & (1 << offs);
}

// 获得键码缓冲区
u8* KEYBOARD_GetBuf() small{
	return keyCodeState;
}
