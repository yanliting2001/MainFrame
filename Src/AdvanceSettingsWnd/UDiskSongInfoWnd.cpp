#include "UDiskSongInfoWnd.h"
#include "BaseApp.h"
#include "XMLParser.h"
#include "UserMsgDefine.h"
#include "../GlobalUIClass.h"
#include "../SQL/SearchSQL.h"

#define BUTTON_ID_CONFIRM			0
#define BUTTON_ID_CANCEL			1
#define BUTTON_ID_BACK				2
#define BUTTON_ID_CLEAR				3
#define BUTTON_ID_HANDITEMBASE		5
#define BUTTON_ID_SELECTITEMLEFTBASE	10
#define BUTTON_IN_SELECTITEMRIGHTBASE	25

static char gSongItem[SONGINFONUM][32] = {
		"歌曲名称",
		"歌星名称",
		"拼音缩写",
		"歌曲字数",
		"歌曲类型",
		"歌曲语种"
};

static char gSongCount[9][16] = {
		"1字",
		"2字",
		"3字",
		"4字",
		"5字",
		"6字",
		"7字",
		"8字",
		"多字",
};

//static char gSongType[10][32] = {
//		"流行歌曲",
//		"民族歌曲",
//		"情歌对唱",
//		"革命歌曲",
//		"祝福歌曲",
//		"戏曲",
//		"舞曲",
//		"摇滚乐",
//		"儿童歌曲",
//		"电影"
//};
//
//static char gLanguage[9][32] = {
//		"国语",
//		"粤语",
//		"台语",
//		"英语",
//		"日语",
//		"韩语",
//		"泰语",
//		"缅甸语",
//		"潮汕话",
//};

CUDiskSongInfo::CUDiskSongInfo()
{
	mCurSongInfo = NULL;
	memset(mcSongType, 0, sizeof(mcSongType));
	memset(mcLanguage, 0, sizeof(mcLanguage));

	miHandIdx		= 0;

	miSongCount		= sizeof(gSongCount)/sizeof(gSongCount[0]);
	miCurrSongIdx	= 0;

	miSongTypeCount	= gSearchSongType.sChildItemList.GetCount();
	miCurrSongTypeIdx	= 0;

	miLangTypeCount	= gSearchLanguageType.sChildItemList.GetCount();
	miCurrLangIdx	= 0;
}

CUDiskSongInfo::~CUDiskSongInfo()
{
	DbgOutput(DBG_INFO, "function %s quit\n", __FUNCTION__);
}

void CUDiskSongInfo::Create(
		CBaseWnd *pParent)
{
	RECT rc = {0, 50, LAYOUT_WIDTH, 600};

	CParentClass::Create(
		"UDiskSongInfo",
		pParent,
		rc,
		WINDOWFLAG_DEFAULT,
		WINDOWSTATE_INVISIBLE);

	RECT rcControl;
	SetRectXY(&rcControl, 0, 42, 375, 328);
	mFileNameTitleWnd.CreateStatic(this, rcControl);

	int i;
	for ( i = 0 ; i < SONGINFONUM ; i++ )
	{
		mSongItemPromptWnd[i].CreateStatic(this, rcControl);
		mSongItemPromptWnd[i].SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
		mSongItemPromptWnd[i].SetWindowTextA(gSongItem[i]);
	}

	for(i=0; i<3; i++)
	{
		mSelectItemWnd[i].Create(this, rcControl);

		mSelectItemLeftBtn[i].Create(this, rcControl);
		mSelectItemLeftBtn[i].SetWndID(BUTTON_ID_SELECTITEMLEFTBASE+i);
		mSelectItemLeftBtn[i].SetOnClickListener(this);

		mSelectItemRightBtn[i].Create(this, rcControl);
		mSelectItemRightBtn[i].SetWndID(BUTTON_IN_SELECTITEMRIGHTBASE+i);
		mSelectItemRightBtn[i].SetOnClickListener(this);

		mHandItemBtn[i].Create(this, rcControl,BUTTONTYPE_RADIO);
		mHandItemBtn[i].SetWndID(BUTTON_ID_HANDITEMBASE+i);
		mHandItemBtn[i].SetOnClickListener(this);
		mTabGroup.AddToGroup(&mHandItemBtn[i]);
	}
	mTabGroup.SetSelectButton(&mHandItemBtn[0]);

	mInputPanel.Create(this, rcControl, TRUE, FALSE);
	mInputPanel.SetMsgRecvWnd(this);

	mConfirmBtn.Create(this, rcControl);
	mConfirmBtn.SetWndID(BUTTON_ID_CONFIRM);
	mConfirmBtn.SetOnClickListener(this);

	mCancelBtn.Create(this, rcControl);
	mCancelBtn.SetWndID(BUTTON_ID_CANCEL);
	mCancelBtn.SetOnClickListener(this);

	LoadResource();
}

void CUDiskSongInfo::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "UDiskSongInfo.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml

	RECT rcControl;
	int size, yOffset;

	XmlLoadRect(&parser, "rcLocation", &rcControl);
	CParentClass::MoveWindow(&rcControl);

	XmlLoadRect(&parser, "FileNameTitleWndInfo", &rcControl);
	mFileNameTitleWnd.MoveWindow(&rcControl);
	size = parser.GetIntValue("FileNameTitleWndInfo", "size", 20);
	mFileNameTitleWnd.SetFontSize(size);

	RECT rcItem;
	int i;
	XmlLoadRect(&parser, "SongItemPromptLocation", &rcItem);
	yOffset = parser.GetIntValue("SongItemPromptLocation", "yOffset", 371);
	for(int i=0; i<SONGINFONUM; i++)
	{
		InflatRect(&rcControl, &rcItem, 0, i*yOffset);
		mSongItemPromptWnd[i].MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "SongItemInfoLocation", &rcItem);
	yOffset = parser.GetIntValue("SongItemInfoLocation", "yOffset", 371);
	for(int i=0; i<3; i++)
	{
		InflatRect(&rcControl, &rcItem, 0, i*yOffset);
		mHandItemBtn[i].MoveWindow(&rcControl);
	}
	for(int i=0; i<3; i++)
	{
		InflatRect(&rcControl, &rcItem, 0, (i+3)*yOffset);
		mSelectItemWnd[i].MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "SelectItemLeftLocation", &rcItem);
	yOffset = parser.GetIntValue("SelectItemLeftLocation", "yOffset", 371);
	for(int i=0; i<3; i++)
	{
		InflatRect(&rcControl, &rcItem, 0, i*yOffset);
		mSelectItemLeftBtn[i].MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "SelectItemRightLocation", &rcItem);
	yOffset = parser.GetIntValue("SelectItemRightLocation", "yOffset", 371);
	for(int i=0; i<3; i++)
	{
		InflatRect(&rcControl, &rcItem, 0, i*yOffset);
		mSelectItemRightBtn[i].MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "PreviewRectLocation", &mPreviewRect);

	XmlLoadRect(&parser, "InputPanelInfo", &rcControl);
	mInputPanel.MoveWindow(rcControl, rcControl);
	mInputPanel.SetInputMethodType(INPUTMETHODTYPE_SPELL);

	XmlLoadRect(&parser, "ConfirmBtnLocation", &rcControl);
	mConfirmBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "CancelBtnLocation", &rcControl);
	mCancelBtn.MoveWindow(&rcControl);

	CTexture bkTexture;
	char imgPath[64] = {0};

	CTexture btnTextures[4];
	SAFE_STRNCPY(imgPath, parser.GetStrValue("HandItemBtnImg","path","KGo/item"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	for(int i=0; i<3; i++)
	{
		mHandItemBtn[i].SetTextures(btnTextures);
	}

	SAFE_STRNCPY(imgPath, parser.GetStrValue("SelectItemWndImg","path","KGo/item"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	for(int i=0; i<3; i++)
	{
		mSelectItemWnd[i].SetTextures(btnTextures);
	}

	SAFE_STRNCPY(imgPath, parser.GetStrValue("SelectItemLeftImg","path","KGo/item"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	for(int i=0; i<3; i++)
	{
		mSelectItemLeftBtn[i].SetTextures(btnTextures);
	}

	SAFE_STRNCPY(imgPath, parser.GetStrValue("SelectItemRightImg","path","KGo/item"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	for(int i=0; i<3; i++)
	{
		mSelectItemRightBtn[i].SetTextures(btnTextures);
	}

	SAFE_STRNCPY(imgPath, parser.GetStrValue("ComfirmImg","path","KGo/pageup"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mConfirmBtn.SetTextures(btnTextures);

	SAFE_STRNCPY(imgPath, parser.GetStrValue("CancelImg","path","KGo/pagedown"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mCancelBtn.SetTextures(btnTextures);

	for(i = 0; i < SONGINFONUM; i++)
	{
		char temp[32] = {0};
		sprintf(temp, "txt%d", i);
		SAFE_STRNCPY(gSongItem[i], parser.GetStrValue("SongItemPrompt", temp, "未启用"), sizeof(gSongItem[i]));
		mSongItemPromptWnd[i].SetWindowTextA(gSongItem[i]);
	}

	GetSongType(miCurrSongTypeIdx);
	GetLangType(miCurrLangIdx);
	mSelectItemWnd[0].SetWindowTextA(gSongCount[miCurrSongIdx]);
	mSelectItemWnd[1].SetWindowTextA(mcSongType);
	mSelectItemWnd[2].SetWindowTextA(mcLanguage);
}

void CUDiskSongInfo::OnWindowVisible(
		BOOL bVisible)
{
	if(!bVisible)
	{
		gPlayerInfo->StopPreview();
	}
	CParentClass::OnWindowVisible(bVisible);
}

void CUDiskSongInfo::OnMsg(
	UINT32 uType,
	UINT64 wParam,
	UINT64 lParam)
{
	if ( uType == INPUTWND_CHARMSG )
	{
		if( miHandIdx >= 3 )
			return;

		mHandItemBtn[miHandIdx].SetWindowTextW((wchar_t *)wParam);
	}
}

void CUDiskSongInfo::OnClick(
	CBaseWnd *pWnd,
	POINT pt)
{
	int nWndID = pWnd->GetWndID();

	if (nWndID >= BUTTON_IN_SELECTITEMRIGHTBASE)
	{
		int idx = nWndID - BUTTON_IN_SELECTITEMRIGHTBASE;
		OnRightBtnClick(idx);
	}
	else if(nWndID >= BUTTON_ID_SELECTITEMLEFTBASE)
	{
		int idx = nWndID - BUTTON_ID_SELECTITEMLEFTBASE;
		OnLeftBtnClick(idx);
	}
	else if(nWndID >= BUTTON_ID_HANDITEMBASE)
	{
		mInputPanel.ClearInputText();
		miHandIdx = nWndID - BUTTON_ID_HANDITEMBASE;
		mTabGroup.SetSelectButton(&mHandItemBtn[miHandIdx]);
	}
	else if (nWndID == BUTTON_ID_CANCEL)
	{
		OnCancelClick();
	}
	else if (nWndID == BUTTON_ID_CONFIRM)
	{
		OnConfrimClick();
	}
}

void CUDiskSongInfo::OnRightBtnClick(int idx)
{
	switch ( idx )
	{
	case 0:
		++miCurrSongIdx;
		if(miCurrSongIdx >= miSongCount )
			miCurrSongIdx = 0;

		mSelectItemWnd[0].SetWindowTextA(gSongCount[miCurrSongIdx]);
		break;

	case 1:
		++miCurrSongTypeIdx;
		if( miCurrSongTypeIdx >= miSongTypeCount )
			miCurrSongTypeIdx = 0;

		GetSongType(miCurrSongTypeIdx);

		mSelectItemWnd[1].SetWindowTextA(mcSongType);
		break;

	case 2:
		++miCurrLangIdx;
		if( miCurrLangIdx >= miLangTypeCount )
			miCurrLangIdx = 0;

		GetLangType(miCurrLangIdx);

		mSelectItemWnd[2].SetWindowTextA(mcLanguage);
		break;

	default:
		break;
	}
}

void CUDiskSongInfo::OnLeftBtnClick(int idx)
{
	switch (idx)
	{
	case 0:
		--miCurrSongIdx;
		if(miCurrSongIdx < 0)
			miCurrSongIdx = miSongCount - 1;

		mSelectItemWnd[0].SetWindowTextA(gSongCount[miCurrSongIdx]);
		break;

	case 1:
		--miCurrSongTypeIdx;
		if( miCurrSongTypeIdx < 0 )
			miCurrSongTypeIdx = miSongTypeCount -1 ;

		GetSongType(miCurrSongTypeIdx);

		mSelectItemWnd[1].SetWindowTextA(mcSongType);
		break;

	case 2:
		--miCurrLangIdx;
		if(miCurrLangIdx < 0)
			miCurrLangIdx = miLangTypeCount - 1;

		GetLangType(miCurrLangIdx);

		mSelectItemWnd[2].SetWindowTextA(mcLanguage);
		break;

	default:
		break;
	}
}

void CUDiskSongInfo::OnConfrimClick()
{
	if(mCurSongInfo == NULL)
	{
		OnCancelClick();
		return ;
	}

	CBaseStringA stringName,stringSinger,stringSpell;
	const wchar_t *wszOldText = mHandItemBtn[0].GetWindowTextW();
	stringName.Set(wszOldText);
	if (stringName.GetLength() <= 0)
		return ;

	wszOldText = mHandItemBtn[1].GetWindowTextW();
	stringSinger.Set(wszOldText);
	if (stringSinger.GetLength() <= 0)
		return ;


	wszOldText = mHandItemBtn[2].GetWindowTextW();
	stringSpell.Set(wszOldText);
	if (stringSpell.GetLength() <= 0)
		return;

	strcpy(mCurSongInfo->cName,stringName.GetString());
	strcpy(mCurSongInfo->cSinger,stringSinger.GetString());
	strcpy(mCurSongInfo->cSpell,stringSpell.GetString());
	mCurSongInfo->wWdCt = miCurrSongIdx + 1;
	SAFE_STRNCPY(mCurSongInfo->cType, mcSongType, sizeof(mCurSongInfo->cType));
	SAFE_STRNCPY(mCurSongInfo->cLang, mcLanguage, sizeof(mCurSongInfo->cLang));

	gPlayerInfo->StopPreview();
	SetWindowVisible(FALSE);
	gAddSongWnd->UpdateUdiskInfo();
	gAddSongWnd->SetWindowVisible(TRUE);
	mCurSongInfo = NULL;
}

void CUDiskSongInfo::OnCancelClick()
{
	mCurSongInfo = NULL;
	gPlayerInfo->StopPreview();
	SetWindowVisible(FALSE);
	gAddSongWnd->SetWindowVisible(TRUE);
}

void CUDiskSongInfo::SetSongInfo(UDSONGINFO *pSong)
{
	if(pSong == NULL)
		return;

	CBaseStringA title;
	title.Format("文件名: %s",pSong->cFileName);
	mCurSongInfo = pSong;
	mFileNameTitleWnd.SetWindowTextA(title.GetString());
	gPlayerInfo->StartPreview(pSong->cFilePath,pSong->cFilePath,
									mPreviewRect.left,
									mPreviewRect.top,
									mPreviewRect.right-mPreviewRect.left,
									mPreviewRect.bottom-mPreviewRect.top);
	miHandIdx = 0;
	mTabGroup.SetSelectButton(&mHandItemBtn[miHandIdx]);

	mHandItemBtn[0].SetWindowTextA(pSong->cName);
	mHandItemBtn[1].SetWindowTextA(pSong->cSinger);
	mHandItemBtn[2].SetWindowTextA(pSong->cSpell);
	mInputPanel.ClearInputText();
}

void CUDiskSongInfo::GetSongType(int idx)
{
	const SEARCHITEM *pSongTypeItem = (const SEARCHITEM *)gSearchSongType.sChildItemList.GetAt(idx);
	if ( pSongTypeItem )
	{
		memcpy(mcSongType, pSongTypeItem->cItemName, sizeof(mcSongType));
	}
	else
	{
		memset(mcSongType, 0, sizeof(mcSongType));
	}
}

void CUDiskSongInfo::GetLangType(int idx)
{
	const SEARCHITEM *pLangTypeItem = (const SEARCHITEM *)gSearchLanguageType.sChildItemList.GetAt(idx);
	if ( pLangTypeItem )
	{
		memcpy(mcLanguage, pLangTypeItem->cItemName, sizeof(mcLanguage));
	}
	else
	{
		memset(mcLanguage, 0, sizeof(mcLanguage));
	}
}


