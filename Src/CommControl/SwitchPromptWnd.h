#ifndef _SWITCHPROMPTWND_H
#define _SWITCHPROMPTWND_H
#include "BaseWnd.h"

class CSwitchPromptWnd: public CBaseWnd
{
	typedef CBaseWnd CParentClass;

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

	// interface
	virtual void OnTimer(int nTimerID);

public:
	void SwitchPrompt(void);

};
#endif		// _SWITCHPROMPTWND_H
