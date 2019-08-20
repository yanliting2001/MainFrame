#ifndef _SETTINGMUSICLIBWND_H
#define _SETTINGMUSICLIBWND_H
#include "BaseWnd.h"
#include "BaseButton.h"
#include "RadioButtonGroup.h"
#include "CSCloudSongLib.h"

class CMusicItemWnd : public CBaseWnd,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

	typedef enum _eItemType
	{
		Type_SmartDel_Btn = 0,		// 智能删歌
		Type_NewSong_Btn,		     // 新歌列表设置
		Type_OrderTime_Btn,		// 点击率排行
		Type_SongWord_Btn,    //歌曲字数排行
		Type_SongName_Btn,    //歌曲名称
		Type_UpgradeSong_Btn, //升级歌库
		Type_Music_Count,
	} TIEM_TYPE_E;

public:
	void Create(	CBaseWnd *pParent, RECT rc);

	void LoadResource();

	// interface
public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

private:
	void SetOrderType(BOOL bSearchSong=FALSE);

	void SmartDeleteSong(void);

	void SaveSingedToNewSong(void);
    
  void ConverSongDb();

public:
	CBaseButton	mItemsBtn[Type_Music_Count];
	
	CRadioButtonGroup mOrderBtnGroup;
	CSongLibrary *mpCloudData;

};

#endif	// _SETTINGMUSICLIBWND_H
