#include "WaitingWnd.h"
#include "../GlobalUIClass.h"

CWaitingWnd::CWaitingWnd()
{

}

CWaitingWnd::~CWaitingWnd()
{
	DbgOutput(DBG_INFO, "function %s quit\n", __FUNCTION__);
}

void CWaitingWnd::Create(
	CBaseWnd *pParent)
{
	RECT rc = {0, 0, LAYOUT_WIDTH, LAYOUT_HEIGHT};

	SetZorderNumber(WINDOWS_ZORDER_WAITINGWNDPOPUP - 30);
	CreateStatic(
		pParent,
		rc,
		WINDOWFLAG_MANUALSETZORDER|WINDOWFLAG_POPUP,
		WINDOWSTATE_INVISIBLE);

	RECT rcControl;
	SetRectXY(&rcControl, 240, 210, 800, 300);
	mWaitCursorWnd.SetAnimationSpeed(250);
	mWaitCursorWnd.Create(this, rcControl);

	LoadResource();
}

void CWaitingWnd::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "WaitingWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml

	RECT rcControl;

	XmlLoadRect(&parser, "mainWnd", &rcControl);
	MoveWindow(&rcControl);

	XmlLoadRect(&parser, "cursorWnd", &rcControl);
	mWaitCursorWnd.MoveWindow(&rcControl);

	CTexture bkTexture;
	char imgPath[64] = {0};

	strcpy(imgPath, parser.GetStrValue("MaskImg","path","Mask"));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);

	strcpy(imgPath, parser.GetStrValue("WaitingImg","path","Waiting"));

	mWaitCursorWnd.StopAnimation();
	mWaitCursorWnd.RemoveAllTexture();
	int i;
	for ( i = 1 ; i <= 9 ; i++ )
	{
		sprintf(cFileName, "%s/%d",imgPath,i);
		CreateImgTexture(cFileName, &bkTexture);
		mWaitCursorWnd.AddAnimationTexture(&bkTexture);
	}
}

void CWaitingWnd::LoadResourceForMovieControl()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "MovieControlWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml

	RECT rcControl;

	XmlLoadRect(&parser, "mainWnd", &rcControl);
	MoveWindow(&rcControl);

	XmlLoadRect(&parser, "cursorWnd", &rcControl);
	mWaitCursorWnd.MoveWindow(&rcControl);

	CTexture bkTexture;
	char imgPath[64] = {0};

	strcpy(imgPath, parser.GetStrValue("MaskImg","path","Mask"));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);

	strcpy(imgPath, parser.GetStrValue("WaitingImg","path","Waiting"));

	mWaitCursorWnd.StopAnimation();
	mWaitCursorWnd.RemoveAllTexture();
	int i;
	for ( i = 1 ; i <= 4 ; i++ )
	{
		sprintf(cFileName, "%s/%d",imgPath,i);
		CreateImgTexture(cFileName, &bkTexture);
		mWaitCursorWnd.AddAnimationTexture(&bkTexture);
	}
}

void CWaitingWnd::StartWaiting()
{
	SetWindowVisible(TRUE);
	OnRedraw(mWndRect);

	mWaitCursorWnd.StartAnimation();
}

void CWaitingWnd::StopWaiting()
{
	mWaitCursorWnd.StopAnimation();
	SetWindowVisible(FALSE);
}
