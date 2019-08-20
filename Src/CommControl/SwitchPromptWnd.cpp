#include "SwitchPromptWnd.h"
#include "XMLParser.h"
#include "GetResParentPath.h"

#define PROMPT_TIMERID			0x01
#define PROMPT_TIME_OUT		5000	// 5s

void CSwitchPromptWnd::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 0, LAYOUT_WIDTH, LAYOUT_HEIGHT};

	SetZorderNumber(WINDOWS_ZORDER_MOUSETEXTURE+8);
	CreateStatic(
		pParent,
		rc,
		WINDOWFLAG_MANUALSETZORDER|WINDOWFLAG_POPUP,
		WINDOWSTATE_INVISIBLE);

	LoadResource();
}

void CSwitchPromptWnd::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cParentName[MAX_PATH] = {0};
	char cFileName[MAX_PATH];

	GetXmlBasePath(cParentName, sizeof(cParentName));
	CombinePathName(cFileName, cParentName, "MainCtrlPage.xml");

	if( !parser.Load(cFileName) )
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}
	// read from xml

	RECT rcShow;
	char imgPath[64] = {0};
	CTexture bkTexture;

	XmlLoadRect(&parser, "SwitchPromptWndInfo", &rcShow);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("SwitchPromptWndInfo", "path", "SwitchPrompt"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);
	CParentClass::MoveWindow(&rcShow);

}

void CSwitchPromptWnd::OnTimer(int nTimerID)
{
	CParentClass::OnTimer(nTimerID);
	if ( nTimerID == PROMPT_TIMERID )
	{
		DelTimer(nTimerID);
		SetWindowVisible(FALSE);
	}
}

void CSwitchPromptWnd::SwitchPrompt(void)
{
	SetWindowVisible(TRUE);
	AddTimer(PROMPT_TIMERID, PROMPT_TIME_OUT);
}
