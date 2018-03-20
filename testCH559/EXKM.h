#ifndef EXKM_H_
#define EXKM_H_


#include "typedef.h"


// type of initialize device
#ifndef DEV_KEYBOARD
#define DEV_KEYBOARD	0x30
#define DEV_MOUSE		0x31
#define DEV_MOUSE_BIOS	0x32
#endif


// ��ʼ��KM��
void	KM_LibInit();
// ��ѯ�豸���ӣ��豸�Ͽ��ȡ���ÿ֡����
u8		KM_ProcessSystem();
// ���ڼ��̵�Ʋ���
void	KM_HostSetReport(u8 report);
// ��ȡ��ֵ
// index	�˿������ţ�0/1��
// dst		���ڴ�ż�ֵ�Ļ�����
// dstLen	��ȡ��ֵ�����ݳ��ȣ�����Ϊ8�����Ϊ4
// src		δ֪
// srcLen	δ֪
u8		KM_HostGetData(u8 index, u8* dst, u8* dstLen, u8* src, u8* srcLen);
// ��ѯ�˿��豸����
// index	�˿������ţ�0/1��
// dst		���ڴ�����͵Ļ����� �ӿ�(B)+����(B)
// len		��ȡ�����ݳ���
void	KM_TypeQuery(u8 index, u8* dst, u8* len);


#endif	/* EXKM_H_ */
