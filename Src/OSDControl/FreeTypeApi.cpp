#include "FreeTypeApi.h"
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

#if USE_MODULE_BCM

#include <list>
using std::list;

#define Print(fmt,...)	printf(fmt, ## __VA_ARGS__)

CFreeType::CFreeType()
{
	mLibrary = NULL;
	mFace = NULL;
	mFonts_pixel_size = 0;
}

CFreeType::~CFreeType()
{
	if ( mFace != NULL )
	{
		FT_Done_Face(mFace);
		mFace = NULL;
	}

	if ( mLibrary != NULL )
	{
		FT_Done_FreeType(mLibrary);
		mLibrary = NULL;
	}
}

int CFreeType::LoadFont(const char *pFileName)
{
	if ( !pFileName )
	{
		Print("#Err:%s->%u, LoadFont failed.\n", __FILE__, __LINE__);
		return -1;
	}

	if( FT_Init_FreeType(&mLibrary) )
	{
		Print("FT_Init_FreeType failed\n");
		return -1;
	}

	if (FT_New_Face(mLibrary, pFileName, 0, &mFace))
	{
		Print("#Err:%s->%u, FT_New_Face failed.\n", __FILE__, __LINE__);
		return -1;
	}

	FT_Select_Charmap(mFace, FT_ENCODING_UNICODE);			/* 设定为UNICODE，默认的也是 */
	return SetFontSize(40);
}

int CFreeType::SetFontSize(int iSize)
{
	mFonts_pixel_size = iSize;
	FT_Select_Charmap(mFace, FT_ENCODING_UNICODE);			/* 设定为UNICODE，默认的也是 */
	//FT_Set_Char_Size(mFace, 0, mFonts_pixel_size<<6, 300, 300);
	FT_Set_Pixel_Sizes(mFace, 0, mFonts_pixel_size);		/* 设定字体大小 */

	return 0;
}

BOOL CFreeType::GetGlyphInfo(const wchar_t ch, FT_Glyph *glyph, U32 *width)
{
	FT_GlyphSlot slot = mFace->glyph;

	if ( FT_Load_Char(mFace, ch, FT_LOAD_RENDER) )
	{
		Print("#Err: FT_Load_Char %lc failed.\n", ch);
		return FALSE;
	}

	FT_Glyph currGlyph;
	FT_Get_Glyph(slot, &currGlyph);
	FT_Glyph newGlyph;
	FT_Glyph_Copy(currGlyph, &newGlyph);
	FT_Done_Glyph(currGlyph);

	if ( width != NULL )
		*width = (slot->advance.x >> 6 ) + 1;

	if ( glyph == NULL )
		FT_Done_Glyph(newGlyph);
	else
		*glyph = newGlyph;

	return TRUE;
}

void CFreeType::BitBltFontBitmap(const int& pixelSize, U8 *dst, const int& dstStrip,
					  const int& dstSizeX, const int &dstSizeY, const int &dstPosX, const int &dstPosY,
					  const FT_Bitmap *fontBitmap, RGBQUAD sColor)
{
	if ( !dst || !fontBitmap )
		return ;

	if ( dstPosX >= dstSizeX || dstPosY >= dstSizeY )
		return;

	int rows = fontBitmap->rows;
	if ( rows > dstSizeY - dstPosY )
		rows = dstSizeY - dstPosY;

	int width = fontBitmap->width;
	if ( width > dstSizeX - dstPosX )
		width = dstSizeX - dstPosX;


	//dst += int(dstPosX * pixelSize + dstStrip * dstPosY);	// 生成的图片是倒立的 获取需要拷贝的起始位置
	dst += int(dstPosX * pixelSize + dstStrip * (dstSizeY - dstPosY));
	U8 *src = fontBitmap->buffer;
	for ( int i = 0; i < rows; ++i )
	{
		//memcpy(dst, src, width);
		for (int j = 0; j < width; ++j )
		{
		    unsigned char alpha;

			alpha = src[j];
			if (alpha > 0)
            {
                int dstred, dstgreen, dstblue, dstalpha;
                int srcred, srcgreen, srcblue, srcalpha;

				dstalpha = dst[4*j + 3];
 				dstred   = dst[4*j + 2];
				dstgreen = dst[4*j + 1];
				dstblue  = dst[4*j];
				
                if(alpha < 255) 
                {
                    srcred   = (int)sColor.rgbRed* alpha / 255;
                    srcgreen = (int)sColor.rgbGreen * alpha / 255;
                    srcblue  = (int)sColor.rgbBlue * alpha / 255;
					
                    //¼ÆËãÄ¿±êÑÕÉ«Öµ 
                    srcred   = srcred + (int)dstred * (255 -  alpha) /255;
                    srcgreen = srcgreen  + (int)dstgreen * (255 -  alpha) /255;
                    srcblue  = srcblue  + (int)dstblue * (255 -  alpha) /255;
                    alpha    = alpha  + (int)dstalpha * (255 -  alpha) /255;
                    
                    dstred   = srcred < 256 ? (BYTE)srcred : 255;
                    dstgreen = srcgreen < 256 ? (BYTE)srcgreen : 255;
                    dstblue  = srcblue < 256 ? (BYTE)srcblue : 255;
                    dstalpha = alpha < 256 ? (BYTE)alpha : 255;

    				srcred	 = dstred;
    				srcgreen = dstgreen;
    				srcblue  = dstblue;
    				srcalpha = dstalpha;
                }
				else
				{
                    srcred   = (int)sColor.rgbRed;
                    srcgreen = (int)sColor.rgbGreen;
                    srcblue  = (int)sColor.rgbBlue;
                    srcalpha = (int)alpha;
				}
                
				dst[4*j]     = srcblue;
				dst[4*j + 1] = srcgreen;
				dst[4*j + 2] = srcred;
				dst[4*j + 3] = alpha;				
            }	
		}
		
		dst -= dstStrip;
		src += fontBitmap->width;
	}
}

//获取字符串(宽字节)的位图信息和数据,保存在sBitmapData结构中,包含头文件信息
int CFreeType::GetBmpDataFromString(wchar_t *strText,	//输入的字符串
							RGBQUAD sColor,							//字体颜色rgb
							RGBQUAD sBgColor,						//背景颜色rgb
							Font_Bitmap_Data *out_pFonts)			//输出的位图数据,Pic_Data是结构体
{
	if( (strText == NULL) || (out_pFonts == NULL) )
	{
		Print("#Err:%s->%u, Parameter is null.\n", __FILE__, __LINE__);
		return -1;
	}

	if ( out_pFonts->text_alpha )
	{
		free(out_pFonts->text_alpha);
		out_pFonts->text_alpha = NULL;
	}

	int strWidth = 0;
	const int pixelSize = 4;
	const int fontHeight = mFace->size->metrics.height >> 6;
	const int charCount = wcslen(strText);
	deque<FT_Glyph> glyphs;

	bool bMallocOK = true;
	do
	{
		for ( int charIdx = 0; charIdx < charCount; ++charIdx )
		{
			FT_Glyph currGlyph;
			U32 glyphWidth;
			if ( !GetGlyphInfo(strText[charIdx], &currGlyph, &glyphWidth) )
			{
				continue;
			}

			strWidth += glyphWidth;
			glyphs.push_back(currGlyph);
		}

		//const int distLineStrip = (pixelSize * strWidth + 3) & ~uint(0x3); // 4字节对齐
		//const int bmppitch = GET4BTYELEN(strWidth);	//4字节对齐
		const int bmppitch = strWidth*pixelSize; //4å­èå¯¹é½

		INT32 iFileHeadLen = sizeof(BITMAPFILE);
		INT32 size = bmppitch * fontHeight + iFileHeadLen;

		out_pFonts->iWidth  	= strWidth;
		out_pFonts->iHeight		= fontHeight;
		out_pFonts->text_alpha 	= (UINT8 *)malloc(size);
		if(out_pFonts->text_alpha == NULL)
		{
			Print("#Err:%s->%u, calloc:%d failed.\n", __FILE__, __LINE__, size);
			bMallocOK = FALSE;
			break;
		}
		memset(out_pFonts->text_alpha, 0x00, size);
		//设置bmp文件头信息
		U8 *pTextBuf = out_pFonts->text_alpha;
		BMP_SetBmpHead(pTextBuf, strWidth, fontHeight, pixelSize);
		
		pTextBuf += iFileHeadLen;

		// 为每个点设置颜色
		U8 *ptr = pTextBuf;
		
		for ( int i = 0; i < out_pFonts->iHeight; ++i )
		{
			for( int j = 0; j < out_pFonts->iWidth; ++j )
			{
				ptr[i*bmppitch + j * sizeof(RGBQUAD)]     = sBgColor.rgbBlue;
				ptr[i*bmppitch + j * sizeof(RGBQUAD) + 1] = sBgColor.rgbGreen;
				ptr[i*bmppitch + j * sizeof(RGBQUAD) + 2] = sBgColor.rgbRed;
				ptr[i*bmppitch + j * sizeof(RGBQUAD) + 3] = 0xFF;
			}
		}

		int penX = 0;
		int penY = (mFace->size->metrics.height + mFace->size->metrics.descender) >> 6;
		for ( deque<FT_Glyph>::iterator it = glyphs.begin(); it != glyphs.end(); ++it )
		{
			FT_Glyph glyph = *it;
			if ( glyph == NULL )
				continue;
			FT_BBox acbox;
			FT_Glyph_Get_CBox(*it, FT_GLYPH_BBOX_PIXELS, &acbox);

			FT_BitmapGlyph bmpGlyph = (FT_BitmapGlyph)glyph;
			FT_Bitmap *pBitmap = &(bmpGlyph->bitmap);

			BitBltFontBitmap(pixelSize, pTextBuf, bmppitch,
					strWidth, fontHeight, penX, (penY-acbox.yMax),
					pBitmap, sColor);

			penX += (glyph->advance.x >> 16) + 1;

			// 释放字符数据
			FT_Done_Glyph(glyph);
		}
	}while(0);

	if ( !bMallocOK )
	{
		for ( deque<FT_Glyph>::iterator it = glyphs.begin(); it != glyphs.end(); ++it )
		{
			FT_Glyph glyph = *it;
			// 释放字符数据
			FT_Done_Glyph(glyph);
		}
	}

	return bMallocOK ? 0 : -1;
}

int CFreeType::GetBmpDataFromRect(RECT rc, Font_Bitmap_Data *out_pFonts)
{
	if ( !out_pFonts )
		return -1;

	const int pixelSize = 4;
	const int fontHeight = mFace->size->metrics.height >> 6;
	int text_width = RECTWIDTH(rc);
	INT32 iFileHeadLen = sizeof(BITMAPFILE);
	//INT32 bmppitch = GET4BTYELEN(text_width);	//4字节对齐
	INT32 bmppitch	 = text_width * pixelSize;
	INT32 size = bmppitch * fontHeight + iFileHeadLen;

	out_pFonts->iWidth 	= text_width;    	/* 要输出的位图的宽度 */
	out_pFonts->iHeight	= fontHeight;        /* 要输出的位图的高度 */
	if(out_pFonts->text_alpha != NULL)
	{
		free(out_pFonts->text_alpha);
		out_pFonts->text_alpha = NULL;
	}
	out_pFonts->text_alpha = (UINT8 *)malloc(size);
	if(out_pFonts->text_alpha == NULL)
	{
		Print("#Err:%s->%u, calloc:%d failed.\n", __FILE__, __LINE__, size);
		return -1;
	}
	//设置bmp文件头信息
	U8 *pTextBuf = out_pFonts->text_alpha;
	BMP_SetBmpHead(pTextBuf, text_width, fontHeight, pixelSize);
	return 0;
}

//根据指定长\宽BMP图片，得到截取指定位置的图片信息(包含图片头和图片信息头)
int CFreeType::BMP_BilterFromBmpData(const INT32 iDestWidth,		//目的图片的宽
						const INT32 iDestStartx,						//目的起始位置
						Font_Bitmap_Data *pDestBmpData,					//目的图片
						const INT32 iSrcStartx,							//开始截取
						const Font_Bitmap_Data *pSrcBmpData)			//源图片
{
	if ( !pDestBmpData || !pDestBmpData->text_alpha ||
		 !pSrcBmpData  || !pSrcBmpData->text_alpha )
		return -1;

	INT32 iFileHeadLen = sizeof(BITMAPFILE);
	U8 *ptrDest = pDestBmpData->text_alpha + iFileHeadLen;
	U8 *ptrSrc  = pSrcBmpData->text_alpha + iFileHeadLen;
	if ( !ptrDest || !ptrSrc )
		return -1;

	INT32	iPix = 4;
    //UINT32 SrcPitch = GET4BTYELEN(pSrcBmpData->iWidth);
    //UINT32 DestPitch = GET4BTYELEN(pDestBmpData->iWidth);
    UINT32 SrcPitch = pSrcBmpData->iWidth * iPix;
    UINT32 DestPitch = pDestBmpData->iWidth * iPix;
	for ( int h = 0; h < pDestBmpData->iHeight; ++h )
	{
		int DestPos  = h*DestPitch + iDestStartx*iPix;
		int SrcPos   = h*SrcPitch + iSrcStartx*iPix;
		int CpyWidth = (iSrcStartx+iDestWidth <= pSrcBmpData->iWidth) ? iDestWidth : (pSrcBmpData->iWidth - iSrcStartx);

		//printf("==the DestPos:%d, SrcPos:%d, CpyWidth:%d.\n", DestPos, SrcPos, CpyWidth);
		memset(ptrDest+h*DestPitch, 0x00, DestPitch);
		memcpy(ptrDest+DestPos, ptrSrc+SrcPos, CpyWidth*iPix);
	}
	return 0;
}

//根据指定长\宽BMP图片，得到截取指定位置的图片信息(包含图片头和图片信息头)
int CFreeType::BMP_BilterFromBmpBuffer(INT32 iDestWidth,		//目的图片的宽
						INT32 iDestStartx,							//目的起始位置
						UINT8 *pDestBmpBuf,							//目的图片
						INT32 iSrcWidth,							//源图片的宽
						INT32 iSrcHeight,							//源图片的长
						INT32 iSrcStartx,							//开始截取,
						UINT8 *pSrcBmpbuf,							//源图片
						INT32 iLength)								//截取的长度
{
	if((pDestBmpBuf == NULL) || (pSrcBmpbuf == NULL))
	{
		Print("#Err:%s->%u, Parameter is null.\n", __FILE__, __LINE__);
		return -1;
	}

	if(iLength <= 0)
		return 0;

	if(iLength > iSrcWidth)
		iLength = iSrcWidth;

	INT32	iPix = 4;
	UINT8 *toData = pDestBmpBuf;
//    UINT32 srcPitch = GET4BTYELEN(iSrcWidth);
//    UINT32 destPitch = GET4BTYELEN(iDestWidth);
	UINT32 srcPitch = iSrcWidth * iPix;
	UINT32 destPitch = iDestWidth * iPix;

    int h,w,srcPos;
    for(h = (iSrcHeight-1); h >= 0; h--)
    {
    	srcPos = iSrcStartx;
        for(w = iDestStartx; w < (iDestStartx+iLength); w++)
        {
        	//copy by a pixel
        	toData[(iSrcHeight-h-1)*destPitch + w*4+0] = pSrcBmpbuf[h*srcPitch + srcPos*4 + 0];
        	toData[(iSrcHeight-h-1)*destPitch + w*4+1] = pSrcBmpbuf[h*srcPitch + srcPos*4 + 1];
        	toData[(iSrcHeight-h-1)*destPitch + w*4+2] = pSrcBmpbuf[h*srcPitch + srcPos*4 + 2];
        	toData[(iSrcHeight-h-1)*destPitch + w*4+3] = pSrcBmpbuf[h*srcPitch + srcPos*4 + 3];
        	srcPos++;
        }
    }
    return 1;
}

//设置bmp头文件信息 ,free type获取到的是data，无bmp头文件信息，
//所有需要设置bmp头文件信息
void CFreeType::BMP_SetBmpHead(UINT8 *pData,					//图片buffer，包含头信息buffer
							INT32 iWidth,							//图片宽
							INT32 iHeight,
							INT32 iPix)							//图片高
{
	if(pData == NULL)
	{
		Print("#Err:%s->%u, Parameter is null.\n", __FILE__, __LINE__);
		return;
	}

	UINT32 bmppitch = GET4BTYELEN(32,iWidth);
	if (iPix == 4)
	{
	    bmppitch = iWidth*4;
	}
	
	UINT32 filesize = bmppitch * iHeight;

	BITMAPFILE bmpfile;

	bmpfile.bfHeader.bfType 		= 0x4D42;
	bmpfile.bfHeader.bfSize 		= filesize + sizeof(BITMAPFILE);
	bmpfile.bfHeader.bfReserved1 	= 0;
	bmpfile.bfHeader.bfReserved2 	= 0;
	bmpfile.bfHeader.bfOffBits 	 	= sizeof(BITMAPFILE);

	bmpfile.biInfo.biSize 			= sizeof(BITMAPINFOHEADER);
	bmpfile.biInfo.biWidth 			= iWidth;
	bmpfile.biInfo.biHeight 		= iHeight;
	bmpfile.biInfo.biPlanes 		= 1;
	
	if (iPix == 4)
	{
		bmpfile.biInfo.biBitCount = 32;
	}
	else
	{
		bmpfile.biInfo.biBitCount = 24;
	}
	
	bmpfile.biInfo.biCompression 	= 0;
	bmpfile.biInfo.biSizeImage 		= 0;
	bmpfile.biInfo.biXPelsPerMeter 	= 0;
	bmpfile.biInfo.biYPelsPerMeter 	= 0;
	bmpfile.biInfo.biClrUsed 		= 0;
	bmpfile.biInfo.biClrImportant 	= 0;

	memcpy(pData, &bmpfile, sizeof(BITMAPFILE));
}

//显示bmp头文件信息
void CFreeType::BMP_ShowBmpHead(BITMAPFILE *pBmpHead)
{
	if(pBmpHead == NULL)
	{
		Print("#Err:%s->%s:%u, Parameter is null.\n", __FILE__, __FUNCTION__, __LINE__);
		return;
	}
	printf("位图文件头:\n");
	printf("文件头类型:0x%02X.\n",pBmpHead->bfHeader.bfType);
	printf("文件大小:%d.\n",pBmpHead->bfHeader.bfSize);
	printf("保留字_1:%d.\n",pBmpHead->bfHeader.bfReserved1);
	printf("保留字_2:%d.\n",pBmpHead->bfHeader.bfReserved2);
	printf("实际位图数据的偏移字节数:%d.\n",pBmpHead->bfHeader.bfOffBits);
	printf("结构体的长度:%d.\n",pBmpHead->biInfo.biSize);
	printf("位图宽:%d.\n",pBmpHead->biInfo.biWidth);
	printf("位图高:%d.\n",pBmpHead->biInfo.biHeight);
	printf("biPlanes平面数:%d.\n",pBmpHead->biInfo.biPlanes);
	printf("biBitCount采用颜色位数:%d.\n",pBmpHead->biInfo.biBitCount);
	printf("压缩方式:%d.\n",pBmpHead->biInfo.biCompression);
	printf("biSizeImage实际位图数据占用的字节数:%d.\n",pBmpHead->biInfo.biSizeImage);
	printf("X方向分辨率:%d.\n",pBmpHead->biInfo.biXPelsPerMeter);
	printf("Y方向分辨率:%d.\n",pBmpHead->biInfo.biYPelsPerMeter);
	printf("使用的颜色数:%d.\n",pBmpHead->biInfo.biClrUsed);
	printf("重要颜色数:%d.\n",pBmpHead->biInfo.biClrImportant);
}


//获取BMP文件的位图数据(无颜色表的位图):丢掉BMP文件的文件信息头和位图信息头
UINT8* CFreeType::BMP_GetBmpData(UINT8 *out_pBitCountPerPix,
					INT32 *out_iWidth,
					INT32 *out_iHeight,
					const char* strFilename)
{
	if((out_pBitCountPerPix == NULL) || (out_iWidth == NULL)
		|| (out_iHeight == NULL) || (strFilename == NULL))
	{
		Print("#Err:%s->%s:%u, Parameter is null.\n", __FILE__, __FUNCTION__, __LINE__);
		return NULL;
	}

	FILE *pf = fopen(strFilename, "rb");
    if(pf == NULL)
    {
    	Print("#Err:%s->%s:%u, Open file:%s failed.\n", __FILE__, __FUNCTION__, __LINE__, strFilename);
        return NULL;
    }

    BITMAPFILE bmpfile;
    fread(&(bmpfile.bfHeader), sizeof(BITMAPFILEHEADER), 1, pf);
    fread(&(bmpfile.biInfo), sizeof(BITMAPINFOHEADER), 1, pf);

	*out_pBitCountPerPix = bmpfile.biInfo.biBitCount;
	*out_iWidth = bmpfile.biInfo.biWidth;
	*out_iHeight = abs(bmpfile.biInfo.biHeight);

  //  U32 bmppicth = (((*width)*(*bitCountPerPix) + 31) >> 5) << 2;
    UINT32 bmppicth = (*out_iWidth)*4;
    INT32 len = (*out_iHeight)*bmppicth;

    UINT8 *pdata = (UINT8*)malloc(len);
    if(pdata == NULL)
    {
    	Print("#Err:%s->%u ,malloc:%d failed, the bmppicth:%d.\n", __FILE__, __LINE__, len, bmppicth);
        fclose(pf);
    	return NULL;
	}

    UINT8 *pEachLinBuf = (UINT8*)malloc(bmppicth);
	if(pEachLinBuf == NULL)
    {
    	Print("#Err:%s->%u ,malloc:%d failed.\n", __FILE__, __LINE__, bmppicth);
        free (pdata);
        fclose(pf);
    	return NULL;
	}

	UINT8 *data = pdata;
    UINT8 BytePerPix = (*out_pBitCountPerPix) >> 3;
    UINT32 pitch = (*out_iWidth) * BytePerPix;

    if(bmpfile.biInfo.biHeight < 0)
    {
		int w, h;
		for(h = 0; h < (*out_iHeight); h++)
		{
			memset(pEachLinBuf, 0, bmppicth);
			fread(pEachLinBuf, bmppicth, 1, pf);
			for(w = 0; w < (*out_iWidth); w++)
			{
				data[h*pitch + w*BytePerPix + 0] = pEachLinBuf[w*BytePerPix+0];
				data[h*pitch + w*BytePerPix + 1] = pEachLinBuf[w*BytePerPix+1];
				data[h*pitch + w*BytePerPix + 2] = pEachLinBuf[w*BytePerPix+2];
				data[h*pitch + w*BytePerPix + 3] = pEachLinBuf[w*BytePerPix+3];
			}
		}
    }
    else
    {
		int w, h;
    	for(h = ((*out_iHeight)-1); h >= 0; h--)
    	{
			memset(pEachLinBuf, 0, bmppicth);
    		fread(pEachLinBuf, bmppicth, 1, pf);
    		for(w = 0; w < (*out_iWidth); w++)
    		{
    			data[h*pitch + w*BytePerPix + 0] = pEachLinBuf[w*BytePerPix+0];
    			data[h*pitch + w*BytePerPix + 1] = pEachLinBuf[w*BytePerPix+1];
    			data[h*pitch + w*BytePerPix + 2] = pEachLinBuf[w*BytePerPix+2];
    			data[h*pitch + w*BytePerPix + 3] = pEachLinBuf[w*BytePerPix+3];
    		}
    	}
    }
    fclose(pf);
	free(pEachLinBuf);
    return pdata;
}

#endif
