#ifndef _SINGERTYPEFRAGMENT_H
#define _SINGERTYPEFRAGMENT_H
#include "BaseWnd.h"
#include "BaseButton.h"
#include "FragmentInterface.h"

class CSingerTypeFragment : public CFragment,
	public IClickEventListener,
	public ITouchEventListener
{
	typedef enum _eSingerType
	{
		singerType_dlMale = 0,		// 大陆男星
		singerType_dlFemale,			// 大陆女星
		singerType_gwMale,				// 国外男星
		singerType_gtMale,				// 港台男星
		singerType_gtFemale,			// 港台女星
		singerType_gwFemale,		// 国外女星
		singerType_RHMale,				// 日韩男星
		singerType_RHFemale,		// 日韩女星
		singerType_ldGroup,			// 乐对组合
		singerType_count
	} SINGER_TYPE_E;

	typedef struct _sSingerTypeID
	{
		int id;
		char typeName[32];
	} SINGERTYPE_ID_S;

	typedef CFragment CParentClass;

public:
	CSingerTypeFragment();
	virtual ~CSingerTypeFragment();

public:
	void Create(CBaseWnd *pParent);
	virtual void LoadResource();

public:
	virtual void OnWindowVisible(	BOOL bVisible);

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

	virtual void OnTouchDown(CBaseWnd *pWnd, POINT pt);
	virtual void OnTouchUp(CBaseWnd *pWnd, POINT pt, int xDistance, int yDistance, UINT64 uUsedTime);
	virtual void OnTouchDrag(CBaseWnd *pWnd, int xDragLen, int yDragLen);
	virtual void OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd);
	virtual void OnTouchMove(CBaseWnd *pWnd, POINT pt);

private:
	//CBaseButton	mReturnBtn;
	//CBaseWnd mSearchPrompt;

	CBaseWnd	mItemListBKWnd;
	CBaseButton mItemsBtn[singerType_count];

	RECT mrcBaseItem;
	int miXoffset;
	RECT mrcOrigItemsBKShow;

private:
	POINT mPtPressedScreen;
	CBaseWnd *mpPressedWnd;

private:
	SINGERTYPE_ID_S mTypesID[singerType_count];
};

#endif		// _SINGERTYPEFRAGMENT_H

