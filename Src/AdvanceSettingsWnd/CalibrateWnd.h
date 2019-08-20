#pragma once

#include "BaseWnd.h"

#include "tslib.h"


typedef struct {
	int x[5], xfb[5];
	int y[5], yfb[5];
	int a[7];
} calibration;

class CCalibrateWnd : public CBaseWnd
{
	typedef CBaseWnd CParentClass;

public:
	CCalibrateWnd();
	~CCalibrateWnd();

public:
	void Create(
			CBaseWnd *pParent,
			RECT rc);

	void LoadResource();

	// interface
public:
	void InitCalibration();

	void Getxy(struct tsdev *ts,
			int *x,int *y);

	void GetSample(	int index,
			int x,int y,const char *name);

	int PerformCalibration();
	int PerformCalibration1();

	void ClearBuf();

	void SaveToConfig();

	void DeInitCalibration();

	void PromptVisible(BOOL visible);

private:
	CBaseWnd mDotWnd;
	CBaseWnd mPromptWnd;
	struct tsdev *ts;
	calibration cal;
};



