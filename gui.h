#ifndef __EPD_GUI_H__
#define __EPD_GUI_H__
#include "base_types.h"
#include "epd_lcd.h"
typedef void Demo;
typedef struct PAINT_T{
	u8* 		      		Image_Piont;
	u16 					Image_width;
	u16 					Image_height;
	u16 					Image_widthMemory;
	u16 					Image_heightMemory;
	u16 					Image_color;
	u16 					Image_rotate;
	u16 					Image_widthByte;
	u16 					Image_heightByte;
	void (*init) (u8 *image,u16 Width,u16 Height,u16 Rotate,u16 Color);
	void (*clear)(u8 Color);
	void (*pixel)(u16 Xpoint,u16 Ypoint,u16 Color);
	void (*Line) (u16 Xstart,u16 Ystart,u16 Xend,u16 Yend,u16 Color);
	void (*Rectangle)(u16 Xstart,u16 Ystart,u16 Xend,u16 Yend,u16 Color,u8 mode);
	void  (*Char) (u16 x,u16 y,u16 chr,u8 mode ,u16 Fontx,u16 Fonty,u16 color);
	void (*String)(u16 x,u16 y,u8 *chr,u8 mode ,u16 Fontx,u16 Fonty,u16 color);
	void (*Circle)(u16 X_Center,u16 Y_Center,u16 Radius,u16 Color,u8 mode);
	void (*image)(u16 x,u16 y,u8* image,u16 Fontx,u16 Fonty,u16 color);
	void (*triangle)(u8 ax, u8 ay, u8 bx, u8 by, u8 d, u8 Color);
    void (*RLEimage)(u16 x,u16 y,u8* image,u16 Fontx,u16 Fonty,u16 len ,u16 color);
}PAINT;
extern const unsigned char maintemp_1_2550[];
extern const unsigned char maintemp_2_2550[];
extern const unsigned char maintemp_3_2550[];
extern const unsigned char maintemp_4_2550[];
extern const unsigned char maintemp_5_2550[];
extern const unsigned char maintemp_6_2550[];
extern const unsigned char maintemp_7_2550[];
extern const unsigned char maintemp_8_2550[];
extern const unsigned char maintemp_9_2550[];
extern const unsigned char maintemp_10_2550[];
extern const unsigned char maintemp_11_2550[];
extern const unsigned char maintemp_12_2550[];
extern const unsigned char maintemp_13_2550[];
extern PAINT Paint;
void Paint_Clear(u8 Color);
void Paint_Init(u8 *image,u16 Width,u16 Height,u16 Rotate,u16 Color);
void Paint_SetPixel(u16 Xpoint,u16 Ypoint,u16 Color);
void Paint_DrawLine(u16 Xstart,u16 Ystart,u16 Xend,u16 Yend,u16 Color);
void Paint_DrawRectangle(u16 Xstart,u16 Ystart,u16 Xend,u16 Yend,u16 Color,u8 mode);
void Paint_ShowChar(u16 x,u16 y,u16 chr,u8 mode ,u16 Fontx,u16 Fonty,u16 color);
void Paint_ShowString(u16 x,u16 y,u8 *chr,u8 mode ,u16 Fontx,u16 Fonty,u16 color);
void Paint_DrawCircle(u16 X_Center,u16 Y_Center,u16 Radius,u16 Color,u8 mode);
void Paint_Showimage(u16 x,u16 y,u8* image,u16 Fontx,u16 Fonty,u16 color);
void Paint_Triangle(u8 ax, u8 ay, u8 bx, u8 by, u8 d, u8 Color);
void Paint_Showimage2(u16 x,u16 y,u8* image,u16 Fontx,u16 Fonty,u16 len ,u16 color);
u8 RLE_decompression(u8* data, int len,int t);
#endif
