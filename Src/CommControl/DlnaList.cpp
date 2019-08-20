#include "DlnaList.h"
#include "BaseApp.h"
#include "XMLParser.h"
#include "InternetCtrl/NetCtrlApi.h"


#define BTN_DLNA_COUNT	  10

#define BTN_PUSHINFO_ID			0x01
#define BTN_PUSHITEM_ID		0x02
#define BTN_CLOSE_ID		0x03


void CDlnaList::Create(CBaseWnd *pParent)
{
	RECT rcShow;
	RECT rcHide;
	SetRectXY(&rcShow, 300, 200, 525, 370);
	SetRectXY(&rcHide, 300, -370, 525, 370);

	SetZorderNumber(WINDOWS_ZORDER_MOUSETEXTURE+18);
	CParentClass::Create(
		"DlnalistWnd",
		pParent,
		WINDOWFLAG_MANUALSETZORDER,
		rcShow,
		rcHide,
		FALSE,
		TRUE);

	RECT rcControl = {50, 36, 100, 100};
  
	//mPushInfo.CreateStatic(this, rcControl);

  mPushItem.CreateStatic(this, rcControl);

  mClose.Create(this, rcControl);
  mClose.SetWndID(BTN_CLOSE_ID);
  mClose.SetOnClickListener(this);

	LoadResource();
}

void CDlnaList::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "DlnaList.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	RECT rcControl;
	CTexture bkTexture, btnTextures[4];
	char str[64] = {0};
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


  XmlLoadRect(&parser, "PushItemWndInfo", &rcControl);
  size = parser.GetIntValue("PushItemWndInfo", "size", 30);
	strcpy(colorbuf,parser.GetStrValue("PushItemWndInfo", "color", "FFFFFFFF"));
	color = strtoul(colorbuf, NULL,16);
	mPushItem.SetFontColor(color);
	mPushItem.SetFontSize(size);

  snprintf(str, sizeof(str), "%s", get_dlna_name());
  mPushItem.SetWindowTextA(str);
	mPushItem.MoveWindow(&rcControl);

  
}

void CDlnaList::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return ;

	int nWndID = pWnd->GetWndID();
  
	switch (nWndID)
	{

	case BTN_CLOSE_ID:
    SlideShowWindow(FALSE);
		break;

	default:
		break;
	}

}


