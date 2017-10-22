#include"stdafx.h"
#include "CList_View.h"
#include <tchar.h>
#include "CDrive.h"
#include<vector>
#include "resource.h"
using namespace std;
#include <shlwapi.h>
#pragma comment(lib, "shlwapi.lib")

#define I_FOLDER 0
#define I_FILE 1
#define I_MYCOMPUTER 2
#define I_FLOPPY 3
#define I_USB 4
#define I_HDD 5
#define I_CD  6 


#define MAX_PATH_LEN 10240

CListView::CListView()
{
	m_hInst = NULL;
	m_hParent = NULL;
	m_hListView = NULL;
	m_nID = 0;
}

CListView::~CListView()
{
	DestroyWindow(m_hListView);
}

void CListView::Create(HWND parentWnd, long ID, HINSTANCE hParentInst,
	int nWidth, int nHeight,
	int x, int y, long lExtStyle, long lStyle)
{
	InitCommonControls();
	CListView::m_hInst = hParentInst;
	CListView::m_hParent = parentWnd;
	CListView::m_hListView = CreateWindowEx(lExtStyle, WC_LISTVIEWW, _T("List View"),
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_TABSTOP |LVS_REPORT|WS_HSCROLL| lStyle,
		x, y, nWidth, nHeight, m_hParent, (HMENU)ID, m_hInst, NULL);
	CListView::m_nID = ID;
	Init4Cols();
	InitImageList();
}
void CListView::DisplayInfoCurSel()
{
	int nCurSelIndex = ListView_GetNextItem(GetDlgItem(m_hParent, IDC_LISTVIEW), -1, LVNI_FOCUSED);
	TCHAR *text = new TCHAR[256];
	LVITEM lv;
	lv.mask = LVIF_TEXT;
	lv.iItem = nCurSelIndex;
	lv.iSubItem = 0;
	lv.pszText = text;
	lv.cchTextMax = 256;

	ListView_GetItem(m_hListView, &lv);
	SendMessage(GetDlgItem(m_hParent, IDC_STATUSBAR), SB_SETTEXT, 2, (LPARAM)text);

	lv.iSubItem = 2;
	ListView_GetItem(m_hListView, &lv);

	if (!StrCmpI(lv.pszText, _T("Thư mục")))
		SendMessage(GetDlgItem(m_hParent, IDC_STATUSBAR), SB_SETTEXT, 1, NULL);
	else
	{
		lv.iSubItem = 1;
		ListView_GetItem(m_hListView, &lv);
		SendMessage(GetDlgItem(m_hParent, IDC_STATUSBAR), SB_SETTEXT, 1, (LPARAM)text);
	}

}
void CListView::InitImageList()
{
	InitCommonControls();
	HIMAGELIST *hLarge = new HIMAGELIST;
	HIMAGELIST *hSmall = new HIMAGELIST;

	*hSmall = ImageList_Create(16, 16, ILC_COLOR32 | ILC_MASK,
		8,0);

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

	ListView_SetImageList(m_hListView, *hSmall, LVSIL_SMALL);
}
void CListView::Init4Cols()
{
	LVCOLUMN lvCol;

	lvCol.mask = LVCF_FMT | LVCF_TEXT | LVCF_WIDTH;
	lvCol.fmt = LVCFMT_LEFT;

	lvCol.cx = 130;
	lvCol.pszText = _T("Tên");
	ListView_InsertColumn(m_hListView, 0, &lvCol);

	lvCol.fmt = LVCFMT_LEFT | LVCF_WIDTH;
	lvCol.cx = 100;
	lvCol.pszText = _T("Loại");
	ListView_InsertColumn(m_hListView, 1, &lvCol);

	lvCol.fmt = LVCFMT_RIGHT;
	lvCol.pszText = _T("Kích thước");
	ListView_InsertColumn(m_hListView, 2, &lvCol);

	lvCol.pszText = _T("Mô tả");
	ListView_InsertColumn(m_hListView, 3, &lvCol);
}

void CListView::InitDriveColumn()
{
	LVCOLUMN lvCol;

	lvCol.mask = LVCF_TEXT | LVCF_FMT;

	lvCol.fmt = LVCFMT_LEFT | LVCF_WIDTH;
	lvCol.cx = 100;
	lvCol.pszText = _T("Loại");
	ListView_SetColumn(m_hListView, 1, &lvCol);
	lvCol.fmt = LVCFMT_RIGHT | LVCF_WIDTH;
	lvCol.cx = 80;
	lvCol.pszText = _T("");
	ListView_SetColumn(m_hListView, 2, &lvCol);

	lvCol.cx = 80;
	lvCol.pszText = _T("");
	ListView_SetColumn(m_hListView, 3, &lvCol);
}
void CListView::InitFolderColumn()
{
	LVCOLUMN lvCol;

	lvCol.mask = LVCF_WIDTH;
	lvCol.cx = 180;
	ListView_SetColumn(m_hListView, 0, &lvCol);

	lvCol.mask = LVCF_TEXT | LVCF_FMT;
	lvCol.fmt = LVCFMT_RIGHT;
	lvCol.pszText = _T("Kích thước");
	ListView_SetColumn(m_hListView, 1, &lvCol);


	lvCol.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_FMT;
	lvCol.fmt = LVCFMT_LEFT;
	lvCol.cx = 130;
	lvCol.pszText = _T("Loại tập tin");
	ListView_SetColumn(m_hListView, 2, &lvCol);

	lvCol.pszText = _T("Ngày chỉnh sửa");
	ListView_SetColumn(m_hListView, 3, &lvCol);
}
/*****************************************************************************************/



HWND CListView::GetHandle()
{
	return m_hListView;
}

LPCWSTR CListView::GetPath(int iItem)
{
	LVITEM lv;
	lv.mask = LVIF_PARAM;
	lv.iItem = iItem;
	lv.iSubItem = 0;
	ListView_GetItem(m_hListView, &lv);
	return (LPCWSTR)lv.lParam;
}

LPCWSTR CListView::GetCurSelPath()
{
	return GetPath(ListView_GetSelectionMark(m_hListView));
}


void CListView::LoadDesktop()
{
	DeleteAll();
	InitDriveColumn();

	LV_ITEM lv;
	lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
	lv.iItem = 0;
	lv.iSubItem = 0;
	lv.pszText = _T("My Computer");
	lv.iImage = I_MYCOMPUTER;
	lv.lParam = (LPARAM)_T("MyComputer");
	ListView_InsertItem(m_hListView, &lv);

}
void CListView::LoadMyComputer(CDrive *drive)
{
	DeleteAll();
	InitDriveColumn();
	
	LV_ITEM lv;

	for (int i = 0; i < drive->GetCount(); ++i)
	{
		lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM;
		lv.iItem = i;
		lv.iImage = drive->GetIconIndex(i);

		lv.iSubItem = 0;
		lv.pszText = drive->GetDisplayName(i);
		lv.lParam = (LPARAM)drive->GetDriveName(i);
		lv.iImage = drive->GetIconIndex(i);
		ListView_InsertItem(m_hListView, &lv);

		lv.mask = LVIF_TEXT;

		lv.iSubItem = 1;
		lv.pszText = drive->GetType(i);
		ListView_SetItem(m_hListView, &lv);

	}

}

void CListView::LoadChild(LPCWSTR path, CDrive *drive)
{
	if (path == NULL)
		return;

	if (!StrCmp(path, _T("Desktop")))
		LoadDesktop();
	else
		if (!StrCmp(path, _T("MyComputer")))
			LoadMyComputer(drive);
		else
			LoadFileAndFolder(path);
}
struct fileFolder_inf
{
	int iItem;
	LPWSTR filename;
	LPARAM folderpath;
	LPWSTR dateModify;
	LPWSTR size;
	LPWSTR type;
};
void CListView::LoadFileAndFolder(LPCWSTR path)
{
	DeleteAll();
	InitFolderColumn();
	

	TCHAR buffer[10240];
	StrCpy(buffer, path);

	if (wcslen(path) == 3) 
		StrCat(buffer, _T("*"));
	else
		StrCat(buffer, _T("\\*"));

	WIN32_FIND_DATA fd;
	HANDLE hFile;
	BOOL bFound = true;

	vector <fileFolder_inf> listFolderItem;//list chứa folder
	vector <fileFolder_inf> listFileItem;//list chứa file

	TCHAR * folderPath;
	int nItemCount = 0;
	int nFileCount = 0;
	TCHAR *filePath;
	DWORD fileSizeCount = 0;

	hFile = FindFirstFileW(buffer, &fd);
	bFound = TRUE;

	if (hFile == INVALID_HANDLE_VALUE)
		bFound = FALSE;

	while (bFound)
	{
		//Nếu là folder
		if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN) &&
			(StrCmp(fd.cFileName, _T(".")) != 0) && (StrCmp(fd.cFileName, _T("..")) != 0))
		{
			folderPath = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];
			StrCpy(folderPath, path);

			if (wcslen(path) != 3)
				StrCat(folderPath, _T("\\"));

			StrCat(folderPath, fd.cFileName);
			fileFolder_inf folder_inf;
			folder_inf.iItem = nItemCount;
			LPWSTR temp= new wchar_t[wcslen(fd.cFileName)];
			wcscpy(temp , fd.cFileName);
			folder_inf.filename = temp;
			folder_inf.folderpath = (LPARAM)folderPath;
			temp = new wchar_t[wcslen(_GetDateModified(fd.ftLastWriteTime))];
			wcscpy(temp, _GetDateModified(fd.ftLastWriteTime));
			folder_inf.dateModify = temp;
			listFolderItem.push_back(folder_inf);
			
			++nItemCount;
		}//Nếu là file
		else if (((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != FILE_ATTRIBUTE_SYSTEM) &&
			((fd.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != FILE_ATTRIBUTE_HIDDEN))
		{
			filePath = new TCHAR[wcslen(path) + wcslen(fd.cFileName) + 2];
			StrCpy(filePath, path);

			if (wcslen(path) != 3)
				StrCat(filePath, _T("\\"));

			StrCat(filePath, fd.cFileName);
			fileFolder_inf file_inf;

			file_inf.iItem = nFileCount;

			LPWSTR temp = new wchar_t[wcslen(fd.cFileName)];
			wcscpy(temp, fd.cFileName);
			file_inf.filename = temp;

			file_inf.folderpath = (LPARAM)folderPath;

			temp = new wchar_t[wcslen(_GetDateModified(fd.ftLastWriteTime))];
			wcscpy(temp, _GetDateModified(fd.ftLastWriteTime));
			file_inf.dateModify = temp;

			temp = new wchar_t[wcslen(_GetSize(fd))];
			wcscpy(temp, _GetSize(fd));
			file_inf.size = temp;
			
			listFileItem.push_back(file_inf);
			++nFileCount;
		}

		bFound = FindNextFileW(hFile, &fd);
	}//while

	for (size_t i = 0; i < listFolderItem.size(); i++)// insert folder trước
	{
		LV_ITEM lv;
		lv.mask = LVIF_TEXT | LVIF_PARAM|LVIF_IMAGE;
		lv.iItem = listFolderItem[i].iItem;
		lv.iSubItem = 0;
		lv.pszText = listFolderItem[i].filename;
		lv.lParam = listFolderItem[i].folderpath;
		lv.iImage = I_FOLDER;
		ListView_InsertItem(m_hListView, &lv);
		ListView_SetItemText(m_hListView, lv.iItem, 2, _T("Thư mục"));

		//Date modified
		ListView_SetItemText(m_hListView, lv.iItem, 3, listFolderItem[i].dateModify);
	}
	for (size_t i = 0; i < listFileItem.size(); i++)//insert tập tin
	{
		LV_ITEM lv;
		lv.mask = LVIF_TEXT | LVIF_PARAM|LVIF_IMAGE;
		lv.iItem = listFileItem[i].iItem+listFolderItem.size();
		lv.iSubItem = 0;
		lv.pszText = listFileItem[i].filename;
		lv.lParam = listFileItem[i].folderpath;
		lv.iImage = I_FILE;
		ListView_InsertItem(m_hListView, &lv);
	
		//Cột thứ hai là Size
		ListView_SetItemText(m_hListView, lv.iItem, 1,listFileItem[i].size);

		////Type
		ListView_SetItemText(m_hListView, lv.iItem, 2, _T("Tập tin"));
		//Date modified
		ListView_SetItemText(m_hListView, lv.iItem, 3, listFileItem[i].dateModify);
	}
	/*************************************************************************************/
}

void CListView::LoadCurSel()
{

	LPCWSTR path = GetCurSelPath();

	WIN32_FIND_DATA fd;
	GetFileAttributesEx(path, GetFileExInfoStandard, &fd);

	if (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
	{
		ListView_DeleteAllItems(m_hListView);
		LoadFileAndFolder(path);
	}
}


/*****************************************************************************************/


void CListView::DeleteAll()
{
	ListView_DeleteAllItems(m_hListView);
}


/*****************************************************************************************/



#define ENTIRE_STRING NULL

LPWSTR CListView::_GetSize(const WIN32_FIND_DATA & fd)
{
	DWORD dwSize = fd.nFileSizeLow;

	return Convert(dwSize);
}

LPWSTR CListView::_GetDateModified(const FILETIME &ftLastWrite)
{

	//Chuyển đổi sang local time
	SYSTEMTIME stUTC, stLocal;
	FileTimeToSystemTime(&ftLastWrite, &stUTC);
	SystemTimeToTzSpecificLocalTime(NULL, &stUTC, &stLocal);

	TCHAR *buffer = new TCHAR[50];
	wsprintf(buffer, _T("%02d/%02d/%04d %02d:%02d %s"),
		stLocal.wDay, stLocal.wMonth, stLocal.wYear,
		(stLocal.wHour>12) ? (stLocal.wHour / 12) : (stLocal.wHour),
		stLocal.wMinute,
		(stLocal.wHour>12) ? (_T("PM")) : (_T("AM")));

	return buffer;
}
/*****************************************************************************************/

LPWSTR Convert(__int64 nSize)
{
	int nType = 0; //Bytes

	while (nSize >= 1048576) //
	{
		nSize /= 1024;
		++nType;
	}

	__int64 nRight;

	if (nSize >= 1024)
	{
		//Lấy một chữ số sau thập phân của nSize chứa trong nRight
		nRight = nSize % 1024;

		while (nRight > 99)
			nRight /= 10;

		nSize /= 1024;
		++nType;
	}
	else
		nRight = 0;

	TCHAR *buffer = new TCHAR[11];
	_itow_s(nSize, buffer, 11, 10);

	if (nRight != 0 && nType > 1)
	{
		StrCat(buffer, _T("."));
		TCHAR *right = new TCHAR[3];
		_itow_s(nRight, right, 3, 10);
		StrCat(buffer, right);
	}

	switch (nType)
	{
	case 0://Bytes
		StrCat(buffer, _T(" bytes"));
		break;
	case 1:
		StrCat(buffer, _T(" KB"));
		break;
	case 2:
		StrCat(buffer, _T(" MB"));
		break;
	case 3:
		StrCat(buffer, _T(" GB"));
		break;
	case 4:
		StrCat(buffer, _T(" TB"));
		break;
	}

	return buffer;
}