#pragma once

#include "FragmentInterface.h"
#include "MainCtrlPage.h"
#include "PhantomPage.h"

typedef enum ePAGE_TYPE
{
	// full screen fragment
	Page_MainCtrlPage=0,
	Page_PhantomPage
} PAGE_TYPE;

class CPageManager
{
public:
	CPageManager();
	virtual ~CPageManager();

public:
	void InitPages();

	void SetCurrentPage(
		PAGE_TYPE ePageType);

	void ReturnToLastPage();

	PAGE_TYPE GetCurPageType();

	CFragment* GetCurPage();

private:
	CFragment* GetPageByID(
		PAGE_TYPE ePageType);

	virtual void ShowPage(
		PAGE_TYPE ePageType);

public:
	CPhantomPage mPhantomPage;
	CMainCtrlPage mMainCtrlPage;

private:
	PAGE_TYPE mCurPageType;
};
