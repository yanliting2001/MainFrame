#ifndef _FREETYPEAPI_H
#define _FREETYPEAPI_H
#include "BCM_Type.h"

#if USE_MODULE_BCM

#include <wchar.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H

#define FREE_TYPE_API

#define BMP_OPERATE_API

#define GET4BTYELEN(pixel,x)	((((x)*pixel + 31) >> 5) << 2)			//4字节对齐

typedef unsigned char 		U8;
typedef unsigned short  	U16;
typedef unsigned int 		U32;

#pragma pack(1)

typedef struct _sFont_Bitmap_Data
{
	int iWidth;
	int iHeight;
	U8 *text_alpha;
}Font_Bitmap_Data;

//下面两个结构是位图的结构
typedef struct tagBITMAPFILEHEADER
{
	U16 bfType;
	U32 bfSize;
	U16 bfReserved1;
	U16 bfReserved2;
	U32 bfOffBits;
} BITMAPFILEHEADER;

typedef struct tagBITMAPINFOHEADER
{
	U32 biSize;
	int biWidth;
	int biHeight;
	U16 biPlanes;
	U16 biBitCount;
	U32 biCompression;
	U32 biSizeImage;
	U32 biXPelsPerMeter;
	U32 biYPelsPerMeter;
	U32 biClrUsed;
	U32 biClrImportant;
} BITMAPINFOHEADER;

//typedef union _uColor
//{
//	struct
//	{
//		U8 rgbBlue;
//		U8 rgbGreen;
//		U8 rgbRed;
//		U8 rgbReserved;
//	} PerColor;
//	U32 Color;
//} RGBQUAD;
typedef struct tagRGBQUAD
{
	U8 rgbBlue;
	U8 rgbGreen;
	U8 rgbRed;
	U8 rgbReserved;
} RGBQUAD;

typedef struct tagColor24
{
	U8 rgbBlue;
	U8 rgbGreen;
	U8 rgbRed;
} Color24;

typedef struct tagBITMAPINFO
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD bmiColors[1];
} BITMAPINFO;


typedef struct tagBITMAP
{
	BITMAPFILEHEADER bfHeader;
	BITMAPINFOHEADER biInfo;
}BITMAPFILE;

#pragma pack()

#define FONTPATH "./KTV_Data/fonts/arialuni.ttf"
//#define FONTPATH	"KTV_Data/fonts/simsun.ttc"

class CFreeType
{
public:
	CFreeType();
	virtual ~CFreeType();

public:
	int LoadFont(const char *pFileName);

	int	SetFontSize(int iSize);

	BOOL GetGlyphInfo(const wchar_t ch, FT_Glyph *glyph, U32 *width);

	void BitBltFontBitmap(const int& pixelSize, U8 *dst, const int& dstStrip,
						  const int& dstSizeX, const int &dstSizeY, const int &dstPosX, const int &dstPosY,
						  const FT_Bitmap *fontBitmap, RGBQUAD sColor);

	//获取字符串(宽字节)的位图信息和数据,保存在sBitmapData结构中,包含头文件信息
	int GetBmpDataFromString(wchar_t *strText,	//输入的字符串
							RGBQUAD sColor,							//字体颜色rgb
							RGBQUAD sBgColor,							//背景颜色rgb
							Font_Bitmap_Data *out_pFonts);				//输出的位图数据,Pic_Data是结构体

	int  GetBmpDataFromRect(RECT rc, Font_Bitmap_Data *out_pFonts);

	//根据指定长\宽BMP图片，得到截取指定位置的图片信息(包含图片头和图片信息头)
	int BMP_BilterFromBmpData(const INT32 iDestWidth,		//目的图片的宽
						const INT32 iDestStartx,						//目的起始位置
						Font_Bitmap_Data *pDestBmpData,					//目的图片
						const INT32 iSrcStartx,							//开始截取
						const Font_Bitmap_Data *pSrcBmpData);			//源图片

	//根据指定长\宽BMP图片，得到截取指定位置的图片信息(包含图片头和图片信息头)
	int BMP_BilterFromBmpBuffer(INT32 iDestWidth,		//目的图片的宽
						INT32 iDestStartx,							//目的起始位置
						UINT8 *pDestBmpBuf,							//目的图片
						INT32 iSrcWidth,							//源图片的宽
						INT32 iSrcHeight,							//源图片的长
						INT32 iSrcStartx,							//开始截取,
						UINT8 *pSrcBmpbuf,							//源图片
						INT32 iLength);								//截取的长度

	//设置bmp头文件信息 ,free type获取到的是data，无bmp头文件信息，
	//所有需要设置bmp头文件信息
	void BMP_SetBmpHead(UINT8 *pData,					//图片buffer，包含头信息buffer
						INT32 iWidth,							//图片宽
						INT32 iHeight,
						INT32 iPix = 3);							//图片高

	//显示bmp头文件信息
	void BMP_ShowBmpHead(BITMAPFILE *pBmpHead);

	//获取BMP文件的位图数据(无颜色表的位图):丢掉BMP文件的文件信息头和位图信息头
	UINT8* BMP_GetBmpData(UINT8 *out_pBitCountPerPix,
					INT32 *out_iWidth,
					INT32 *out_iHeight,
					const char* strFilename);

private:
	FT_Library 	mLibrary;
	FT_Face 	mFace;
	int 		mFonts_pixel_size;			//字体大小
};

#endif

#endif
