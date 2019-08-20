#ifndef _SERVERWND_H
#define _SERVERWND_H
#include "BaseWnd.h"
#include "../Effect/PopupSlideWnd.h"

class CServerWnd : public CPopupSlideWnd
{
	typedef CPopupSlideWnd CParentClass;

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();
};

#endif // _SERVERWND_H
