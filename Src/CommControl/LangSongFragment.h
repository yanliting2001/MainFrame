#ifndef _LANGSONGFRAGMENT_H
#define _LANGSONGFRAGMENT_H
#include "FragmentInterface.h"
#include "BaseButton.h"
#include "Song.h"

class CLangSongFragment : public CFragment,
	public IClickEventListener,
	public ITouchEventListener
{
	typedef CFragment CParentClass;

	typedef enum _eLangType
	{
    langType_japan = 0,							// 日语
    langType_korea,						// 韩语
    langType_yueyu,						// 粤语
		langType_chinese,				// 国语
		langType_englisth,					// 英语
		langType_minnan,					// 闽南
		langType_other,							// 其他
		
		langType_count
	} LANG_TYPE_E;

	typedef struct _sLangTypeID
	{
		int id;
		char typeName[32];
	} LANGTYPE_ID_S;

public:
	CLangSongFragment();
	virtual ~CLangSongFragment();

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	virtual void OnTouchDown(CBaseWnd *pWnd, 	POINT pt);
	virtual void OnTouchUp(CBaseWnd *pWnd, POINT pt, int xDistance, int yDistance, UINT64 uUsedTime);
	virtual void OnTouchMove(CBaseWnd *pWnd, POINT pt);
	virtual void OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd);

private:
	//CBaseButton	mReturnBtn;
	//CBaseWnd mSearchPrompt;

	CBaseWnd	mItemListBKWnd;
	CBaseButton mItemsBtn[langType_count];

	RECT mrcBaseItem;
	int miXoffset;
	RECT mrcOrigItemsBKShow;

private:
	POINT mPtPressedScreen;
	CBaseWnd *mpPressedWnd;

private:
	LANGTYPE_ID_S mTypesID[langType_count];
};

#endif /* _LANGSONGFRAGMENT_H */
