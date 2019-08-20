#ifndef _NUMINPUTWND_H
#define _NUMINPUTWND_H
#include "BaseWnd.h"
#include "BaseButton.h"

//#include "TreasureBoxDef.h"

#define NUMINPUT_MAX	12

class CNumInputWnd : public CBaseWnd,
	public IClickEventListener,
	public IMsgRecvWndInterface
{
	typedef CBaseWnd CParentClass;

public:
	CNumInputWnd();
	~CNumInputWnd();

public:
	void Create(CBaseWnd *pParent, RECT rc);

	virtual void LoadResource();

	void LoadResourceByXmlFile(const char *cXmlFileName);

	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

private:
	CBaseButton mNumBtn[NUMINPUT_MAX];
	CBaseButton mBackBtn;
	CBaseButton mDotBtn;

private:
#if APP_ID == QIKUN500
	void LoadResource_v500();
#elif APP_ID == QIKUN510
	void LoadResource_v510();
#elif APP_ID == QIKUN501
	void LoadResource_v501();
#elif APP_ID == QIKUN520
	void LoadResource_v520();
#endif
};

#endif
