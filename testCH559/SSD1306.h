#ifndef SSD1306_H_
#define SSD1306_H_


#include "typedef.h"


void SSD1306_Init() small;
void SSD1306_Refresh() small;
void SSD1306_Clear() small;
void SSD1306_DrawChar(u8 fontSize, u16 chr, s8 x, s8 y, bit c) small;
void SSD1306_DrawString(u8 fontSize, const char* str, s16 x, s16 y, bit c) small;
void SSD1306_DrawPixel(u8 x, u8 y, bit c) small;
void SSD1306_DrawLine(u8 x0, u8 y0, u8 x1, u8 y1, bit c) small;
void SSD1306_FillRect(u8 x, u8 y, u8 w, u8 h, bit c) small;
void SSD1306_DrawCircle(u8 x1, u8 y1, u8 r, bit c) small;


#endif	/* SSD1306_H_ */
