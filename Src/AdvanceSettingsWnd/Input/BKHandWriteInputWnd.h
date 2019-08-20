#ifndef _BKHANDWRITEINPUTWND_H
#define _BKHANDWRITEINPUTWND_H
#include "BaseWnd.h"
#include "BaseButton.h"
//#include "../../InputMethodWnd/InputMethodDef.h"
#include "LineWnd.h"

#define BKCANDIDATE_TEXT_COUNT		9

class CBKHandWriteInputWnd : public CBaseWnd,
	public IMsgRecvWndInterface,
	public ITouchEventListener,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;
public:
	CBKHandWriteInputWnd();
	virtual ~CBKHandWriteInputWnd();

public:
	void Create(CBaseWnd *pParent, RECT rc);
	virtual void LoadResource();

public:
	virtual void OnDestroy();

	virtual void OnWindowVisible(BOOL bVisible);

	virtual void OnTimer(int iTimerID);

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	virtual void OnTouchDown(CBaseWnd *pWnd, POINT pt);

	virtual void OnTouchUp(CBaseWnd *pWnd, POINT pt, int xDistance, int yDistance, UINT64 uUsedTime);

	virtual void OnTouchMove(CBaseWnd *pWnd, POINT pt);

	virtual void OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd);

private:
	void AddPointToLocus(POINT pt);

	void DoRecognize();

	void OnSpecialSymbolClick();

private:
	wchar_t		mCandidateText[BKCANDIDATE_TEXT_COUNT];
	CBaseButton	mCandidateBtns[BKCANDIDATE_TEXT_COUNT];
	CBaseButton mSpaceBtn;
	CBaseButton mSpecialSymbolBtn;
	CBaseButton mBackBtn;

	CLineWnd	mLineWnd;
	short		*mpLocusPoint;
	int			miLocusCount;
	int 		miLocusIndex;
	BOOL		mbInSymbol;
};

#endif
