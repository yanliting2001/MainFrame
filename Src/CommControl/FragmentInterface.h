#pragma once

#include "BaseWnd.h"

class CFragment : public CBaseWnd
{
	typedef CBaseWnd CParentClass;

public:
	// return TRUE if subclass process return message
	// return FALSE if want fragment manager to process return message
	virtual BOOL OnReturn()
	{
		return FALSE;
	}

	void SwitchTo(
		int from,
		int to)
	{
		if (from != to)
		{
			mLastFragment = from;
		}
	}

	int GetLastFragmentType()
	{
		return mLastFragment;
	}

private:
	int mLastFragment;
};
