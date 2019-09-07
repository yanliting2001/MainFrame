#include "SearchTypeBar.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"
#include "FontCodeConvApi.h"
#include "../AdvanceSettingsWnd/AdvanceSettingApi.h"

CSearchTypeBar::CSearchTypeBar()
{
	mFirstShowItemWhenPressed = 0;
	mpCurSearchItem = NULL;
	mFirstShowItemIndex = 0;
	mSelectedIndex = 0;

	mIsOnClickValid = TRUE;
}

CSearchTypeBar::~CSearchTypeBar()
{
	DbgOutput(DBG_INFO, "function %s quit\n", __FUNCTION__);
}

void CSearchTypeBar::Create(
	CBaseWnd *pParent,
	RECT rc)
{
	CParentClass::Create("SearchTypeBar", pParent, rc);

	SetOnTouchListener(this);

	int nItemWidth = RECTWIDTH(mWndRect) / SEARCH_ITEM_COUNT;

	RECT rcItem;

	SetRectXY(&rcItem, 0, 0, nItemWidth, RECTHEIGHT(rc));
	mSelectedWnd.CreateStatic(this, rcItem);

	int i;
	for ( i = 0 ; i < SEARCH_ITEM_COUNT+1; i++ )
	{
		SetRectXY(&rcItem, nItemWidth*i, 0, nItemWidth, RECTHEIGHT(mWndRect));

		CBaseWnd *pItemWnd = new CBaseWnd;
		if (!pItemWnd)
		{
			break;
		}

		pItemWnd->CreateStatic(this, rcItem);
		pItemWnd->SetOnTouchListener(this);
		mSubItemWndList.AddData(pItemWnd);
	}

	LoadResource();
}

void CSearchTypeBar::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "SearchTypeBar.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml

	int nItemWidth = RECTWIDTH(mWndRect) / SEARCH_ITEM_COUNT;

	RECT rcItem;
	SetRectXY(&rcItem, 0, 0, nItemWidth, RECTHEIGHT(mWndRect));
	mSelectedWnd.MoveWindow(&rcItem);

	int i;
	for ( i = 0 ; i < SEARCH_ITEM_COUNT+1; i++ )
	{
		SetRectXY(&rcItem, nItemWidth*i, 0, nItemWidth, RECTHEIGHT(mWndRect));

		CBaseWnd *pItemWnd = (CBaseWnd *)mSubItemWndList.GetAt(i);
		if (!pItemWnd)
		{
			continue;
		}

		pItemWnd->MoveWindow(&rcItem);
	}

	CTexture bkTexture;
	char imgPath[64] = {0};

	strcpy(imgPath, parser.GetStrValue("BkGroundImg","path","SearchBar/BkGround"));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);

	strcpy(imgPath, parser.GetStrValue("SelectedImg","path","SearchBar/Selected"));
	CreateImgTexture(imgPath, &bkTexture);
	mSelectedWnd.SetBackgroundTexture(&bkTexture);
}

void CSearchTypeBar::LoadResourceForInternetTv()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "SearchTypeBar.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	CTexture bkTexture;
	char imgPath[64] = {0};

	strcpy(imgPath, parser.GetStrValue("BkGroundForInternetvImg","path","SearchBar/BkGround"));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);
}

void CSearchTypeBar::OnDestroy()
{
	CParentClass::OnDestroy();

	DelNormalList(&mSubItemWndList, CBaseWnd);
}

void CSearchTypeBar::OnTouchDown(
	CBaseWnd *pWnd,
	POINT pt)
{
	if ( !mIsOnClickValid )
		return ;

	pWnd->ConvertWndPointToScreenPoint(&mPtPressedScreen, pt);
	ConvertScreenPointToWndPoint(&mPtPressedScreen, mPtPressedScreen);

	mFirstShowItemWhenPressed = mFirstShowItemIndex;

	pWnd->SetCapture();
}

void CSearchTypeBar::OnTouchUp(
	CBaseWnd *pWnd,
	POINT pt,
	int xDistance,
	int yDistance,
	UINT64 uUsedTime)
{
	if ( !mIsOnClickValid )
		return ;

	POINT ptDrag;
	pWnd->ConvertWndPointToScreenPoint(&ptDrag, pt);
	ConvertScreenPointToWndPoint(&ptDrag, ptDrag);

	if (ptDrag.x < mWndRect.left)
	{
		ptDrag.x = mWndRect.left;
	}
	else if (ptDrag.x >= mWndRect.right)
	{
		ptDrag.x = mWndRect.right-1;
	}

	int xDragLen = ptDrag.x-mPtPressedScreen.x;
	int nItemWidth = RECTWIDTH(mWndRect) / SEARCH_ITEM_COUNT;
	int nItemMove = ((abs(xDragLen)+nItemWidth/2) / nItemWidth) * (xDragLen > 0 ? 1 : -1);

	SetFirstShowItem(mFirstShowItemWhenPressed-nItemMove);
	InitSubItem(mFirstShowItemIndex);
	ShowSubItem(0);

	BOOL bNeedUpdate = mFirstShowItemWhenPressed != mFirstShowItemIndex ? TRUE : FALSE;

	if (abs(mPtPressedScreen.x-ptDrag.x) < CLICK_MAX_DISTANCE)
	{
		int nOldSelectIndex = mSelectedIndex;
		int nNewSelectIndex = SetSelectedIndex(GetIndexByXPos(ptDrag.x));
		if (nOldSelectIndex != nNewSelectIndex)
		{
			bNeedUpdate = TRUE;
		}
	}

	if (bNeedUpdate)
	{
		ReportMsg(SEARCHITEM_CLICKMSG, 0, 0);
	}

	pWnd->ReleaseCapture();
}

void CSearchTypeBar::OnTouchMove(
	CBaseWnd *pWnd,
	POINT pt)
{
	if ( !mIsOnClickValid )
		return ;

	if ( !mpCurSearchItem )
		return ;

	POINT ptDrag;
	pWnd->ConvertWndPointToScreenPoint(&ptDrag, pt);
	ConvertScreenPointToWndPoint(&ptDrag, ptDrag);

	int nItemWidth = RECTWIDTH(mWndRect) / SEARCH_ITEM_COUNT;

	if (ptDrag.x < mWndRect.left)
	{
		ptDrag.x = mWndRect.left;
	}
	else if (ptDrag.x >= mWndRect.right)
	{
		ptDrag.x = mWndRect.right-1;
	}

	int xDragLen = ptDrag.x-mPtPressedScreen.x;

	int nItemsMove = 0;
	int nOffset = 0;

	if (xDragLen > 0)
	{
		nItemsMove = (xDragLen+nItemWidth) / nItemWidth;
		nOffset = nItemWidth - (xDragLen % nItemWidth);
	}
	else
	{
		nItemsMove = (xDragLen) / nItemWidth;
		nOffset = (-xDragLen) % nItemWidth;
	}

	int nSubItemCount = mpCurSearchItem->sChildItemList.GetCount();
	if ((mFirstShowItemWhenPressed-nItemsMove) < -1)
	{
		return;
	}
	else if (nSubItemCount-mFirstShowItemWhenPressed+nItemsMove < SEARCH_ITEM_COUNT)
	{
		return;
	}

	if (abs(nItemsMove) > 0)
	{
		int nLastFirstShowItem = mFirstShowItemIndex;
		SetDragFirstShowItem(mFirstShowItemWhenPressed-nItemsMove);
		if (nLastFirstShowItem != mFirstShowItemIndex)
		{
			int nMoveCount = mFirstShowItemIndex-nLastFirstShowItem;
			if (nMoveCount > 0)
			{
				mSubItemWndList.MoveData(0, nMoveCount, mSubItemWndList.GetCount());
			}
			else
			{
				mSubItemWndList.MoveData(mSubItemWndList.GetCount()+nMoveCount, -nMoveCount, -1);
			}

			InitSubItem(mFirstShowItemIndex);
		}
	}

	ShowSubItem(nOffset);
}

void CSearchTypeBar::OnTouchLeave(
	CBaseWnd *pOldTouchWnd,
	CBaseWnd *pNewTouchWnd)
{
	if ( !mIsOnClickValid )
		return ;

	SetFirstShowItem(mFirstShowItemIndex);
	InitSubItem(mFirstShowItemIndex);
	ShowSubItem(0);
}

void CSearchTypeBar::SetSearchItem(
	SEARCHITEM *pSearchItem,
	int nFirstShowItemIndex)
{
	mpCurSearchItem = pSearchItem;
	if (mpCurSearchItem == NULL)
	{
		SetWindowVisible(FALSE);

		return;
	}

	SetFirstShowItem(nFirstShowItemIndex);
	SetSelectedIndex(nFirstShowItemIndex-mFirstShowItemIndex);

	if (mpCurSearchItem->sChildItemList.GetCount() == 0)
	{
		SetWindowVisible(FALSE);

		return;
	}

	SetWindowVisible(TRUE);

	InitSubItem(mFirstShowItemIndex);
	ShowSubItem(0);
}

SEARCHITEM * CSearchTypeBar::GetSelectedSubItem()
{
	if (!mpCurSearchItem)
	{
		return NULL;
	}

	if (mpCurSearchItem->sChildItemList.GetCount() == 0)
	{
		return mpCurSearchItem;
	}

	SEARCHITEM *pSubItem = NULL;
	if ( gs_SystemSetting.miLanguageID == LanguageID_Chinese )
	{
		pSubItem = (SEARCHITEM *)mpCurSearchItem->sChildItemList.GetAt(mFirstShowItemIndex+mSelectedIndex);
	}
	else if ( mpCurSearchItem->eType == SearchBySinger )
	{
		pSubItem = (SEARCHITEM *)mpCurSearchItem->sChildItemList.GetAt(mFirstShowItemIndex+mSelectedIndex);
	}
	else if ( mpCurSearchItem->eType == SearchBySongType )
	{
		pSubItem = (SEARCHITEM *)gSearchSongType.sChildItemList.GetAt(mFirstShowItemIndex+mSelectedIndex);
	}
	else if ( mpCurSearchItem->eType == SearchByLanguageType )
	{
		pSubItem = (SEARCHITEM *)gSearchLanguageType.sChildItemList.GetAt(mFirstShowItemIndex+mSelectedIndex);
	}
	else if ( mpCurSearchItem->eType == SearchByMovieSong )
	{
		pSubItem = (SEARCHITEM *)gSearchMovieSong.sChildItemList.GetAt(mFirstShowItemIndex+mSelectedIndex);
	}
	else if ( mpCurSearchItem->eType == SearchByNewSong )
	{
		pSubItem = (SEARCHITEM *)gSearchNewSong.sChildItemList.GetAt(mFirstShowItemIndex+mSelectedIndex);
	}
	else if ( mpCurSearchItem->eType == SearchByPinyin )
	{
		pSubItem = (SEARCHITEM *)gSearchSpell.sChildItemList.GetAt(mFirstShowItemIndex+mSelectedIndex);
	}

//	SEARCHITEM *pSubItem = (SEARCHITEM *)
//			mpCurSearchItem->sChildItemList.GetAt(
//					mFirstShowItemIndex+mSelectedIndex);

//	printf("=========type:%d, name:%s\n", mpCurSearchItem->eType, mpCurSearchItem->cItemName);

	return pSubItem;
}

SEARCHITEM * CSearchTypeBar::GetSelectedSubItemForSinger()
{
	if (!mpCurSearchItem)
	{
		return NULL;
	}

	if (mpCurSearchItem->sChildItemList.GetCount() == 0)
	{
		return mpCurSearchItem;
	}

	SEARCHITEM *pSubItem = NULL;
	if ( gs_SystemSetting.miLanguageID == LanguageID_Chinese )
	{
		pSubItem = (SEARCHITEM *)mpCurSearchItem->sChildItemList.GetAt(mFirstShowItemIndex+mSelectedIndex);
	}
	else if ( mpCurSearchItem->eType == SearchBySinger )
	{
		pSubItem = (SEARCHITEM *)gSearchSingerType.sChildItemList.GetAt(mFirstShowItemIndex+mSelectedIndex);
	}
	else if ( mpCurSearchItem->eType == SearchBySongType )
	{
		pSubItem = (SEARCHITEM *)gSearchSongType.sChildItemList.GetAt(mFirstShowItemIndex+mSelectedIndex);
	}
	else if ( mpCurSearchItem->eType == SearchByLanguageType )
	{
		pSubItem = (SEARCHITEM *)gSearchLanguageType.sChildItemList.GetAt(mFirstShowItemIndex+mSelectedIndex);
	}
	else if ( mpCurSearchItem->eType == SearchByMovieSong )
	{
		pSubItem = (SEARCHITEM *)gSearchMovieSong.sChildItemList.GetAt(mFirstShowItemIndex+mSelectedIndex);
	}
	else if ( mpCurSearchItem->eType == SearchByNewSong )
	{
		pSubItem = (SEARCHITEM *)gSearchNewSong.sChildItemList.GetAt(mFirstShowItemIndex+mSelectedIndex);
	}
	else if ( mpCurSearchItem->eType == SearchByPinyin )
	{
		pSubItem = (SEARCHITEM *)gSearchSpell.sChildItemList.GetAt(mFirstShowItemIndex+mSelectedIndex);
	}

//	SEARCHITEM *pSubItem = (SEARCHITEM *)
//			mpCurSearchItem->sChildItemList.GetAt(
//					mFirstShowItemIndex+mSelectedIndex);

//	printf("=========type:%d, name:%s\n", mpCurSearchItem->eType, mpCurSearchItem->cItemName);

	return pSubItem;
}

void CSearchTypeBar::SetDragFirstShowItem(
	int nFirstShowItem)
{
	if ( !mpCurSearchItem )
		return ;

	int nSubItemCount = mpCurSearchItem->sChildItemList.GetCount();

	if (nSubItemCount - nFirstShowItem < SEARCH_ITEM_COUNT)
	{
		nFirstShowItem = nSubItemCount - SEARCH_ITEM_COUNT;
	}

	if (nFirstShowItem < -1)
	{
		nFirstShowItem = -1;
	}

	mFirstShowItemIndex = nFirstShowItem;
}

void CSearchTypeBar::SetFirstShowItem(
	int nFirstShowItem)
{
	if ( !mpCurSearchItem )
		return ;

	int nSubItemCount = mpCurSearchItem->sChildItemList.GetCount();

	if (nSubItemCount - nFirstShowItem < SEARCH_ITEM_COUNT)
	{
		nFirstShowItem = nSubItemCount - SEARCH_ITEM_COUNT;
	}

	if (nFirstShowItem < 0)
	{
		nFirstShowItem = 0;
	}

	mFirstShowItemIndex = nFirstShowItem;
}

int CSearchTypeBar::SetSelectedIndex(
	int nSelectedIndex)
{
	if ( !mpCurSearchItem )
		return 0;

	int nCount = mpCurSearchItem->sChildItemList.GetCount();

	if (nSelectedIndex + mFirstShowItemIndex >= nCount)
	{
		nSelectedIndex = nCount - 1 - mFirstShowItemIndex;
	}

	if (nSelectedIndex < 0)
	{
		nSelectedIndex = 0;
	}

	mSelectedIndex = nSelectedIndex;

	int nItemWidth = RECTWIDTH(mWndRect) / SEARCH_ITEM_COUNT;
	RECT rcItem;
	SetRectXY(&rcItem,
		nItemWidth*nSelectedIndex,
		0,
		nItemWidth,
		RECTHEIGHT(mWndRect));

	mSelectedWnd.MoveWindow(&rcItem);

	return mSelectedIndex;
}

void CSearchTypeBar::InitSubItem(
	int nFirstShowItem)
{
	if ( !mpCurSearchItem )
		return ;

	int i;
	int nCount = mSubItemWndList.GetCount();
	for ( i = 0 ; i < nCount ; i++ )
	{
		CBaseWnd *pItemWnd = (CBaseWnd *)mSubItemWndList.GetAt(i);

		SEARCHITEM *pSubItem = (SEARCHITEM *)
				mpCurSearchItem->sChildItemList.GetAt(i+nFirstShowItem);
		if (!pSubItem)
		{
			pItemWnd->SetWindowTextA(NULL);
		}
		else
		{
			//pItemWnd->SetWindowTextA(pSubItem->cItemName);
			CBaseStringW string;
			CBaseStringW tmp;
			wchar_t FontConv[128] = {0};

			if ( theBaseApp->GetLanguageID() == LanguageID_CHT)
			{
				tmp.Set(pSubItem->cItemName);
				//Simplified2Traditional(&tmp, &string);
				Traditional2Simplified(tmp.GetString(), tmp.GetLength(), FontConv, 127);
				string.Set(FontConv);
			}
			else
			{
				string.Set(pSubItem->cItemName);
			}
			pItemWnd->SetWindowTextW(string.GetString());
		}

	}
}

void CSearchTypeBar::ShowSubItem(
	int nOffset)
{
	RECT rcItem;
	int i;
	int nCount = mSubItemWndList.GetCount();
	for ( i = 0 ; i < nCount ; i++ )
	{
		GetSubItemRect(i, &rcItem);
		SetRectXY(&rcItem,
			rcItem.left-nOffset,
			rcItem.top,
			RECTWIDTH(rcItem),
			RECTHEIGHT(rcItem));

		CBaseWnd *pItemWnd = (CBaseWnd *)mSubItemWndList.GetAt(i);
		pItemWnd->MoveWindow(&rcItem);
	}
}

void CSearchTypeBar::GetSubItemRect(
	int nIndex,
	RECT *prc)
{
	int nItemWidth = RECTWIDTH(mWndRect) / SEARCH_ITEM_COUNT;
	int nItemHeight = RECTHEIGHT(mWndRect);
	SetRectXY(prc, nItemWidth*nIndex, 0, nItemWidth, nItemHeight);
}

int CSearchTypeBar::GetIndexByXPos(
	int xPos)
{
	if (xPos < mWndRect.left)
	{
		return 0;
	}
	if (xPos > mWndRect.right)
	{
		return SEARCH_ITEM_COUNT-1;
	}

	int nItemWidth = RECTWIDTH(mWndRect) / SEARCH_ITEM_COUNT;
	return xPos / nItemWidth;
}

void CSearchTypeBar::SetOnClickValid(BOOL bValid)
{
	mIsOnClickValid = bValid;
}
