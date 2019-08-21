#ifndef _FoodWND_H
#define _FoodWND_H
#include "BaseWnd.h"
#include "../Effect/PopupSlideWnd.h"
#define MAX_FOOD_ITEMS		20

typedef struct _sFoodFileInfo
{
	char  cFileName[256];     
	char  cFilePath[256];     	
} FOOD_FILE_INFO_S;

class CFoodWnd: public CPopupSlideWnd,
public IClickEventListener,
public ITouchEventListener

{
  typedef CPopupSlideWnd CParentClass;

public:
	void Create(CBaseWnd *pParent);

	virtual void LoadResource();

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);
	void OnTouchDown(CBaseWnd *pWnd, POINT pt);
	void OnTouchUp(CBaseWnd *pWnd, POINT pt, int xDistance, int yDistance, 	UINT64 uUsedTime);
	void OnTouchMove(CBaseWnd *pWnd, POINT pt);
	void OnTouchDrag(CBaseWnd *pWnd, int xDragLen, int yDragLen);
	void OnTouchLeave(CBaseWnd *pOldTouchWnd, CBaseWnd *pNewTouchWnd);
private:
	void getFilesFromdir(CPtrCtrl *list, const char *path, int depth);

private:
	CBaseWnd mPushInfo;
  
  CBaseWnd mPushItem;
  CBaseButton mFoodItems[MAX_FOOD_ITEMS];

  CBaseButton mClose;
  
public:
	CBaseWnd	mHomeItemBKWnd;
	RECT mrcOrigItemsBKShow;

private:
	POINT mPtPressedScreen;
	CBaseWnd *mpPressedWnd;
	int miXoffset;
	CPtrListCtrl mFoodFileLists;

};

#endif /* _SELECTEDFRAGMENT_H */
