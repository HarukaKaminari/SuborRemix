#ifndef EXKM_H_
#define EXKM_H_


#include "typedef.h"


// type of initialize device
#ifndef DEV_KEYBOARD
#define DEV_KEYBOARD	0x30
#define DEV_MOUSE		0x31
#define DEV_MOUSE_BIOS	0x32
#endif


// 初始化KM库
void	KM_LibInit();
// 查询设备连接，设备断开等。需每帧调用
u8		KM_ProcessSystem();
// 用于键盘点灯操作
void	KM_HostSetReport(u8 report);
// 获取键值
// index	端口索引号（0/1）
// dst		用于存放键值的缓冲区
// dstLen	获取键值的数据长度，键盘为8，鼠标为4
// src		未知
// srcLen	未知
u8		KM_HostGetData(u8 index, u8* dst, u8* dstLen, u8* src, u8* srcLen);
// 查询端口设备类型
// index	端口索引号（0/1）
// dst		用于存放类型的缓冲区 接口(B)+类型(B)
// len		获取的数据长度
void	KM_TypeQuery(u8 index, u8* dst, u8* len);


#endif	/* EXKM_H_ */
