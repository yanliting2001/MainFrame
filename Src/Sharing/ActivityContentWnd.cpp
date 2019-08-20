#include "ActivityContentWnd.h"
#include "XMLParser.h"
#include "BaseApp.h"
#include "../CommControl/MainCtrlPage.h"


#define BTN_CLOSE_ID		0x01

void CActivityContentWnd::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 0, LAYOUT_WIDTH, LAYOUT_HEIGHT};
	SetZorderNumber(WINDOWS_ZORDER_MOUSETEXTURE+7);

	CreateStatic(pParent,	rc,  WINDOWFLAG_MANUALSETZORDER|WINDOWFLAG_POPUP, WINDOWSTATE_INVISIBLE);
	RECT rcControl = {0, 0, 100, 100};
  SetOnClickListener(this);
	//mCloseBtn.Create(this, rcControl);
	//mCloseBtn.SetWndID(BTN_CLOSE_ID);
	//mCloseBtn.SetOnClickListener(this);

	LoadResource();
}

void CActivityContentWnd::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "ActivityContentWnd.xml");
	if ( false == parser.Load(cFileName) )
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}
	// read from xml

	RECT rcShow;
	RECT rcControl;
	char imgPath[64] = {0};
	CTexture bkTexture, btnTextures[4];

	XmlLoadRect(&parser, "ShopWndInfo", &rcShow);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ShopWndInfo", "path", "ActivityBKGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);
	CParentClass::MoveWindow(&rcShow);

  #if 0
	XmlLoadRect(&parser, "CloseBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("CloseBtnInfo", "path", "Close"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mCloseBtn.SetTextures(btnTextures);
	mCloseBtn.MoveWindow(&rcControl);
  #endif

}

void CActivityContentWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	SetWindowVisible(FALSE);
}

