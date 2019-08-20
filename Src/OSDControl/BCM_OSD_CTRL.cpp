#include "BCM_OSD_CTRL.h"
#if USE_MODULE_BCM

CBCM_OSDCtrl	g_OSDCtrl;
static int 		g_iExist = FALSE;
CBaseLock		g_OSD_Lock;

static BOOL		bMainScrollStop = FALSE;

static void Waiting()
{
	usleep(100000);
}

BOOL GenTextBitmapData(CFreeType *pFontClass,
						const wchar_t *pText,
						RGBQUAD sColor,			//字体颜色rgb
						RGBQUAD sBgColor,
						Font_Bitmap_Data *pOutData)
{
	if ( !pFontClass || !pText || !pOutData )
		return FALSE;

	int iLoadOK = pFontClass->GetBmpDataFromString((wchar_t *)pText, sColor, sBgColor, pOutData);

	return iLoadOK == 0 ? TRUE : FALSE;
}

static BOOL GenBitmapDataFromRect(CFreeType *pFontClass, RECT rc, Font_Bitmap_Data *pOutData)
{
	if ( !pFontClass || !pOutData )
		return FALSE;

	int iLoadOK = pFontClass->GetBmpDataFromRect(rc, pOutData);

	return iLoadOK == 0 ? TRUE : FALSE;
}

LPVOID ScrollTextThreadProc(LPVOID lpParam)
{
	sScrollTextInfo Info, *ptr = NULL;
	ptr = (sScrollTextInfo *)lpParam;
	if ( !ptr )
		return NULL;

	//Info.mText.Set(ptr->mText.GetString());
	memcpy(&(Info.mRc), &(ptr->mRc), sizeof(RECT));
	memcpy(&(Info.msBgColor), &(ptr->msBgColor), sizeof(Info.msBgColor));
	memcpy(&(Info.msColor), &(ptr->msColor), sizeof(Info.msColor));
	Info.miSize = ptr->miSize;
	Info.miSpeed = ptr->miSpeed;
	Info.miTime = ptr->miTime;
	Info.meType = ptr->meType;

	CBaseStringW 	*pText 		 = NULL;
	BOOL 			*bpIsInThead = NULL;
	BOOL			*bpUpdate	 = NULL;
	RGBQUAD			*psColor	 = NULL;

	if ( Info.meType == ScrollType_Main )
	{
		pText 		= &g_OSDCtrl.mMainScrollText;
		bpIsInThead	= &g_OSDCtrl.mbIsInMainScrollThread;
		bpUpdate	= &g_OSDCtrl.mbUpdateMainScrollText;
		psColor		= &g_OSDCtrl.msTextColor;
	}
	else if ( Info.meType == ScrollType_Sub )
	{
		pText 		= &g_OSDCtrl.mSubScrollText;
		bpIsInThead	= &g_OSDCtrl.mbIsInSubScrollThread;
		bpUpdate	= &g_OSDCtrl.mbUpdateSubScrollText;
		psColor		= &(Info.msColor);
	}
	else if ( Info.meType == ScrollType_Room )
	{
		pText 		= &g_OSDCtrl.mRoomScrollText;
		bpIsInThead	= &g_OSDCtrl.mbIsInRoomScrollThread;
		bpUpdate	= &g_OSDCtrl.mbUpdateRoomScrollText;
		psColor		= &(Info.msColor);
	}

	if ( !pText || !bpIsInThead || !bpUpdate )
		return NULL;

	CFreeType Font;
	Font.LoadFont(FONTPATH);
	Font.SetFontSize(Info.miSize);
	//  生成字符串的bitmap
	Font_Bitmap_Data Text_Data;
	Text_Data.text_alpha = NULL;
	if ( !GenTextBitmapData(&Font, pText->GetString(), Info.msColor, Info.msBgColor, &Text_Data) )
	{
		g_OSDCtrl.FreeBitmapData(&Text_Data);
		*bpIsInThead = FALSE;
		return NULL;
	}

	// 生成需要输出的bitmap,空白的内容
	Font_Bitmap_Data ScrollText_Data;
	ScrollText_Data.text_alpha = NULL;
	Info.mRc.bottom = Info.mRc.top + Text_Data.iHeight;
	if ( !GenBitmapDataFromRect(&Font, Info.mRc, &ScrollText_Data) )
	{
		g_OSDCtrl.FreeBitmapData(&Text_Data);
		g_OSDCtrl.FreeBitmapData(&ScrollText_Data);
		*bpIsInThead = FALSE;
		return NULL;
	}

	int iOutputWidth = RECTWIDTH(Info.mRc);

	*bpIsInThead = TRUE;
	*bpUpdate	 = FALSE;
	for ( int num = Info.miTime; (!g_iExist && (Info.miTime == 0 || num > 0)); --num )
	{
		// 初始化
		int iDestStartX	 = iOutputWidth;
		int iSrcStartX	 = 0;
		int iWidth 		 = 0;
		do
		{
			if ( *bpUpdate )
			{
				// 需要更新,重新滚动
				*bpUpdate = FALSE;
				g_OSDCtrl.FreeBitmapData(&Text_Data);
				//if ( !GenTextBitmapData(&Font, pText->GetString(), Info.msColor, Info.msBgColor, &Text_Data) )
				if ( !GenTextBitmapData(&Font, pText->GetString(), *psColor, Info.msBgColor, &Text_Data) )
				{
					g_OSDCtrl.FreeBitmapData(&Text_Data);
					g_OSDCtrl.FreeBitmapData(&ScrollText_Data);
					*bpIsInThead = FALSE;
					return NULL;
				}
				num = Info.miTime;
				break;
			}
			iDestStartX -= Info.miSpeed;
			iWidth		+= Info.miSpeed;
			if ( iDestStartX < 0 )
			{
				iDestStartX = 0;
				iWidth		= iOutputWidth;
				iSrcStartX	+= Info.miSpeed;
				if ( iSrcStartX > Text_Data.iWidth )	// 一轮结束
					break;
			}
			Font.BMP_BilterFromBmpData(iWidth, iDestStartX, &ScrollText_Data,
								  iSrcStartX, &Text_Data);

			if ( bMainScrollStop && Info.meType == ScrollType_Main )
			{
				g_OSDCtrl.EraseImg(Info.mRc);
			}
			else
			{
				g_OSD_Lock.Lock();
				g_OSDCtrl.EraseImg(Info.mRc);
				g_OSDCtrl.Show24BitmapImgData(&ScrollText_Data, Info.mRc);
				g_OSD_Lock.Unlock();
			}

			if ( Info.meType == ScrollType_Main )
				g_OSDCtrl.Update();

			Waiting();

		}while(!g_iExist);
	}

	g_OSDCtrl.FreeBitmapData(&Text_Data);
	g_OSDCtrl.FreeBitmapData(&ScrollText_Data);
	*bpIsInThead = FALSE;
	DbgOutput(DBG_INFO, "===the %s quit===\n", __FUNCTION__);

	return NULL;
}

CBCM_OSDCtrl::CBCM_OSDCtrl()
{
	mbUpdateMainScrollText = FALSE;
	mbIsInMainScrollThread = FALSE;
	mbUpdateSubScrollText  = FALSE;
	mbIsInSubScrollThread  = FALSE;
	mbUpdateRoomScrollText = FALSE;
	mbIsInRoomScrollThread = FALSE;
}

CBCM_OSDCtrl::~CBCM_OSDCtrl()
{
	RECT rc = {0, 0, LAYOUT_WIDTH, LAYOUT_HEIGHT};
	EraseImg(rc);
	DeInit();
}

int CBCM_OSDCtrl::Init()
{
	//return FreeType_Init();
	return 0;
}

void CBCM_OSDCtrl::DeInit()
{
	//FreeType_DeInit();
	RECT rc = {0, 0, LAYOUT_WIDTH, LAYOUT_HEIGHT};
	EraseImg(rc);
	Update();
	g_iExist = TRUE;
	Waiting();
	mSubScroll.StopThread();
	mMainScroll.StopThread();
}

int CBCM_OSDCtrl::ShowBitmapFile(const char *pImgName, RECT rc)
{
	if ( !pImgName )
		return -1;

	D_RECT rcShow;
	 rcShow.Left = rc.left;
	 rcShow.Top = rc.top;
	 rcShow.Right = rc.right;
	 rcShow.Bottom = rc.bottom;

	g_pPlayerCtrl->ShowBmpFile(pImgName, rcShow);
	//WSD_AL_OSD_DrawBmp(rc.left, rc.top, 1, (unsigned char *)pImgName);

	return 0;
}

int CBCM_OSDCtrl::ShowPicFile(const char *pPicName, RECT rc)
{
	if ( !pPicName )
		return -1;

	D_RECT rcShow;
	rcShow.Left = rc.left;
	rcShow.Top = rc.top;
	rcShow.Right = rc.right;
	rcShow.Bottom = rc.bottom;

	g_pPlayerCtrl->ShowPicFile(pPicName, rcShow);
//	WSD_AL_OSD_DrawPic(rc.left, rc.top, 0, (unsigned char *)pPicName);

	return 0;
}

int CBCM_OSDCtrl::Show24BitmapImgData(Font_Bitmap_Data *pDataInfo, RECT rc)
{
	if ( !pDataInfo && !pDataInfo->text_alpha )
		return -1;

	g_pPlayerCtrl->ShowBmpBuffer(rc.left, rc.top, (char *)pDataInfo->text_alpha);

//	WSD_AL_OSD_DrawBmpBuffer((D_INT32)rc.left, (D_INT32)rc.top, 1, (unsigned char *)pDataInfo->text_alpha);

	return 0;
}

int CBCM_OSDCtrl::Show32BitmapImgData(Font_Bitmap_Data *pDataInfo, RECT rc)
{
	if ( !pDataInfo && !pDataInfo->text_alpha )
		return -1;
	g_pPlayerCtrl->ShowBmpData(rc.left, rc.top, (char *)pDataInfo->text_alpha, pDataInfo->iWidth, pDataInfo->iHeight);
//	WSD_AL_OSD_CopyToScreen((long int)rc.left, (long int)rc.top, (signed char *)pDataInfo->text_alpha, (long int)pDataInfo->iWidth, (long int)pDataInfo->iHeight);

	return 0;
}

void CBCM_OSDCtrl::EraseImg(RECT rc)
{
	D_RECT rcShow;
	rcShow.Left = rc.left;
	rcShow.Top = rc.top;
	rcShow.Right = rc.right;
	rcShow.Bottom = rc.bottom;

	g_pPlayerCtrl->EraseImg(rcShow);
//	WSD_AL_OSD_ClearRect(rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top);
}

void CBCM_OSDCtrl::Update()
{
	g_OSD_Lock.Lock();
	g_pPlayerCtrl->UpdateOsd();
	//WSD_AL_OSD_Update();
	g_OSD_Lock.Unlock();
}

void CBCM_OSDCtrl::FreeBitmapData(Font_Bitmap_Data *pData)
{
	if ( pData )
	{
		pData->iHeight = 0;
		pData->iWidth = 0;
		if ( pData->text_alpha )
		{
			free(pData->text_alpha);
			pData->text_alpha = NULL;
		}
	}
}

int CBCM_OSDCtrl::ShowScrollText(const wchar_t *msg, sScrollTextInfo *pScrollInfo)
{
	if ( !msg || !pScrollInfo )
		return -1;

	g_iExist = FALSE;

	if ( pScrollInfo->meType == ScrollType_Main )
	{
		bMainScrollStop = FALSE;

		mMainScrollText.Set((msg));
		if ( !mbIsInMainScrollThread )
		{
			mMainScroll.StopThread();
			if ( mMainScroll.StartThread(ScrollTextThreadProc, pScrollInfo, "ScrollTextThreadProc") )
				mbIsInMainScrollThread = TRUE;
		}
		else
		{
			mbUpdateMainScrollText = TRUE;
			memcpy(&msTextColor, &pScrollInfo->msColor, sizeof(msTextColor));
		}
	}
	else if (pScrollInfo->meType == ScrollType_Sub )
	{
		mSubScrollText.Set(msg);
		if ( !mbIsInSubScrollThread )
		{
			mSubScroll.StopThread();
			if ( mSubScroll.StartThread(ScrollTextThreadProc, pScrollInfo, "ScrollTextThreadProc") )
				mbIsInSubScrollThread = TRUE;
		}
		else
			mbUpdateSubScrollText = TRUE;
	}
	else if ( pScrollInfo->meType == ScrollType_Room )
	{
		mRoomScrollText.Set(msg);
		if ( !mbIsInRoomScrollThread )
		{
			mRoomScroll.StopThread();
			if ( mRoomScroll.StartThread(ScrollTextThreadProc, pScrollInfo, "ScrollTextThreadProc") )
				mbIsInRoomScrollThread = TRUE;
		}
		else
			mbUpdateRoomScrollText = TRUE;
	}

	return 0;
}

void CBCM_OSDCtrl::StopMainScrollText()
{
	bMainScrollStop = TRUE;
}

#if WORKMODE == NET_WORKMODE
void RoomScrollInfo(const char *msg, RECT rc, int iSize, int iTime, RGBQUAD sColor)
{
	CBaseStringW text;
	text.Set(msg);

	g_OSDCtrl.mRoomScrollTextInfo.meType = ScrollType_Room;
	memcpy(&g_OSDCtrl.mRoomScrollTextInfo.mRc, &rc, sizeof(RECT));
	g_OSDCtrl.mRoomScrollTextInfo.miSize = iSize;
	g_OSDCtrl.mRoomScrollTextInfo.miSpeed = gs_RoomOSDScroll.miScrollSpeed;
	g_OSDCtrl.mRoomScrollTextInfo.miTime = iTime;
	memcpy(&g_OSDCtrl.mRoomScrollTextInfo.msColor, &sColor, sizeof(RGBQUAD));
	memset(&g_OSDCtrl.mRoomScrollTextInfo.msBgColor, 0, sizeof(RGBQUAD));

	g_OSDCtrl.ShowScrollText(text.GetString(), &g_OSDCtrl.mRoomScrollTextInfo);
}
#endif

#endif
