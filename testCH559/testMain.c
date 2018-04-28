#include "typedef.h"
#include "SYSTEM.h"
#include "STDIO.h"
#include "EXKM.h"
//#include "TIMER0.h"
#include "KEYBOARD.h"
#include "EXPINTERFACE.h"
#include <stdio.h>
#include <string.h>


bit  InitFlag;
static u16 t = 0;

static u8 data mode = 0;
bit isChangedMode = 1;


/*******************************************************************************
* Function Name  : mDelayus(UNIT16 n)
* Description    : us延时函数
* Input          : UNIT16 n
* Output         : None
* Return         : None
*******************************************************************************/
void    mDelayuS( UINT16 n )                           // 以uS为单位延时
{
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

/*******************************************************************************
* Function Name  : mDelayms(UNIT16 n)
* Description    : ms延时函数
* Input          : UNIT16 n
* Output         : None
* Return         : None
*******************************************************************************/
void    mDelaymS( UINT16 n )                              // 以mS为单位延时
{
    while ( n )
    {
        mDelayuS( 1000 );
        -- n;
    }
}

/*******************************************************************************
* Function Name  : CfgFsys( )
* Description    : CH559时钟选择和配置函数,默认使用内部晶振12MHz，如果定义了FREQ_SYS可以
                   根据PLL_CFG和CLOCK_CFG配置得到，公式如下：
                   Fsys = (Fosc * ( PLL_CFG & MASK_PLL_MULT ))/(CLOCK_CFG & MASK_SYS_CK_DIV);
                   具体时钟需要自己配置
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/ 
/*
void	CfgFsys( )  
{
    SAFE_MOD = 0x55;                                                           //开启安全模式
    SAFE_MOD = 0xAA;                                                 
//  CLOCK_CFG |= bOSC_EN_XT;                                                   //使能外部晶振                                         
//  CLOCK_CFG &= ~bOSC_EN_INT;                                              
	  CLOCK_CFG &= ~MASK_SYS_CK_DIV;
    CLOCK_CFG |= 6;                                                            //配置系统时钟48MHz
//  CLOCK_CFG |= 8;                                                            //配置系统时钟36MHz
//  CLOCK_CFG |= 10;                                                           //配置系统时钟28.8MHz
//  CLOCK_CFG |= 12;                                                           //配置系统时钟24MHz
//  CLOCK_CFG |= 16;                                                           //配置系统时钟18MHz  	
    SAFE_MOD = 0xFF;                                                           //关闭安全模式  
//  如果修改主频，要同时修改FREQ_SYS，否则会造成延时函数不准
}
*/



void main() small{
	// 单片机上电之后，首先bootloader会检测是否处于下载模式。此时GPIO口的状态文档中并未提起，因此是未知的。但愿是高阻状态吧！
	SYSTEM_SetFreq();
	SYSTEM_Delayms(20);
	STDIO_Init();
	STDIO_SetUART0Pin(1);
	printf("UART initialized.\n");
	EXPINTERFACE_Init(0);
	printf("ExpInterface initialized.\n");
	KM_LibInit();
	printf("KMLib initialized.\n");
	while(1){
		u8 s;
		u8 srcBuf[10];
		u8 dstBuf[10];
		u8 srcLen;
		u8 dstLen;
		
		KM_ProcessSystem();
		s = KM_HostGetData(0, dstBuf, &dstLen, srcBuf, &srcLen);
		if(s == 0){
			// 获取数据成功 buf 存放数据，len数据长度
			for(s=0;s<dstLen;s++){
				printf("%02X ", (UINT16)dstBuf[s]);
			}
			printf("\n");
			
			KEYBOARD_Refresh(dstBuf);
			// 组合键Ctrl+Alt+Shift+Esc，切换键盘工作模式
			if(
				(KEYBOARD_IsKeyHold(0x100) | KEYBOARD_IsKeyHold(0x104)) &
				(KEYBOARD_IsKeyHold(0x101) | KEYBOARD_IsKeyHold(0x105)) &
				(KEYBOARD_IsKeyHold(0x102) | KEYBOARD_IsKeyHold(0x106))){
				if(KEYBOARD_IsKeyDown(0x29)){
					mode++;
					isChangedMode = 1;
				}
				if(KEYBOARD_IsKeyUp(0x29)){
					
				}
			}
			
			if(isChangedMode){
				switch(mode){
					case 0:{
						EXPINTERFACE_InitMode0();
					}break;
					case 1:{
						EXPINTERFACE_InitMode1();
					}break;
					case 2:{
						EXPINTERFACE_InitMode2();
					}break;
					case 3:{
						EXPINTERFACE_InitMode3();
					}break;
					case 4:{
						EXPINTERFACE_InitMode4();
					}break;
				}
				isChangedMode = 0;
			}
			if(1){
				switch(mode){
					case 0:{
						EXPINTERFACE_UpdateMode0();
					}break;
					case 1:{
						EXPINTERFACE_UpdateMode1();
					}break;
					case 2:{
						EXPINTERFACE_UpdateMode2();
					}break;
					case 3:{
						EXPINTERFACE_UpdateMode3();
					}break;
					case 4:{
						EXPINTERFACE_UpdateMode4();
					}break;
				}
			}
		}
		
		if(t == 0){
			t = 0xFFFF;
			KM_TypeQuery(0, dstBuf, &dstLen);
			for(s=0;s<dstLen;s=s+2){	
				if(dstBuf[s + 1] == DEV_KEYBOARD){			
					printf("ifc %02x keyboard\n", (UINT16)dstBuf[s]);
				}	 
				if(dstBuf[s + 1] == DEV_MOUSE){			
					printf("ifc %02x mouse\n", (UINT16)dstBuf[s]);
				}
				if(dstBuf[s + 1] == DEV_MOUSE_BIOS){			
					printf("ifc %02x mouse-bios\n", (UINT16)dstBuf[s]);
				}
			}
		}
		t--;
	}
}
