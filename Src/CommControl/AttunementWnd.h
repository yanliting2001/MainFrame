#ifndef _ATTUNEMENTWND_H
#define _ATTUNEMENTWND_H

#include "BaseWnd.h"
#include "BaseSlide.h"
#include "RadioButtonGroup.h"
#include "../Effect/PopupSlideWnd.h"
#include "../SerialCtrl/PreampDevCtrl.h"

class CAttunementWnd : public CPopupSlideWnd,
	public IClickEventListener,
	public IPositionChangeEventListener
{
	typedef CPopupSlideWnd CParentClass;

public:
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

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

	virtual void OnWindowVisible(	BOOL bVisible);

	// interface
public:
	virtual void OnClick(CBaseWnd *pWnd, 	POINT pt);

	virtual void OnPositionChange(CBaseWnd *pWnd, int nOldPosition, int nNewPostion, BOOL bChangeEnd);

public:
	void ResetAttunement(void);

	void SetTune(int idx, BOOL bSave = FALSE);

	void SetMircoVal(int val, BOOL bSave = FALSE);

	void SetVolumeVal(int val, BOOL bSave = FALSE);

private:
	CBaseButton mTuneItemsBtn[tune_count];
	CRadioButtonGroup mToneBtnGroup;

	CBaseWnd mMicroBoxWnd;
	CBaseWnd mMicroIcoWnd;
	CBaseSlide mMicroSlide;
	CBaseWnd mMicroValWnd;

	CBaseWnd mVolBoxWnd;
	CBaseWnd mVolIcoWnd;
	CBaseSlide mVolumeSlide;
	CBaseWnd mVolValWnd;

	CBaseButton mResetWnd;		// 默认调音

	CBaseButton mCloseBtn;
  
public:
  int mAttuneType;  //混音类型

private:
	CPreampDevCtrl mPreampDevCtrl;
};

#endif // _ATTUNEMENTWND_H
