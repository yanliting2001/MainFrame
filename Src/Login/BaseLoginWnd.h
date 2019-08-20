#ifndef _BASELOGINWND_H
#define _BASELOGINWND_H
#include "BaseWnd.h"
#include "BaseButton.h"
#include "../Effect/PopupSlideWnd.h"
#include "../Settings/SettingInputWnd.h"

class CBaseLoginWnd : public CPopupSlideWnd,
	public IClickEventListener,
	public IMsgRecvWndInterface
{
	typedef CPopupSlideWnd CParentClass;

public:
	// 登陆类型，有设置登陆，收藏登陆，关机登陆和删除登陆
	typedef enum _eLoginType
	{
		type_setting = 0,
		type_favorit,
		type_shutdown,
		type_delete,
		type_count
	} LOGINTYPE_E;

	// 登陆提示类型，登陆提示信息
	// 登陆失败
	//
	typedef enum _ePromptType
	{
		prompt_normalPwd= 0,	// 正常提示输入密码
		prompt_normalFavoirtID,	//
		prompt_errPwd,			// 密码错误
		prompt_errFavoritID,	// 收藏号码错误，
		prompt_count,
	} PROMPTTYPE_E;

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

	// interface
public:
	virtual void OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam);

	virtual void OnClick(CBaseWnd *pWnd, 	POINT pt);

public:
	void SetLoginType(LOGINTYPE_E type);

	LOGINTYPE_E GetLoginType(void);

	void SetPwdPromptType(PROMPTTYPE_E type);

	void AddSongToFavorit(const char *cSongID);

	void SaveFavorit();

	void ExitFavoritLogin();

private:
	void ShowLoginID(void);

	void ClearLoginID();

	BOOL IsValidFavoritID();

	void AddSongToFavoritByFavoritID(const char *cSongID);

	void 	OnShutdownClick(void);

	void OnSettingClick(void);

	void OnDeleteClick(void);

	void OnFavoritClick(void);

private:
	CBaseButton mCloseBtn;
	CBaseWnd mLoginTypeLabel;
	CBaseWnd mLoginPromptLabel;

	CBaseButton mPwdEditText;
	CBaseButton mConfirmBtn;

	CBaseWnd mShutdownPromptLabel;

	CSettingInputWnd mInputWnd;

	char mLoginTypePrompt[type_count][64];
	char mPwdTypePrompt[prompt_count][64];

	LOGINTYPE_E mCurLoginType;

private:
	CBaseStringA 	mLoginID;		// 登录ID
	CBaseStringA	mFavoritID;		// 收藏号
	char 		mSongID[8+1];
};

#endif // _BASELOGINWND_H
