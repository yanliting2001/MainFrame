#ifndef _SETTINGINPUTWND_H
#define _SETTINGINPUTWND_H
#include "BaseWnd.h"
#include "BaseButton.h"

class CSettingInputWnd : public CBaseWnd,
	public IMsgRecvWndInterface,
	public ITouchEventListener,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

	enum _eInputItemType
	{
		inputItem_Count = 12,
	};

public:
	void Create(CBaseWnd *pParent, RECT rc);

	virtual void LoadResource();
	// interface
public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	virtual void OnTouchDown(CBaseWnd *pWnd, POINT pt);
	virtual void OnTouchUp(CBaseWnd *pWnd, POINT pt,  int xDistance, int yDistance, UINT64 uUsedTime);
	virtual void OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd);

private:
	void RefreshItemText(int clickIdx);

private:
	CBaseButton mItemsWnd[inputItem_Count];
	CBaseButton mComfirmBtn;
	CBaseButton mCancleBtn;
};

#endif // _SETTINGINPUTWND_H
