#ifndef CTree_View_H
#define CTree_View_H

#include <windows.h>
#include <tchar.h>

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "CDrive.h"

class CTree_View
{
private:
	HINSTANCE	m_hInst;
	HWND		m_hParent;
	HWND		m_hTreeView;
	int			m_nID;


public:
	CTree_View();
	~CTree_View();

	void Create(HWND parentWnd, long ID, HINSTANCE hParentInst, int nWidth, int nHeight,
		long lExtStyle = 0, long lStyle = TVS_HASLINES | TVS_LINESATROOT | TVS_HASBUTTONS | TVS_SHOWSELALWAYS,
		int x = CW_USEDEFAULT, int y = 0);

	void GetImageList();

	HWND		GetHandle();
	int			GetID();
	LPCWSTR		GetPath(HTREEITEM hItem);
	LPCWSTR		GetCurPath();
	HTREEITEM	GetCurSel();
	HTREEITEM	GetChild(HTREEITEM hItem);
	void		GetFocus();
	HTREEITEM	GetDesktop();
	HTREEITEM	GetMyComputer();

	void LoadMyComputer(CDrive *);
	void LoadChild(HTREEITEM &hParent, LPCWSTR path, BOOL bShowHiddenSystem = FALSE);
	void PreloadExpanding(HTREEITEM hPrev, HTREEITEM hCurSel);
	void Expand(HTREEITEM hItem);
	void PreLoad(HTREEITEM hItem);
};

#endif