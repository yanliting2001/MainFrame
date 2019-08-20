#include "PromptMsg.h"
#include "BaseApp.h"
#include "XMLParser.h"
#include "MainCtrlPage.h"

#define BTN_PROMPT_CLOSE_ID		0x03

extern CMainCtrlPage *gMainCtrlPage;

void CPromptMsg::Create(CBaseWnd *pParent)
{
	RECT rcShow;
	RECT rcHide;
	SetRectXY(&rcShow, 300, 200, 525, 370);
	SetRectXY(&rcHide, 300, -370, 525, 370);

	SetZorderNumber(WINDOWS_ZORDER_AUTOHIDEPOPUP - 30);
	CParentClass::Create(
		"PromptMsgWnd",
		pParent,
		WINDOWFLAG_MANUALSETZORDER,
		rcShow,
		rcHide,
		FALSE,
		TRUE);

	RECT rcControl = {50, 36, 100, 100};
  
	mPromptInfo.CreateStatic(this, rcControl);


  mClose.Create(this, rcControl);
  mClose.SetWndID(BTN_PROMPT_CLOSE_ID);
  mClose.SetOnClickListener(this);

	LoadResource();
}

void CPromptMsg::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "PromptMsg.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	RECT rcControl;
	CTexture bkTexture, btnTextures[4];
	char colorbuf[16];
	unsigned long color;
	int size;
  char imgPath[64] = {0};
  RECT rcShow, rcHide;
  
  XmlLoadRect(&parser, "ShowWindowInfo", &rcShow);
	XmlLoadRect(&parser, "HideWindowInfo", &rcHide);
  SAFE_STRNCPY(imgPath, parser.GetStrValue("ShowWindowInfo", "path", "BkGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);
  CParentClass::MoveWindow(rcShow, rcHide);

  XmlLoadRect(&parser, "CloseBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("CloseBtnInfo", "path", "LeftPier/Close"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mClose.SetTextures(btnTextures);
	mClose.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "PromptInfoWndInfo", &rcControl);
  size = parser.GetIntValue("PromptInfoWndInfo", "size", 30);
	strcpy(colorbuf,parser.GetStrValue("PromptInfoWndInfo", "color", "FFFFFFFF"));
	color = strtoul(colorbuf, NULL,16);
	mPromptInfo.SetFontSize(size);
  mPromptInfo.SetFontColor(color);
  mPromptInfo.SetWindowTextA("云端歌曲正在下载中，稍后播放...");
	mPromptInfo.MoveWindow(&rcControl);
  
  //AddTimer(PromptType_TimeClose, 3000);
  
}

void CPromptMsg::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return ;

	int nWndID = pWnd->GetWndID();
  
	switch (nWndID)
	{

	case BTN_PROMPT_CLOSE_ID:
    gMainCtrlPage->mPromptMsgWnd.SlideShowWindow(FALSE);
		break;

	default:
		break;
	}

}

void CPromptMsg::SetPromptMsg(int Promptstype, int autotime)
{
  
	switch (Promptstype)
	{

  	case PromptType_TimeClose:
      
      mPromptInfo.SetWindowTextA("云端歌曲正在下载中，稍后播放...");
      
      if(!gMainCtrlPage->mPromptMsgWnd.IsWindowVisible())
      {
        AddTimer(PromptType_TimeClose, autotime);
        gMainCtrlPage->mPromptMsgWnd.SlideShowWindow(TRUE);
	    }
  		break;
    case PromptType_ManualClose:
      mPromptInfo.SetWindowTextA("云端歌曲正在下载中，稍后播放...");
      if(!gMainCtrlPage->mPromptMsgWnd.IsWindowVisible())
      gMainCtrlPage->mPromptMsgWnd.SlideShowWindow(TRUE);
      break;
  	default:
  		break;
	}

}

void CPromptMsg::SetPrompString(const char* cString)
{
  mPromptInfo.SetWindowTextA(cString);
  if(!gMainCtrlPage->mPromptMsgWnd.IsWindowVisible())
  gMainCtrlPage->mPromptMsgWnd.SlideShowWindow(TRUE);
}


void CPromptMsg::OnTimer(int nTimerID)
{
	CParentClass::OnTimer(nTimerID);
	if ( nTimerID == PromptType_TimeClose )
	{
    if(gMainCtrlPage->mPromptMsgWnd.IsWindowVisible())
    gMainCtrlPage->mPromptMsgWnd.SlideShowWindow(FALSE);
	}
}


