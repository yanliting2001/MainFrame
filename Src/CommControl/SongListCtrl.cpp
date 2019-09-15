#include "SongListCtrl.h"
#include "UserMsgDefine.h"
#include "../GlobalUIClass.h"
#include "XMLParser.h"

int FindItemBySongID(const LISTITEM *pItem, const void *pData)
{
	const char *cID = (const char *)pData;

	if (pItem && cID)
	{
		SONGINFO *pSongInfo = (SONGINFO *) (pItem->uItemData);
		if (pSongInfo && strcmp(pSongInfo->cID, cID) == 0)
		{
			return 1;
		}
	}

	return 0;
}

CSongListCtrl::CSongListCtrl()
{
	SetRectXY(&mItemImageRect, 32, 16, 150, 85);
	SetRectXY(&mSongNameRect, 222, 30, 170, 68);
	SetRectXY(&mLanguageTypeRect, 222, 98, 170, 34);
	SetRectXY(&mSingerNameRect, 222, 98, 170, 34);
	SetRectXY(&mFavoritBtnRect, 17, 112, 59, 24);
	SetRectXY(&mRushBtnRect, 77, 112, 59, 24);
	SetRectXY(&mDelBtnRect, 137, 112, 59, 24);
  	SetRectXY(&mCloudBtnRect, 137, 80, 59, 24);
  	SetRectXY(&mGradeBtnRect, 100, 80, 59, 24);
  
	//SetRectXY(&mPreviewRect, 137, 112, 59, 24);

	mLastPressedItemIndex = -1;
	mLastPressedSubitemIndex = -1;

	mSongNameColor = 0;
	mSingerNameColor = 0;
	mSelectedColor = 0;
	mSongSize = 20;
	mSingerSize = 20;
}

CSongListCtrl::~CSongListCtrl()
{
	DbgOutput(DBG_INFO, "function %s quit\n", __FUNCTION__);
}

void CSongListCtrl::Create(CBaseWnd *pParent, RECT rc)
{
	SetOperateProvider(this);

	LoadResource();

	CParentClass::Create(pParent, rc);
}

void CSongListCtrl::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "SongListCtrl.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	XmlLoadRect(&parser, "ItemImageRectLocation", &mItemImageRect);
	XmlLoadRect(&parser, "SongNameRectLocation", &mSongNameRect);
	XmlLoadRect(&parser, "LanguageTypeRectLocation", &mLanguageTypeRect);
	XmlLoadRect(&parser, "SingerNameRectLocation", &mSingerNameRect);
	XmlLoadRect(&parser, "FavoritBtnRectLocation", &mFavoritBtnRect);
	XmlLoadRect(&parser, "RushBtnRectLocation", &mRushBtnRect);
  	XmlLoadRect(&parser, "DelBtnRectLocation", &mDelBtnRect);
  	XmlLoadRect(&parser, "CloudBtnRectLocation", &mCloudBtnRect);
	XmlLoadRect(&parser, "GradeBtnRectLocation", &mGradeBtnRect);
	mSongSize = parser.GetIntValue("SongNameRectLocation", "size", 25);
	mSingerSize = parser.GetIntValue("SingerNameRectLocation", "size", 18);

	char imgPath[64] = {0};
	strcpy(imgPath, parser.GetStrValue("ChineseImg","path","SongList/Chinese"));
	CreateImgTexture(imgPath, &(mSongBkTextures[BKIMAGE_CHINESE]));

	strcpy(imgPath, parser.GetStrValue("GlodImg","path","SongList/Other"));
	CreateImgTexture(imgPath, &(mSongBkTextures[BKIMAGE_GLOD]));

	strcpy(imgPath, parser.GetStrValue("SilverImg","path","SongList/Silver"));
	CreateImgTexture(imgPath, &(mSongBkTextures[BKIMAGE_SILVER]));

	strcpy(imgPath, parser.GetStrValue("BronzeImg","path","SongList/Cantonese"));
	CreateImgTexture(imgPath, &(mSongBkTextures[BKIMAGE_BRONZE]));

  strcpy(imgPath, parser.GetStrValue("FavoritImg","path","SongList/Favorit"));
	CreateBtnImgTextures(imgPath, mFavoritBtnTextures);

	strcpy(imgPath, parser.GetStrValue("RushImg","path","SongList/Rush"));
	CreateBtnImgTextures(imgPath, mRushBtnTextures);

	strcpy(imgPath, parser.GetStrValue("DelImg","path","SongList/Del"));
	CreateBtnImgTextures(imgPath, mDelBtnTextures);

  strcpy(imgPath, parser.GetStrValue("CloudImg","path","SongList/Cloud"));
	CreateBtnImgTextures(imgPath, mCloudBtnTextures);

  strcpy(imgPath, parser.GetStrValue("GradeImg","path","SongList/Grade"));
	CreateBtnImgTextures(imgPath, mGradeBtnTextures);

	char colorbuf[16];
	strcpy(colorbuf,parser.GetStrValue("NameColor","SongName","FF1F0D00"));
	mSongNameColor = strtoul(colorbuf,NULL,16);
	strcpy(colorbuf,parser.GetStrValue("NameColor","SingerName","FF1F0D00"));
	mSingerNameColor = strtoul(colorbuf,NULL,16);
	strcpy(colorbuf,parser.GetStrValue("NameColor","Selected","FFFCFAF8"));
	mSelectedColor = strtoul(colorbuf,NULL,16);
}

void CSongListCtrl::OnDeleteItem(LISTITEM *pli)
{
}

void CSongListCtrl::OnItemClicked(int nItemIndex, int nSubItemIndex)
{
	if (nItemIndex < 0)
	{
		return;
	}
  
	ReportMsg(LISTCTRL_ITEM_CLICKMSG, nItemIndex, nSubItemIndex);
}

void CSongListCtrl::OnItemPressed(int nItemIndex, int nSubItemIndex)
{
	SetSubItemBtnState(nItemIndex, nSubItemIndex, BUTTONSTATE_PRESSED);
}

void CSongListCtrl::ClearItemPressed()
{
	SetSubItemBtnState(-1, -1, BUTTONSTATE_NORMAL);
}

void CSongListCtrl::OnFirstItemIndexChanged(int nNewFirstItemIndex)
{
	ReportMsg(LISTCTRL_FIRSTITEM_CHANGEMSG, nNewFirstItemIndex, 0);
}

int CSongListCtrl::GetSubItemCount()
{
	return lviSongSubCount;
}

int CSongListCtrl::GetSubItemByPoint(POINT pt)
{
	if (PtInRect(&mItemImageRect, pt))
	{
		return lviSongItemImage;
	}
	else if (PtInRect(&mSongNameRect, pt))
	{
		return lviSongSongName;
	}
	else if()
	{
		return lviLanguageType;
	}
	else if (PtInRect(&mSingerNameRect, pt))
	{
		return lviSongSingerName;
	}
	else if (PtInRect(&mFavoritBtnRect, pt))
	{
    DbgOutput(DBG_INFO, "LINE =  %d lviSongFavoritBtn\n", __LINE__);
		return lviSongFavoritBtn;
	}
	else if (PtInRect(&mRushBtnRect, pt))
  {
    DbgOutput(DBG_INFO, "LINE =  %d lviSongRushBtn\n", __LINE__);
		return lviSongRushBtn;
	}
	else if (PtInRect(&mDelBtnRect, pt))
	{
    
    DbgOutput(DBG_INFO, "LINE =  %d lviSongDelBtn\n", __LINE__);
		return lviSongDelBtn;
	}
	else
	{
		return lviSongBackground;
	}
}

void CSongListCtrl::GetFixedSubItemInfo(int nItemIndex)
{
	LISTITEM *pli = GetListItem(nItemIndex);
	if (!pli || !pli->pSubItem)
	{
		return;
	}

	SONGINFO *pSongInfo = (SONGINFO *) (pli->uItemData);

	BOOL bPlaying = (pSongInfo==NULL) ? FALSE :
		(gPlayerInfo->IsSongInSelectedList(pSongInfo->cID)
				|| gPlayerInfo->IsSongInCloudList(pSongInfo->cID) );

//	DbgOutput(DBG_INFO,"GetBkImageType = %d \n",GetSongBkImageType(pSongInfo) );

	pli->pSubItem[lviSongBackground].texture.SetTexture(
		&(mSongBkTextures[GetSongBkImageType(pSongInfo)]));

	SetRectXY(&(pli->pSubItem[lviSongBackground].rect), 0, 0, GetItemWidth(), GetItemHeight());
	pli->pSubItem[lviSongBackground].font = FALSE;
	pli->pSubItem[lviSongBackground].color = 0xFFFFFFFF;
	pli->pSubItem[lviSongBackground].drawCircle = FALSE;

	pli->pSubItem[lviSongFavoritBtn].texture.SetTexture(&(mFavoritBtnTextures[0]));
	CopyRect(&(pli->pSubItem[lviSongFavoritBtn].rect), &mFavoritBtnRect);
	pli->pSubItem[lviSongFavoritBtn].font = FALSE;
	pli->pSubItem[lviSongFavoritBtn].color = 0xFFFFFFFF;
	pli->pSubItem[lviSongFavoritBtn].drawCircle = FALSE;

	pli->pSubItem[lviSongRushBtn].texture.SetTexture(&(mRushBtnTextures[0]));
	CopyRect(&(pli->pSubItem[lviSongRushBtn].rect), &mRushBtnRect);
	pli->pSubItem[lviSongRushBtn].font = FALSE;
	pli->pSubItem[lviSongRushBtn].color = 0xFFFFFFFF;
	pli->pSubItem[lviSongRushBtn].drawCircle = FALSE;

	pli->pSubItem[lviSongDelBtn].texture.SetTexture(&(mDelBtnTextures[0]));
  CopyRect(&(pli->pSubItem[lviSongDelBtn].rect), &mDelBtnRect);
	pli->pSubItem[lviSongDelBtn].font = FALSE;
	pli->pSubItem[lviSongDelBtn].color = 0xFFFFFFFF;
	pli->pSubItem[lviSongDelBtn].drawCircle = FALSE;

  if(pSongInfo->bExist == FALSE)
  {
    pli->pSubItem[lviSongCloudBtn].texture.SetTexture(&(mCloudBtnTextures[0]));
    CopyRect(&(pli->pSubItem[lviSongCloudBtn].rect), &mCloudBtnRect);
  	pli->pSubItem[lviSongCloudBtn].font = FALSE;
  	pli->pSubItem[lviSongCloudBtn].color = 0xFFFFFFFF;
  	pli->pSubItem[lviSongCloudBtn].drawCircle = FALSE;
  }

  if(pSongInfo->bChos == TRUE)
  {
    pli->pSubItem[lviSongGradeBtn].texture.SetTexture(&(mGradeBtnTextures[0]));
    CopyRect(&(pli->pSubItem[lviSongGradeBtn].rect), &mGradeBtnRect);
  	pli->pSubItem[lviSongGradeBtn].font = FALSE;
  	pli->pSubItem[lviSongGradeBtn].color = 0xFFFFFFFF;
  	pli->pSubItem[lviSongGradeBtn].drawCircle = FALSE;
  }

	pli->pSubItem[lviSongItemImage].font = FALSE;
	pli->pSubItem[lviSongItemImage].color = 0xFFFFFFFF;
	pli->pSubItem[lviSongItemImage].drawCircle = FALSE;

//	pli->pSubItem[lviPreview].texture.SetTexture(&(mPreviewTextures[0]));
//	CopyRect(&(pli->pSubItem[lviPreview].rect), &mPreviewRect);
//	pli->pSubItem[lviPreview].font = FALSE;
//	pli->pSubItem[lviPreview].color = 0xFFFFFFFF;
//	pli->pSubItem[lviPreview].drawCircle = FALSE;

	pli->pSubItem[lviSongSongName].font = TRUE;
	pli->pSubItem[lviSongSongName].color = bPlaying ? mSelectedColor:mSongNameColor;
	pli->pSubItem[lviSongSongName].drawCircle = FALSE;

	pli->pSubItem[lviSongSingerName].font = TRUE;
	pli->pSubItem[lviSongSingerName].color = bPlaying ? mSelectedColor:mSingerNameColor;
	pli->pSubItem[lviSongSingerName].drawCircle = FALSE;
}

void CSongListCtrl::GetUnFixedSubItemInfo(int nItemIndex)
{
	LISTITEM *pli = GetListItem(nItemIndex);
	if (!pli || !pli->pSubItem)
	{
		return;
	}

	SONGINFO *pSongInfo = (SONGINFO *) (pli->uItemData);

	int nDirect;
	SIZE szTexture;

//	const char *cSongPath = theBaseApp->GetSongTextureFolder();
//	char cTemp[MAX_PATH];
//	char cImgPath[MAX_PATH];
//	//int idx;
//	memset(cTemp,0,MAX_PATH);
//	if (pSongInfo && !pli->pSubItem[lviSongItemImage].texture.HasTexture())
//	{
//#ifdef DEBUG
//		//CombinePathName(cTemp, cSongPath, "1.mem");
//		//CombinePathName(cTemp, cSongPath, pSongInfo->cPicPath);
//#else
//		//CombinePathName(cTemp, cSongPath, pSongInfo->cPicPath);
//#endif
//
////		sscanf(pSongInfo->cPicPath,"%d.jpg",&idx);
////		sprintf(cImgPath,"%08d_1.jpg",idx);
//		sprintf(cImgPath,"%s_1.jpg",pSongInfo->cID);
//		CombinePathName(cTemp, cSongPath, cImgPath);
//
//		 if(cTemp && strlen(cTemp)>0)
//		 {
//			if (!IsFileExist(cTemp))
//			{
//				{
//					SetFileExtName(cTemp, ".jpg");
//					if (!IsFileExist(cTemp))
//					{
//						SetFileExtName(cTemp, ".png");
//						CombinePathName(cTemp, cSongPath, "00000000_0.png");
//					}
//				}
//			}
//
//			pli->pSubItem[lviSongItemImage].texture.CreateFromImgFile(cTemp);
//		 }
//	}
//	CopyRect(&(pli->pSubItem[lviSongItemImage].rect), &mItemImageRect);

	const char *cSingerPath = theBaseApp->GetSingerTextureFolder();
	char cTemp[MAX_PATH];
	memset(cTemp,0,MAX_PATH);
	char cImgPath[MAX_PATH];
	if (pSongInfo && !pli->pSubItem[lviSongItemImage].texture.HasTexture())
	{
		sprintf(cImgPath,"%d.jpg", pSongInfo->singerID);
		CombinePathName(cTemp, cSingerPath, cImgPath);

		if(cTemp && strlen(cTemp)>0)
		{
			if (!IsFileExist(cTemp))
			{
				sprintf(cTemp, "%s/0.png", cSingerPath);
			}
			pli->pSubItem[lviSongItemImage].texture.CreateFromImgFile(cTemp);
		}
	}
	CopyRect(&(pli->pSubItem[lviSongItemImage].rect), &mItemImageRect);
	pli->pSubItem[lviSongItemImage].font = FALSE;
	pli->pSubItem[lviSongItemImage].color = 0xFFFFFFFF;

	if (pSongInfo && strlen(pSongInfo->cName))
	{
		if (!pli->pSubItem[lviSongSongName].texture.HasTexture())
		{
			CBaseStringA in;
			//in.Format("item %d %s", nItemIndex, pSongInfo->cName);
			in.Set(pSongInfo->cName);

			DbgOutput(DBG_DEBUG, "GetUnFixedSubItemInfo(%d) %s\n", nItemIndex, in.GetString());

			pli->pSubItem[lviSongSongName].texture.CreateFromText(
				in.GetString(),
				mSongSize,
				TRUE,
				RECTWIDTH(mSongNameRect),
				RECTHEIGHT(mSongNameRect));
		}

		szTexture.cx = pli->pSubItem[lviSongSongName].texture.GetImageWidth();
		szTexture.cy = pli->pSubItem[lviSongSongName].texture.GetImageHeight();
	}
	else
	{
		szTexture.cx = RECTWIDTH(mSongNameRect);
		szTexture.cy = RECTHEIGHT(mSongNameRect);
	}
	nDirect = DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER;
	//nDirect = DRAWTEXT_DIRECT_CENTER|DRAWTEXT_DIRECT_VCENTER;
	GetTextRectByDirect(mSongNameRect, szTexture.cx, szTexture.cy,
		nDirect, &(pli->pSubItem[lviSongSongName].rect));

	if(pSongInfo)
	{
		char *languageType = FastSearch_GetLanguageTypeNameByID(pSongInfo->langID);
		if (!pli->pSubItem[lviLanguageType].texture.HasTexture())
		{
			CBaseStringW in;
			in.Set(languageType ? languageType : "");

			pli->pSubItem[lviLanguageType].texture.CreateFromText(
				in.GetString(),
				mSingerSize,
				TRUE,
				RECTWIDTH(mLanguageTypeRect),
				RECTHEIGHT(mLanguageTypeRect));
		}

		szTexture.cx = pli->pSubItem[lviLanguageType].texture.GetImageWidth();
		szTexture.cy = pli->pSubItem[lviLanguageType].texture.GetImageHeight();
	}else
	{
		szTexture.cx = RECTWIDTH(mLanguageTypeRect);
		szTexture.cy = RECTHEIGHT(mLanguageTypeRect);
	}
	nDirect = DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER;
	//nDirect = DRAWTEXT_DIRECT_CENTER|DRAWTEXT_DIRECT_VCENTER;
	GetTextRectByDirect(mLanguageTypeRect, szTexture.cx, szTexture.cy,
		nDirect, &(pli->pSubItem[lviLanguageType].rect));

	if (pSongInfo )
	{
		char *singer = FastSearch_GetSingerByID(pSongInfo->singerID);

		if (!pli->pSubItem[lviSongSingerName].texture.HasTexture())
		{
			CBaseStringW in;
			in.Set(singer ? singer : "");

			pli->pSubItem[lviSongSingerName].texture.CreateFromText(
				in.GetString(),
				mSingerSize,
				TRUE,
				RECTWIDTH(mSingerNameRect),
				RECTHEIGHT(mSingerNameRect));
		}

		szTexture.cx = pli->pSubItem[lviSongSingerName].texture.GetImageWidth();
		szTexture.cy = pli->pSubItem[lviSongSingerName].texture.GetImageHeight();
	}
	else
	{
		szTexture.cx = RECTWIDTH(mSingerNameRect);
		szTexture.cy = RECTHEIGHT(mSingerNameRect);
	}
	nDirect = DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER;
	//nDirect = DRAWTEXT_DIRECT_CENTER|DRAWTEXT_DIRECT_VCENTER;
	GetTextRectByDirect(mSingerNameRect, szTexture.cx, szTexture.cy,
		nDirect, &(pli->pSubItem[lviSongSingerName].rect));
}

void CSongListCtrl::SetSubItemBtnState(int nItemIndex, int nSubItemIndex, int nBtnState)
{
	if (mLastPressedItemIndex >= 0)
	{
		CTexture *pTexture = NULL;
		if (mLastPressedSubitemIndex == lviSongFavoritBtn)
		{
      
			DbgOutput(DBG_DEBUG, "lviSongFavoritBtn\n");
			pTexture = mFavoritBtnTextures;
		}
		if (mLastPressedSubitemIndex == lviSongRushBtn)
		{
      
			DbgOutput(DBG_DEBUG, "lviSongRushBtn\n");
			pTexture = mRushBtnTextures;
		}
		if (mLastPressedSubitemIndex == lviSongDelBtn)
		{
      
			DbgOutput(DBG_DEBUG, "lviSongDelBtn\n");
			pTexture = mDelBtnTextures;
		}

		if (pTexture)
		{
			SetItemTexture(
				mLastPressedItemIndex,
				mLastPressedSubitemIndex,
				&(pTexture[0]));
		}
	}

	if (nItemIndex >= 0)
	{
		CTexture *pTexture = NULL;
		if (nSubItemIndex == lviSongFavoritBtn)
		{
      
			DbgOutput(DBG_DEBUG, "lviSongFavoritBtn\n");
			pTexture = mFavoritBtnTextures;
		}
		if (nSubItemIndex == lviSongRushBtn)
		{
      
			DbgOutput(DBG_DEBUG, "lviSongRushBtn\n");
			pTexture = mRushBtnTextures;
		}
		if (nSubItemIndex == lviSongDelBtn)
		{
      
			DbgOutput(DBG_DEBUG, "lviSongDelBtn\n");
			pTexture = mDelBtnTextures;
		}

		if (pTexture)
		{
			CTexture *pSet = &(pTexture[0]);

			if ( nBtnState & BUTTONSTATE_PRESSED )
			{
				pSet = &(pTexture[1]);
			}

			if ( !pSet->HasTexture() )
			{
				pSet = &(pTexture[0]);
			}

			SetItemTexture(
				nItemIndex,
				nSubItemIndex,
				pSet);
		}
	}

	mLastPressedItemIndex = nItemIndex;
	mLastPressedSubitemIndex = nSubItemIndex;
}

void CSongListCtrl::AddSongItem(const SONGINFO *pSongInfo, int nIndex)
{
	if ( !pSongInfo )
	{
		return;
	}

	SetItemData(nIndex, (UINT64)pSongInfo);
}

int CSongListCtrl::FindItemByID(const char *cID)
{
	return FindItem((UINT64)cID, FindItemBySongID);
}

SONGBKIMAGETYPE CSongListCtrl::GetSongBkImageType(SONGINFO *pSongInfo)
{
	if (!pSongInfo)
	{
		return BKIMAGE_CHINESE;
	}

//	if ( pSongInfo->bCloud == 1 && pSongInfo->bExist == 0)
//	{
//		return BKIMAGE_CLOUD;
//	}
	return BKIMAGE_CHINESE;

//	if ( strcmp(pSongInfo->cLang, "国语") == 0 )
//	{
//		return BKIMAGE_CHINESE;
//	}
//	else if ( strcmp(pSongInfo->cLang, "粤语") == 0 )
//	{
//		return BKIMAGE_CANTONESE;
//	}
//	else if ( strcmp(pSongInfo->cLang, "闽南") == 0 )
//	{
//		return BKIMAGE_MINNAN;
//	}
//	else if ( strcmp(pSongInfo->cLang, "韩语") == 0 )
//	{
//		return BKIMAGE_KOREAN;
//	}
//	else if ( strcmp(pSongInfo->cLang, "日语") == 0 )
//	{
//		return BKIMAGE_JAPNESE;
//	}
//	else if ( strcmp(pSongInfo->cLang, "英语") == 0 )
//	{
//		return BKIMAGE_ENGLISH;
//	}
//	else
//	{
//		return BKIMAGE_OTHER;
//	}
}

CTexture *CSongListCtrl::GetSongBkImageTexture(SONGINFO * pSongInfo, int pos)
{
	if ( !pSongInfo ) return NULL;
	if ( pos == -1 ) return &(mSongBkTextures[GetSongBkImageType(pSongInfo)]);
	else if ( pos == 0 ) return &mSongBkTextures[BKIMAGE_GLOD];
	else if ( pos == 1 ) return &mSongBkTextures[BKIMAGE_SILVER];
	else if ( pos == 2 ) return &mSongBkTextures[BKIMAGE_BRONZE];

	return &mSongBkTextures[BKIMAGE_CHINESE];
}
