#include"stdafx.h"
#include "CTree_View.h"
#include "resource.h"
//Dùng để sử dụng hàm StrCpy, StrNCat
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")


//Vị trí trong image list sau khi trích xuất từ shell32.dll


#define I_FOLDER 0
#define I_FILE 1
#define I_MYCOMPUTER 2
#define I_FLOPPY 3
#define I_USB 4
#define I_HDD 5
#define I_CD  6 

//Độ dài tối đa đường dẫn
#define MAX_PATH_LEN 10240
/*****************************************************************************************/

CTree_View::CTree_View()
{
	m_hInst = NULL;
	m_hParent = NULL;
	m_hTreeView = NULL;
	m_nID = 0;
}

CTree_View::~CTree_View()
{
	DestroyWindow(m_hTreeView);
}

void CTree_View::Create(HWND parentWnd, long ID, HINSTANCE hParentInst, int nWidth, int nHeight,
	long lExtStyle, long lStyle, int x, int y)
{
	InitCommonControls();
	CTree_View::m_hParent = parentWnd;
	CTree_View::m_hInst = hParentInst;
	CTree_View::m_hTreeView = CreateWindowEx(lExtStyle, WC_TREEVIEW, _T("Tree View"),
		WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL |WS_TABSTOP  | lStyle,
		x, y, nWidth, nHeight, parentWnd,
		(HMENU)ID, hParentInst, NULL);
	CTree_View::m_nID = ID;

	GetImageList();
}
void CTree_View::GetImageList()
{
	InitCommonControls();
	HIMAGELIST *hLarge = new HIMAGELIST;
	HIMAGELIST *hSmall = new HIMAGELIST;

	*hSmall = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK,
		2, 0);

	HICON hIcon;

	hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_FOLDER));
	ImageList_AddIcon(*hSmall, hIcon);
	
	hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_FILE));
	ImageList_AddIcon(*hSmall, hIcon);
	
	hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_MYCOMPUTER));
	ImageList_AddIcon(*hSmall, hIcon);

	hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_FLOOPY));
	ImageList_AddIcon(*hSmall, hIcon);

	hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_USB));
	ImageList_AddIcon(*hSmall, hIcon);

	hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_HDD));
	ImageList_AddIcon(*hSmall, hIcon);

	hIcon = LoadIcon(m_hInst, MAKEINTRESOURCE(IDI_CD));
	ImageList_AddIcon(*hSmall, hIcon);
	TreeView_SetImageList(m_hTreeView, *hSmall, TVSIL_NORMAL);
}
HWND CTree_View::GetHandle()
{
	return m_hTreeView;
}

int CTree_View::GetID()
{
	return m_nID;
}

LPCWSTR CTree_View::GetPath(HTREEITEM hItem)
{
	TVITEMEX tv;
	tv.mask = TVIF_PARAM;
	tv.hItem = hItem;
	TreeView_GetItem(m_hTreeView, &tv);
	return (LPCWSTR)tv.lParam;
}

LPCWSTR CTree_View::GetCurPath()
{
	return GetPath(GetCurSel());
}

HTREEITEM CTree_View::GetCurSel()
{
	return TreeView_GetNextItem(m_hTreeView, NULL, TVGN_CARET);
}


HTREEITEM CTree_View::GetChild(HTREEITEM hItem)
{
	return TreeView_GetChild(m_hTreeView, hItem);
}
/*****************************************************************************************/

void CTree_View::GetFocus()
{
	SetFocus(m_hTreeView);
}
HTREEITEM CTree_View::GetDesktop()
{
	return TreeView_GetRoot(m_hTreeView);
}

HTREEITEM CTree_View::GetMyComputer()
{
	return TreeView_GetChild(m_hTreeView, GetDesktop());
}

/*****************************************************************************************/

void CTree_View::LoadMyComputer(CDrive *drive)
{
	TV_INSERTSTRUCT tvInsert;
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;

	//Desktop

	tvInsert.hParent = NULL;
	tvInsert.hInsertAfter = TVI_ROOT;
	tvInsert.item.iImage = I_MYCOMPUTER;
	tvInsert.item.iSelectedImage = I_MYCOMPUTER;
	tvInsert.item.pszText = _T("Desktop");
	tvInsert.item.lParam = (LPARAM)_T("Desktop");
	HTREEITEM hDesktop = TreeView_InsertItem(m_hTreeView, &tvInsert);

	//My Computer
	tvInsert.hParent = hDesktop;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item.iImage = I_MYCOMPUTER;
	tvInsert.item.iSelectedImage = I_MYCOMPUTER;
	tvInsert.item.pszText = _T("My Computer");
	tvInsert.item.lParam = (LPARAM)_T("MyComputer");
	HTREEITEM hMyComputer = TreeView_InsertItem(m_hTreeView, &tvInsert);

	//ổ đĩa

	for (int i = 0; i < drive->GetCount(); ++i)
	{
		tvInsert.hParent = hMyComputer; //Them
		tvInsert.item.iImage = drive->GetIconIndex(i);
		tvInsert.item.iSelectedImage = drive->GetIconIndex(i);
		tvInsert.item.pszText = drive->GetDisplayName(i);
		tvInsert.item.lParam = (LPARAM)drive->GetDriveName(i);
		HTREEITEM hDrive = TreeView_InsertItem(m_hTreeView, &tvInsert);

		//Preload
		if ((drive->GetIconIndex(i) == I_HDD) || (drive->GetIconIndex(i) == I_USB))
		{
			tvInsert.hParent = hDrive; //Them
			tvInsert.item.pszText = _T("PreLoad"); //Them
			tvInsert.item.lParam = (LPARAM)_T("PreLoad");
			TreeView_InsertItem(m_hTreeView, &tvInsert);
		}
	}//for

	TreeView_SelectItem(m_hTreeView, hMyComputer);
}


void CTree_View::LoadChild(HTREEITEM &hParent, LPCWSTR path, BOOL bShowHiddenSystem)
{
	TCHAR buffer[MAX_PATH_LEN];
	StrCpy(buffer, path);

	StrCat(buffer, _T("\\*"));

	TV_INSERTSTRUCT tvInsert;
	tvInsert.hParent = hParent;
	tvInsert.hInsertAfter = TVI_LAST;
	tvInsert.item.mask = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
	tvInsert.item.iImage = I_FOLDER;
	tvInsert.item.iSelectedImage = I_FOLDER;

	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFileW(buffer, &fd);
	BOOL bFound = 1;

	if (hFile == INVALID_HANDLE_VALUE)
		bFound = FALSE;

	TCHAR * folderPath;
	while (bFound)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& ((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN)
			&& ((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM)
			&& (StrCmp(fd.cFileName, _T(".")) != 0) && (StrCmp(fd.cFileName, _T("..")) != 0))
		{
			tvInsert.item.pszText = fd.cFileName;
			folderPath = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];

			StrCpy(folderPath, path);
			if (wcslen(path) != 3)
				StrCat(folderPath, _T("\\"));
			StrCat(folderPath, fd.cFileName);

			tvInsert.item.lParam = (LPARAM)folderPath;
			HTREEITEM hItem = TreeView_InsertItem(m_hTreeView, &tvInsert);
			//Preload
			PreLoad(hItem);
		}

		bFound = FindNextFileW(hFile, &fd);
	}//while
}

void CTree_View::PreloadExpanding(HTREEITEM hPrev, HTREEITEM hCurSel)
{
	if (hCurSel == GetMyComputer()) //Nếu là My Computer không nạp
		return;

	HTREEITEM hCurSelChild = TreeView_GetChild(m_hTreeView, hCurSel);

	if (!StrCmp(GetPath(hCurSelChild), _T("PreLoad")))
	{
		TreeView_DeleteItem(m_hTreeView, hCurSelChild);
		LoadChild(hCurSel, GetPath(hCurSel));
	}
}

void CTree_View::PreLoad(HTREEITEM hItem)
{
	TCHAR buffer[MAX_PATH_LEN];
	StrCpy(buffer, GetPath(hItem));

	if (wcslen(buffer) == 3) 
	{
		if (StrStr(buffer, _T("A:")) || StrStr(buffer, _T("B:"))) 
			return;
	}
	else
		StrCat(buffer, _T("\\"));

	StrCat(buffer, _T("*"));

	WIN32_FIND_DATA fd;
	HANDLE hFile = FindFirstFileW(buffer, &fd);

	if (hFile == INVALID_HANDLE_VALUE)
		return;

	BOOL bFound = true;

	//Trong khi còn tìm thấy file hay thư mục
	while (bFound)
	{
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
			&& (StrCmp(fd.cFileName, _T(".")) != 0) && (StrCmp(fd.cFileName, _T("..")) != 0))
		{
			TV_INSERTSTRUCT tvInsert;
			tvInsert.hParent = hItem;
			tvInsert.hInsertAfter = TVI_LAST;
			tvInsert.item.mask = TVIF_TEXT | TVIF_PARAM;							
			tvInsert.item.pszText = NULL;
			tvInsert.item.lParam = (LPARAM)_T("PreLoad");
			TreeView_InsertItem(m_hTreeView, &tvInsert);
			bFound = FALSE;
		}
		else
			bFound = FindNextFileW(hFile, &fd);
	}//while
}

void CTree_View::Expand(HTREEITEM hItem)
{
	TreeView_Expand(m_hTreeView, hItem, TVE_EXPAND);
}
