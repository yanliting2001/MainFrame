#ifndef _ADDUDISKFILEWND_H
#define _ADDUDISKFILEWND_H
#include "BaseButton.h"
#include "UDiskFileInfo.h"

class CAddUDiskFileWnd : public CBaseWnd,
	public IClickEventListener
{
	typedef CBaseWnd CParentClass;

	typedef struct _sItemInfo
	{
		CBaseWnd itemBKGroundWnd;
		CBaseWnd titleWnd;
		CBaseWnd nameWnd;;
		CBaseButton prevBtn;;
		CBaseButton nextBtn;
		CBaseWnd fengexianWnd;
	} ITEMINFO_S;

	typedef enum _eItemCount
	{
		item_file = 0,
		item_song,
		item_spell,
		item_singer,
		item_lang,
		item_type,
		item_word,
		item_track,
		item_nothing,
		item_count,
	} ITEMCOUNT_E;

public:
	CAddUDiskFileWnd() : mpUdiskFileInfo(NULL), miCurLangIdx(-1), miCurTypeIdx(-1) {}

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	void OnWindowVisible(BOOL bVisible);

	void SetAddUDiskFileInfo(UDISK_FILE_INFO_S *pFileInfo);

	UDISK_FILE_INFO_S* GetAddUDiskFileInfo();

	void SetAddResultPrompt(BOOL bOK);

private:
	void OnPrevBtnClick(int idx);

	void OnNextBtnClick(int idx);

private:
	ITEMINFO_S mItemList[item_count];
	UDISK_FILE_INFO_S *mpUdiskFileInfo;
	char mTrackText[track_count][64];

	int miCurLangIdx;
	int miCurTypeIdx;
};

#endif /* _ADDUDISKFILEWND_H */
