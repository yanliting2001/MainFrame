#include "MusicLibItemWnd.h"
#include "BaseApp.h"
#include "XMLParser.h"
#include "../SQL/FastSearch.h"
#include "../CommControl/MainCtrlPage.h"
#include "../Settings/SettingsFragment.h"

extern CMainCtrlPage *gMainCtrlPage;

#define BUTTON_ID_PREVBTN			0x01
#define BUTTON_ID_NEXTBTN			0x02
#define BUTTON_ID_CONFIRM			0x03
#define BUTTON_ID_BACK				0x04
#define BUTTON_ID_CHOOSESONG		0x05

#define BUTTON_ID_CHOOSE_BASE		0x10

static char gTypeTitile[TYPE_MUSIC_COUNT][64]=
{
		"公播设置",
		"新歌设置",
		"批量删歌"
};

CMusicLibTypeWnd::CMusicLibTypeWnd()
{
	mSongCurPage = 0;
	mSongPageCount = 0;
	memset(mTitleIcoBasePath,0,sizeof(mTitleIcoBasePath));
	mCurType = TYPE_PUBLIC;
	mpChooseSongWnd = NULL;
}

CMusicLibTypeWnd::~CMusicLibTypeWnd()
{
	FreeListData();
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CMusicLibTypeWnd::Create(
		CBaseWnd *pParent,
		RECT rc)
{
	CParentClass::Create("MusicLibTypeWnd", pParent, rc);

	RECT rcControl;
	SetRectXY(&rcControl, 50, 122, 1122, 371);
	mTitleIcoWnd.CreateStatic(this, rcControl);
	mTitlePromptWnd.CreateStatic(this, rcControl);
	mTiaoWnd.CreateStatic(this, rcControl);

	int i = 0;

	for ( i = 0 ; i < MUSICLIBEITEM_SHOW_COUNT ; i++ )
	{
		mItem[i].SongNameWnd.CreateStatic(this, rcControl);
		mItem[i].SongNameWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

		mItem[i].SingerNameWnd.CreateStatic(this, rcControl);
		mItem[i].SingerNameWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

		mItem[i].ChooseBtn.Create(this, rcControl,BUTTONTYPE_RADIO);
		mItem[i].ChooseBtn.SetWndID(BUTTON_ID_CHOOSE_BASE + i);
		mItem[i].ChooseBtn.SetOnClickListener(this);

		mItem[i].TiaoWnd.Create(this,rcControl);
	}

	mPageInfoWnd.CreateStatic(this, rcControl);

	SetRectXY(&rcControl, 285, 503, 103, 36);
	mSongPagePrevBtn.Create(this, rcControl);
	mSongPagePrevBtn.SetWndID(BUTTON_ID_PREVBTN);
	mSongPagePrevBtn.SetOnClickListener(this);

	mSongPageNextBtn.Create(this, rcControl);
	mSongPageNextBtn.SetWndID(BUTTON_ID_NEXTBTN);
	mSongPageNextBtn.SetOnClickListener(this);

	mConfirmBtn.Create(this, rcControl);
	mConfirmBtn.SetWndID(BUTTON_ID_CONFIRM);
	mConfirmBtn.SetOnClickListener(this);

	mBackBtn.Create(this, rcControl);
	mBackBtn.SetWndID(BUTTON_ID_BACK);
	mBackBtn.SetOnClickListener(this);

	mChooseSongBtn.Create(this, rcControl);
	mChooseSongBtn.SetWndID(BUTTON_ID_CHOOSESONG);
	mChooseSongBtn.SetOnClickListener(this);

	LoadResource();
}

void CMusicLibTypeWnd::LoadResource()
{
	/*init position by xml*/
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "MusicLibItem.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rc;

	XmlLoadRect(&parser, "ShowWindowLocation", &rc);
	CParentClass::MoveWindow(&rc);

	RECT rcControl;

///////////////////////////////////
	XmlLoadRect(&parser, "TitleIcoLocation", &rcControl);
	mTitleIcoWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "TitlePromptLocation", &rcControl);
	mTitlePromptWnd.MoveWindow(&rcControl);

	RECT rcSongItem,rcSingerItem,rcChooseItem,rcTiao;
	int i, yOffset;

	XmlLoadRect(&parser, "SongNameLocation", &rcSongItem);
	yOffset = parser.GetIntValue("SongNameLocation", "yOffset", 371);
	XmlLoadRect(&parser, "SingerNameLocation", &rcSingerItem);
	XmlLoadRect(&parser, "ChooseBtnLocation", &rcChooseItem);
	XmlLoadRect(&parser, "TiaoWndLocation", &rcTiao);
	for(i = 0; i < MUSICLIBEITEM_SHOW_COUNT; i++)
	{
		InflatRect(&rcControl, &rcSongItem, 0, i*yOffset);
		mItem[i].SongNameWnd.MoveWindow(&rcControl);

		InflatRect(&rcControl, &rcSingerItem, 0, i*yOffset);
		mItem[i].SingerNameWnd.MoveWindow(&rcControl);

		InflatRect(&rcControl, &rcChooseItem,0, i*yOffset);
		mItem[i].ChooseBtn.MoveWindow(&rcControl);

		InflatRect(&rcControl, &rcTiao,0, i*yOffset);
		mItem[i].TiaoWnd.MoveWindow(&rcControl);
	}

	InflatRect(&rcControl, &rcTiao,0, i*yOffset);
	mTiaoWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "PageInfoWndLocation", &rcControl);
	mPageInfoWnd.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "SongPagePrevBtnLocation", &rcControl);
	mSongPagePrevBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "SongPageNextBtnLocation", &rcControl);
	mSongPageNextBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ConfirmBtnLocation", &rcControl);
	mConfirmBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "BackBtnLocation", &rcControl);
	mBackBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "ChooseSongBtnLocation", &rcControl);
	mChooseSongBtn.MoveWindow(&rcControl);

	/*end init position by xml*/

	CTexture bkTexture;
	char imgPath[64] = {0};
	strcpy(mTitleIcoBasePath,parser.GetStrValue("TitleIcoBasePath","path","AdvanceSettings/MusicLib"));

	strcpy(imgPath,parser.GetStrValue("PageInfoWndImg","path","AdvanceSettings/MusicLib/SearchPrompt"));
	CreateImgTexture(imgPath, &bkTexture);
	mPageInfoWnd.SetBackgroundTexture(&bkTexture);

	CTexture btnTextures[4],tiaoTextures[4];
	strcpy(imgPath,parser.GetStrValue("ChooseBtnImg","path","AdvanceSettings/MusicLib/ChooseSong/Choose"));
	CreateBtnImgTextures(imgPath, btnTextures);

	strcpy(imgPath,parser.GetStrValue("TiaoImg","path","AdvanceSettings/MusicLib/ChooseSong/Choose"));
	CreateBtnImgTextures(imgPath, tiaoTextures);
	mTiaoWnd.SetTextures(tiaoTextures);
	for (i = 0 ; i < MUSICLIBEITEM_SHOW_COUNT ; i++ )
	{
		mItem[i].ChooseBtn.SetTextures(btnTextures);
		mItem[i].TiaoWnd.SetTextures(tiaoTextures);
	}

	strcpy(imgPath,parser.GetStrValue("PrevBtnImg","path","AdvanceSettings/MusicLib/ChooseSong/Prev"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mSongPagePrevBtn.SetTextures(btnTextures);

	strcpy(imgPath,parser.GetStrValue("NextBtnImg","path","AdvanceSettings/MusicLib/ChooseSong/Next"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mSongPageNextBtn.SetTextures(btnTextures);

	strcpy(imgPath,parser.GetStrValue("ConfirmBtnImg","path","AdvanceSettings/MusicLib/ChooseSong/Confirm"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mConfirmBtn.SetTextures(btnTextures);

	strcpy(imgPath,parser.GetStrValue("BackBtnImg","path","AdvanceSettings/MusicLib/ChooseSong/Back"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mBackBtn.SetTextures(btnTextures);

	strcpy(imgPath,parser.GetStrValue("ChooseSongBtnImg","path","AdvanceSettings/MusicLib/ChooseSong/Back"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mChooseSongBtn.SetTextures(btnTextures);

	strcpy(gTypeTitile[TYPE_PUBLIC], parser.GetStrValue("Prompt", "PublicMsg", "公播设置"));
	strcpy(gTypeTitile[TYPE_NEWSONG], parser.GetStrValue("Prompt", "NewSongMsg", "新歌设置"));
	strcpy(gTypeTitile[TYPE_DELETE], parser.GetStrValue("Prompt", "DeleteMsg", "批量删歌"));
}

void CMusicLibTypeWnd::InitArrayList()
{
	FreeListData();
	SEARCHINFO searchinfo={"\0", INPUTMETHODTYPE_SPELL, WordLength_All};
	CPtrArrayCtrl sSongList;

	if(mCurType == TYPE_PUBLIC)
	{
		FastSearch_SearchPubBroadCastSongList(
			&searchinfo,
			&sSongList);

	}
	else if(mCurType == TYPE_NEWSONG)
	{
		FastSearch_SearchNewSongList(
			&searchinfo,
			&sSongList);
	}

	mSongCurPage = 0;
	mSongPageCount = 0;
	int nSongCount = sSongList.GetCount();
	int i;
	for ( i = 0 ; i < nSongCount ; i++ )
	{
		SONGINFO *pSongInfo = (SONGINFO *)sSongList.GetAt(i);
		if(pSongInfo)
			AddSongItem(pSongInfo);
	}
}

void CMusicLibTypeWnd::OnClick(
	CBaseWnd *pWnd,
	POINT pt)
{
	int nWndID = pWnd->GetWndID();

	if (nWndID == BUTTON_ID_PREVBTN)
	{
		if (mSongCurPage > 0)
		{
			mSongCurPage --;
			UpdateItem();
		}
	}
	else if (nWndID == BUTTON_ID_NEXTBTN)
	{
		if (mSongCurPage < (mSongPageCount-1))
		{
			mSongCurPage ++;
			UpdateItem();
		}
	}
	else if (nWndID >= BUTTON_ID_CHOOSE_BASE &&
			nWndID <= BUTTON_ID_CHOOSE_BASE+MUSICLIBEITEM_SHOW_COUNT)
	{
		int idx = nWndID - BUTTON_ID_CHOOSE_BASE;
		OnChooseBtnClick(idx);
	}
	else if (nWndID == BUTTON_ID_CONFIRM)
	{
		OnConfirmBtnClick();
	}
	else if (nWndID == BUTTON_ID_BACK)
	{
		OnBackBtnClick();
	}
	else if(nWndID== BUTTON_ID_CHOOSESONG)
	{
		OnChooseSongBtnClick();
	}
}

void CMusicLibTypeWnd::SetMusicLibType(MUSICLIBTYPE type)
{
	mCurType = type;

	char ico[64] = {0};
	sprintf(ico,"%s/ico%d",mTitleIcoBasePath,mCurType);

	CTexture bkTexture;
	CreateImgTexture(ico, &bkTexture);
	mTitleIcoWnd.SetBackgroundTexture(&bkTexture);
	mTitlePromptWnd.SetWindowTextA(gTypeTitile[mCurType]);

	InitArrayList();
	UpdateItem();
}

void CMusicLibTypeWnd::AddSongItem(
	SONGINFO *pSongInfo)
{
	SONGINFO *pNewSongInfo = new SONGINFO;
	if (!pNewSongInfo)
	{
		return;
	}
	memcpy(pNewSongInfo, pSongInfo, sizeof(SONGINFO));
	pNewSongInfo->bPlaying = TRUE;
	mSongItemList.AddData(pNewSongInfo);
	mSongPageCount = (mSongItemList.GetCount()+MUSICLIBEITEM_SHOW_COUNT-1) / MUSICLIBEITEM_SHOW_COUNT;
}

void CMusicLibTypeWnd::UpdateItem()
{
	int nFirstIndex = mSongCurPage * MUSICLIBEITEM_SHOW_COUNT;
	int i;

	for ( i = 0 ; i < MUSICLIBEITEM_SHOW_COUNT ; i++ )
	{
		SONGINFO *pSong = (SONGINFO *)mSongItemList.GetAt(i+nFirstIndex);
		if (pSong)
		{
			mItem[i].SongNameWnd.SetWindowTextA(pSong->cName);
			char *singer = FastSearch_GetSingerByID(pSong->singerID);
			mItem[i].SingerNameWnd.SetWindowTextA(singer ? singer : "");
			mItem[i].ChooseBtn.SetWindowVisible(TRUE);

			if(!pSong->bPlaying)
			{
				mItem[i].ChooseBtn.SetRadioBtnPressed(FALSE);
			}
			else
			{
				mItem[i].ChooseBtn.SetRadioBtnPressed(TRUE);
			}
		}
		else
		{
			mItem[i].SongNameWnd.SetWindowTextA(NULL);
			mItem[i].SingerNameWnd.SetWindowTextA(NULL);
			mItem[i].HotNumWnd.SetWindowTextA(NULL);
			mItem[i].ChooseBtn.SetWindowVisible(FALSE);
		}
	}

	CBaseStringW sTmp;
	sTmp.Format("%d/%d",mSongCurPage+1,mSongPageCount);
	mPageInfoWnd.SetWindowTextW(sTmp.GetString());
}

void CMusicLibTypeWnd::OnChooseBtnClick(int Index)
{
	if((Index<0) || (Index >=MUSICLIBEITEM_SHOW_COUNT))
	{
		DbgOutput(DBG_VERB, "function %s:%d ...\n", __FILE__,__LINE__);
		return ;
	}

	int nFirstIndex = mSongCurPage * MUSICLIBEITEM_SHOW_COUNT;

	SONGINFO *pSong = (SONGINFO *)mSongItemList.GetAt(Index+nFirstIndex);
	if(!pSong)
	{
		DbgOutput(DBG_VERB, "function %s:%d ...\n", __FILE__,__LINE__);
		return ;
	}

	if(mItem[Index].ChooseBtn.GetRadioBtnPressed())
		pSong->bPlaying = TRUE;
	else
		pSong->bPlaying = FALSE;
}


void CMusicLibTypeWnd::OnConfirmBtnClick()
{
	gMainCtrlPage->StartWaiting();
	if(mCurType == TYPE_PUBLIC)
	{
		FastSearch_SavePubBroadCastSongList(&mSongItemList);
	}
	else if(mCurType == TYPE_NEWSONG)
	{
		FastSearch_SaveNewSongList(&mSongItemList);
	}
	else
	{
		FastSearch_DeleteSongList(&mSongItemList);
	}
	FreeListData();
	gMainCtrlPage->StopWaiting();
	SetWindowVisible(FALSE);
	gMusicLibWnd->SetMusicLibInfo();
	gMusicLibWnd->SetWindowVisible(TRUE);
}

void CMusicLibTypeWnd::OnBackBtnClick()
{
	FreeListData();
	SetWindowVisible(FALSE);
	gMusicLibWnd->SetWindowVisible(TRUE);
}

void CMusicLibTypeWnd::OnChooseSongBtnClick()
{
	SetWindowVisible(FALSE);
	mpChooseSongWnd->InitSelectedItemList(&mSongItemList);
	mpChooseSongWnd->SetWindowVisible(TRUE);
}

void CMusicLibTypeWnd::SetChooseSongWnd(CSetMusicLibWnd *pChooseSongWnd)
{
	mpChooseSongWnd = pChooseSongWnd;
}

void CMusicLibTypeWnd::RefreshItem()
{
	mSongCurPage = 0;
	mSongPageCount = (mSongItemList.GetCount()+MUSICLIBEITEM_SHOW_COUNT-1) / MUSICLIBEITEM_SHOW_COUNT;
	UpdateItem();
}

void CMusicLibTypeWnd::FreeListData(void)
{
	DelNormalList(&mSongItemList, SONGINFO);
}
