#ifndef _SETTINGPLAYWND_H
#define _SETTINGPLAYWND_H
#include "BaseWnd.h"
#include "BaseButton.h"

#define KEEP_VOLUME_SETTING		-1		// -1 表示启用跟随上一曲的音量


class CPlayItemWnd : public CBaseWnd,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

  typedef enum _ePlayType
	{
    Type_Volume_Btn	= 0,	
    Type_Original_Btn,			
    Type_OnPhantom_Btn,
    Type_Desc_phantom_Btn,
    Type_Inc_phantom_Btn,
    Type_AllSinger_Btn,
    Type_Play_Count,
	} PLAYTYPE_E;

public:
	void Create(	CBaseWnd *pParent, RECT rc);

	void LoadResource();

	// interface
public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

private:
	void ShowPlayVolume(void);

	void ShowPhantomDelayTime(void);

public:
	CBaseButton	mItemsBtn[Type_Play_Count];

	CBaseWnd mVal_phantomWnd;
  CBaseWnd mPlayWnd;


};

#endif	// _SETTINGPLAYWND_H
