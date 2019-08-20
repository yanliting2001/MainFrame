#ifndef _SETTINGSYSTEMWND_H
#define _SETTINGSYSTEMWND_H
#include "BaseWnd.h"
#include "BaseButton.h"

class CSystemItemWnd : public CBaseWnd,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

	typedef enum _eItemType
	{
		Type_DecVolume_Btn = 0,		// 开机音量
		Type_IncVolume_Btn,
    Type_DesEffectVol_Btn,
    Type_IncEffectVol_Btn,
		Type_BackPwd_Btn,			// 后台密码
		Type_DeletePwd_Btn,		// 删歌密码
		Type_ShutPwd_Btn,			// 关机密码
		Type_Touch_Btn,					// 触屏校准
		Type_Sys_count,
	} TIEM_TYPE_E;

	typedef enum _eVolType
	{
		volume_on = 0,
		volume_effect,
	} VOLTYPE_E;

	typedef enum _ePwdType
	{
		pwd_back = 0,
		pwd_delete,
		pwd_shutdown,
	} PWDTYPE_E;

public:
	void Create(	CBaseWnd *pParent, RECT rc);

	void LoadResource();

	// interface
public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

private:
	void SetVolumeValue(int val, VOLTYPE_E type);

	void SetPwdValue(const char *pwd, PWDTYPE_E type);

	void OnCalScreenClick();

public:
	CBaseButton	mItemsBtn[Type_Sys_count];
	// 开机音量
	CBaseWnd mVal_onVolWnd;
	// 效果音量
	CBaseWnd mVal_effectVolWnd;

	CBaseThread mCalibrateThread;
};

#endif	// _SETTINGSYSTEMWND_H
