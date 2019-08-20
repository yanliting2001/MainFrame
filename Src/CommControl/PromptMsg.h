#ifndef _PROMPTMSG_H
#define _PROMPTMSG_H
#include "BaseWnd.h"
#include "../Effect/PopupSlideWnd.h"


typedef enum _ePromptMsgType
	{
    
		PromptType_TimeClose = 1,    //定时关闭菜单
    PromptType_ManualClose,      //手动关闭菜单
		PromptType_Count,
	} PROMPTMSGTYPE_E;

class CPromptMsg: public CPopupSlideWnd,
public IClickEventListener


{
  typedef CPopupSlideWnd CParentClass;

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);
  
  void SetPromptMsg(int Promptstype, int time);
  
  void SetPrompString(const char* cString);

  
  void OnTimer(int nTimerID);

public:
	CBaseWnd mPromptInfo;
  

  CBaseButton mClose;

};

#endif /* _SELECTEDFRAGMENT_H */
