#include <INTRINS.h>
#include <stdio.h>
#include "I2C.h"
#include <CH559.h>
#include "GPIO.h"


/*
	I2C.c
	��׼80C51��Ƭ��ģ��I2C���ߵ���������
	���������ѧϰ�ο������ṩ�κοɿ��Է���ĵ���������������ҵĿ��
˵����
	����һ����C51ģ��I2C����Э��ľ���汾
	ֻ����һ�����ģʽ�������Ƕ���ģʽ
	������I2C����Э���е�ʱ��ͬ������
	Ҫ���������˽Ȿ�����ϸ�ڣ���ο�Philips��˾�����Э���׼
�÷���
	1. ���ļ�I2C.h��I2C.cһ���Ƶ����Ĺ����ļ�����
	2. ����ʵ�ʵ�Ƭ�����ͺţ�����SFR��ͷ�ļ�<reg52.h>
	3. ����ʵ�ʵ�·���޸�I2C���������ź���SCL��SDA�Ķ���
	4. ͨ���궨��I2C_DELAY_VALUE����I2C���ߵ��ٶ�ʹ�����ʵ����Ҫ
	5. ���ļ�I2C.c��ӽ������У�����Ҫ�ĵط�����ͷ�ļ�I2C.h
	6. ��main()�����Ŀ�ʼ����Ҫ����һ�γ�ʼ������I2C_Init()
	7. I2C_Puts()��I2C_Gets()��I2C�����ۺ϶�д�������뿴��ע�ͺ���ʹ��
	8. ����ȫ�����ʵı�ʶ������I2C_��ͷ������Ч����������ͻ����
	9. ע�⣺�ӻ���ַ����7λ����ַ��ʾ��������дλ�������λ��Ч
*/

// ����I2C����ʱ�ӵ���ʱֵ��Ҫ����ʵ������޸ģ�ȡֵ1��255
// 1T��Ƭ��@30MHz��Ƶ�£�ֻҪ���ֵ����33����ô��ʱʱ��ͳ�����4.7uS
// ���ǵ�1T��Ƭ��IO�ڿ�����������Ӧ�����ĸı䣬�Լ��������ݺͷֲ�������ɵĲ��α�Ե�����ͣ����ֵӦ���Դ��ڼ���ֵ
#define I2C_DELAY_VALUE		64

// ����I2C����ֹͣ������һ�ο�ʼ֮ǰ�ĵȴ�ʱ�䣬ȡֵ1��65535
// �ȴ�ʱ��Ϊ(I2C_STOP_WAIT_VALUE*5)/��ƵuS
// ���ڶ�������ȡֵΪ1���ɣ�������ĳЩ������˵���ϳ�����ʱ�Ǳ����
#define I2C_STOP_WAIT_VALUE		1

// ����I2C�����ȴ��ӻ���Ӧ�ĳ�ʱʱ�䣬ȡֵ1~65535
#define I2C_WAIT_ACK_TIMEOUT	255


// ģ��I2C���ߵ����Ŷ���
sbit I2C_SCL = P0^2;
sbit I2C_SDA = P0^3;


/*
������I2C_Delay()
���ܣ���ʱ��ģ��I2C����ר��
˵�������������ʱʱ��Ϊ((6+2+I2C_DELAY_VALUE*4+4)/��Ƶ)uS
*/
static void I2C_Delay() small{
	u8 I2C_Delay_t = (I2C_DELAY_VALUE);
	while(--I2C_Delay_t != 0);
}

/*
������I2C_Init()
���ܣ�I2C���߳�ʼ����ʹ���ߴ��ڿ���״̬
˵������main()�����Ŀ�ʼ����ͨ��Ӧ��Ҫִ��һ�α�����
*/
void I2C_Init() small{
	// ��I2C_SCL��I2C_SDA���óɿ�©
	GPIO_SelMode(0, 5, 2);
	GPIO_SelMode(0, 5, 3);
	
	I2C_SCL = 1;
	I2C_Delay();
	I2C_SDA = 1;
	I2C_Delay();
}

/*
������I2C_Start()
���ܣ�����I2C���ߵ���ʼ״̬
˵����
	SCL���ڸߵ�ƽ�ڼ䣬��SDA�����½���ʱ����I2C����
	����SDA��SCL����ʲô��ƽ״̬��������������ȷ������ʼ״̬
	������Ҳ�������������ظ���ʼ״̬
	������ִ�к�I2C���ߴ���æ״̬
*/
void I2C_Start() small{
	I2C_SDA = 1;
	I2C_Delay();
	I2C_SCL = 1;
	I2C_Delay();
	I2C_SDA = 0;
	I2C_Delay();
	I2C_SCL = 0;
	I2C_Delay();
}

/*
������I2C_Write()
���ܣ���I2C����д1���ֽڵ�����
������
	dat��Ҫд�������ϵ�����
*/ 
void I2C_Write(u8 dat) small{
	u8 t = 8;
	do{
		I2C_SDA = (bit)(dat & 0x80);
		I2C_Delay();
		I2C_SCL = 1;
		I2C_Delay();
		I2C_SCL = 0;
		I2C_Delay();
		dat <<= 1;
	}while(--t != 0);
}

/*
������I2C_Read()
���ܣ��Ӵӻ���ȡ1���ֽڵ�����
���أ���ȡ��һ���ֽ�����
*/
u8 I2C_Read() small{
	u8 dat;
	u8 t = 8;
	I2C_SDA = 1;	// �ڶ�ȡ����֮ǰ��Ҫ��SDA����
	I2C_Delay();
	do{
		I2C_SCL = 1;		// �����ͷ�SCL�ߣ��ȴ��ӻ�
		I2C_Delay();
		while(!I2C_SCL);	// �ȴ��ӻ��ͷ�SCL���ӻ��ͷ�SCL����ӻ��Ѿ���������SDA����׼���ã��������̶�ȡ
		dat <<= 1;
		dat |= I2C_SDA;
		I2C_SCL = 0;
		I2C_Delay();
	}while(--t != 0);
	return dat;
}

/*
������I2C_GetAck()
���ܣ���ȡ�ӻ�Ӧ��λ
���أ�
	0���ӻ�Ӧ��
	1���ӻ���Ӧ��
˵����
	�ӻ����յ������ϵĵ�ַ��������Լ��ĵ�ַ��ͬ�������Ӧ��λ
	�ӻ����յ�ÿ���ֽڵ����ݺ�Ҫ����Ӧ��λ
	�ӻ����յ����1���ֽڵ����ݺ�һ��Ҫ������Ӧ��λ
*/
bit I2C_GetAck() small{
	u16 timeout = I2C_WAIT_ACK_TIMEOUT;
	I2C_SDA = 1;
	I2C_Delay();
	I2C_SCL = 1;
	I2C_Delay();
	while(I2C_SDA){
		timeout--;
		if(timeout==0){
			//printf("Not Acknowledged.\r\n");
			return 1;
		}
	}
	I2C_SCL = 0;
	I2C_Delay();
	return 0;
}

/*
������I2C_PutAck()
���ܣ���������Ӧ��λ���Ӧ��λ
������
	ack=0����������Ӧ��λ
	ack=1������������Ӧ��λ
˵����
	�����ڽ�����ÿһ���ֽڵ����ݺ󣬶�Ӧ������Ӧ��λ
	�����ڽ��������һ���ֽڵ����ݺ�Ӧ��������Ӧ��λ
*/
void I2C_PutAck(bit ack) small{
	I2C_SDA = ack;
	I2C_Delay();
	I2C_SCL = 1;
	I2C_Delay();
	I2C_SCL = 0;
	I2C_Delay();
}

/*
������I2C_Stop()
���ܣ�����I2C���ߵ�ֹͣ״̬
˵����
	SCL���ڸߵ�ƽ�ڼ䣬��SDA����������ʱֹͣI2C����
	����SDA��SCL����ʲô��ƽ״̬��������������ȷ����ֹͣ״̬
	������ִ�к�I2C���ߴ��ڿ���״̬
*/
void I2C_Stop() small{
	u16 t = I2C_STOP_WAIT_VALUE;
	I2C_SDA = 0;
	I2C_Delay();
	I2C_SCL = 1;
	I2C_Delay();
	I2C_SDA = 1;
	I2C_Delay();
	while(--t != 0);	// ����һ�β���Start֮ǰ��Ҫ��һ������ʱ
}

/*
������CheckI2CSlaveValidity()
���ܣ�������������е�I2C����������Ϸ���������ַ
˵����
	������ͼ������д��ַ�������������Ӧ����˵���������Ϸ�
*/
void CheckI2CSlaveValidity() small{
	u8 i = 0;
	printf("Start checking slaves!\r\n");
	I2C_Init();
	while(1){
		bit result;
		I2C_Start();
		I2C_Write(i);
		result = I2C_GetAck();
		if(result == 0){
			printf("Slave %d is valid.\r\n", (int)i);
		}
		I2C_Stop();
		i++;
		if(i == 0)break;
	}
	printf("Finished checking slaves!\r\n");
}

/*
������I2C_Puts()
���ܣ�I2C�����ۺϷ��ͺ�������ӻ����Ͷ���ֽڵ�����
������
	SlaveAddr���ӻ���ַ��7λ����ַ��������дλ��
	SubAddr���ӻ����ӵ�ַ
	SubMod���ӵ�ַģʽ��0�����ӵ�ַ��1�����ֽ��ӵ�ַ��2��˫�ֽ��ӵ�ַ
	*dat��Ҫ���͵�����
	Size�����ݵ��ֽ���
���أ�
	0�����ͳɹ�
	1���ڷ��͵�ַ�����г����쳣
	-1���ڷ������ݹ����г����쳣
˵����
	�������ܹ��ܺõ���Ӧ���г�����I2C�������������Ƿ����ӵ�ַ
	���ӻ�û���ӵ�ַʱ������SubAddr���⣬��SubModӦ��Ϊ0
*/
s8 I2C_Puts(u8 SlaveAddr, u16 SubAddr, u8 SubMod, u8* dat, u16 Size) small{
	// ������ʱ����
	u8 i;
	u8 a[3];
	// ��鳤��
	if(Size == 0)return 0;
	// ׼���ӻ���ַ
	a[0] = (SlaveAddr << 1);
	// ����ӵ�ַģʽ
	if(SubMod > 2)SubMod = 2;
	// ȷ���ӵ�ַ
	switch(SubMod){
	case 0:
		break;
	case 1:
		a[1] = (u8)(SubAddr & 0xFF);
		break;
	case 2:
		a[1] = (u8)((SubAddr >> 8) & 0xFF);
		a[2] = (u8)(SubAddr & 0xFF);
		break;
	default:
		break;
	}
	// ���ʹӻ���ַ�����ŷ����ӵ�ַ��������ӵ�ַ�Ļ���
	SubMod++;
	I2C_Start();
	for(i=0;i<SubMod;++i){
		I2C_Write(a[i]);
		if(I2C_GetAck()){
			I2C_Stop();
			return 1;
		}
	}
	// ��������
	do{
		I2C_Write(*dat++);
		if(I2C_GetAck())break;
	}while(--Size != 0);
	// ������ϣ�ֹͣI2C���ߣ������ؽ��
	I2C_Stop();
	if(Size == 0){ 
		return 0;
	}else{
		return -1;
	}
}

/*
������I2C_Gets()
���ܣ�I2C�����ۺϽ��պ������Ӵӻ����ն���ֽڵ�����
������
	SlaveAddr���ӻ���ַ��7λ����ַ��������дλ��
	SubAddr���ӻ����ӵ�ַ
	SubMod���ӵ�ַģʽ��0�����ӵ�ַ��1�����ֽ��ӵ�ַ��2��˫�ֽ��ӵ�ַ
	*dat��������յ�������
	Size�����ݵ��ֽ���
���أ�
	0�����ճɹ�
	1���ڽ��չ��̵ķ��͵�ַʱ�����쳣
˵����
	�������ܹ��ܺõ���Ӧ���г�����I2C�������������Ƿ����ӵ�ַ
	���ӻ�û���ӵ�ַʱ������SubAddr���⣬��SubModӦ��Ϊ0
*/
s8 I2C_Gets(u8 SlaveAddr, u16 SubAddr, u8 SubMod, u8* dat, u16 Size) small{
	// ������ʱ����
	u8 i;
	u8 a[3];
	// ��鳤��
	if(Size == 0)return 0;
	// ׼���ӻ���ַ
	a[0] = (SlaveAddr << 1);
	// ����ӵ�ַģʽ
	if(SubMod > 2)SubMod = 2;
	// ��������ӵ�ַ�Ĵӻ�����Ҫ�ȷ��ʹӻ���ַ���ӵ�ַ
	if(SubMod != 0){
		//ȷ���ӵ�ַ
		if(SubMod == 1){
			a[1] = (u8)(SubAddr & 0xFF);
		}else{
			a[1] = (u8)((SubAddr >> 8) & 0xFF);
			a[2] = (u8)(SubAddr & 0xFF);
		}
		// ���ʹӻ���ַ�����ŷ����ӵ�ַ
		SubMod++;
		I2C_Start();
		for(i=0;i<SubMod;++i){
			I2C_Write(a[i]);
			if(I2C_GetAck()){
				I2C_Stop();
				return 1;
			}
		}
	}
	// �����I2C_Start()�������ӵ�ַ�Ĵӻ����ظ���ʼ״̬
	// �������ӵ�ַ�Ĵӻ�������������ʼ״̬
	I2C_Start();
	// ���ʹӻ���ַ
	I2C_Write(a[0] + 1);
	if(I2C_GetAck()){
		I2C_Stop();
		return 1;
	}
	// ��������
	for(;;){
		*dat++ = I2C_Read();
		if(--Size == 0){
			I2C_PutAck(1);
			break;
		}
		I2C_PutAck(0);
	}
	// ������ϣ�ֹͣI2C���ߣ������ؽ��
	I2C_Stop();
	return 0;
}
