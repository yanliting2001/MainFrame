#include "SettingPlayWnd.h"
#include "BaseApp.h"
#include "XMLParser.h"
#include "../AdvanceSettingsWnd/AdvanceSettingApi.h"

void CPlayItemWnd::Create(CBaseWnd *pParent, RECT rc)
{
	CParentClass::Create("PlaySetting", pParent, rc);

	RECT rcControl ={0, 0 , 100, 100};

  //mPlayWnd.CreateStatic(this, rcControl);
  mVal_phantomWnd.CreateStatic(this, rcControl);
  
  for (int i = 0; i < Type_Play_Count; i++ )
	{
    if(i == Type_Desc_phantom_Btn || i == Type_Inc_phantom_Btn)
		mItemsBtn[i].Create(this, rcControl, BUTTONTYPE_NORMAL, false, PRESSED_SCALE);
    else
		mItemsBtn[i].Create(this, rcControl, BUTTONTYPE_RADIO, false, PRESSED_SCALE);
		mItemsBtn[i].SetWndID(Type_Volume_Btn+i);
		mItemsBtn[i].SetOnClickListener(this);
    
	}
  
	ShowPhantomDelayTime();
	LoadResource();
  
}

void CPlayItemWnd::LoadResource()
{
	XMLParser parser;

	char cFileName[MAX_PATH];
	const char* cXmlFolder = theBaseApp->GetXmlFolder();

	CombinePathName(cFileName, cXmlFolder, "Settings/SettingPlayWnd.xml");
	if(false == parser.Load(cFileName))
	{
		DbgOutput(DBG_ERR, "failed to load xml file<%s>!!\n", cFileName);
	}

	// read from xml
	RECT rcControl, rcBaseItem;
	CTexture bkTexture,  btnTextures[4], btnDescTextures[4], btnIncTextures[4];
	char imgPath[64] = {0};
	char colorbuf[16];
	int size;
	UINT32 color;
  
  DbgOutput(DBG_ERR, "%s, line = %d!\n", __FUNCTION__, __LINE__);
  
	XmlLoadRect(&parser, "rcLocation", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("rcLocation", "path", "Settings/Publicbk"), sizeof(imgPath));
  CreateImgTexture(imgPath, &bkTexture);
  SetBackgroundTexture(&bkTexture);
	CParentClass::MoveWindow(&rcControl);

	size = parser.GetIntValue("rcLocation", "fontSize", 26);
	SAFE_STRNCPY(colorbuf, parser.GetStrValue("rcLocation", "fontColor", "FFFFFFFF") , sizeof(colorbuf));
	color = strtoul(colorbuf,NULL,16);

	XmlLoadRect(&parser, "VolumeBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("VolumeBtnInfo", "path", "Settings/ON"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_Volume_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_Volume_Btn].MoveWindow(&rcControl);
	mItemsBtn[Type_Volume_Btn].SetRadioBtnPressed(!gs_SystemSetting.mbPlayVol_keep);
  
  XmlLoadRect(&parser, "OriginalBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("OriginalBtnInfo", "path", "Settings/ON"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_Original_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_Original_Btn].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "OnPhantomBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("OnPhantomBtnInfo", "path", "Settings/ON"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_OnPhantom_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_OnPhantom_Btn].MoveWindow(&rcControl);
	mItemsBtn[Type_OnPhantom_Btn].SetRadioBtnPressed(!gs_SystemSetting.miScreenSave_type);
  
  XmlLoadRect(&parser, "Desc_phantomBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("Desc_phantomBtnInfo", "path", "Settings/Prev"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_Desc_phantom_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_Desc_phantom_Btn].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "Inc_phantomBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("Inc_phantomBtnInfo", "path", "Settings/Next"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_Inc_phantom_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_Inc_phantom_Btn].MoveWindow(&rcControl);

  XmlLoadRect(&parser, "AllSingerBtnInfo", &rcControl);
	SAFE_STRNCPY(imgPath, parser.GetStrValue("AllSingerBtnInfo", "path", "Settings/ON"), sizeof(imgPath));
	CreateBtnImgTextures(imgPath, btnTextures);
	mItemsBtn[Type_AllSinger_Btn].SetTextures(btnTextures);
	mItemsBtn[Type_AllSinger_Btn].MoveWindow(&rcControl);
	mItemsBtn[Type_AllSinger_Btn].SetRadioBtnPressed(!gs_SystemSetting.mbShowSingerByPic);
	
	XmlLoadRect(&parser, "Val_phantomWndInfo", &rcControl);
	mVal_phantomWnd.SetFontSize(size);
	mVal_phantomWnd.SetFontColor(color);
	mVal_phantomWnd.MoveWindow(&rcControl);

}

void CPlayItemWnd::OnClick(CBaseWnd *pWnd, POINT pt)
{
	if ( !pWnd )  return ;

	int nWndID = pWnd->GetWndID();
	switch ( nWndID )
	{
	case Type_Volume_Btn:
		gs_SystemSetting.mbPlayVol_keep = !mItemsBtn[Type_Volume_Btn].GetRadioBtnPressed();
		break;

	case Type_Original_Btn:
		// TODO:
		break;
	case Type_OnPhantom_Btn:
		gs_SystemSetting.miScreenSave_type = !mItemsBtn[Type_OnPhantom_Btn].GetRadioBtnPressed();
		break;
	case Type_Desc_phantom_Btn:
    
    gs_SystemSetting.miScreenSave_time -= 5;
		if ( gs_SystemSetting.miScreenSave_time < 30 )
			gs_SystemSetting.miScreenSave_time = 30;
		ShowPhantomDelayTime();
		break;
	case Type_Inc_phantom_Btn:
    gs_SystemSetting.miScreenSave_time += 5;
		if ( gs_SystemSetting.miScreenSave_time > 60 * 60 )
			gs_SystemSetting.miScreenSave_time = 360;
		ShowPhantomDelayTime();
		break;
	case Type_AllSinger_Btn:
		gs_SystemSetting.mbShowSingerByPic = !mItemsBtn[Type_AllSinger_Btn].GetRadioBtnPressed();
		break;
	default:
		break;
	}
	SaveSystemSetting();
}

void CPlayItemWnd::ShowPlayVolume(void)
{
	char text[32] = {0};
	sprintf(text, "%d", gs_SystemSetting.miPlayVol_val);
	//mVal_volumeWnd.SetWindowTextA(text);
}

void CPlayItemWnd::ShowPhantomDelayTime(void)
{
	char text[32] = {0};
	sprintf(text, "%d", gs_SystemSetting.miScreenSave_time);
  
  DbgOutput(DBG_ERR, "%s, miScreenSave_time = %d!\n", __FUNCTION__, gs_SystemSetting.miScreenSave_time);
	mVal_phantomWnd.SetWindowTextA(text);
}
