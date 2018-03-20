#include "SYSTEM.h"


/*******************************************************************************
* Function Name  : CfgFsys( )
* Description    : CH559ʱ��ѡ������ú���,Ĭ��ʹ���ڲ�����12MHz�����������FREQ_SYS����
                   ����PLL_CFG��CLOCK_CFG���õõ�����ʽ���£�
                   Fsys = (Fosc * ( PLL_CFG & MASK_PLL_MULT ))/(CLOCK_CFG & MASK_SYS_CK_DIV);
                   ����ʱ����Ҫ�Լ�����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/ 
void SYSTEM_SetFreq() small{
    SAFE_MOD = 0x55;                                                           //������ȫģʽ
    SAFE_MOD = 0xAA;                                                 
//  CLOCK_CFG |= bOSC_EN_XT;                                                   //ʹ���ⲿ����                                         
//  CLOCK_CFG &= ~bOSC_EN_INT;                                              
	CLOCK_CFG &= ~MASK_SYS_CK_DIV;
    CLOCK_CFG |= 6;                                                            //����ϵͳʱ��48MHz
//  CLOCK_CFG |= 8;                                                            //����ϵͳʱ��36MHz
//  CLOCK_CFG |= 10;                                                           //����ϵͳʱ��28.8MHz
//  CLOCK_CFG |= 12;                                                           //����ϵͳʱ��24MHz
//  CLOCK_CFG |= 16;                                                           //����ϵͳʱ��18MHz  	
    SAFE_MOD = 0xFF;                                                           //�رհ�ȫģʽ  
//  ����޸���Ƶ��Ҫͬʱ�޸�FREQ_SYS������������ʱ������׼
}

void SYSTEM_Delayus(UINT16 n) small{
	while ( n ) {  // total = 12~13 Fsys cycles, 1uS @Fsys=12MHz
		++ SAFE_MOD;  // 2 Fsys cycles, for higher Fsys, add operation here
#ifdef	FREQ_SYS
#if		FREQ_SYS >= 14000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 16000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 18000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 20000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 22000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 24000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 26000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 28000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 30000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 32000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 34000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 36000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 38000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 40000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 42000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 44000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 46000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 48000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 50000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 52000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 54000000
		++ SAFE_MOD;
#endif
#if		FREQ_SYS >= 56000000
		++ SAFE_MOD;
#endif
#endif
		-- n;
	}
}

void SYSTEM_Delayms(UINT16 n) small{
	while(n){
		SYSTEM_Delayus(1000);
		--n;
	}
}
