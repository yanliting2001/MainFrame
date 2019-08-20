#ifndef _UPGRADEWND_H
#define _UPGRADEWND_H
#include "BaseWnd.h"
#include "BaseButton.h"
#include "CSCloudSongLib.h"

class CUpgradeWnd: public CBaseWnd
{
	typedef CBaseWnd CParentClass;

public:
	void Create(CBaseWnd *pParent, RECT rc);

	virtual void LoadResource();

public:
	void Upgrade(void);

	void UpgradeAppProcess(struct stbboot_event *event);

private:
	BOOL IsDataOld(void);

	BOOL IsAppOld(void);

	void UpgradeData(void);

	void UpgradeApp(void);

private:
	void ConverDb();

private:
	CSongLibrary *mpCloudData;
	CBaseThread mUpgradeThread;
	int miScreenSave_Type;					//保存当前屏保类型，防止在下载升级的时候，出现幻影或者是屏保出现
};

#endif		// _UPGRADEWND_H
