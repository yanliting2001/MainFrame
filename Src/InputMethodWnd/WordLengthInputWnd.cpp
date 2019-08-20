#include "WordLengthInputWnd.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"

#define SONGNAMELENGTHSEARCHBTN_CLICKID_BASE	1

void CWordLengthInputWnd::Create(CBaseWnd *pParent, RECT rc)
{
	CParentClass::Create("SongNameLengthInput", pParent, rc, WINDOWFLAG_DEFAULT, WINDOWSTATE_INVISIBLE);

	RECT rcControl = {0, 0, 70, 70};
	char text[8] = {0};
	for ( int i = 0 ; i < WordLength_TypeCount ; i++ )
	{
		mLengthBtns[i].Create(this, rcControl, BUTTONTYPE_RADIO);
		mLengthBtns[i].SetWndID(SONGNAMELENGTHSEARCHBTN_CLICKID_BASE+i);
		mLengthBtns[i].SetOnTouchListener(this);
		mLengthBtns[i].SetOnClickListener(this);
		mLengthGroup.AddToGroup(&(mLengthBtns[i]));

		if ( i == WordLength_All )
			mLengthBtns[i].SetWindowTextA("全");
		else
		{
			snprintf(text, sizeof(text), "%d", i+1);
			mLengthBtns[i].SetWindowTextA(text);
		}
	}

	SetWordLengthType(WordLength_All);

	LoadResource();
}

void CWordLengthInputWnd::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "WordLengthInputWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT  rcControl, rcBase;
	CTexture btnTextures[4];
	char imgPath[64] = {0};
	char colorbuf[16];
	int size, columnNum, xOffset, yOffset;
	UINT32 color;

	XmlLoadRect(&parser, "LengthItemBtnInfo", &rcBase);
	size = parser.GetIntValue("LengthItemBtnInfo", "size", 26);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("LengthItemBtnInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	columnNum = parser.GetIntValue("LengthItemBtnInfo", "columns", 6);
	xOffset = parser.GetIntValue("LengthItemBtnInfo", "xOffset", 4);
	yOffset = parser.GetIntValue("LengthItemBtnInfo", "yOffset", 63);
	SAFE_STRNCPY(imgPath,parser.GetStrValue("LengthItemBtnInfo", "path", "InputWnd/Region"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);

	int i, j, k;
	for ( i = 0 ; i < WordLength_TypeCount; i++ )
	{
		j = i % columnNum;
		k = i / columnNum;
		InflatRect(&rcControl, &rcBase, j*xOffset, k*yOffset);
		mLengthBtns[i].SetFontSize(size);
		mLengthBtns[i].SetFontColor(color);
		mLengthBtns[i].SetTextures(btnTextures);
		mLengthBtns[i].MoveWindow(&rcControl);
	}
}

void CWordLengthInputWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	int nWndID = pWnd->GetWndID();
	int nClickID = nWndID-SONGNAMELENGTHSEARCHBTN_CLICKID_BASE;

	if (nClickID < 0 || nClickID >= WordLength_TypeCount)
	{
		return;
	}

	SetWordLengthType((WORDLENGTHTYPE)nClickID);

	ReportMsg(INPUTWND_WORDLENGTHMSG, nClickID, 0);
}

void CWordLengthInputWnd::OnTouchDown(
	CBaseWnd *pWnd,
	POINT pt)
{
	int nWndID = pWnd->GetWndID();
	int nClickID = nWndID-SONGNAMELENGTHSEARCHBTN_CLICKID_BASE;
	RefreshItemText(nClickID);
}

void CWordLengthInputWnd::OnTouchUp(
	CBaseWnd *pWnd,
	POINT pt,
	int xDistance,
	int yDistance,
	UINT64 uUsedTime)
{
}

void CWordLengthInputWnd::OnTouchLeave(
	CBaseWnd *pOldTouchWnd,
	CBaseWnd *pNewTouchWnd)
{

}

void CWordLengthInputWnd::SetWordLengthType(	WORDLENGTHTYPE wordLengthType)
{
	mLengthGroup.SetSelectButton(wordLengthType);
}

WORDLENGTHTYPE CWordLengthInputWnd::GetWordLengthType()
{
	return (WORDLENGTHTYPE)mLengthGroup.GetSelectIndex();
}

void CWordLengthInputWnd::RefreshItemText(int clickIdx)
{
	mLengthBtns[clickIdx].SetWindowVisible(FALSE);
	mLengthBtns[clickIdx].SetWindowVisible(TRUE);
}
