#ifndef EXP_INTERFACE_H_
#define EXP_INTERFACE_H_


#include "typedef.h"
#include "CH559.h"


void EXPINTERFACE_Init(bit isEnable) small;
void EXPINTERFACE_InitMode0() small;
void EXPINTERFACE_UpdateMode0() small;
void EXPINTERFACE_InitMode1() small;
void EXPINTERFACE_UpdateMode1() small;
void EXPINTERFACE_InitMode2() small;
void EXPINTERFACE_UpdateMode2() small;


#endif	/* EXP_INTERFACE_H_ */
