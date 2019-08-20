#ifndef _LEFTPIERPANEL_H
#define _LEFTPIERPANEL_H
#include "BaseWnd.h"
#include "../Effect/PopupSlideWnd.h"
//#include "RadioButtonGroup.h"
#include "../SerialCtrl/PreampDevCtrl.h"

class CLeftPierPanel : public CSlideWnd,
	public IClickEventListener
{
	typedef CSlideWnd CParentClass;
	//
	typedef enum _eVolType
	{
		music_vol = 0x0,
		vol_count,
	} VOL_TYPE_E;
	
	typedef enum _eTuneType
	{
		tune_profession = 0, 	// 专业
		tune_normal,					// 标准
		tune_KSong,					// K 歌
		tune_theater,					// 剧院
		tune_count,
	} TUNETYPE_E;

  	typedef enum _eAttuneType
	{
		MealKtv_Attune = 0, 	  // 餐k效果器
		SingBar_Attune,					// 唱吧效果器
		Vietnam_Attune,					// 越南效果器
		Attune_count,
	} AttuneTYPE_E;

  // 子类项定义
	typedef enum _eSubItem
	{
    
		Logo_item = 0,
    		Setup_item,
    		UserBK_item,
    		NameBg_item,
    		VolumeBG_item,
    		VolDown_item,
    		VolUp_item,
    		MicDown_item,
    		MicUp_item,
    		EffectStand_item,
    		EffectSinger_item,
    		EffectPopular_item,
		Pay_item,
		TimeBg_item,
		LeftItem_count,
	} SUBITEM_E;
    
	// 音量项  
	typedef struct _sVolumeItem
	{
		CBaseWnd bkGroundWnd;
		CBaseWnd volIcoWnd;
		CBaseWnd valVolWnd;
		CBaseWnd btnBKGroundWnd;
		CBaseButton descVolBtn;
		CBaseButton incVolBtn;
	} VOLUMEITEM_S;

public:
	void Create(CBaseWnd *pParent);
	virtual void LoadResource();

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

public:
 void SetLoginUserInfo(const char *name);

	void RefreshLeftTime(int type, int left);


	// type:类型
	// left:天数, 只有在type在包月时有效
	void RefreshLeftDate(int type, int left);
	void ShowRepayVisible(BOOL bVisible);
  
  void StartTime(void);
  void OnTimer(int nTimerID);

	void UpdateMircoVal(int val);

	void UpdageVolumeVal(int val);
	void ResetAttunement(void);
	void SetTune(int idx, BOOL bSave);
	void SetMircoVal(int val, BOOL bSave);
	void SetVolumeVal(int val, BOOL bSave);

private:
	
  CBaseButton mLeftItem[LeftItem_count];
  
	CBaseWnd mLoginUserImgWnd;
  
	CBaseWnd mLeftTimeWnd;

	CBaseWnd mLoginUserWnd;

	CBaseWnd mVolValueWnd;

	CBaseWnd mMicValueWnd;
  
	CTexture mDefaultUserImgTexture;
	
	CPreampDevCtrl mPreampDevCtrl;
public:
	  int mAttuneType;  //混音类型
};

#endif 	// _LEFTPIERPANEL_H
