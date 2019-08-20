#include "SelectMusicItem.h"
#include "BaseApp.h"
#include "XMLParser.h"
#include "../SQL/FastSearch.h"
#include "UserMsgDefine.h"
//#include "AdvanceSettings.h"

#define SELECTALLBTN_CLICK_ID		0x00
#define UNSELECTALLBTN_CLICK_ID		0x01
#define PREVPAGEBTN_CLICK_ID		0x02
#define NEXTPAGEBTN_CLICK_ID		0x03
#define SELECTITEMBTN_CLICK_BASEID	0x04
#define TYPEBTN_CLICK_BASEID		0x10

static int FindSelectedSong(const void *pItem, const void *pData)
{
	if ( !pItem || !pData )
		return 0;
	char *cID = (char *)pData;
	//sItemListInfo *pSelectedItem = (sItemListInfo *)pItem;
	//SONGINFO *pSong	= (SONGINFO *)pSelectedItem->pData;
	SONGINFO *pSong = (SONGINFO *)pItem;
	if ( strcmp(pSong->cID, cID) == 0 )
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

CSelectedItem::CSelectedItem()
{
	miCurrPage  = 0;
	miPageCount = 0;
	mpInitList  = NULL;
	miSelectedNum = 0;
}

CSelectedItem::~CSelectedItem()
{
	FreeAllList();
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CSelectedItem::Create(CBaseWnd *pParent, RECT rc)
{
	CParentClass::Create("SelectedMusicWnd", pParent, rc);

	RECT rcControl;
	mSelectedPromptWnd.CreateStatic(this, rcControl);
	mSelectedPromptWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
	mTitleLineWnd.CreateStatic(this, rcControl);
	mPageInfoWnd.CreateStatic(this, rcControl);

	mSelectedAllWnd.CreateStatic(this, rcControl);
	mSelectedAllWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
	mUnSelectedAllWnd.CreateStatic(this, rcControl);
	mUnSelectedAllWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

	mSelectedAllBtn.Create(this, rcControl);
	mSelectedAllBtn.SetWndID(SELECTALLBTN_CLICK_ID);
	mSelectedAllBtn.SetOnClickListener(this);

	mUnSelectedAllBtn.Create(this, rcControl);
	mUnSelectedAllBtn.SetWndID(UNSELECTALLBTN_CLICK_ID);
	mUnSelectedAllBtn.SetOnClickListener(this);

	int i = 0;
	for ( i = 0; i < SELECTITEM_COUNT; ++i )
	{
		mItemWnd[i].SongNameWnd.CreateStatic(this, rcControl);
		mItemWnd[i].SongNameWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

		mItemWnd[i].HotNumWnd.CreateStatic(this, rcControl);

		mItemWnd[i].LineWnd.CreateStatic(this,rcControl);

		mItemWnd[i].ChooseBtn.Create(this, rcControl, BUTTONTYPE_RADIO);
		mItemWnd[i].ChooseBtn.SetWndID(SELECTITEMBTN_CLICK_BASEID + i);
		mItemWnd[i].ChooseBtn.SetOnClickListener(this);
	}

	mPrevPageBtn.Create(this, rcControl);
	mPrevPageBtn.SetWndID(PREVPAGEBTN_CLICK_ID);
	mPrevPageBtn.SetOnClickListener(this);

	mNextPageBtn.Create(this, rcControl);
	mNextPageBtn.SetWndID(NEXTPAGEBTN_CLICK_ID);
	mNextPageBtn.SetOnClickListener(this);

	LoadResource();
}

void CSelectedItem::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "SelectMusicItem.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	RECT rcControl;
	XmlLoadRect(&parser, "SelectedItemWndInfo", &rcControl);
	CParentClass::MoveWindow(&rcControl);

	int i, yOffset, size;
	char strImgPath[64] = {0};
	CTexture bkTexture;
	CTexture btnTextures[4];

	XmlLoadRect(&parser, "SelectedPromptInfo", &rcControl);
	size = parser.GetIntValue("SelectedPromptInfo", "size", 30);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("SelectedPromptInfo", "text", "查找歌曲："), sizeof(strImgPath));
	mSelectedPromptWnd.MoveWindow(&rcControl);
	mSelectedPromptWnd.SetFontSize(size);
	mSelectedPromptWnd.SetWindowTextA(strImgPath);

	XmlLoadRect(&parser, "HeadLineInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("HeadLineInfo", "imgPath", "AdvanceSettings/MusicLib/MusicLibItem/Line"), sizeof(strImgPath));
	CreateImgTexture(strImgPath, &bkTexture);
	mTitleLineWnd.MoveWindow(&rcControl);
	mTitleLineWnd.SetBackgroundTexture(&bkTexture);

	XmlLoadRect(&parser, "SelectedAllWndInfo", &rcControl);
	size = parser.GetIntValue("SelectedAllWndInfo", "size", 30);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("SelectedAllWndInfo", "text", "全选"), sizeof(strImgPath));
	mSelectedAllWnd.MoveWindow(&rcControl);
	mSelectedAllWnd.SetFontSize(size);
	mSelectedAllWnd.SetWindowTextA(strImgPath);

	XmlLoadRect(&parser, "SelectedAllBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("SelectedAllBtnInfo", "imgPath", "AdvanceSettings/MusicLib/MusicLibItem/Selected"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mSelectedAllBtn.MoveWindow(&rcControl);
	mSelectedAllBtn.SetTextures(btnTextures);

	XmlLoadRect(&parser, "UnSelectedAllWndInfo", &rcControl);
	size = parser.GetIntValue("UnSelectedAllWndInfo", "size", 30);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("UnSelectedAllWndInfo", "text", "反选"), sizeof(strImgPath));
	mUnSelectedAllWnd.MoveWindow(&rcControl);
	mUnSelectedAllWnd.SetFontSize(size);
	mUnSelectedAllWnd.SetWindowTextA(strImgPath);

	XmlLoadRect(&parser, "UnSelectedAllBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("UnSelectedAllBtnInfo", "imgPath", "AdvanceSettings/MusicLib/MusicLibItem/Selected"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mUnSelectedAllBtn.MoveWindow(&rcControl);
	mUnSelectedAllBtn.SetTextures(btnTextures);

	RECT rcSong, rcHotNum, rcLine, rcSelected;
	XmlLoadRect(&parser, "BaseSongNameWndInfo", &rcSong);
	XmlLoadRect(&parser, "BaseHotNumWndInfo", &rcHotNum);
	XmlLoadRect(&parser, "BaseLineWndInfo", &rcLine);
	XmlLoadRect(&parser, "BaseChooseBtnInfo", &rcSelected);
	yOffset = parser.GetIntValue("BaseSongNameWndInfo", "yOffset", 50);
	for ( i =  0; i < SELECTITEM_COUNT; ++i )
	{
		int y = i*yOffset;
		InflatRect(&rcControl, &rcSong, 0, y);
		mItemWnd[i].SongNameWnd.MoveWindow(&rcControl);
		mItemWnd[i].SongNameWnd.SetFontSize(size);

		InflatRect(&rcControl, &rcHotNum, 0, y);
		mItemWnd[i].HotNumWnd.MoveWindow(&rcControl);
		mItemWnd[i].HotNumWnd.SetFontSize(size);

		InflatRect(&rcControl, &rcLine, 0, y);
		mItemWnd[i].LineWnd.MoveWindow(&rcControl);
		mItemWnd[i].LineWnd.SetBackgroundTexture(&bkTexture);

		InflatRect(&rcControl, &rcSelected, 0, y);
		mItemWnd[i].ChooseBtn.MoveWindow(&rcControl);
		mItemWnd[i].ChooseBtn.SetTextures(btnTextures);
	}

	XmlLoadRect(&parser, "PageCountWndInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("PageCountWndInfo", "imgPath", "SelectedList/PageSel"), sizeof(strImgPath));
	CreateImgTexture(strImgPath, &bkTexture);
	mPageInfoWnd.SetFontSize(size);
	mPageInfoWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "PrevPageBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("PrevPageBtnInfo", "imgPath", "SelectedList/PrevPage"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mPrevPageBtn.MoveWindow(&rcControl);
	mPrevPageBtn.SetTextures(btnTextures);

	XmlLoadRect(&parser, "NextPageBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("NextPageBtnInfo", "imgPath", "SelectedList/NextPage"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mNextPageBtn.MoveWindow(&rcControl);
	mNextPageBtn.SetTextures(btnTextures);
}

void CSelectedItem::OnClick(CBaseWnd *pWnd, POINT pt)
{
	int iWndID = pWnd->GetWndID();
	if ( iWndID == SELECTALLBTN_CLICK_ID )
	{
		//if ( mSelectedAllBtn.GetRadioBtnPressed() )
		//{
		//	mUnSelectedAllBtn.SetRadioBtnPressed(FALSE);
			OnAllSelectedClick(TRUE);
//		}
//		//else
//		{
//			//mSelectedAllBtn.SetRadioBtnPressed(TRUE);
//		//}
	}
	else if ( iWndID == UNSELECTALLBTN_CLICK_ID )
	{
		OnAllSelectedClick(FALSE);
//		if ( mUnSelectedAllBtn.GetRadioBtnPressed() )
//		{
//			mSelectedAllBtn.SetRadioBtnPressed(FALSE);
//			OnAllSelectedClick(FALSE);
//		}
//		else
//		{
//			mUnSelectedAllBtn.SetRadioBtnPressed(TRUE);
//		}
	}
	else if ( iWndID == PREVPAGEBTN_CLICK_ID )
	{
		if ( miCurrPage > 0 )
		{
			miCurrPage--;
			UpdateItemList();
		}
	}
	else if ( iWndID == NEXTPAGEBTN_CLICK_ID )
	{
		if ( miCurrPage < miPageCount )
		{
			miCurrPage++;
			UpdateItemList();
		}
	}
	else if ( iWndID >= SELECTITEMBTN_CLICK_BASEID && (iWndID < SELECTITEMBTN_CLICK_BASEID + SELECTITEM_COUNT) )
	{
		int iClickID = iWndID - SELECTITEMBTN_CLICK_BASEID;
		OnItemClick(iClickID);
	}
}

void CSelectedItem::InitSelectedItemList(CPtrListCtrl *pInitList)
{
	if ( !pInitList )
		return ;

	FreeAllList();
	mpInitList = pInitList;
}

void CSelectedItem::FreeSearchItemList()
{
	DelNormalList(&mSearchList, sItemListInfo);
}

void CSelectedItem::FreeSelectedItemList()
{
}

void CSelectedItem::FreeAllList()
{
	FreeSearchItemList();
	FreeSelectedItemList();
}

void CSelectedItem::AddSearchListToSelectedList(void)
{
	int iCount = mSearchList.GetCount();
	int i = 0;
	int iselectid = 0;

	for ( i = 0 ; i < iCount; ++i )
	{
		if ( iselectid >= miSelectedNum )
			break;

		sItemListInfo *pSelectItem = (sItemListInfo *)mSearchList.GetAt(i);
		if ( pSelectItem && pSelectItem->bSelected && pSelectItem->pData )
		{
			SONGINFO *pSong = (SONGINFO *)pSelectItem->pData;
			int iExistId = mpInitList->FindFirst(pSong->cID, FindSelectedSong);
			if ( iExistId != -1 )
			{
				SONGINFO *pInitSong = (SONGINFO *)mpInitList->GetAt(iExistId);
				if ( pInitSong )
					pInitSong->bPlaying = TRUE;
			}
			else
			{
				// 需要增加进去
				SONGINFO *pNewSong = new SONGINFO;
				if ( !pNewSong )
					break;

				memcpy(pNewSong, pSong, sizeof(SONGINFO));
				pNewSong->bPlaying = TRUE;
				mpInitList->AddData(pNewSong);
			}
			iselectid++;
		}
	}
}

void CSelectedItem::UpdateItemByCondition(const sSearchCondition condition)
{
	CPtrArrayCtrl *pSongList = &gAllSongList;
	int iAllSongNum = pSongList->GetCount();
	int i;
	SONGINFO *pSong = NULL;

	FreeSearchItemList();

	BOOL bSetLang 		= strlen(condition.cLang) == 0 ? FALSE : TRUE;
	BOOL bSetSongName  	= strlen(condition.cName) == 0 ? FALSE : TRUE;
	BOOL bSetSingerName = strlen(condition.cSinger) == 0 ? FALSE : TRUE;
	BOOL bSetType		= strlen(condition.cType) == 0 ? FALSE : TRUE;
	BOOL bSetOrderTimes = condition.wOrderTimes == 0 ? FALSE : TRUE;
	BOOL bSetWdCt		= condition.wWdCt == 0 ? FALSE : TRUE;

	for ( i = 0; i < iAllSongNum; ++i )
	{
		pSong = (SONGINFO *)pSongList->GetAt(i);
		if ( !pSong )
			continue;

		if ( (!pSong->bExist) ||
			 (bSetSongName && strcasestr(pSong->cSpell, condition.cName) == NULL) ||
			 (bSetOrderTimes && condition.wOrderTimes < pSong->wOrderTimes) ||
			 (bSetWdCt && condition.wWdCt != pSong->wWdCt) )
		{
			// 不符合查询条件
			continue;
		}

		sItemListInfo *pNewSearchItem = new sItemListInfo;
		if ( !pNewSearchItem )
		{
			DbgOutput(DBG_ERR, "New sItemListInfo failed.\n");
			break  ;
		}

//		if ( IsInSelectedList(pSong) )
//			pNewSearchItem->bSelected = TRUE;
//		else
		pNewSearchItem->bSelected = FALSE;
		pNewSearchItem->pData = (void *)pSong;

		mSearchList.AddData(pNewSearchItem);
	}

	miSelectedNum = 0;
	miCurrPage = 0;
	miPageCount = (mSearchList.GetCount() -1) / SELECTITEM_COUNT;

	UpdateItemList();
}

void CSelectedItem::UpdateItemList()
{
	int iBaseIdx = miCurrPage * SELECTITEM_COUNT;
	int i;
	CBaseStringW tmp;

	for ( i = 0; i < SELECTITEM_COUNT; ++i )
	{
		sItemListInfo *pItem = (sItemListInfo *)mSearchList.GetAt(iBaseIdx+i);
		if ( pItem && pItem->pData )
		{
			mItemWnd[i].SongNameWnd.SetWindowVisible(TRUE);
			mItemWnd[i].HotNumWnd.SetWindowVisible(TRUE);
			mItemWnd[i].ChooseBtn.SetWindowVisible(TRUE);

			SONGINFO *pSong = (SONGINFO *)pItem->pData;

			tmp.Format("%s", pSong->cName);
			if ( tmp.GetLength() > 6 )
				tmp.Trunk(6);
			tmp.Append(L"--");
			char *singer = FastSearch_GetSingerByID(pSong->singerID);
			tmp.Append(singer ? singer : "");
			mItemWnd[i].SongNameWnd.SetWindowTextW(tmp.GetString());

			tmp.Format("%d", pSong->wOrderTimes);
			mItemWnd[i].HotNumWnd.SetWindowTextW(tmp.GetString());

			if ( pItem->bSelected )
				mItemWnd[i].ChooseBtn.SetRadioBtnPressed(TRUE);
			else
				mItemWnd[i].ChooseBtn.SetRadioBtnPressed(FALSE);
		}
		else
		{
			mItemWnd[i].SongNameWnd.SetWindowVisible(FALSE);
			mItemWnd[i].HotNumWnd.SetWindowVisible(FALSE);
			mItemWnd[i].ChooseBtn.SetWindowVisible(FALSE);
		}
	}

	UpdatePageInfo();
}

void CSelectedItem::UpdatePageInfo()
{
	CBaseStringA tmp;
	tmp.Format("%d/%d", miCurrPage+1, miPageCount+1);
	mPageInfoWnd.SetWindowTextA(tmp.GetString());
}

void CSelectedItem::OnAllSelectedClick(BOOL bAllSelected)
{
	int i;
	int iSearchNum = mSearchList.GetCount();
	int iCurBaseIdx = miCurrPage * SELECTITEM_COUNT;

	if ( bAllSelected )
	{
		// 全选
		for ( i = 0; i < iSearchNum; ++i )
		{
			sItemListInfo *pItem = (sItemListInfo *)mSearchList.GetAt(i);
			if ( !pItem || pItem->bSelected )
				continue;

			miSelectedNum++;
			pItem->bSelected = TRUE;
			if ( i >= iCurBaseIdx && i < (iCurBaseIdx + SELECTITEM_COUNT) )
			{
				int idx = i - iCurBaseIdx;
				mItemWnd[idx].ChooseBtn.SetRadioBtnPressed(TRUE);
			}
		}
	}
	else
	{
		// 反选
		for ( i = 0; i < iSearchNum; ++i )
		{
			sItemListInfo *pItem = (sItemListInfo *)mSearchList.GetAt(i);
			if ( !pItem  )
				continue;

			if ( pItem->bSelected )
			{
				miSelectedNum--;
				pItem->bSelected = FALSE;
			}
			else
			{
				miSelectedNum++;
				pItem->bSelected = TRUE;
			}

			if ( i >= iCurBaseIdx && i < (iCurBaseIdx + SELECTITEM_COUNT) )
			{
				int idx = i - iCurBaseIdx;
				if ( pItem->bSelected )
					mItemWnd[idx].ChooseBtn.SetRadioBtnPressed(TRUE);
				else
					mItemWnd[idx].ChooseBtn.SetRadioBtnPressed(FALSE);
			}
		}
	}

	//printf("==============the selected num:%d\n", miSelectedNum);
}

void CSelectedItem::OnItemClick(int idx)
{
	int iRealIdx = miCurrPage * SELECTITEM_COUNT + idx;
	sItemListInfo *pItem = (sItemListInfo *)mSearchList.GetAt(iRealIdx);
	if (pItem )
	{
		if ( pItem->bSelected )
		{
			--miSelectedNum;
			pItem->bSelected = FALSE;
		}
		else
		{
			++miSelectedNum;
			pItem->bSelected = TRUE;
		}
	}

	//printf("==============the selected num:%d\n", miSelectedNum);
}

// 把查找表中的已选择的加入到选择列表中
//void CSelectedItem::AddSearchListToSelectedList(SONGINFO *pSearchSong)
//{
//	if ( !pSearchSong )
//		return;
//
//	int iExistId = mSelectedList.FindFirst(pSearchSong->cID, FindSelectedSong);
//	if ( iExistId >= 0 )
//	{
//		sItemListInfo *pOldItem = (sItemListInfo *)mSelectedList.GetAt(iExistId);
//		if ( pOldItem && !pOldItem->bSelected )
//			pOldItem->bSelected = TRUE;
//	}
//	else
//	{
//		//  不存在，新的需要添加
//		SONGINFO *pNewSelectedSong = new SONGINFO;
//		if ( !pNewSelectedSong )
//			return ;
//		sItemListInfo *pNewItem = new sItemListInfo;
//		if ( !pNewItem )
//		{
//			delete pNewSelectedSong;
//			return ;
//		}
//
//		memcpy(pNewSelectedSong, pSearchSong, sizeof(SONGINFO));
//		pNewItem->pData = (void *)pNewSelectedSong;
//		pNewItem->bSelected = TRUE;
//
//		mSelectedList.AddData(pNewItem);
//	}
//}

//void CSelectedItem::DeleteSelectedListFromSearchList(SONGINFO *pSearchSong)
//{
//	if ( !pSearchSong )
//		return;
//
//	int iExistId = mSelectedList.FindFirst(pSearchSong->cID, FindSelectedSong);
//	if ( iExistId != -1 )
//	{
//		sItemListInfo *pOldItem = (sItemListInfo *)mSelectedList.DeleteAt(iExistId);
//		if ( pOldItem )
//		{
//			if ( pOldItem->pData )
//			{
//				SONGINFO *pOldSong = (SONGINFO *)pOldItem->pData;
//				delete pOldSong;
//			}
//			delete pOldItem;
//		}
//	}
//}

BOOL CSelectedItem::IsInSelectedList(SONGINFO *pSearchSong)
{
	if ( !pSearchSong )
		return FALSE;
	int iExistId = mpInitList->FindFirst(pSearchSong->cID, FindSelectedSong);
	if ( iExistId != -1 )
	{
		return TRUE;
	}

	return FALSE;
}

CTypeConditionWnd::CTypeConditionWnd()
{
	miCurrPage 	= 0;
	miPageCount = 0;
	mpTypeList  = NULL;
}

CTypeConditionWnd::~CTypeConditionWnd()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CTypeConditionWnd::Create(CBaseWnd *pParent, RECT rc)
{
	CParentClass::Create("CTypeConditionWnd", pParent, rc);

	RECT rcControl;
	int i = 0;

	for ( i = 0; i < CONDITIONTYPE_COUNT; ++i )
	{
		mTypeTextBtn[i].Create(this, rcControl, BUTTONTYPE_RADIO);
		mTypeTextBtn[i].SetWndID(TYPEBTN_CLICK_BASEID + i);
		mTypeTextBtn[i].SetOnClickListener(this);
	}

	mPrevPageBtn.Create(this, rcControl);
	mPrevPageBtn.SetWndID(PREVPAGEBTN_CLICK_ID);
	mPrevPageBtn.SetOnClickListener(this);

	mNextPageBtn.Create(this, rcControl);
	mNextPageBtn.SetWndID(NEXTPAGEBTN_CLICK_ID);
	mNextPageBtn.SetOnClickListener(this);

	LoadResource();
}

void CTypeConditionWnd::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "SelectMusicItem.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	int i, xOffset,yOffset, size, colnum;
	char strImgPath[64] = {0};
	CTexture bkTexture;
	CTexture btnTextures[4];

	RECT rcControl;
	XmlLoadRect(&parser, "TypeConditionGBWndInfo", &rcControl);
	CParentClass::MoveWindow(&rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("TypeConditionGBWndInfo", "imgPath", "AdvanceSettings/MusicLib/MusicLibItem/TypeConditionBG"), sizeof(strImgPath));
	CreateImgTexture(strImgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);


	XmlLoadRect(&parser, "TypeConditionItemInfo", &rcControl);
	size 	= parser.GetIntValue("TypeConditionItemInfo", "size", 30);
	xOffset = parser.GetIntValue("TypeConditionItemInfo", "xOffset", 45);
	yOffset = parser.GetIntValue("TypeConditionItemInfo", "yOffset", 50);
	colnum	= parser.GetIntValue("TypeConditionItemInfo", "colNums", 2);
	RECT rcItem;
	for ( i = 0; i < CONDITIONTYPE_COUNT; ++i )
	{
		int x = i % colnum;
		int y = i / colnum;
		InflatRect(&rcItem, &rcControl, x*xOffset, y*yOffset);
		mTypeTextBtn[i].MoveWindow(&rcItem);
		mTypeTextBtn[i].SetFontSize(size);
	}

	XmlLoadRect(&parser, "TypePrevPageBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("TypePrevPageBtnInfo", "imgPath", "SelectedList/PrevPage"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mPrevPageBtn.MoveWindow(&rcControl);
	mPrevPageBtn.SetTextures(btnTextures);

	XmlLoadRect(&parser, "TypeNextPageBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("TypeNextPageBtnInfo", "imgPath", "SelectedList/NextPage"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mNextPageBtn.MoveWindow(&rcControl);
	mNextPageBtn.SetTextures(btnTextures);
}

void CTypeConditionWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	int iWndID = pWnd->GetWndID();
	if ( iWndID == PREVPAGEBTN_CLICK_ID )
	{
		if ( miCurrPage > 0 )
		{
			miCurrPage--;
			UpdateConditionItem();
		}
	}
	else if ( iWndID == NEXTPAGEBTN_CLICK_ID )
	{
		if ( miCurrPage < miPageCount )
		{
			miCurrPage++;
			UpdateConditionItem();
		}
	}
	else if ( iWndID >= TYPEBTN_CLICK_BASEID && (iWndID < TYPEBTN_CLICK_BASEID + CONDITIONTYPE_COUNT) )
	{
		int iClickID = iWndID - TYPEBTN_CLICK_BASEID;
		OnItemClick(iClickID);
	}
}

void CTypeConditionWnd::InitTypeConditonList(CPtrListCtrl *pList)
{
	if ( !pList )
	{
		for ( int i = 0; i < CONDITIONTYPE_COUNT; ++i )
		{
			mTypeTextBtn[i].SetWindowVisible(FALSE);
		}
		return ;
	}

	mpTypeList = pList;
	miCurrPage = 0;
	miPageCount = (mpTypeList->GetCount() -1) / CONDITIONTYPE_COUNT;
	UpdateConditionItem();
}

char * CTypeConditionWnd::GetTypeCondition()
{
	return (char *)mSelectedTypeName.GetString();
}

void CTypeConditionWnd::UpdateConditionItem()
{
	int iBaseIdx = miCurrPage * CONDITIONTYPE_COUNT;
	int i;

	if ( !mpTypeList )
		return ;

	for ( i = 0; i < CONDITIONTYPE_COUNT; ++i )
	{
		const SEARCHITEM *pData = (const SEARCHITEM *)mpTypeList->GetAt(iBaseIdx + i);
		if ( pData )
		{
			mTypeTextBtn[i].SetWindowVisible(TRUE);
			mTypeTextBtn[i].SetWindowTextA(pData->cItemName);
		}
		else
		{
			mTypeTextBtn[i].SetWindowVisible(FALSE);
		}
	}
}

void CTypeConditionWnd::OnItemClick(int idx)
{
	int iRealIdx = miCurrPage * CONDITIONTYPE_COUNT + idx;
	const SEARCHITEM *pData = (const SEARCHITEM *)mpTypeList->GetAt(iRealIdx);
	if ( pData )
	{
		mSelectedTypeName.Set(pData->cItemName);
		ReportMsg(INPUTWND_BKTYPECONDITION_CHARMSG, 0, 0);
	}
}

