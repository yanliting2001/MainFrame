#ifndef _BCM_OSD_CTRL_H
#define _BCM_OSD_CTRL_H

//#include "BCM_Type.h"
#include "FreeTypeApi.h"
#include <wchar.h>
#if USE_MODULE_BCM

#if defined(__mips__)
#include "BCM7231_Control.h"
#elif defined(__arm__)
#include "hisi3719_Control.h"
#endif

typedef enum  _eSCROLLTEXT_TYPE
{
	ScrollType_Main=0,
	ScrollType_Sub,
	ScrollType_Room,
	ScrollType_Count,
}eScrollType;

typedef struct _sScrollTextInfo
{
	int 			miSize;
	RGBQUAD 		msColor;			//字体颜色rgb
	RGBQUAD 		msBgColor;
	RECT 			mRc;
	int 			miSpeed;
	int 			miTime;				// 滚动次数，0：代表无限滚动
	eScrollType		meType;
}sScrollTextInfo;

// 电视水印信息
typedef struct _sWartermarkInfo
{
	int 			miSize;
	RGBQUAD 		msColor;			//字体颜色rgb
	RGBQUAD 		msBgColor;
	RECT 			mRc;
	int				miNum;				// 显示数量
	BOOL			mbOpen;				// 是否开启,默认关闭
} sWartermarkInfo;

class CBCM_OSDCtrl
{
public:
	CBCM_OSDCtrl();

	virtual ~CBCM_OSDCtrl();

public:
	int Init();

	void DeInit();

	int ShowBitmapFile(const char *pImgName, RECT rc);

	// 显示如jpeg图片
	int ShowPicFile(const char *pPicName, RECT rc);

	int Show24BitmapImgData(Font_Bitmap_Data *pDataInfo, RECT rc);

	int Show32BitmapImgData(Font_Bitmap_Data *pDataInfo, RECT rc);

	void EraseImg(RECT rc);

	void Update();

	void FreeBitmapData(Font_Bitmap_Data *pData);

	int ShowScrollText(const wchar_t *msg, sScrollTextInfo *pScrollInfo);

	void StopMainScrollText();

public:
	CBaseThread 	mMainScroll;
	CBaseStringW	mMainScrollText;		// 主跑马灯信息
	BOOL			mbIsInMainScrollThread;
	BOOL 			mbUpdateMainScrollText;
	RGBQUAD 		msTextColor;

	CBaseThread 	mSubScroll;
	CBaseStringW	mSubScrollText;			// 祝福语跑马灯
	BOOL			mbIsInSubScrollThread;
	BOOL			mbUpdateSubScrollText;

	CBaseThread 	mRoomScroll;
	CBaseStringW	mRoomScrollText;			// 房间信息跑马灯
	BOOL			mbIsInRoomScrollThread;
	BOOL			mbUpdateRoomScrollText;
	sScrollTextInfo	mRoomScrollTextInfo;

};

extern CBCM_OSDCtrl	g_OSDCtrl;
extern CBaseLock	g_OSD_Lock;

BOOL GenTextBitmapData(CFreeType *pFontClass,
						const wchar_t *pText,
						RGBQUAD sColor,			//字体颜色rgb
						RGBQUAD sBgColor,
						Font_Bitmap_Data *pOutData);

#if WORKMODE == NET_WORKMODE

void RoomScrollInfo(const char *msg, RECT rc, int iSize, int iTime, RGBQUAD sColor);

#endif

#endif
#endif
