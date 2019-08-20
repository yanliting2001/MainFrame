#include "SetMusicLibWnd.h"
#include "BaseApp.h"
#include "XMLParser.h"
#include "UserMsgDefine.h"
#include "../Settings/SettingsFragment.h"

#define SONGNAMEBTN_CLICK_ID	0x00
#define SINGERBTN_CLICK_ID		0x01
#define LANGTYPEBTN_CLICK_ID	0x02
#define SONGTYPEBTN_CLICK_ID	0x03
#define WDCTBTN_CLICK_ID		0x04
#define ORDERTIMEBTN_CLICK_ID	0x05
#define CONFIRMBTN_CLICK_ID		0x06
#define CANCELBTN_CLICK_ID		0x07

CSetMusicLibWnd::CSetMusicLibWnd()
{
	mpSelectedList = NULL;
}

CSetMusicLibWnd::~CSetMusicLibWnd()
{
	DbgOutput(DBG_VERB, "function %s quit\n", __FUNCTION__);
}

void CSetMusicLibWnd::Create(CBaseWnd *pParent, RECT rc)
{
	CParentClass::Create("CSetMusicLibWnd", pParent, rc);

	RECT rcControl;
	mConditionIcoWnd.CreateStatic(this, rcControl);

	mSongNameWnd.CreateStatic(this, rcControl);
	mSongNameWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

	mSongNameBtn.Create(this, rcControl, BUTTONTYPE_RADIO);
	mSongNameBtn.SetWndID(SONGNAMEBTN_CLICK_ID);
	mSongNameBtn.SetOnClickListener(this);

	mSingerWnd.CreateStatic(this, rcControl);
	mSingerWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

	mSingerBtn.Create(this, rcControl, BUTTONTYPE_RADIO);
	mSingerBtn.SetWndID(SINGERBTN_CLICK_ID);
	mSingerBtn.SetOnClickListener(this);

	mLangTypeWnd.CreateStatic(this, rcControl);
	mLangTypeWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

	mLangTypeBtn.Create(this, rcControl, BUTTONTYPE_RADIO);
	mLangTypeBtn.SetWndID(LANGTYPEBTN_CLICK_ID);
	mLangTypeBtn.SetOnClickListener(this);

	mSongTypeWnd.CreateStatic(this, rcControl);
	mSongTypeWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

	mSongTypeBtn.Create(this, rcControl, BUTTONTYPE_RADIO);
	mSongTypeBtn.SetWndID(SONGTYPEBTN_CLICK_ID);
	mSongTypeBtn.SetOnClickListener(this);

	mTypeCondWnd.Create(this, rcControl);
	mTypeCondWnd.SetMsgRecvWnd(this);

	mWdCtBtn.Create(this, rcControl, BUTTONTYPE_RADIO);
	mWdCtBtn.SetWndID(WDCTBTN_CLICK_ID);
	mWdCtBtn.SetOnClickListener(this);

	mOrdertimersBtn.Create(this, rcControl, BUTTONTYPE_RADIO);
	mOrdertimersBtn.SetWndID(ORDERTIMEBTN_CLICK_ID);
	mOrdertimersBtn.SetOnClickListener(this);

	mTabGroup.AddToGroup(&mSongNameBtn);
	mTabGroup.AddToGroup(&mSingerBtn);
	mTabGroup.AddToGroup(&mLangTypeBtn);
	mTabGroup.AddToGroup(&mSongTypeBtn);
	mTabGroup.AddToGroup(&mWdCtBtn);
	mTabGroup.AddToGroup(&mOrdertimersBtn);

	mSelectedItemWnd.Create(this, rcControl);
	mInputWnd.Create(this, rcControl, TRUE, FALSE);
	mInputWnd.SetMsgRecvWnd(this);

	mConfirmBtn.Create(this, rcControl);
	mConfirmBtn.SetWndID(CONFIRMBTN_CLICK_ID);
	mConfirmBtn.SetOnClickListener(this);

	mCancelBtn.Create(this, rcControl);
	mCancelBtn.SetWndID(CANCELBTN_CLICK_ID);
	mCancelBtn.SetOnClickListener(this);

	LoadResource();
}

void CSetMusicLibWnd::LoadResource()
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
	XmlLoadRect(&parser, "ShowWindowLocation", &rcControl);
	CParentClass::MoveWindow(&rcControl);

	int i=0, yOffset, size;
	char strImgPath[64] = {0};
	CTexture bkTexture;
	CTexture btnTextures[4];

	XmlLoadRect(&parser, "ConditionIcoInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("ConditionIcoInfo", "imgPath", "AdvanceSettings/MusicLib/MusicLibItem/Line"), sizeof(strImgPath));
	CreateImgTexture(strImgPath, &bkTexture);
	mConditionIcoWnd.MoveWindow(&rcControl);
	mConditionIcoWnd.SetBackgroundTexture(&bkTexture);

	RECT rcPromptWnd, rcBtn;
	XmlLoadRect(&parser, "CondSongNameWndInfo", &rcPromptWnd);
	size = parser.GetIntValue("CondSongNameWndInfo", "size", 30);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("CondSongNameWndInfo", "SongNametext", "歌曲名"), sizeof(strImgPath));
	mSongNameWnd.MoveWindow(&rcPromptWnd);
	mSongNameWnd.SetFontSize(size);
	mSongNameWnd.SetWindowTextA(strImgPath);

	yOffset = parser.GetIntValue("CondSongNameBtnInfo", "yOffset", 40);
	XmlLoadRect(&parser, "CondSongNameBtnInfo", &rcBtn);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("CondSongNameBtnInfo", "imgPath", "AdvanceSettings/MusicLib/MusicLibItem/Selected"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mSongNameBtn.MoveWindow(&rcBtn);
	mSongNameBtn.SetTextures(btnTextures);

	i++;
	int y = i * yOffset;
	InflatRect(&rcControl, &rcPromptWnd, 0, y);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("CondSongNameWndInfo", "Singertext", "歌星名"), sizeof(strImgPath));
	mSingerWnd.MoveWindow(&rcControl);
	mSingerWnd.SetFontSize(size);
	mSingerWnd.SetWindowTextA(strImgPath);

	InflatRect(&rcControl, &rcBtn, 0, y);
	mSingerBtn.MoveWindow(&rcControl);
	mSingerBtn.SetTextures(btnTextures);

	i++;
	y = i * yOffset;
	InflatRect(&rcControl, &rcPromptWnd, 0, y);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("CondSongNameWndInfo", "LangTypetext", "语种"), sizeof(strImgPath));
	mLangTypeWnd.MoveWindow(&rcControl);
	mLangTypeWnd.SetFontSize(size);
	mLangTypeWnd.SetWindowTextA(strImgPath);

	InflatRect(&rcControl, &rcBtn, 0, y);
	mLangTypeBtn.MoveWindow(&rcControl);
	mLangTypeBtn.SetTextures(btnTextures);

	i++;
	y = i * yOffset;
	InflatRect(&rcControl, &rcPromptWnd, 0, y);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("CondSongNameWndInfo", "SongTypetext", "曲种"), sizeof(strImgPath));
	mSongTypeWnd.MoveWindow(&rcControl);
	mSongTypeWnd.SetFontSize(size);
	mSongTypeWnd.SetWindowTextA(strImgPath);

	InflatRect(&rcControl, &rcBtn, 0, y);
	mSongTypeBtn.MoveWindow(&rcControl);
	mSongTypeBtn.SetTextures(btnTextures);

	XmlLoadRect(&parser, "WdCtBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("WdCtBtnInfo", "imgPath", "AdvanceSettings/MusicLib/MusicLibItem/Selected"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mWdCtBtn.MoveWindow(&rcControl);
	mWdCtBtn.SetTextures(btnTextures);
	mWdCtBtn.SetFontColor(0xFF000000);

	XmlLoadRect(&parser, "OrderTimesBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("OrderTimesBtnInfo", "imgPath", "AdvanceSettings/MusicLib/MusicLibItem/Selected"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mOrdertimersBtn.MoveWindow(&rcControl);
	mOrdertimersBtn.SetTextures(btnTextures);
	mOrdertimersBtn.SetFontColor(0xFF000000);

	XmlLoadRect(&parser, "CanfirmBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("CanfirmBtnInfo", "imgPath", "AdvanceSettings/MusicLib/MusicLibItem/Selected"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mConfirmBtn.MoveWindow(&rcControl);
	mConfirmBtn.SetTextures(btnTextures);

	XmlLoadRect(&parser, "CancelBtnInfo", &rcControl);
	SAFE_STRNCPY(strImgPath, parser.GetStrValue("CancelBtnInfo", "imgPath", "AdvanceSettings/MusicLib/MusicLibItem/Selected"), sizeof(strImgPath));
	CreateBtnImgTextures(strImgPath, btnTextures);
	mCancelBtn.MoveWindow(&rcControl);
	mCancelBtn.SetTextures(btnTextures);

	XmlLoadRect(&parser, "InputPanelWndInfo", &rcControl);
	mInputWnd.MoveWindow(rcControl, rcControl);
}

void CSetMusicLibWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	int iWndID = pWnd->GetWndID();
	switch ( iWndID )
	{
	case SONGNAMEBTN_CLICK_ID:
		mTabGroup.SetSelectButton(&mSongNameBtn);
		mInputWnd.ClearInputText();
		mInputWnd.SetInputMethodType(INPUTMETHODTYPE_SPELL);
		break;

	case SINGERBTN_CLICK_ID:
		mTabGroup.SetSelectButton(&mSingerBtn);
		mInputWnd.ClearInputText();
		mInputWnd.SetInputMethodType(INPUTMETHODTYPE_HANDWRITE);
		break;

	case LANGTYPEBTN_CLICK_ID:
		mTabGroup.SetSelectButton(&mLangTypeBtn);
		mInputWnd.ClearInputText();
		mInputWnd.SetInputMethodType(INPUTMETHODTYPE_SPELL);
		mTypeCondWnd.InitTypeConditonList(&gSearchLanguageType.sChildItemList);
		break;

	case SONGTYPEBTN_CLICK_ID:
		mTabGroup.SetSelectButton(&mSongTypeBtn);
		mInputWnd.ClearInputText();
		mInputWnd.SetInputMethodType(INPUTMETHODTYPE_SPELL);
		mTypeCondWnd.InitTypeConditonList(&gSearchSongType.sChildItemList);
		break;

	case WDCTBTN_CLICK_ID:
		mTabGroup.SetSelectButton(&mWdCtBtn);
		mInputWnd.ClearInputText();
		mInputWnd.SetInputMethodType(INPUTMETHODTYPE_SPELL);
		break;

	case ORDERTIMEBTN_CLICK_ID:
		mTabGroup.SetSelectButton(&mOrdertimersBtn);
		mInputWnd.ClearInputText();
		mInputWnd.SetInputMethodType(INPUTMETHODTYPE_SPELL);
		break;

	case CONFIRMBTN_CLICK_ID:
		mSelectedItemWnd.AddSearchListToSelectedList();
		mSelectedItemWnd.FreeAllList();
		SetWindowVisible(FALSE);
		gMusicLibTypeWnd->RefreshItem();
		gMusicLibTypeWnd->SetWindowVisible(TRUE);
		break;

	case CANCELBTN_CLICK_ID:
		mSelectedItemWnd.FreeAllList();
		SetWindowVisible(FALSE);
		gMusicLibTypeWnd->RefreshItem();
		gMusicLibTypeWnd->SetWindowVisible(TRUE);
		break;
	}
}

void CSetMusicLibWnd::OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam)
{
	CParentClass::OnMsg(uType, wParam, lParam);
	switch(uType)
	{
	case INPUTWND_CHARMSG:
		// 输入信息
		OnInputPanelClick(wParam);
		break;

	case INPUTWND_BKTYPECONDITION_CHARMSG:
		// 种类
		OnTypeItemClick();
		break;

	default:
		break;
	}
}

void CSetMusicLibWnd::InitSelectedItemList(CPtrListCtrl *pSelectedList)
{
	ClearCondition();
	mTypeCondWnd.InitTypeConditonList(NULL);
	mpSelectedList = pSelectedList;
	mSelectedItemWnd.InitSelectedItemList(mpSelectedList);
	mSelectedItemWnd.UpdateItemByCondition(mCondition);
	mTabGroup.SetSelectButton(&mSongNameBtn);
	mInputWnd.ClearInputText();
	mInputWnd.SetInputMethodType(INPUTMETHODTYPE_SPELL);
}

void CSetMusicLibWnd::OnInputPanelClick(UINT64 wParam)
{
	int idx = mTabGroup.GetSelectIndex();
	wchar_t *pText = (wchar_t *)wParam;
	CBaseStringA tmp ;
	tmp.Set(pText);

	switch ( idx )
	{
	case 0:
	{
		mSongNameBtn.SetWindowTextW(pText);
		if ( tmp.GetString() )
		{
			SAFE_STRNCPY(mCondition.cName, tmp.GetString(), sizeof(mCondition.cName));
		}
		else
		{
			memset(mCondition.cName, 0, sizeof(mCondition.cName));
		}
	}
		break;

	case 1:
	{
		mSingerBtn.SetWindowTextW(pText);
		if ( tmp.GetString() )
		{
			SAFE_STRNCPY(mCondition.cSinger, tmp.GetString(), sizeof(mCondition.cSinger));
		}
		else
		{
			memset(mCondition.cSinger, 0, sizeof(mCondition.cSinger));
		}
	}
		break;

	case 2:
		mLangTypeBtn.SetWindowTextW(pText);
		if ( tmp.GetString() )
		{
			SAFE_STRNCPY(mCondition.cLang, tmp.GetString(), sizeof(mCondition.cLang));
		}
		else
		{
			memset(mCondition.cLang, 0, sizeof(mCondition.cLang));
		}
		break;

	case 3:
		mSongTypeBtn.SetWindowTextW(pText);
		if ( tmp.GetString() )
		{
			SAFE_STRNCPY(mCondition.cType, tmp.GetString(), sizeof(mCondition.cType));
		}
		else
		{
			memset(mCondition.cType, 0, sizeof(mCondition.cType));
		}
		break;

	case 4:
	{
		mWdCtBtn.SetWindowTextW(pText);
		int iWdCt = 0;
		if ( tmp.GetString() )
			iWdCt = atoi(tmp.GetString());
		mCondition.wWdCt = iWdCt;
	}
		break;

	case 5:
	{
		mOrdertimersBtn.SetWindowTextW(pText);
		int iOrderTimers = 0;
		if ( tmp.GetString() )
			iOrderTimers = atoi(tmp.GetString());
		mCondition.wOrderTimes = iOrderTimers;
	}
		break;

	default:
		break;
	}

	mSelectedItemWnd.UpdateItemByCondition(mCondition);
}

void CSetMusicLibWnd::OnTypeItemClick(void)
{
	int idx = mTabGroup.GetSelectIndex();
	char *pText = mTypeCondWnd.GetTypeCondition();

	switch ( idx )
	{
	case 2:
	{
		mLangTypeBtn.SetWindowTextA(pText);
		if ( pText )
		{
			SAFE_STRNCPY(mCondition.cLang, pText, sizeof(mCondition.cLang));
		}
		else
		{
			memset(mCondition.cLang, 0, sizeof(mCondition.cLang));
		}
	}
		break;

	case 3:
	{
		mSongTypeBtn.SetWindowTextA(pText);
		if ( pText )
		{
			SAFE_STRNCPY(mCondition.cType, pText, sizeof(mCondition.cType));
		}
		else
		{
			memset(mCondition.cType, 0, sizeof(mCondition.cType));
		}
	}
		break;
	}

	mSelectedItemWnd.UpdateItemByCondition(mCondition);
	mInputWnd.ClearInputText();
}


void CSetMusicLibWnd::ClearCondition()
{
	memset(&mCondition, 0, sizeof(mCondition));
	mSongNameBtn.SetWindowTextW(NULL);
	mSingerBtn.SetWindowTextW(NULL);
	mLangTypeBtn.SetWindowTextA(NULL);
	mSongTypeBtn.SetWindowTextA(NULL);
	mOrdertimersBtn.SetWindowTextW(NULL);
	mWdCtBtn.SetWindowTextW(NULL);
}

