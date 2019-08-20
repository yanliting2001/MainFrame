#include "ServerWnd.h"
#include "BaseApp.h"
#include "XMLParser.h"

void CServerWnd::Create(CBaseWnd *pParent)
{
	RECT rcShow;
	RECT rcHide;
	SetRectXY(&rcShow, 300, 200, 525, 370);
	SetRectXY(&rcHide, 300, -370, 525, 370);

	SetZorderNumber(WINDOWS_ZORDER_MOUSETEXTURE+16);
	CParentClass::Create(
		"ServerWnd",
		pParent,
		WINDOWFLAG_MANUALSETZORDER,
		rcShow,
		rcHide,
		FALSE,
		TRUE);

	LoadResource();
}

void CServerWnd::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "ServerWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcShow, rcHide;
	char imgPath[64] = {0};
	CTexture bkTexture;

	XmlLoadRect(&parser, "ShowWindowInfo", &rcShow);
	XmlLoadRect(&parser, "HideWindowInfo", &rcHide);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ShowWindowInfo", "path", "RepayWnd/RepayBKGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);
	CParentClass::MoveWindow(rcShow, rcHide);

}
