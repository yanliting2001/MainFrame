#include "AddUDiskFileWnd.h"
#include "BaseApp.h"
#include "XMLParser.h"

#define BTN_BASE_PREV_ID	0x10
#define BTN_BASE_NEXT_ID		0x20

void CAddUDiskFileWnd::Create(CBaseWnd *pParent)
{
	RECT rc = {0, 50, LAYOUT_WIDTH, 600};
	CParentClass::Create(	"AddUDiskFileWnd", pParent, 	rc, WINDOWFLAG_DEFAULT, WINDOWSTATE_INVISIBLE);

	RECT rcControl = {50, 36, 100, 100};
	for ( int i = 0; i < item_count; i++ )
	{
		if ( i < item_nothing )
		{
			mItemList[i].itemBKGroundWnd.CreateStatic(this, rcControl);

			mItemList[i].titleWnd.CreateStatic(&mItemList[i].itemBKGroundWnd, rcControl);
			mItemList[i].titleWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

			mItemList[i].nameWnd.CreateStatic(&mItemList[i].itemBKGroundWnd, rcControl);
			mItemList[i].nameWnd.SetFontDirect(DRAWTEXT_DIRECT_LEFT|DRAWTEXT_DIRECT_VCENTER);

			if ( i >= item_lang )
			{
				mItemList[i].prevBtn.Create(&mItemList[i].itemBKGroundWnd, rcControl);
				mItemList[i].prevBtn.SetWndID(BTN_BASE_PREV_ID + i);
				mItemList[i].prevBtn.SetOnClickListener(this);

				mItemList[i].nextBtn.Create(&mItemList[i].itemBKGroundWnd, rcControl);
				mItemList[i].nextBtn.SetWndID(BTN_BASE_NEXT_ID + i);
				mItemList[i].nextBtn.SetOnClickListener(this);
			}
		}

		mItemList[i].fengexianWnd.CreateStatic(&mItemList[i].itemBKGroundWnd, rcControl);
	}

	LoadResource();
}

void CAddUDiskFileWnd::LoadResource()
{
	CParentClass::LoadResource();

	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "UDisk/AddUDiskFileWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcControl, rcBase ;
	CTexture bkTexture, btnTextures[4];
	char imgPath[64] = {0};
	char colorbuf[16];
	unsigned long color;
	int size, yOffset = 0;

	XmlLoadRect(&parser, "BaseItemInfo", &rcBase);
	size = parser.GetIntValue("BaseItemInfo", "size", 20);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("BaseItemInfo", "color", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);
	yOffset = parser.GetIntValue("BaseItemInfo", "yOffset", 20);

	SetFontSize(size);
	SetFontColor(0xFFFF0000);

	for ( int i = 0; i < item_count; i++ )
	{
		InflatRect(&rcControl, &rcBase, 0, i*yOffset);
		mItemList[i].itemBKGroundWnd.MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "TitleWndInfo", &rcControl);
	for ( int i = 0; i < item_nothing; i++ )
	{
		char tmp[32] = {0};
		sprintf(tmp, "text%d", i);

		mItemList[i].titleWnd.SetFontSize(size);
		mItemList[i].titleWnd.SetFontColor(color);
		mItemList[i].titleWnd.MoveWindow(&rcControl);
		mItemList[i].titleWnd.SetWindowTextA(parser.GetStrValue("TitleTextInfo", tmp, "未设置"));
	}

	XmlLoadRect(&parser, "NameWndInfo", &rcControl);
	for ( int i = 0; i < item_count; i++ )
	{
		mItemList[i].nameWnd.SetFontSize(size);
		mItemList[i].nameWnd.SetFontColor(color);
		mItemList[i].nameWnd.MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "PrevBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("PrevBtnInfo", "path", "LangType/Chinese"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	for ( int i = item_lang; i < item_count; i++ )
	{
		mItemList[i].prevBtn.SetTextures(btnTextures);
		mItemList[i].prevBtn.MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "NextBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("NextBtnInfo", "path", "LangType/Chinese"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	for ( int i = item_lang; i < item_count; i++ )
	{
		mItemList[i].nextBtn.SetTextures(btnTextures);
		mItemList[i].nextBtn.MoveWindow(&rcControl);
	}

	XmlLoadRect(&parser, "FengexianWndInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("FengexianWndInfo", "path", "DLNA/BkGround"), sizeof(imgPath));
	CreateImgTexture(imgPath, &bkTexture);
	for ( int i = 0; i < item_count; i++ )
	{
		mItemList[i].fengexianWnd.SetBackgroundTexture(&bkTexture);
		mItemList[i].fengexianWnd.MoveWindow(&rcControl);
	}

	for ( int i = 0; i < track_count; i++ )
	{
		char tmp[32] = {0};
		sprintf(tmp, "text%d", i);
		SAFE_STRNCPY(mTrackText[i], parser.GetStrValue("TrackTypeInfo", tmp, "声道"), sizeof(mTrackText[i]));
	}
}

void CAddUDiskFileWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd ) return ;
	int nWndID = pWnd->GetWndID();
	if ( nWndID >= BTN_BASE_PREV_ID && nWndID < (BTN_BASE_PREV_ID + item_count) )
	{
		OnPrevBtnClick(nWndID - BTN_BASE_PREV_ID);
	}
	else if ( nWndID >= BTN_BASE_NEXT_ID && nWndID < (BTN_BASE_NEXT_ID + item_count) )
	{
		OnNextBtnClick(nWndID - BTN_BASE_NEXT_ID);
	}
}

void CAddUDiskFileWnd::OnWindowVisible(BOOL bVisible)
{
	CParentClass::OnWindowVisible(bVisible);
	if ( !bVisible )
	{
		mpUdiskFileInfo = NULL;
		SetWindowTextA(NULL);
	}
}

void CAddUDiskFileWnd::SetAddUDiskFileInfo(UDISK_FILE_INFO_S *pFileInfo)
{
	mpUdiskFileInfo = pFileInfo;
	if ( mpUdiskFileInfo )
	{
		mItemList[item_file].nameWnd.SetWindowTextA(mpUdiskFileInfo->cFileName);
		mItemList[item_song].nameWnd.SetWindowTextA(mpUdiskFileInfo->cSongName);
		mItemList[item_spell].nameWnd.SetWindowTextA(mpUdiskFileInfo->cSpell);
		mItemList[item_singer].nameWnd.SetWindowTextA(mpUdiskFileInfo->cSinger);
		mItemList[item_lang].nameWnd.SetWindowTextA(mpUdiskFileInfo->cLang);
		mItemList[item_type].nameWnd.SetWindowTextA(mpUdiskFileInfo->cType);
		CBaseStringA tmp;
		tmp.Format("%d", mpUdiskFileInfo->wWdCt);
		mItemList[item_word].nameWnd.SetWindowTextA(tmp.GetString());
		mItemList[item_track].nameWnd.SetWindowTextA(mTrackText[mpUdiskFileInfo->wTrack%track_count]);

		miCurLangIdx = GetLangIdxByName(mpUdiskFileInfo->cLang);
		miCurTypeIdx = GetTypeIdxByName(mpUdiskFileInfo->cType);
	}
}

void CAddUDiskFileWnd::OnPrevBtnClick(int idx)
{
	switch ( idx )
	{
	case item_lang:
		if ( miCurLangIdx > 0 )
		{
			miCurLangIdx--;
			const char *lang = GetLangNameByIdx(miCurLangIdx);
			if ( lang )
			{
				mItemList[item_lang].nameWnd.SetWindowTextA(lang);
				SAFE_STRNCPY(mpUdiskFileInfo->cLang, lang, sizeof(mpUdiskFileInfo->cLang));
			}
		}
		break;

	case item_type:
		if ( miCurTypeIdx > 0 )
		{
			miCurTypeIdx--;
			const char *type = GetTypeNameByIdx(miCurTypeIdx);
			if ( type )
			{
				mItemList[item_type].nameWnd.SetWindowTextA(type);
				SAFE_STRNCPY(mpUdiskFileInfo->cType, type, sizeof(mpUdiskFileInfo->cType));
			}
		}
		break;

	case item_word:
		if ( mpUdiskFileInfo->wWdCt > 1 )
		{
			mpUdiskFileInfo->wWdCt--;
			CBaseStringA tmp;
			tmp.Format("%d", mpUdiskFileInfo->wWdCt);
			mItemList[item_word].nameWnd.SetWindowTextA(tmp.GetString());
		}
		break;

	case item_track:
		if ( mpUdiskFileInfo->wTrack > track_left )
		{
			mpUdiskFileInfo->wTrack--;
			mpUdiskFileInfo->wTrack %= track_count;
			mItemList[item_track].nameWnd.SetWindowTextA(mTrackText[mpUdiskFileInfo->wTrack]);
		}
		break;
	}
}

void CAddUDiskFileWnd::OnNextBtnClick(int idx)
{
	switch ( idx )
	{
	case item_lang:
	{
			miCurLangIdx++;
			const char *lang = GetLangNameByIdx(miCurLangIdx);
			if ( lang == NULL )
				miCurLangIdx--;
			else
			{
				mItemList[item_lang].nameWnd.SetWindowTextA(lang);
				SAFE_STRNCPY(mpUdiskFileInfo->cLang, lang, sizeof(mpUdiskFileInfo->cLang));
			}
	}
		break;

	case item_type:
	{
		miCurTypeIdx++;
			const char *type = GetTypeNameByIdx(miCurTypeIdx);
			if ( type == NULL )
				miCurTypeIdx--;
			else
			{
				mItemList[item_type].nameWnd.SetWindowTextA(type);
				SAFE_STRNCPY(mpUdiskFileInfo->cType, type, sizeof(mpUdiskFileInfo->cType));
			}
	}
		break;

	case item_word:
		if ( mpUdiskFileInfo->wWdCt < 7 )
		{
			mpUdiskFileInfo->wWdCt++;
			CBaseStringA tmp;
			tmp.Format("%d", mpUdiskFileInfo->wWdCt);
			mItemList[item_word].nameWnd.SetWindowTextA(tmp.GetString());
		}
		break;

	case item_track:
		if ( mpUdiskFileInfo->wTrack < track_rigth )
		{
			mpUdiskFileInfo->wTrack++;
			mpUdiskFileInfo->wTrack %= track_count;
			mItemList[item_track].nameWnd.SetWindowTextA(mTrackText[mpUdiskFileInfo->wTrack]);
		}
		break;
	}
}

UDISK_FILE_INFO_S* CAddUDiskFileWnd::GetAddUDiskFileInfo()
{
	return mpUdiskFileInfo;
}

void CAddUDiskFileWnd::SetAddResultPrompt(BOOL bOK)
{
	SetWindowTextA(bOK ? "添加成功" : "添加失败");
}
