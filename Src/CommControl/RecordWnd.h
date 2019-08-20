#ifndef _RECORDWND_H
#define _RECORDWND_H
#include "BaseWnd.h"
#include "../Effect/PopupSlideWnd.h"

class CRecordWnd : public CPopupSlideWnd,
public IClickEventListener

{
	typedef CPopupSlideWnd CParentClass;

public:
	void Create(CBaseWnd *pParent);
  
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);
	virtual void LoadResource();
  
  void SetLoginUserInfo(const char *name);
  
private:
  
	CBaseWnd mLoginQRCodeWnd;		// µÇÂ¼¶þÎ¬Âë
  CBaseWnd mLoginUserImgWnd;
	CBaseWnd mLoginUserBKWnd;
  
  CBaseButton mLogout;
	CBaseButton mReturn;

	CTexture mDefaultUserImgTexture;
};

#endif // _SERVERWND_H
