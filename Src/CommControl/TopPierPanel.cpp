#include "TopPierPanel.h"
#include "BaseApp.h"
#include "XMLParser.h"
#include "UserMsgDefine.h"
#include "MainCtrlPage.h"

#include "../PlayerApi/PlayerInfo.h"

#define PLAYING_PROMPT_MSG		"正在播放:"
#define NEXT_PROMPT_MSG				"下一首:"
extern CMainCtrlPage *gMainCtrlPage;

void CTopPierPanel::Create(	CBaseWnd *pParent)
{
	RECT rcShow;
	RECT rcHide;
	SetRectXY(&rcShow, 2, 0, 1276, 74);
	SetRectXY(&rcHide, 2, -74, 1276, 74);
	CParentClass::Create(
		"TopPier",
		pParent,
		WINDOWFLAG_DEFAULT,
		rcShow,
		rcHide,
		FALSE);

	RECT rcControl = {10, 0, 500, 30};

	mPlayNoticeWnd.CreateStatic(this, rcControl);
	mPlayNoticeWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

   /*
   for (int i = 0; i < BTNTOPTYPE_COUNDT; i++ )
	{

		mTopItem[i].Create(this, rcControl, BUTTONTYPE_NORMAL, false, PRESSED_SCALE);
		mTopItem[i].SetWndID(BTN_SEARCH_ID+i);
		mTopItem[i].SetOnClickListener(this);
	}
    */
    	mTopItem[BTN_PLAYINGBG_ID].CreateStatic(this, rcControl);

	LoadResource();
}

void CTopPierPanel::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;
	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

  RECT rcControl;
	char imgPath[64] = {0};
	CTexture btnTextures[4];
	int size;
  
	CombinePathName(cFileName, cXmlFolder, "TopPierlPanel.xml");
	if ( false == parser.Load(cFileName) )
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcShow;
	RECT rcHide;
	XmlLoadRect(&parser, "ShowWindowLocation", &rcShow);
	XmlLoadRect(&parser, "HideWindowLocation", &rcHide);
	CParentClass::MoveWindow(rcShow, rcHide);

	XmlLoadRect(&parser, "PlayNoticeWndBg", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("PlayNoticeWndBg", "path", "TopPier/playinginfo"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mTopItem[BTN_PLAYINGBG_ID].SetTextures(btnTextures);
	mTopItem[BTN_PLAYINGBG_ID].MoveWindow(&rcControl);

	XmlLoadRect(&parser, "PlayNoticeWndInfo", &rcControl);
	size = parser.GetIntValue("PlayNoticeWndInfo", "size", 16);
	mPlayNoticeWnd.SetFontSize(size);
	mPlayNoticeWnd.MoveWindow(&rcControl);

  #if 0
  XmlLoadRect(&parser, "CloudDownBntInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("CloudDownBntInfo", "path", "TopPier/CloudDown"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mTopItem[BTN_CLOUDDOWN_ID].SetTextures(btnTextures);
	mTopItem[BTN_CLOUDDOWN_ID].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "PhoneSongBntInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("PhoneSongBntInfo", "path", "TopPier/PhoneSong"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mTopItem[BTN_PHONESONG_ID].SetTextures(btnTextures);
	mTopItem[BTN_PHONESONG_ID].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "WifiBntInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("WifiBntInfo", "path", "TopPier/Wifi"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mTopItem[BTN_WIFI_ID].SetTextures(btnTextures);
	mTopItem[BTN_WIFI_ID].MoveWindow(&rcControl);
  #endif
  //XmlLoadRect(&parser, "UsbSongBntInfo", &rcControl);
	//SAFE_STRNCPY(imgPath, parser.GetStrValue("UsbSongBntInfo", "path", "TopPier/UsbSong"), sizeof(imgPath));
  //CreateBtnImgTextures(imgPath, btnTextures);
  //mTopItem[BTN_USBSONG_ID].SetTextures(btnTextures);
	//mTopItem[BTN_USBSONG_ID].MoveWindow(&rcControl);
  
}

void CTopPierPanel::OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam)
{
	switch (uType)
	{
	case PLAYERINFO_SONGSTATE_CHANGE:
		RefreshPlayNotice();
		break;
	}
}

void CTopPierPanel::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return ;
	int nWndID = pWnd->GetWndID();
	
    switch ( nWndID )
		{

    #if 0
    case BTN_CLOUDDOWN_ID:
			if ( gMainCtrlPage->GetCurFragmentType() != Fragment_Download )
				gMainCtrlPage->SetCurrentFragment(Fragment_Download);
			break;

		case BTN_PHONESONG_ID:
			gMainCtrlPage->mDlnaWnd.SetWindowVisible(TRUE);
			break;
		case  BTN_WIFI_ID:
			gMainCtrlPage->mBarCodeWnd.SetWindowVisible(TRUE);
      break;
   // case  BTN_USBSONG_ID:
    //  if ( gMainCtrlPage )
			//		gMainCtrlPage->SetCurrentFragment(Fragment_UDisk);
    //  break;
    #endif
    case  BTN_PLAYINGBG_ID:
      gMainCtrlPage->SetPopuWndVisible(login_item, TRUE, CBaseLoginWnd::type_shutdown);
      break;  
 
		default:
			break;
		}
    
 }

void CTopPierPanel::RefreshPlayNotice(void)
{
	SONGINFO *pPlayingInfo = gPlayerInfo->GetCurPlaySongInfo();
	SONGINFO *pNextPlayInfo = gPlayerInfo->GetSelectedSongInfoByIndex(0);
	CBaseStringA temp;

	if ( !pPlayingInfo || strlen(pPlayingInfo->cID) <= 0 )
	{
		mPlayNoticeWnd.SetWindowTextA(PLAYING_PROMPT_MSG);
		return ;
	}

	temp.Format("%s%s", PLAYING_PROMPT_MSG, pPlayingInfo->cName);
	if ( pNextPlayInfo && strlen(pNextPlayInfo->cID) > 0 )
	{
		char strNextPlay[256];
		snprintf(strNextPlay, sizeof(strNextPlay), "  %s%s", NEXT_PROMPT_MSG, pNextPlayInfo->cName);
		temp.Append(strNextPlay);
	}

	mPlayNoticeWnd.SetWindowTextA(temp.GetString());
}
