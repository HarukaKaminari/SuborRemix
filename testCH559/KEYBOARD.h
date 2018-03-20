#ifndef KEYBOARD_H_
#define KEYBOARD_H_


#include "typedef.h"


void KEYBOARD_Refresh(u8* buf) small;
bit KEYBOARD_IsKeyDown(u8 keyCode) small;
bit KEYBOARD_IsKeyUp(u8 keyCode) small;
bit KEYBOARD_IsKeyHold(u8 keyCode) small;
u8* KEYBOARD_GetBuf() small;


#endif	/* KEYBOARD_H_ */
