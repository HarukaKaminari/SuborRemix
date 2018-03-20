#ifndef I2C_H_
#define I2C_H_


// I2C电路连接请参见“I2C接口电路.png”


#include "typedef.h"


void I2C_Init();
void I2C_Start();
void I2C_Write(u8 dat);
u8 I2C_Read();
bit I2C_GetAck();
void I2C_PutAck(bit ack);
void I2C_Stop();

void CheckI2CSlaveValidity();

// I2C总线综合发送函数，向从机发送多个字节的数据
s8 I2C_Puts(u8 SlaveAddr, u16 SubAddr, u8 SubMod, u8* dat, u16 Size);

// I2C总线综合接收函数，从从机接收多个字节的数据
s8 I2C_Gets(u8 SlaveAddr, u16 SubAddr, u8 SubMod, u8* dat, u16 Size);


#endif	/* I2C_H_ */
