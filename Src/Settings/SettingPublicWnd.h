#ifndef _SETTINGPUBLICWND_H
#define _SETTINGPUBLICWND_H
#include "BaseWnd.h"
#include "BaseButton.h"
#include "RadioButtonGroup.h"

class CPublicItemWnd : public CBaseWnd,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

  typedef enum _ePlayType
	{
    Type_Des_Volume_Btn	= 0,	
    Type_Inc_Volume_Btn,
    Type_Tune_Btn,			
    Type_Setting_Btn,
    Type_SingedList_Btn,
    Type_Public_Count,
	} PLAYTYPE_E;

public:
	void Create(	CBaseWnd *pParent, RECT rc);

	void LoadResource();

	// interface
public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

private:
	void SetVolumeVal(BOOL bSave=FALSE);

	void SetPublicTune(BOOL bSave=FALSE);

	void SetPublicSetting(BOOL bSave=FALSE);

	void SaveSingedToNewSong(void);

public:
	CBaseButton	mItemsBtn[Type_Public_Count];
	// 开机音量

	CBaseWnd mVal_effectVolWnd;
	
};

#endif	// _SETTINGPUBLICWND_H
