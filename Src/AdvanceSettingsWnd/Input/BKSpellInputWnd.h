#ifndef _BKSPELLINPUTWND_H
#define _BKSPELLINPUTWND_H
#include "BaseWnd.h"
#include "BaseButton.h"
//#include "../../InputMethodWnd/InputMethodDef.h"

#define BKSPELLITEM_COUNT	26
#define BKNUMBERITEM_COUNT	10

class CBKSpellInputWnd : public CBaseWnd,
	public IMsgRecvWndInterface,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;
public:
	CBKSpellInputWnd();
	virtual ~CBKSpellInputWnd();

public:
	void Create(CBaseWnd *pParent, RECT rc);

	virtual void LoadResource();

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

private:
	void OnSpecialSymbolClick();

private:
	CBaseButton mNumberItemBtn[BKNUMBERITEM_COUNT];
	CBaseButton mSpellItemBtn[BKSPELLITEM_COUNT];
	CBaseButton mSpellFlagBtn;			// 大小写字母标示
	CBaseButton mSpaceBtn;				// 空格
	CBaseButton mSpecialSymbolBtn;		// 特殊字符
	CBaseButton mBackBtn;				// 删除

	BOOL		mbInSymbol;				// 是否在特殊字符中，区分[、]
	BOOL		mbInUpper;				// 是否大写，默认是
};

#endif
