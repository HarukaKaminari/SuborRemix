#ifndef SYSTEM_H_
#define SYSTEM_H_


#include "CH559.h"


#define   FREQ_SYS    48000000	// ÏµÍ³Ö÷Æµ12MHz


void SYSTEM_SetFreq() small;
void SYSTEM_Delayus(UINT16 n) small;
void SYSTEM_Delayms(UINT16 n) small;


#endif	/* SYSTEM_H_ */
