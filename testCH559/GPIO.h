#ifndef GPIO_H_
#define GPIO_H_


#include "CH559.h"


void GPIO_DrivCap(UINT8 Port, bit Cap);
void GPIO_SelMode(UINT8 Port, UINT8 Mode, UINT8 PinNum);
void GPIO_SelP4Mode(UINT8 PinNum, bit isOutput, bit isPullup);


#endif	/* GPIO_H_ */
