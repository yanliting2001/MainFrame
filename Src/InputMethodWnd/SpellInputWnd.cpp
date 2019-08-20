#include "SpellInputWnd.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"

#define BTN_REPUT_ID			0x30
#define BTN_SLIDE_ID				0x31
#define BTN_CLOSE_ID			0x32

#define ITEM_PRESSED_SCALE 0.9

void CSpellInputWnd::Create(CBaseWnd *pParent, RECT rc)
{
	CParentClass::Create("SpellInput", pParent, rc);

	RECT rcControl = {0, 0, 68, 68};
	int i;
	for ( i = 0 ; i < INPUTID_SPELL_COUNT; i++ )
	{
		mSpellItemWnd[i].Create(this, rcControl, BUTTONTYPE_NORMAL, true, ITEM_PRESSED_SCALE);
		mSpellItemWnd[i].SetWndID(INPUTID_SPELL_BASE + i);
		mSpellItemWnd[i].SetOnClickListener(this);
	}

	mReputBtn.Create(this, rcControl, BUTTONTYPE_NORMAL, true, ITEM_PRESSED_SCALE);
	mReputBtn.SetWndID(BTN_REPUT_ID);
	mReputBtn.SetOnClickListener(this);

	mSlideBtn.Create(this, rcControl, BUTTONTYPE_NORMAL, true, ITEM_PRESSED_SCALE);
	mSlideBtn.SetWndID(BTN_SLIDE_ID);
	mSlideBtn.SetOnClickListener(this);

	mCloseBtn.Create(this, rcControl, BUTTONTYPE_NORMAL, true, ITEM_PRESSED_SCALE);
	mCloseBtn.SetWndID(BTN_CLOSE_ID);
	mCloseBtn.SetOnClickListener(this);

	LoadResource();
}

void CSpellInputWnd::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "SpellInputWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml

	RECT  rcControl, rcBase;
	CTexture btnTextures[4];
	char imgPath[64] = {0};
	int xOffset;

	XmlLoadRect(&parser, "SpellATOJBtnInfo", &rcBase);
	xOffset = parser.GetIntValue("SpellATOJBtnInfo", "xOffset", 4);
	SAFE_STRNCPY(imgPath,parser.GetStrValue("SpellATOJBtnInfo", "path", "InputWnd/Region"), sizeof(imgPath));

	int i;
	for ( i = 0 ; i < 10; i++ )
	{
		char tmpImg[64] = {0};
		snprintf(tmpImg, sizeof(tmpImg), "%s/%c", imgPath, 'A'+i);
		CreateBtnImgTextures(tmpImg, btnTextures);

		InflatRect(&rcControl, &rcBase, i*xOffset, 0);
		mSpellItemWnd[i].SetTextures(btnTextures);
		mSpellItemWnd[i].MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "SpellKTOSBtnInfo", &rcBase);
	for ( i = 10 ; i < 19; i++ )
	{
		char tmpImg[64] = {0};
		snprintf(tmpImg, sizeof(tmpImg), "%s/%c", imgPath, 'A'+i);
		CreateBtnImgTextures(tmpImg, btnTextures);

		InflatRect(&rcControl, &rcBase, (i-10)*xOffset, 0);
		mSpellItemWnd[i].SetTextures(btnTextures);
		mSpellItemWnd[i].MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "SpellTTOZBtnInfo", &rcBase);
	for ( i = 19 ; i < 26; i++ )
	{
		char tmpImg[64] = {0};
		snprintf(tmpImg, sizeof(tmpImg), "%s/%c", imgPath, 'A'+i);
		CreateBtnImgTextures(tmpImg, btnTextures);

		InflatRect(&rcControl, &rcBase, (i-19)*xOffset, 0);
		mSpellItemWnd[i].SetTextures(btnTextures);
		mSpellItemWnd[i].MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "ReputBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("ReputBtnInfo", "path", "InputWnd/Clear"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mReputBtn.SetTextures(btnTextures);
	mReputBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "SlideBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("SlideBtnInfo", "path", "InputWnd/Clear"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mSlideBtn.SetTextures(btnTextures);
	mSlideBtn.MoveWindow(&rcControl);

	XmlLoadRect(&parser, "CloseBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("CloseBtnInfo", "path", "InputWnd/Clear"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mCloseBtn.SetTextures(btnTextures);
	mCloseBtn.MoveWindow(&rcControl);
}

void CSpellInputWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	int nWndID = pWnd->GetWndID();
	if ( nWndID == BTN_CLOSE_ID )
	{
		ReportMsg(INPUTWND_BACKMSG, 0, 0);
	}
	else if ( nWndID == BTN_REPUT_ID )
	{
		ReportMsg(INPUTWND_CLEARMSG, 0, 0);
	}
	else if ( nWndID == BTN_SLIDE_ID )
	{
		ReportMsg(INTERNETTV_MEDIA_CLOSE, 0, 0);
	}
	else if ( nWndID >= INPUTID_SPELL_BASE && nWndID < (INPUTID_SPELL_BASE + INPUTID_SPELL_COUNT) )
	{
		int nClickID = nWndID - INPUTID_SPELL_BASE;
		wchar_t itemText = L'A' + nClickID;
		ReportMsg(INPUTWND_CHARMSG, (UINT64)itemText,	0);
	}
}
