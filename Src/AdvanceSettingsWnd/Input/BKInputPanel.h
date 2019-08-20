#ifndef _BKINPUTPANEL_H
#define _BKINPUTPANEL_H
#include "BaseWnd.h"
#include "BaseButton.h"
#include "../../Effect/PopupSlideWnd.h"
#include "../../InputMethodWnd/InputMethodDef.h"
#include "BKSpellInputWnd.h"
#include "BKHandWriteInputWnd.h"

class CBKInputPanel : public CPopupSlideWnd,
	public IClickEventListener,
	public IMsgRecvWndInterface
{
	typedef CPopupSlideWnd CParentClass;
public:
	CBKInputPanel();
	virtual ~CBKInputPanel();

public:
	void Create(CBaseWnd *pParent, RECT rcRect, BOOL bInitShowInput, BOOL bPopup);

	virtual void LoadResource();

	virtual void OnWindowVisible(BOOL bVisible);

public:
	virtual void OnMsg(UINT32 uType, UINT64 wParam, UINT64 lParam);

	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	void ClearInputText();

	void GetInputText(CBaseStringW &text);

	void SetInputText(CBaseStringW &text);

	void SetInputMethodType(INPUTMETHODTYPE inputtype);

private:
	CBaseButton			 mSpellBtn;
	CBaseButton 		 mHandWriteBtn;
	INPUTMETHODTYPE		 mInputType;

	CBKSpellInputWnd	 mSpellInput;
	CBKHandWriteInputWnd mHandWirteInput;

	CBaseStringW 		 mInputText;
};

#endif
