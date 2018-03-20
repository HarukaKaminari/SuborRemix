#include "KEYBOARD.h"
#include <STRING.h>


#define KEY_CODE_BUF_SIZE	(32 + 1)
// 256+8�����뻺����������ָʾ256������+8�����Ƽ�������Ctrl������Shift������Alt������Win����״̬��ÿ������ռ1bit
// �����8�����Ƽ��ļ�ֵ�ֱ��ǣ���Ctrl 0x100����Shift 0x101����Alt 0x102����Win 0x103����Ctrl 0x104����Shift 0x105����Alt 0x106����Win 0x107
static u8 xdata keyCodeState[KEY_CODE_BUF_SIZE];
// ��һ֡�ļ��뻺����
static u8 xdata keyCodePrevState[KEY_CODE_BUF_SIZE];


// �����������ݰ���ˢ�¼��뻺����
void KEYBOARD_Refresh(u8* buf) small{
	u8 i = 0;
	// �Ȱ���һ֡�İ���״̬����
	memcpy(keyCodePrevState, keyCodeState, KEY_CODE_BUF_SIZE * sizeof(u8));
	// ���ݰ���8�ֽڣ���0�ֽ�Ϊ8�����Ƽ���״̬
	keyCodeState[KEY_CODE_BUF_SIZE - 1] = buf[0];
	// ���м��븴λ
	memset(keyCodeState, 0, KEY_CODE_BUF_SIZE * sizeof(u8));
	// ��2����7�ֽ�Ϊ��ֵ
	for(i = 2;i < 8;++i){
		if(buf[i] != 0){
			u8 bank = buf[i] >> 3;
			u8 offs = buf[i] & 7;
			keyCodeState[bank] |= (1 << offs);
		}
	}
}

// ĳ���Ƿ��£���һ֡δ������ǰ֡���£�
bit KEYBOARD_IsKeyDown(u8 keyCode) small{
	u8 bank = keyCode >> 3;
	u8 offs = keyCode & 7;
	return ~keyCodePrevState[bank] & keyCodeState[bank] & (1 << offs);
}

// ĳ���Ƿ��ɿ�����һ֡���£���ǰ֡δ���£�
bit KEYBOARD_IsKeyUp(u8 keyCode) small{
	u8 bank = keyCode >> 3;
	u8 offs = keyCode & 7;
	return keyCodePrevState[bank] & ~keyCodeState[bank] & (1 << offs);
}

// ĳ���Ƿ�ס
bit KEYBOARD_IsKeyHold(u8 keyCode) small{
	u8 bank = keyCode >> 3;
	u8 offs = keyCode & 7;
	return keyCodeState[bank] & (1 << offs);
}

// ��ü��뻺����
u8* KEYBOARD_GetBuf() small{
	return keyCodeState;
}