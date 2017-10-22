#ifndef CLISTVIEW_H
#define CLISTVIEW_H
#define _CRT_SECURE_NO_WARNINGS

#include <windows.h>

#include <commctrl.h>
#pragma comment(lib, "comctl32.lib")
#pragma comment(linker,"\"/manifestdependency:type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#include "CDrive.h"

class CListView
{
private:
	HINSTANCE	m_hInst;
	HWND		m_hParent;
	HWND		m_hListView;
	int			m_nID;

	
	LPWSTR	_GetSize(const WIN32_FIND_DATA &fd);
	LPWSTR	_GetDateModified(const FILETIME &ftLastWrite);

public:
	CListView();
	~CListView();

	void Create(HWND parentWnd, long ID, HINSTANCE hParentInst, int nWidth,
		int nHeight, int x = CW_USEDEFAULT, int y = 0,
		long lExtStyle = WS_EX_CLIENTEDGE,
		long lStyle = LVS_ICON | LVS_EDITLABELS | LVS_SHOWSELALWAYS);
	void DisplayInfoCurSel();
	void InitImageList();
	void Init4Cols();
	void InitDriveColumn();
	void InitFolderColumn();
	HWND	GetHandle();
	LPCWSTR GetPath(int iItem);
	LPCWSTR GetCurSelPath();

	void LoadDesktop();
	void LoadMyComputer(CDrive *drive);
	void LoadChild(LPCWSTR path, CDrive *drive);
	void LoadFileAndFolder(LPCWSTR path);
	void LoadCurSel();

	void DeleteAll();
};
LPWSTR Convert(__int64 nSize);
#endif