#ifndef I2C_H_
#define I2C_H_


// I2C��·������μ���I2C�ӿڵ�·.png��


#include "typedef.h"


void I2C_Init();
void I2C_Start();
void I2C_Write(u8 dat);
u8 I2C_Read();
bit I2C_GetAck();
void I2C_PutAck(bit ack);
void I2C_Stop();

void CheckI2CSlaveValidity();

// I2C�����ۺϷ��ͺ�������ӻ����Ͷ���ֽڵ�����
s8 I2C_Puts(u8 SlaveAddr, u16 SubAddr, u8 SubMod, u8* dat, u16 Size);

// I2C�����ۺϽ��պ������Ӵӻ����ն���ֽڵ�����
s8 I2C_Gets(u8 SlaveAddr, u16 SubAddr, u8 SubMod, u8* dat, u16 Size);


#endif	/* I2C_H_ */
