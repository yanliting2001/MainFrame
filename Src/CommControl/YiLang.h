#ifndef _YILANG_H
#define _YILANG_H
#include "BaseWnd.h"
#include "../Effect/PopupSlideWnd.h"
//#include "RadioButtonGroup.h"

class CYiLang : public CSlideWnd,
	public IClickEventListener
{
	typedef CSlideWnd CParentClass;

  // 子类项定义
	typedef enum _eSubItem
	{
		Shop_item = 0,
		Food_item,
		Kbar_item,
		Yiwa_item,
		Yilang_count,
	} SUBITEM_E;
    

public:
	void Create(CBaseWnd *pParent);
	virtual void LoadResource();

public:
	virtual void OnClick(CBaseWnd *pWnd, POINT pt);

public:
	void UpdateMircoVal(int val);

	void UpdageVolumeVal(int val);

private:
	
  CBaseButton mYilangItem[Yilang_count];

	CTexture mDefaultUserImgTexture;
};

#endif 	// _LEFTPIERPANEL_H
