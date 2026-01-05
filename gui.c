#include "config.h"
#ifdef __SUPPORT_EPD__
#include "gui.h"
#include <stdlib.h>
extern const unsigned char asc2_816[][16];
extern const unsigned char asc2_1424[][42];
extern const unsigned char asc2_1116[][22];
extern const unsigned char asc2_0609[][12];
const unsigned char* mainfont[13] = {
    maintemp_1_2550,
    maintemp_2_2550,
    maintemp_3_2550,
    maintemp_4_2550,
    maintemp_5_2550,
    maintemp_6_2550,
    maintemp_7_2550,
    maintemp_8_2550,
    maintemp_9_2550,
    maintemp_10_2550,
    maintemp_11_2550,
    maintemp_12_2550,
    maintemp_13_2550,
};
const unsigned char maintemp_len[13]={10,10,2,102,52,112,116,70,92,132,58,142,124};
PAINT Paint={
	.Image_Piont = 0,
	.Image_width = 0,
	.Image_height = 0,
	.Image_widthMemory = 0,
	.Image_heightMemory = 0,
	.Image_color = 0,
	.Image_rotate = 0,
	.Image_widthByte = 0,
	.Image_heightByte = 0,
	.init = Paint_Init,
	.clear = Paint_Clear,
	.pixel = Paint_SetPixel,
	.Line = Paint_DrawLine,
	.Rectangle = Paint_DrawRectangle,
	.Char = Paint_ShowChar,
	.String = Paint_ShowString,
	.Circle = Paint_DrawCircle,
	.image = Paint_Showimage,
	.triangle = Paint_Triangle,
    .RLEimage = Paint_Showimage2
};
PAINT* This = &Paint;
void Paint_Init(u8 *image,u16 Width,u16 Height,u16 Rotate,u16 Color)
{
	PAINT* paint = This;
	paint -> Image_Piont = image;
	paint -> Image_color = Color;  
	paint -> Image_widthMemory = Height;
	paint -> Image_heightMemory = Width;  
	paint -> Image_widthByte = (Width % 8 == 0)? (Width / 8 ): (Width / 8 + 1);
	paint -> Image_heightByte = Height;     
	paint -> Image_rotate = Rotate;
	if(Rotate == 0 || Rotate == 180) 
	{
		paint -> Image_width = Height;
		paint -> Image_height = Width;
	} 
	else 
	{
		paint -> Image_width = Width;
		paint -> Image_height = Height;
	}
}

void Paint_Clear(u8 Color)
{
	volatile PAINT* paint = This;
	if(NULL == image){
		logd("image NULL\r\n");
		return;
	}
	u16 X,Y;
	u32 Addr;
  for(Y = 0; Y < paint->Image_heightByte; Y++) 
	{
    for(X = 0; X < paint->Image_widthByte; X++) 
		{   
      Addr = X + Y * (paint->Image_widthByte); 
      paint->Image_Piont[Addr] = Color;
    }
  }
}

void Paint_SetPixel(u16 Xpoint,u16 Ypoint,u16 Color)
{
	PAINT* paint = This;
	if(NULL == image)
		return;
	u16 X, Y;
	u32 Addr;
	u8 Rdata;
    switch(paint -> Image_rotate) 
		{
			case 0:
					Y = Xpoint;
                    #if defined(__UC8251_EPD__)
					X = Ypoint + 5;		
                    #elif defined(__SSD1680_EPD__)
                    X = Ypoint;
                    #endif
					break;
			case 90:
					Y = paint -> Image_widthMemory - Ypoint -1;
					X = Xpoint;
					break;
			case 180:
				#ifdef SCREEN_COMPRESS
					Y = (paint -> Image_widthMemory - Xpoint -1) - 29;
				#else
					Y = (paint -> Image_widthMemory - Xpoint -1);
				#endif
                    #if defined(__UC8251_EPD__)
					X = paint -> Image_heightMemory - Ypoint -1;
                    #elif defined(__SSD1680_EPD__)
                    X = (paint -> Image_heightMemory - Ypoint -1) - 3;
                    #endif
					break;
			case 270:
					Y = Ypoint;
					X = paint -> Image_heightMemory - Xpoint -1;
					break;
				default:
						return;
    }
		Addr = X/8 + Y*paint -> Image_widthByte;
    if(Addr >= EPD_WIDTH*EPD_HEIGHT/8){   
        return;
    }
    Rdata=paint -> Image_Piont[Addr];
    if(Color == 0xff)
    {    
			paint -> Image_Piont[Addr]=Rdata|(0x80>>(X % 8));
		}
    else
		{
       paint -> Image_Piont[Addr]=Rdata&~(0x80>>(X % 8)); 
		}
}
void Paint_DrawLine(u16 Xstart,u16 Ystart,u16 Xend,u16 Yend,u16 Color)
{   
	PAINT* paint = This;
	u16 Xpoint, Ypoint;
	int dx, dy;
	int XAddway,YAddway;
	int Esp;
	char Dotted_Len = 0;
  Xpoint = Xstart;
  Ypoint = Ystart;
  dx = (int)Xend - (int)Xstart >= 0 ? Xend - Xstart : Xstart - Xend;
  dy = (int)Yend - (int)Ystart <= 0 ? Yend - Ystart : Ystart - Yend;
  XAddway = Xstart < Xend ? 1 : -1;
  YAddway = Ystart < Yend ? 1 : -1;
  Esp = dx + dy;
  Dotted_Len = 0;
  for (;;) {
        Dotted_Len++;
            paint -> pixel(Xpoint, Ypoint, Color);
        if (2 * Esp >= dy) {
            if (Xpoint == Xend)
                break;
            Esp += dy;
            Xpoint += XAddway;
        }
        if (2 * Esp <= dx) {
            if (Ypoint == Yend)
                break;
            Esp += dx;
            Ypoint += YAddway;
        }
    }
}

/*******************************************************************
		函数说明：画矩形函数
		接口说明：Xstart 矩形x起始坐标参数
              Ystart 矩形Y起始坐标参数
							Xend   矩形x结束坐标参数
              Yend   矩形Y结束坐标参数
              Color  像素点颜色参数
              mode   矩形是否进行填充
		返回值：  无
*******************************************************************/
void Paint_DrawRectangle(u16 Xstart,u16 Ystart,u16 Xend,u16 Yend,u16 Color,u8 mode)
{
	PAINT* paint = This;
	u16 i;
    if (mode)
			{
        for(i = Ystart; i < Yend; i++) 
				{
          paint -> Line(Xstart,i,Xend,i,Color);
        }
      }
		else 
		 {
			 paint -> Line(Xstart, Ystart, Xend, Ystart, Color);
			 paint -> Line(Xstart, Ystart, Xstart, Yend, Color);
			 paint -> Line(Xend, Yend, Xend, Ystart, Color);
			 paint -> Line(Xend, Yend, Xstart, Yend, Color);
		 }
}
u8 filtering_char(u8 chr)
{
    if(chr >= 0 && chr <= 90){
        return chr;
    }
    return 91;
}
/*******************************************************************
		函数说明：显示单个字符
		接口说明：x 		 字符x坐标参数
              y 		 字符Y坐标参数
							chr    要显示的字符
              size1  显示字符字号大小
              Color  像素点颜色参数
		返回值：  无
*******************************************************************/
void Paint_ShowChar(u16 x,u16 y,u16 chr,u8 mode ,u16 Fontx,u16 Fonty,u16 color)
{
	PAINT* paint = This;
	u16 i,m,temp,size,chr1;
	u16 x0,y0;
	x0=x,y0=y;
	size=(Fonty/8+((Fonty%8)?1:0))*(Fontx);  

	chr1=chr - ' ';

	for(i=0;i<size;i++){
		//temp = asc2_816[chr1][i];//asc2_2020[chr1][i];

		switch(mode)
		{
			/*1 25*22 */
			case 0x01: temp=asc2_816[filtering_char(chr1)][i]; break;
			/*2 */
			//case 0x02: temp=maintemp_2550[chr1 - 13][i]; break;
       		case 0x02: temp =RLE_decompression((u8*)mainfont[chr1-13],maintemp_len[chr1-13],i);break;
			/*alarm*/
			case 0x03: temp = asc2_1424[chr1 - 16][i]; break;
            case 0x04: temp = asc2_1116[chr1 - 33][i]; break;
            case 0x05: temp = asc2_0609[chr1 - 16][i]; break;
		}

		for(m=0;m<8;m++){
			if(temp&0x01)
					paint -> pixel(x,y,!color);
			else 
				  paint -> pixel(x,y,color);
			temp>>=1;
			y++;
		}
		x++;
		if((x-x0)== Fontx){
		  x=x0;
			y0=y0+8;
		}
		
		y=y0;
	}
}
void Paint_ShowString(u16 x,u16 y,u8 *chr,u8 mode ,u16 Fontx,u16 Fonty,u16 color)
{
	PAINT* paint = This;
	while(*chr!='\0')
	{
		paint -> Char(x,y,*chr,mode,Fontx,Fonty,color);
		
		if(*chr == '.'){
			x += Fontx/2;
		}else{
			x += Fontx;
		}
		chr++;
  }
}
/*******************************************************************
		函数说明：画圆函数
		接口说明：X_Center 圆心x起始坐标参数
              Y_Center 圆心Y坐标参数
							Radius   圆形半径参数
              Color  像素点颜色参数
              mode   圆形是否填充显示
		返回值：  无
*******************************************************************/
void Paint_DrawCircle(u16 X_Center,u16 Y_Center,u16 Radius,u16 Color,u8 mode)
{
	PAINT* paint = This;
	int Esp, sCountY;
	u16 XCurrent, YCurrent;
  XCurrent = 0;
  YCurrent = Radius;
  Esp = 3 - (Radius << 1 );
    if (mode) {
        while (XCurrent <= YCurrent ) { //Realistic circles
            for (sCountY = XCurrent; sCountY <= YCurrent; sCountY ++ ) {
                paint -> pixel(X_Center + XCurrent, Y_Center + sCountY, Color);//1
                paint -> pixel(X_Center - XCurrent, Y_Center + sCountY, Color);//2
                paint -> pixel(X_Center - sCountY, Y_Center + XCurrent, Color);//3
                paint -> pixel(X_Center - sCountY, Y_Center - XCurrent, Color);//4
                paint -> pixel(X_Center - XCurrent, Y_Center - sCountY, Color);//5
                paint -> pixel(X_Center + XCurrent, Y_Center - sCountY, Color);//6
                paint -> pixel(X_Center + sCountY, Y_Center - XCurrent, Color);//7
                paint -> pixel(X_Center + sCountY, Y_Center + XCurrent, Color);
            }
            if ((int)Esp < 0 )
                Esp += 4 * XCurrent + 6;
            else {
                Esp += 10 + 4 * (XCurrent - YCurrent );
                YCurrent --;
            }
            XCurrent ++;
        }
    } else { //Draw a hollow circle
        while (XCurrent <= YCurrent ) {
            paint -> pixel(X_Center + XCurrent, Y_Center + YCurrent, Color);//1
            paint -> pixel(X_Center - XCurrent, Y_Center + YCurrent, Color);//2
            paint -> pixel(X_Center - YCurrent, Y_Center + XCurrent, Color);//3
            paint -> pixel(X_Center - YCurrent, Y_Center - XCurrent, Color);//4
            paint -> pixel(X_Center - XCurrent, Y_Center - YCurrent, Color);//5
            paint -> pixel(X_Center + XCurrent, Y_Center - YCurrent, Color);//6
            paint -> pixel(X_Center + YCurrent, Y_Center - XCurrent, Color);//7
            paint -> pixel(X_Center + YCurrent, Y_Center + XCurrent, Color);//0
            if ((int)Esp < 0 )
                Esp += 4 * XCurrent + 6;
            else {
                Esp += 10 + 4 * (XCurrent - YCurrent );
                YCurrent --;
            }
            XCurrent ++;
        }
    }
}
void Paint_Showimage(u16 x,u16 y,u8* image,u16 Fontx,u16 Fonty,u16 color)
{
	PAINT* paint = This;
	u16 i,m,data,size,chr1;
	u16 x0,y0;
	x0=x,y0=y;
	size=(Fonty/8+((Fonty%8)?1:0))*(Fontx);  

	for(i=0;i<size;i++){
		
		data = image[i];
		for(m=0;m<8;m++){
			if(data&0x01)
					paint -> pixel(x,y,!color);
			else 
				  paint -> pixel(x,y,color);
			data>>=1;
			y++;
		}
		x++;
		if((x-x0)== Fontx){
		  x=x0;
			y0=y0+8;
		}	
		y=y0;
	}
}
u8 RLE_decompression(u8* data, int len,int t)
{
	int num = 0;
	for (int i = 0; i < len; i += 2) {
		int count = data[i];
		uint8_t ch = data[i + 1]; 
		for (int j = 0; j < count; j++) {
			if (t == num) {
				return ch;
			}
			num++;
		}
	}
	return 0xff;
}
void Paint_Showimage2(u16 x,u16 y,u8* image,u16 Fontx,u16 Fonty,u16 len ,u16 color)
{
	PAINT* paint = This;
	u16 i,m,data,size,chr1;
	u16 x0,y0;
	x0=x,y0=y;
    size=(Fonty/8+((Fonty%8)?1:0))*(Fontx);
    for(i=0;i<size;i++){
       data = RLE_decompression(image,len,i);
        for(m=0;m<8;m++){
            if(data&0x01)
                paint -> pixel(x,y,!color);
            else
                paint -> pixel(x,y,color);
            data>>=1;
            y++;
        }
        x++;
        if((x-x0)== Fontx){
            x=x0;
            y0=y0+8;
        }
        y=y0;
    }
}
void Paint_Triangle(u8 ax, u8 ay, u8 bx, u8 by, u8 d, u8 Color)
{
	PAINT* paint = This;
	u16 i;
	for(i = 0; i < d ; i++){
		paint -> Line(ax, ay + i, bx, by, Color);
	}
}
#endif