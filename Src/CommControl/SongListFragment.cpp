#include "SongListFragment.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"
#include "../GlobalUIClass.h"

#define BTN_RETURN_ID			0x01
#define BTN_PREVPAGE_ID		0x02
#define BTN_NEXTPAGE_ID		0x03

CSongListFragment::CSongListFragment()
{
	mCurSearchType = NULL;
	mSongNameColor = 0;
	mSingerNameColor = 0;
	mSelectedColor = 0;
}

CSongListFragment::~CSongListFragment()
{
	DbgOutput(DBG_INFO, "function %s quit\n", __FUNCTION__);
}

void CSongListFragment::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 55, LAYOUT_WIDTH, 620};

	CParentClass::Create("SongListFragment", pParent, rc, WINDOWFLAG_DEFAULT, WINDOWSTATE_INVISIBLE);

	RECT rcListCtrl;
	SetRectXY(&rcListCtrl, 10, 75, 1260, 459);

	mSongListCtrl.SetItemSize(413, 153);
	mSongListCtrl.SetItemSpace(7, 0);
	mSongListCtrl.Create(this, rcListCtrl);
	mSongListCtrl.SetMsgRecvWnd(this);

	mReturnBtn.Create(this, rcListCtrl);
	mReturnBtn.SetWndID(BTN_RETURN_ID);
	mReturnBtn.SetOnClickListener(this);

	mPrevPageBtn.Create(this, rcListCtrl);
	mPrevPageBtn.SetWndID(BTN_PREVPAGE_ID);
	mPrevPageBtn.SetOnClickListener(this);

	mNextPageBtn.Create(this, rcListCtrl);
	mNextPageBtn.SetWndID(BTN_NEXTPAGE_ID);
	mNextPageBtn.SetOnClickListener(this);

	RECT rcSearchType;
	SetRectXY(&rcSearchType, 0, 35, LAYOUT_WIDTH, 40);
	mSearchTypeBar.Create(this, rcSearchType);
	mSearchTypeBar.SetMsgRecvWnd(this);
	mPageInfoWnd.CreateStatic(this, rcListCtrl);

	LoadResource();
}

void CSongListFragment::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "SongListFragment.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcControl;
	CTexture btnTextures[4];
	char imgPath[64] = {0};
	char colorbuf[16];
	//unsigned long color;
	//int size;

	XmlLoadRect(&parser, "rcLocation", &rcControl);
	CParentClass::MoveWindow(&rcControl);

	int itemWidth = parser.GetIntValue("ItemSizeLocation","w", 7);
	int itemHeight = parser.GetIntValue("ItemSizeLocation","h", 0);
	int xSpace = parser.GetIntValue("ItemSizeLocation","xSpace", 413);
	int ySpace = parser.GetIntValue("ItemSizeLocation","ySpace", 153);
	mSongListCtrl.SetItemSize(itemWidth, itemHeight);
	mSongListCtrl.SetItemSpace(xSpace, ySpace);

	XmlLoadRect(&parser, "ListCtrlLocation", &rcControl);
	mSongListCtrl.MoveWindow(&rcControl);

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

	strcpy(colorbuf,parser.GetStrValue("NameColor","SongName","FFFFFFFF"));
	mSongNameColor = strtoul(colorbuf,NULL,16);
	strcpy(colorbuf,parser.GetStrValue("NameColor","SingerName","FFFFFF00"));
	mSingerNameColor = strtoul(colorbuf,NULL,16);
	strcpy(colorbuf,parser.GetStrValue("NameColor","Selected","FFFF0000"));
	mSelectedColor = strtoul(colorbuf,NULL,16);
}

void CSongListFragment::OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam)
{
	CParentClass::OnMsg(uType, wParam, lParam);

	switch(uType)
	{
	case INPUTWND_SEARCHMSG:
		OnSearchItemChange();
		mSongListCtrl.StartFadeInEffect();
		break;

	case SEARCHITEM_CLICKMSG:
		gMainCtrlPage->mSearchInputWnd.ResetSearchInfo();
		OnSearchItemChange();
		mSongListCtrl.StartFadeInEffect();
		break;

	case LISTCTRL_ITEM_CLICKMSG:
		OnListCtrlItemClicked(wParam, lParam);
		break;

	case LISTCTRL_FIRSTITEM_CHANGEMSG:
		OnListCtrlFirstItemChange(wParam);
		break;

	case PLAYERINFO_SONGSTATE_CHANGE:
		{
			char cID[9];
			cID[0] = wParam & 0x000000FF;
			cID[1] = (wParam & 0x0000FF00) >> 8;
			cID[2] = (wParam & 0x00FF0000) >> 16;
			cID[3] = (wParam & 0xFF000000) >> 24;
			cID[4] = lParam & 0x000000FF;
			cID[5] = (lParam & 0x0000FF00) >> 8;
			cID[6] = (lParam & 0x00FF0000) >> 16;
			cID[7] = (lParam & 0xFF000000) >> 24;
			cID[8] = '\0';
			OnListItemStateChange(cID);
			break;
		}

	case SEARCHPINYINITEM_CLICKMSG:
		{
			gMainCtrlPage->mSearchInputWnd.ResetSearchInfo();
			mSongListCtrl.StartFadeInEffect();
		}
		break;

	default:
		break;
	}
}

void CSongListFragment::OnWindowVisible(BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);
}

BOOL CSongListFragment::OnReturn()
{
	mSongListCtrl.WaitForEffectTimeOver();
	mLastSearchSubItem = NULL;
	mSearchTypeBar.SetSearchItem(NULL, 0);
	return FALSE;
}

void CSongListFragment::OnClick(CBaseWnd *pWnd, POINT pt)
{
	int nWndID = pWnd->GetWndID();

	switch (nWndID)
	{
	case BTN_RETURN_ID:
		gMainCtrlPage->OnReturnClick();
		break;

	case BTN_PREVPAGE_ID:
	{
		int nPageIndex = mSongListCtrl.GetCurPageIndex();
		int nPageCount = mSongListCtrl.GetPageCount();
		if ( nPageIndex > 0 )
		{
			nPageIndex--;
			int nCountPerPage = mSongListCtrl.GetCountPerPage();
			int nItemIndex = nPageIndex *nCountPerPage;

			mSongListCtrl.SetFirstItemIndex(nItemIndex);
			mSongListCtrl.StartFadeInEffect();

			SetPageCountInfo(nPageIndex+1, nPageCount);
		}
	}
		break;

	case BTN_NEXTPAGE_ID:
	{
		int nPageCount = mSongListCtrl.GetPageCount();
		int nPageIndex = mSongListCtrl.GetCurPageIndex();
		int nCountPerPage = mSongListCtrl.GetCountPerPage();
		if ( nPageIndex < nPageCount-1 )
		{
			nPageIndex++;
			int nItemIndex = nPageIndex *nCountPerPage;

			mSongListCtrl.SetFirstItemIndex(nItemIndex);
			mSongListCtrl.StartFadeInEffect();

			SetPageCountInfo(nPageIndex+1, nPageCount);
		}
	}
		break;

	default:
		break;
	}
}

void CSongListFragment::OnSearchItemChange()
{
	if ( !mCurSearchType ) return;

	SEARCHINFO searchinfo={"\0", INPUTMETHODTYPE_SPELL, WordLength_All};
	gMainCtrlPage->mSearchInputWnd.GetSearchInfo(&searchinfo);

	DbgOutput(DBG_INFO, "type:%d, OnSearchItemChange: item=%s, input=%s, depth=%d\n",
			mCurSearchType->eType, mCurSearchType->cItemName, searchinfo.cInputContent, mCurSearchType->nDepth);

	UINT64 uTime0 = GetTickCount();
	mSongListCtrl.DestroyE3DListView();
	mSongListCtrl.DelAllItem();

	UINT64 uTime1 = GetTickCount();

	DbgOutput(DBG_DEBUG, "Release list item used %d ms\n", uTime1-uTime0);

	CPtrArrayCtrl sSongList;

	if (mCurSearchType->eType == SearchBySongType)
	{
		FastSearch_SearchSongListBySongType(
			&mCurSearchType->nDepth,
			&searchinfo,
			&sSongList);
	}
	else if (mCurSearchType->eType == SearchByName)
	{
		FastSearch_SearchSongListBySongName(
			&searchinfo,
			&sSongList);
	}
	else if (mCurSearchType->eType == SearchByLanguageType)
	{
		FastSearch_SearchSongListByLanguageType(
			&mCurSearchType->nDepth,
			&searchinfo,
			&sSongList);
	}
	else if ( mCurSearchType->eType == SearchByRank )
	{
		FastSearch_SearchSongListByRanking(
				&mCurSearchType->nDepth,
				&searchinfo,
				&sSongList);
	} 
  else if ( mCurSearchType->eType == SearchByMonth)
	{
		FastSearch_SearchSongListByCloud(
				&mCurSearchType->nDepth,
				&searchinfo,
				&sSongList);
	}
	else if ( mCurSearchType->eType == SearchBySinger)
	{
		FastSearch_SearchSongListBySingerName(
				&mCurSearchType->nDepth,
				&searchinfo,
				&sSongList);
	}
	else if (mCurSearchType->eType == SearchByNewSong)
	{
		FastSearch_SearchNewSongList(
			&searchinfo,
			&sSongList);
	}
  else if (mCurSearchType->eType == SearchBYGradeSong)
	{
		FastSearch_SearchGradeSongList(
			&searchinfo,
			&sSongList);
	}

	UINT64 uTime2 = GetTickCount();

	DbgOutput(DBG_DEBUG, "Search Item used %d ms\n", uTime2-uTime1);

	int nSongCount = sSongList.GetCount();
	SetSongCount(nSongCount);

	int i;
	for ( i = 0 ; i < nSongCount ; i++ )
	{
		SONGINFO *pSongInfo = (SONGINFO *)sSongList.GetAt(i);
		AddSongItem(pSongInfo, i);
	}

	mSongListCtrl.RenewE3DListView();

	UINT64 uTime3 = GetTickCount();

	DbgOutput(DBG_DEBUG, "Add Item To List used %d ms\n", uTime3-uTime2);

	int nPageCount = mSongListCtrl.GetPageCount();
	int nPageIndex = mSongListCtrl.GetCurPageIndex();
	//mPagePrompt.SetListPageInfo(nPageCount>0?nPageIndex+1:0, nPageCount);
	SetPageCountInfo(nPageCount>0?nPageIndex+1:0, nPageCount);

	UINT64 uTime4 = GetTickCount();

	DbgOutput(DBG_DEBUG, "OnSearchItemChange: total used %llu ms, system tick is %llu\n",
		uTime4-uTime0, uTime4);
}

void CSongListFragment::OnSongItemStateChange(int nItemIndex)
{
	LISTITEM *pli = mSongListCtrl.GetListItem(nItemIndex);
	if ( !pli ) return;

	//printf("==the item idx:%d\n", nItemIndex);

	SONGINFO *pSongInfo = (SONGINFO *)pli->uItemData;
	if ( pSongInfo )
	{
		BOOL bPlaying = (gPlayerInfo->IsSongInSelectedList(pSongInfo->cID)
				|| gPlayerInfo->IsSongInCloudList(pSongInfo->cID) );
		UINT32 uSongNameColor = bPlaying ? mSelectedColor:mSongNameColor;
		UINT32 uSingerNameColor = bPlaying ? mSelectedColor:mSingerNameColor;

		mSongListCtrl.SetItemColor(nItemIndex, lviSongSongName, uSongNameColor);
		mSongListCtrl.SetItemColor(nItemIndex, lviSongSingerName, uSingerNameColor);
		if ( mCurSearchType->eType == SearchByRank )
			mSongListCtrl.SetItemTexture(nItemIndex, lviSongBackground, mSongListCtrl.GetSongBkImageTexture(pSongInfo, nItemIndex));
		else
			mSongListCtrl.SetItemTexture(nItemIndex, lviSongBackground, mSongListCtrl.GetSongBkImageTexture(pSongInfo));
	}
}


// 给服务器发送点击的歌曲ID来计算点击排行
int CSongListFragment::SendClickSongID(const char* cSongID)
{
	int ret = -1;
	char data[128];
  
	sprintf(data,"{\"type\":\"ping\",\"data\":{\"mac\":\"%s\",\"songid\":%s}}",g_sIPInfo.cMac,cSongID);
	
   //DbgOutput(DBG_ERR, "songid Data :%s\n",data);
	ret = CClientSharing::mClientSocket.Send(data,strlen(data));
  
	return ret;
  
}

void CSongListFragment::OnListCtrlItemClicked(int nItemIndex, int nSubItemIndex)
{
	LISTITEM *pli = mSongListCtrl.GetListItem(nItemIndex);
	if ( !pli ) return;

	SONGINFO *pSongInfo = (SONGINFO *)pli->uItemData;
  
	if ( !pSongInfo ) return;
  if ( nSubItemIndex == lviSongSingerName ||
		  nSubItemIndex == lviSongSongName ||
		  nSubItemIndex == lviSongBackground )
	{
     
			if ( !pSongInfo->bExist )
			{
        SendClickSongID(pSongInfo->cID);
				if ( !gPlayerInfo->IsSongInCloudList(pSongInfo->cID) )
				{
					mCloudDownLoadPromptWnd.SetCloudSongInfo(pSongInfo);
					mCloudDownLoadPromptWnd.SlideShowWindow(TRUE);
				}
        
			}
			if ( gPlayerInfo->AddSongToSelectedList(pSongInfo, -1) )
			{
        if(!gMainCtrlPage->mSearchInputWnd.IsPopupSlide())
				{
          if ( pSongInfo->bExist)  //浜戠姝屾洸涓嶆樉绀篺ly鏁堟灉
					mSongListCtrl.StartFlyItem(nItemIndex);
				}
				gMainCtrlPage->mSearchInputWnd.ResetSearchInfo();
			}
      SendClickSongID(pSongInfo->cID);
	}
	else if (nSubItemIndex == lviSongFavoritBtn)
	{
		gMainCtrlPage->AddToFavorit(pSongInfo->cID);
    
	}
	else if (nSubItemIndex == lviSongRushBtn)
	{

    SendClickSongID(pSongInfo->cID);
		if ( !pSongInfo->bExist )
		{
			if ( !gPlayerInfo->IsSongInCloudList(pSongInfo->cID) )
			{
				mCloudDownLoadPromptWnd.SetCloudSongInfo(pSongInfo);
				mCloudDownLoadPromptWnd.SlideShowWindow(TRUE);
			
			}
			return;
		}

		if (gPlayerInfo->IsSongInSelectedList(pSongInfo->cID))
		{
			gPlayerInfo->MoveSelectedSongToFirst(pSongInfo->cID);
			gMainCtrlPage->mSearchInputWnd.ResetSearchInfo();
		}
		else
		{
			if ( gPlayerInfo->AddSongToSelectedList(pSongInfo, 0) )
			{
				mSongListCtrl.StartFlyItem(nItemIndex);
				gMainCtrlPage->mSearchInputWnd.ResetSearchInfo();
			}
		}
	}
	else if (nSubItemIndex == lviSongDelBtn)
	{
		gPlayerInfo->DelSongFromSelectedList(pSongInfo->cID);
		gMainCtrlPage->mSearchInputWnd.ResetSearchInfo();
    
	}
	else if(nSubItemIndex == lviSongSongName)
	{
    SendClickSongID(pSongInfo->cID);
		if ( !pSongInfo->bExist )
		{
			if ( !gPlayerInfo->IsSongInCloudList(pSongInfo->cID) )
			{
				mCloudDownLoadPromptWnd.SetCloudSongInfo(pSongInfo);
				mCloudDownLoadPromptWnd.SlideShowWindow(TRUE);
			}
			return;
		}

		if ( gPlayerInfo->AddSongToSelectedList(pSongInfo, -1) )
		{
			if(!gMainCtrlPage->mSearchInputWnd.IsPopupSlide())
				mSongListCtrl.StartFlyItem(nItemIndex);
			gMainCtrlPage->mSearchInputWnd.ResetSearchInfo();
		}
	}
	else
	{
    SendClickSongID(pSongInfo->cID);
		if ( !pSongInfo->bExist )
		{
			if ( !gPlayerInfo->IsSongInCloudList(pSongInfo->cID) )
			{
				mCloudDownLoadPromptWnd.SetCloudSongInfo(pSongInfo);
				mCloudDownLoadPromptWnd.SlideShowWindow(TRUE);
			}
			return;
		}

		if ( gPlayerInfo->AddSongToSelectedList(pSongInfo, -1) )
		{
			if(!gMainCtrlPage->mSearchInputWnd.IsPopupSlide())
				mSongListCtrl.StartFlyItem(nItemIndex);
			gMainCtrlPage->mSearchInputWnd.ResetSearchInfo();
		}
	}
}

void CSongListFragment::OnListCtrlFirstItemChange(int nItemIndex)
{
	int nPageCount = mSongListCtrl.GetPageCount();
	int nPageIndex = mSongListCtrl.GetCurPageIndex();

	if ( nPageIndex >= nPageCount )
		return;

	//mPagePrompt.SetListPageInfo(nPageCount>0?nPageIndex+1:0, nPageCount);
	SetPageCountInfo(nPageCount>0?nPageIndex+1:0, nPageCount);
	//mPageCountSlide.SetPos(nPageIndex);

	int nCountPerPage = mSongListCtrl.GetCountPerPage();

	int i;
	for ( i = 0 ; i < nCountPerPage ; i++ )
	{
		OnSongItemStateChange(nItemIndex+i);
	}
}

void CSongListFragment::OnListItemStateChange(const char* cID)
{
	if (!cID)
	{
		return;
	}

	int nIndex = mSongListCtrl.FindItemByID(cID);
	OnSongItemStateChange(nIndex);
}

void CSongListFragment::SetSearchBarItem(SEARCHITEM *pSearchItem, int nFirstShowItemIndex)
{
	if (!pSearchItem)
	{
		DbgOutput(DBG_ERR, "SetSearchBarItem, pSearchItem = Null");
		return;
	}

	SEARCHITEM *pCurSearchSubItem = mSearchTypeBar.GetSelectedSubItem();

	if (pCurSearchSubItem && mLastSearchSubItem)
	{
		if (pCurSearchSubItem->eType != pSearchItem->eType)
		{
			if ( pCurSearchSubItem->eType != mLastSearchSubItem->eType )
			{
				mLastSearchSubItem = pCurSearchSubItem;
			}
		}
	}
	else if (pCurSearchSubItem)
	{
		mLastSearchSubItem = pCurSearchSubItem;
	}
	else
	{
		mLastSearchSubItem = NULL;
	}
	if((pSearchItem->eType == SearchByNewSong))
	{
		mSearchTypeBar.SetWindowVisible(FALSE);
	
	}
	
	mSearchTypeBar.SetSearchItem(pSearchItem, nFirstShowItemIndex);


	CBaseStringA sPrompt;

	SEARCHITEM *pRoot = pSearchItem;

	sPrompt.Set(pRoot->cItemName);

	while (pRoot->pParentItem)
	{
		pRoot = pRoot->pParentItem;

		CBaseStringA sTmp(&sPrompt);
		sPrompt.Format("%s->%s", pRoot->cItemName, sTmp.GetString());
	}

	//modify by yq,2013-01-24
//mSearchPrompt.SetWindowTextA(sPrompt.GetString());
//	mPrompt.Set(sPrompt.GetString());
	//end modify by yq,2013-01-24

	DbgOutput(DBG_DEBUG, "search prompt = %s, depth = %d, type = %d\n",
		sPrompt.GetString(), pSearchItem->nDepth, pSearchItem->eType);

	gMainCtrlPage->mSearchInputWnd.ResetSearchInfo();
	OnSearchItemChange();

	mSongListCtrl.StartFlyinEffect();
}

void CSongListFragment::SetSongCount(int nSongCount)
{
	mSongListCtrl.SetItemCount(nSongCount);

//	int nPageCount = mSongListCtrl.GetPageCount();
//
//	mPageCountSlide.SetMinMax(0, nPageCount-1);
//	mPageCountSlide.SetPos(0);
}

void CSongListFragment::AddSongItem(const SONGINFO *pSong, int nIndex)
{
	mSongListCtrl.AddSongItem(pSong, nIndex);
}

void CSongListFragment::SetPageCountInfo(int curPage, int pageCount)
{
	CBaseStringA tmp;
	tmp.Format("%d/%d", curPage, pageCount);
	mPageInfoWnd.SetWindowTextA(tmp.GetString());
}

