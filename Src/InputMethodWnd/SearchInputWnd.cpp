#include "SearchInputWnd.h"
#include "BaseApp.h"
#include "UserMsgDefine.h"
#include "XMLParser.h"
#include "../GlobalUIClass.h"
#include "FontCodeConvApi.h"

#define SEARCHBTN_CLICKID	1
#define TEXTEDIT_CLICKID	2

CSearchInputWnd::CSearchInputWnd()
{  
	mbShowPromptTextShown = FALSE;
	mwszStrokeNumber = NULL;
	SetTextMaxLength(20);
}

CSearchInputWnd::~CSearchInputWnd()
{
	SAFE_DELETEARRAY(mwszStrokeNumber);

	DbgOutput(DBG_INFO, "function %s quit\n", __FUNCTION__);
}

void CSearchInputWnd::Create(
	CBaseWnd *pParent,
	RECT rc,
	RECT rcSearchBtn,
	RECT rcTextEdit,
	RECT rcInputPanel,
	BOOL bInitShowInput,
	BOOL bPopup)
{
	CParentClass::Create("SearchInput", pParent, rc, WINDOWFLAG_DEFAULT, WINDOWSTATE_INVISIBLE);

	mInputMethodPanel.Create(this, rcInputPanel, bInitShowInput, bPopup);
	mInputMethodPanel.SetInputRecvWnd(this);

	mSearchBtn.Create(this, rcSearchBtn);
	mSearchBtn.SetWndID(SEARCHBTN_CLICKID);
	mSearchBtn.SetOnClickListener(this);

	mTextEdit.CreateStatic(this, rcTextEdit);
	mTextEdit.SetWndID(TEXTEDIT_CLICKID);
	mTextEdit.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);
	mTextEdit.SetFontColor(0xFF000000);
	mTextEdit.SetOnClickListener(this);

	ShowPromptText(TRUE);

	LoadResource();
}

void CSearchInputWnd::MoveWindow(
	RECT rc,
	RECT rcSearchBtn,
	RECT rcTextEdit,
	RECT rcInputPanel)
{
	CParentClass::MoveWindow(&rc);
	mSearchBtn.MoveWindow(&rcSearchBtn);
	mTextEdit.MoveWindow(&rcTextEdit);

	RECT rcShow;
	RECT rcHide;
	CopyRect(&rcShow, &rcInputPanel);
	CopyRect(&rcHide, &rcInputPanel);
	rcHide.top = LAYOUT_HEIGHT;
	mInputMethodPanel.MoveWindow(rcShow, rcHide);
	mInputMethodPanel.LoadResource();
}

void CSearchInputWnd::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "InputPanel.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml

	CTexture bkTexture;
	char imgPath[64] = {0};

	strcpy(imgPath, parser.GetStrValue("InputEditImg","path","InputWnd/InputEdit"));
	CreateImgTexture(imgPath, &bkTexture);
	SetBackgroundTexture(&bkTexture);

	CTexture btnTextures[4];
	strcpy(imgPath, parser.GetStrValue("SearchImg","path","InputWnd/Search"));
	CreateBtnImgTextures(imgPath, btnTextures);
	mSearchBtn.SetTextures(btnTextures);

	char colorbuf[16];
	strcpy(colorbuf,parser.GetStrValue("TextColor","color","FF0000FF"));
	mTextColor = strtoul(colorbuf,NULL,16);
	mTextEdit.SetFontColor(mTextColor);

	strcpy(colorbuf,parser.GetStrValue("TextColor","promptColor","FF0000FF"));
	mPromptColor = strtoul(colorbuf,NULL,16);

	int size = parser.GetIntValue("TextColor","size",25);
	mTextEdit.SetFontSize(size);
}

void CSearchInputWnd::OnMsg(
	UINT32 uType,
	UINT64 wParam,
	UINT64 lParam)
{
	CParentClass::OnMsg(uType, wParam, lParam);

	switch(uType)
	{
	case INPUTWND_CHARMSG:
		OnInputChar((wchar_t)wParam);
		break;

	case INPUTWND_BACKMSG:
		OnBackText();
		break;

	case INPUTWND_CLEARMSG:
		OnClearText();
		break;

	case INPUTWND_WORDLENGTHMSG:
		OnSetWordLength();
		break;

	case INTERNETTV_MEDIA_CLOSE:
		mInputMethodPanel.SlideShowWindow(FALSE);
		break;
	}
}

void CSearchInputWnd::OnClick(
	CBaseWnd *pWnd,
	POINT pt)
{
	int nWndID = pWnd->GetWndID();

	switch(nWndID)
	{
	case SEARCHBTN_CLICKID:
	case TEXTEDIT_CLICKID:
		if (mInputMethodPanel.IsInputPanelPopup())
		{
			if ( mInputMethodPanel.IsWindowVisible() )
			{
				mInputMethodPanel.SlideShowWindow(FALSE);
			}
			else
			{
				CHILDPAGE_FRAGMENT_TYPE fragmentType = gMainCtrlPage->GetCurFragmentType();
				if (  fragmentType == Fragment_HomePage ||
						fragmentType == Fragment_AlbumSong ||
						fragmentType == Fragment_LangSong  ||
						fragmentType == Fragment_Settings ||
						fragmentType == Fragment_Selected )
				{
					gSongListFragment->SetSearchBarItem(&gSearchSpell, 0);
					gMainCtrlPage->SetCurrentFragment(Fragment_SongList);
				}
				else if ( fragmentType == Fragment_SingerType )
				{
					gSingerListFragment->SetSearchBarItem(&gSearchSingerType, 0);	
					gMainCtrlPage->SetCurrentFragment(Fragment_SingerList);
				}
//				ShowPromptText(TRUE);
				mInputMethodPanel.SetWordLengthType(WordLength_All);
				mInputMethodPanel.SlideShowWindow(TRUE);
			}
		}
		break;

	default:
		break;
	}
}

void CSearchInputWnd::SetInputWndVisible(BOOL IsVisible)
{
	if(IsVisible)
	{
		if (mInputMethodPanel.IsInputPanelPopup())
		{
			mInputMethodPanel.SlideShowWindow(TRUE);
			//mInputMethodPanel.SetAutoHideWhenInactive(FALSE);
		}
	}
	else
	{
		if (mInputMethodPanel.IsInputPanelPopup())
		{
			if ( mInputMethodPanel.IsWindowVisible() )
			{
				mInputMethodPanel.SlideShowWindow(FALSE);
				mInputMethodPanel.SetAutoHideWhenInactive(TRUE);
			}
		}
	}
}

void CSearchInputWnd::SetPromptText(
	const wchar_t* wszPromptText)
{
	if (!wszPromptText)
	{
		return;
	}

	mPromptText.Set(wszPromptText);
	if (mbShowPromptTextShown)
	{
		mTextEdit.SetWindowTextW(mPromptText.GetString());
	}
}

void CSearchInputWnd::SetTextMaxLength(
	int nTextMaxLength)
{
	mTextMaxLength = nTextMaxLength;

	SAFE_DELETEARRAY(mwszStrokeNumber);
	mwszStrokeNumber = new wchar_t[mTextMaxLength+1];
}

void CSearchInputWnd::SetInputMethodTypeEnable(
	INPUTMETHODTYPE inputmethodtype,
	BOOL bEnable)
{ 
	mInputMethodPanel.SetInputMethodTypeEnable(inputmethodtype, bEnable);
}	

void CSearchInputWnd::GetSearchInfo(
	SEARCHINFO *pSearchInfo)
{
	if (!pSearchInfo)
	{
		return;
	}

	const wchar_t *wszText = L"";
	if (!mbShowPromptTextShown)
	{
		wszText = mTextEdit.GetWindowTextW();
	}

	if (mInputMethodPanel.GetInputMethodType() == INPUTMETHODTYPE_STROKES)
	{
		ConvertStrokeToNumber(wszText, mwszStrokeNumber);

		wszText = mwszStrokeNumber;
	}

	CBaseStringW string;
	CBaseStringW tmp;
	//string.Set(wszText);
	wchar_t FontConv[128] = {0};


	if ( theBaseApp->GetLanguageID() == LanguageID_CHT)
	{
		tmp.Set(wszText);
		//Traditional2Simplified(&tmp, &string);
		Traditional2Simplified(tmp.GetString(), tmp.GetLength(), FontConv, 127);
		string.Set(FontConv);
	}
	else
	{
		string.Set(wszText);
	}
	CBaseStringA s;
	s.Set(string.GetString());
	SAFE_STRNCPY(pSearchInfo->cInputContent, s.GetString(), 64);
	pSearchInfo->eInputMethodType = mInputMethodPanel.GetInputMethodType();
	pSearchInfo->eWordLengthType = mInputMethodPanel.GetWordLengthType();
}

void CSearchInputWnd::ResetSearchInfo()
{
	ShowPromptText(TRUE);

	//mInputMethodPanel.SetInputMethodType(INPUTMETHODTYPE_HANDWRITE);
	mInputMethodPanel.SetInputMethodType(INPUTMETHODTYPE_SPELL);
	mInputMethodPanel.SetWordLengthType(WordLength_All);
}

void CSearchInputWnd::OnInputChar(
	wchar_t wszChar)
{
	if (mbShowPromptTextShown)
	{
		ShowPromptText(FALSE);
	}
		
	const wchar_t *wszOldText = mTextEdit.GetWindowTextW();
	CBaseStringW string;
	string.Set(wszOldText);

	if (string.GetLength() >= mTextMaxLength)
	{
		return;
	}

	string.Append(wszChar);

	mTextEdit.SetWindowTextW(string.GetString());

	DbgOutput(DBG_DEBUG, "Search Input Text is (%ls)\n", string.GetString());

	ReportMsg(INPUTWND_SEARCHMSG, 0, 0);
}

void CSearchInputWnd::OnBackText()
{
	if(mbShowPromptTextShown)
	{
		return ;
	}
	const wchar_t *wszOldText = mTextEdit.GetWindowTextW();
	CBaseStringW string;
	string.Set(wszOldText);
	string.Trunk(string.GetLength()-1); 

	mTextEdit.SetWindowTextW(string.GetString());

	if (string.GetLength() == 0)
	{
		if (!mbShowPromptTextShown)
		{
			ShowPromptText(TRUE);

			DbgOutput(DBG_DEBUG, "Search Input Text is (NULL)\n");

			ReportMsg(INPUTWND_SEARCHMSG, 0, 0);
		}
	}
	else
	{
		DbgOutput(DBG_DEBUG, "Search Input Text is (%s)\n", string.GetString());

		ReportMsg(INPUTWND_SEARCHMSG, 0, 0);
	}
}

void CSearchInputWnd::OnClearText()
{
	if (!mbShowPromptTextShown)
	{
		ShowPromptText(TRUE);

		ReportMsg(INPUTWND_SEARCHMSG, 0, 0);
	}
}

void CSearchInputWnd::OnSetWordLength()
{
	ReportMsg(INPUTWND_SEARCHMSG, 0, 0);
}

void CSearchInputWnd::ShowPromptText(
	BOOL bShow)
{
	mbShowPromptTextShown = bShow;
	if (bShow)
	{
		mTextEdit.SetWindowTextW(mPromptText.GetString());
		mTextEdit.SetFontColor(mPromptColor);
	}
	else 
	{
		mTextEdit.SetWindowTextW(NULL);
 		mTextEdit.SetFontColor(mTextColor);
	}
}

void CSearchInputWnd::ConvertStrokeToNumber(
	const wchar_t *wszStroke,
	wchar_t *wszNumber)
{
	int nLength = wcslen(wszStroke);

	int i;
	for ( i = 0 ; i < nLength; i++ )
	{
		if (wszStroke[i] == STROKE_CHAR_1)
		{
			wszNumber[i] = L'1';
		}
		else if (wszStroke[i] == STROKE_CHAR_2)
		{
			wszNumber[i] = L'2';
		}
		else if (wszStroke[i] == STROKE_CHAR_3)
		{
			wszNumber[i] = L'3';
		}
		else if (wszStroke[i] == STROKE_CHAR_4)
		{
			wszNumber[i] = L'4';
		}
		else if (wszStroke[i] == STROKE_CHAR_5)
		{
			wszNumber[i] = L'5';
		}
		else if (wszStroke[i] == STROKE_CHAR_6)
		{
			wszNumber[i] = L'?';
		}
		else
		{
			wszNumber[i] = L'1';
		}
	}

	wszNumber[i] = '\0';
}


void CSearchInputWnd::SetInputMethodType(INPUTMETHODTYPE inputmethodtype)
{
	mInputMethodPanel.SetInputMethodTypeByPinyin(inputmethodtype);
}

BOOL CSearchInputWnd::IsPopupSlide()
{
	return mInputMethodPanel.IsWindowVisible() ? TRUE : FALSE;
}
