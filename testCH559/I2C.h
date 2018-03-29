#ifndef I2C_H_
#define I2C_H_


// I2C��·������μ���I2C�ӿڵ�·.png��


#include "typedef.h"


void I2C_Init() small;
void I2C_Start() small;
void I2C_Write(u8 dat) small;
u8 I2C_Read() small;
bit I2C_GetAck() small;
void I2C_PutAck(bit ack) small;
void I2C_Stop() small;

void CheckI2CSlaveValidity() small;

// I2C�����ۺϷ��ͺ�������ӻ����Ͷ���ֽڵ�����
s8 I2C_Puts(u8 SlaveAddr, u16 SubAddr, u8 SubMod, u8* dat, u16 Size) small;

// I2C�����ۺϽ��պ������Ӵӻ����ն���ֽڵ�����
s8 I2C_Gets(u8 SlaveAddr, u16 SubAddr, u8 SubMod, u8* dat, u16 Size) small;


#endif	/* I2C_H_ */
