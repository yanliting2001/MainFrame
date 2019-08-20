#ifndef _FoodWND_H
#define _FoodWND_H
#include "BaseWnd.h"
#include "../Effect/PopupSlideWnd.h"


class CFoodWnd: public CPopupSlideWnd,
public IClickEventListener


{
  typedef CPopupSlideWnd CParentClass;

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

private:
	CBaseWnd mPushInfo;
  
  CBaseWnd mPushItem;

  CBaseButton mClose;

};

#endif /* _SELECTEDFRAGMENT_H */
