#include "SingerListFragment.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"
#include "../GlobalUIClass.h"
#include "../SQL/SearchSQL.h"

#define BTN_RETURN_ID			0x01
#define BTN_PREVPAGE_ID		0x02
#define BTN_NEXTPAGE_ID		0x03

void CSingerListFragment::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 55, LAYOUT_WIDTH, 610};

	CParentClass::Create("SingerListFragment", pParent, rc, WINDOWFLAG_DEFAULT, WINDOWSTATE_INVISIBLE);

	RECT rcListCtrl;
	SetRectXY(&rcListCtrl, 10, 75, 1270, 450);

	mSingerListCtrl.SetItemSize(177, 202);
	mSingerListCtrl.SetItemSpace(210-177, 23);
	mSingerListCtrl.Create(this, rcListCtrl);
	mSingerListCtrl.SetMsgRecvWnd(this);

	mReturnBtn.Create(this, rcListCtrl);
	mReturnBtn.SetWndID(BTN_RETURN_ID);
	mReturnBtn.SetOnClickListener(this);

	mPrevPageBtn.Create(this, rcListCtrl);
	mPrevPageBtn.SetWndID(BTN_PREVPAGE_ID);
	mPrevPageBtn.SetOnClickListener(this);

	mNextPageBtn.Create(this, rcListCtrl);
	mNextPageBtn.SetWndID(BTN_NEXTPAGE_ID);
	mNextPageBtn.SetOnClickListener(this);

	mPageInfoWnd.CreateStatic(this, rcListCtrl);
	mTitleWnd.CreateStatic(this, rcListCtrl);
	

	RECT rcSearchType;
	SetRectXY(&rcSearchType, 0, 35, LAYOUT_WIDTH, 40);
	mSearchTypeBar.Create(this, rcSearchType);
	mSearchTypeBar.SetMsgRecvWnd(this);

	LoadResource();
}

void CSingerListFragment::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "SingerListFragment.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml

	RECT rcControl;
	CTexture btnTextures[4];
	char imgPath[64] = {0};

	XmlLoadRect(&parser, "rcLocation", &rcControl);
	CParentClass::MoveWindow(&rcControl);

	int itemWidth = parser.GetIntValue("ItemSizeLocation","w", 177);
	int itemHeight = parser.GetIntValue("ItemSizeLocation","h", 202);
	int itemXSpace = parser.GetIntValue("ItemSizeLocation","xSpace", 33);
	int itemYSpace = parser.GetIntValue("ItemSizeLocation","ySpace", 23);
	mSingerListCtrl.SetItemSize(itemWidth, itemHeight);
	mSingerListCtrl.SetItemSpace(itemXSpace, itemYSpace);

	XmlLoadRect(&parser, "TitleWndInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("TitleWndInfo", "path", "FragmentPublic/SingerTitle"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mTitleWnd.SetBackgroundTexture(btnTextures);
	mTitleWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ListCtrlLocation", &rcControl);
	mSingerListCtrl.MoveWindow(&rcControl);
	
	XmlLoadRect(&parser, "SearchTypeLocation", &rcControl);
	mSearchTypeBar.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ReturnBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ReturnBtnInfo", "path", "FragmentPublic/Return"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mReturnBtn.SetTextures(btnTextures);
	mReturnBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "PrevPageBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("PrevPageBtnInfo", "path", "FragmentPublic/PrevPage"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mPrevPageBtn.SetTextures(btnTextures);
	mPrevPageBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "NextPageBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("NextPageBtnInfo", "path", "FragmentPublic/NextPage"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mNextPageBtn.SetTextures(btnTextures);
	mNextPageBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "PageInfoWndInfo", &rcControl);
	mPageInfoWnd.MoveWindow(&rcControl);
}

void CSingerListFragment::OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam)
{
	CParentClass::OnMsg(uType, wParam, lParam);

	switch(uType)
	{
	case INPUTWND_SEARCHMSG:
		OnSearchItemChange();
		mSingerListCtrl.StartFadeInEffect();
		break;

	case SEARCHITEM_CLICKMSG:
		gMainCtrlPage->mSearchInputWnd.ResetSearchInfo();
		OnSearchItemChange();
		mSingerListCtrl.StartFadeInEffect();
		break;

	case LISTCTRL_ITEM_CLICKMSG:
		OnListCtrlItemClicked(wParam, lParam);
		break;

	case LISTCTRL_FIRSTITEM_CHANGEMSG:
		OnListCtrlFirstItemChange(wParam);
		break;
	}
}

BOOL CSingerListFragment::OnReturn()
{
	mSingerListCtrl.WaitForEffectTimeOver();

	return FALSE;
}

void CSingerListFragment::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return;
	int nWndID = pWnd->GetWndID();

	switch ( nWndID )
	{
	case BTN_RETURN_ID:
		gMainCtrlPage->OnReturnClick();
		break;

	case BTN_PREVPAGE_ID:
	{
		int nPageIndex = mSingerListCtrl.GetCurPageIndex();
		int nPageCount = mSingerListCtrl.GetPageCount();
		if ( nPageIndex > 0 )
		{
			nPageIndex--;
			int nCountPerPage = mSingerListCtrl.GetCountPerPage();
			int nItemIndex = nPageIndex *nCountPerPage;

			mSingerListCtrl.SetFirstItemIndex(nItemIndex);
			mSingerListCtrl.StartFadeInEffect();

			SetPageCountInfo(nPageIndex+1, nPageCount);
		}
	}
		break;

	case BTN_NEXTPAGE_ID:
	{
		int nPageCount = mSingerListCtrl.GetPageCount();
		int nPageIndex = mSingerListCtrl.GetCurPageIndex();
		int nCountPerPage = mSingerListCtrl.GetCountPerPage();
		if ( nPageIndex < nPageCount-1 )
		{
			nPageIndex++;
			int nItemIndex = nPageIndex *nCountPerPage;

			mSingerListCtrl.SetFirstItemIndex(nItemIndex);
			mSingerListCtrl.StartFadeInEffect();

			SetPageCountInfo(nPageIndex+1, nPageCount);
		}
	}
		break;

	default:
		break;
	}
}

void CSingerListFragment::OnPositionChange(
	CBaseWnd *pWnd,
	int nOldPosition,
	int nNewPostion,
	BOOL bChangeEnd)
{
	int nPos = nNewPostion;

	int nCountPerPage = mSingerListCtrl.GetCountPerPage();

	if (bChangeEnd)
	{
		int nItemIndex = nPos *nCountPerPage;

		mSingerListCtrl.SetFirstItemIndex(nItemIndex);
		mSingerListCtrl.StartFadeInEffect();
	}
	else
	{
		int nPageCount = mSingerListCtrl.GetPageCount();

		//mPagePrompt.SetListPageInfo(nPos+1, nPageCount);
	}
}


void CSingerListFragment::OnSearchItemChange()
{
	SEARCHITEM *pSearchItem = mSearchTypeBar.GetSelectedSubItemForSinger();
	if (!pSearchItem)
	{
		return;
	}
	SEARCHINFO searchinfo={"\0", INPUTMETHODTYPE_SPELL, WordLength_All};
	gMainCtrlPage->mSearchInputWnd.GetSearchInfo(&searchinfo);

	DbgOutput(DBG_INFO, "OnSearchItemChange: item=%s, input=%s\n", pSearchItem->cItemName, searchinfo.cInputContent);

	mSingerListCtrl.DestroyE3DListView();
	mSingerListCtrl.DelAllItem();

	CPtrArrayCtrl sSingerList;
	FastSearch_SearchSingerListBySingerType(pSearchItem->cItemName, &searchinfo, &sSingerList);

	int nSingerCount = sSingerList.GetCount();
	SetSingerCount(nSingerCount);

	int i;
	for ( i = 0 ; i < nSingerCount ; i++ )
	{
		SINGERINFO *pSingerInfo = (SINGERINFO *)sSingerList.GetAt(i);
		AddSingerItem(pSingerInfo, i);
	}

	DbgOutput(DBG_DEBUG, "RenewE3DListView\n");
	mSingerListCtrl.RenewE3DListView();

	int nPageCount = mSingerListCtrl.GetPageCount();
	int nPageIndex = mSingerListCtrl.GetCurPageIndex();
	//DbgOutput(DBG_DEBUG, "SetListPageInfo pagecount=%d, pageindex=%d\n", nPageCount, nPageIndex);
	//mPagePrompt.SetListPageInfo(nPageCount>0?nPageIndex+1:0, nPageCount);
	SetPageCountInfo(nPageCount>0?nPageIndex+1:0, nPageCount);
}

void CSingerListFragment::OnListCtrlItemClicked(int nItemIndex, int nSubItemIndex)
{
	if (nSubItemIndex == lviSingerBackground)
	{
		LISTITEM *pli = mSingerListCtrl.GetListItem(nItemIndex);
		if ( pli )
		{
			SINGERINFO *pSingerInfo = (SINGERINFO *) (pli->uItemData);
			if ( pSingerInfo )
			{
				/*
				static SEARCHITEM search;
				search.eType = SearchBySinger;
				search.nDepth = pSingerInfo->id;
				search.pParentItem = NULL;
				SAFE_STRNCPY(search.cItemName, pSingerInfo->cName, sizeof(search.cItemName));
				gSongListFragment->SetSearchBarItem(&search, 0);
				*/
				SEARCHITEM *pSearchItem = mSearchTypeBar.GetSelectedSubItemForSinger();
				UpdateSubSearchType(pSearchItem);
				gSongListFragment->SetSearchBarItem(pSearchItem, nItemIndex);
				gMainCtrlPage->SetCurrentFragment(Fragment_SongList);
//
//				gMainCtrlPage->SetSelectedSingerInfo(pSingerInfo);
//				gMainCtrlPage->SetCurrentFragment(Fragment_SingerSongs);
			}
		}
	}
}

void CSingerListFragment::OnListCtrlFirstItemChange(int nItemIndex)
{
	int nPageCount = mSingerListCtrl.GetPageCount();
	int nPageIndex = mSingerListCtrl.GetCurPageIndex();
	if ( nPageIndex >= nPageCount )
		return;

	//mPagePrompt.SetListPageInfo(nPageCount>0?nPageIndex+1:0, nPageCount);
	SetPageCountInfo(nPageCount>0?nPageIndex+1:0, nPageCount);
	//mPageCountSlide.SetPos(nPageIndex);
}

void CSingerListFragment::SetSearchBarItem(
	SEARCHITEM *pSearchItem,
	int nFirstShowItemIndex)
{
	mSearchTypeBar.SetSearchItem(pSearchItem, nFirstShowItemIndex);
	/*
	SEARCHITEM *pRoot = pSearchItem;
	while (pRoot->pParentItem)
	{
		pRoot = pRoot->pParentItem;

		CBaseStringA sTmp(&sPrompt);
		sPrompt.Format("%s->%s", pRoot->cItemName, sTmp.GetString());
	}
	*/

	//modify by yq,2013-01-24
//	mSearchPrompt.SetWindowTextA(sPrompt.GetString());
//	mPrompt.Set(sPrompt.GetString());
	//end modify by yq,2013-01-24

	DbgOutput(DBG_DEBUG, "search depth = %d, type = %d\n",
		 	pSearchItem->nDepth, pSearchItem->eType);

	gMainCtrlPage->mSearchInputWnd.ResetSearchInfo();
	OnSearchItemChange();

	mSingerListCtrl.StartFlyinEffect();
}

void CSingerListFragment::SetSingerCount(int nSingerCount)
{
	mSingerListCtrl.SetItemCount(nSingerCount);

	//int nPageCount = mSingerListCtrl.GetPageCount();
	//mPageCountSlide.SetMinMax(0, nPageCount-1);
	//mPageCountSlide.SetPos(0);
}

void CSingerListFragment::AddSingerItem(const SINGERINFO *pSinger, int nIndex)
{
	mSingerListCtrl.AddSingerItem(pSinger, nIndex);
}

void CSingerListFragment::UpdateSubSearchType(SEARCHITEM *pSearchItem)
{
	if (!pSearchItem)
	{
		return;
	}

	DelAllSubSearchItem(pSearchItem);

	int nCount = mSingerListCtrl.GetItemCount();
	int i;
	for ( i = 0 ; i < nCount ; i++ )
	{
		LISTITEM *pli = mSingerListCtrl.GetListItem(i);
		if (!pli)
		{
			continue;
		}

		SINGERINFO *pSingerInfo = (SINGERINFO *) (pli->uItemData);
		if (pSingerInfo)
		{
			AddSubSearchItem(pSearchItem, pSingerInfo->cName, pSingerInfo->id);
		}
	}
}

void CSingerListFragment::SetPageCountInfo(int curPage, int pageCount)
{
	CBaseStringA tmp;
	tmp.Format("%d/%d", curPage, pageCount);
	mPageInfoWnd.SetWindowTextA(tmp.GetString());
}

