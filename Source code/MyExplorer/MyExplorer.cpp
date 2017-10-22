// MyExplorer.cpp : Defines the entry point for the application.
//
#include "stdafx.h"
#include "MyExplorer.h"
#include<windows.h>
#include <commctrl.h>
#include"CDrive.h"
#include"CTree_View.h"
#include<windowsX.h>
#include"CList_View.h"
#include"macro.h"
#include<processenv.h>
#include<WinUser.h>
#include <sstream>
RECT main;
#define MAX_LOADSTRING 100
#define CLEAR					0

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name
CTree_View *g_TreeView;
CDrive *g_Drive;
CListView *g_ListView;
BOOL g_bStarted = FALSE;
const int BUFFERSIZE = 260;
WCHAR buffer[BUFFERSIZE];
WCHAR curPath[BUFFERSIZE];
WCHAR configPath[BUFFERSIZE];
static POINT bd;
BOOL fDragMode;
int width = 200;

// Tạo đường dẫn tuyệt đối tới file config




void OnSize(HWND hwnd, UINT state, int cx, int cy);
void OnDestroy(HWND hwnd);
BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct);
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
LRESULT OnNotify(HWND hwnd, int idFrom, NMHDR *pnm);
// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT OnNotify(HWND hwnd, int idFrom, NMHDR *pnm);



int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Place code here.

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_MYEXPLORER, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_MYEXPLORER));

    MSG msg;

    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_MYEXPLORER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_BTNFACE+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_MYEXPLORER);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	GetCurrentDirectory(BUFFERSIZE, curPath);
	wsprintf(configPath, L"%s\\config.ini", curPath);
	GetPrivateProfileString(L"app", L"sizex", L"1280" , buffer, BUFFERSIZE, configPath);
	int cx = _wtoi(buffer);
	GetPrivateProfileString(L"app", L"sizey", L"720", buffer, BUFFERSIZE, configPath);
	int cy = _wtoi(buffer);
   hInst = hInstance; // Store instance handle in our global variable
   UINT nx_size = GetSystemMetrics(SM_CXSCREEN);
   UINT ny_size = GetSystemMetrics(SM_CYSCREEN);

   UINT nx_pos = (nx_size - WINDOW_WIDTH) / 2;
   UINT ny_pos = (ny_size - WINDOW_HEIGHT) / 2;

   HWND hWnd = CreateWindowW(szWindowClass, _T("My Explorer - Phạm Huỳnh Trung -1512617"), WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, cx, cy, nullptr, nullptr, hInstance, nullptr);

   if (!hWnd)
   {
      return FALSE;
   }

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
static HCURSOR hcSizeEW = NULL;
RECT rect;
PAINTSTRUCT ps;
HDC hdc;
static HWND		hclose_button = NULL;
static HWND		hcolor_button = NULL;
static HWND	focus = NULL;
static  int         nleftWnd_width = 0;
HWND m_hStatus = NULL;
// Variable which informs whether the splitter is dragged
// or not
static  BOOL xSizing;
static UINT	nwnd_resizing = CLEAR;

static HBRUSH hBrush;
static HICON hIcon;


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
		HANDLE_MSG(hWnd, WM_DESTROY, OnDestroy);
		HANDLE_MSG(hWnd, WM_CREATE, OnCreate);
		HANDLE_MSG(hWnd, WM_COMMAND, OnCommand);
		HANDLE_MSG(hWnd, WM_NOTIFY, OnNotify);
		HANDLE_MSG(hWnd, WM_SIZE, OnSize);
	case WM_LBUTTONUP:
		ReleaseCapture();
		fDragMode = FALSE;
		break;
	case WM_LBUTTONDOWN:
		bd.x = LOWORD(lParam);
		bd.y = HIWORD(lParam);
		fDragMode = TRUE;
		SetCapture(hWnd);
		break;
	case WM_MOUSEMOVE:
	{
		
		RECT rect;
		GetClientRect(hWnd, &rect);
		int x, y;
		x = LOWORD(lParam);
		y = HIWORD(lParam);
		if (x >= width && x <= width + 5)
			SetCursor(LoadCursor(NULL, IDC_SIZEWE));

		if (fDragMode == TRUE)
		{
			width = width + x - bd.x;
			if (width >= rect.left + 50 && width <= rect.right - 100)
			{
				MoveWindow(g_TreeView->GetHandle(), 0, 0, width, rect.bottom - 5, 1);
				MoveWindow(g_ListView->GetHandle(), width + 5, 0, rect.right - width - 10, rect.bottom - 5, 1);
			}
			bd.x = x; bd.y = y;
		}
		break;
	}
	break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}
void OnSize(HWND hwnd, UINT state, int cx, int cy)
{
	GetClientRect(hwnd, &rect);
	GetCurrentDirectory(BUFFERSIZE, curPath);
	wsprintf(configPath, L"%s\\config.ini", curPath);
	wchar_t buffer[256];
	wsprintfW(buffer, L"%d", cx);
	
	WritePrivateProfileString(L"app", L"sizex", buffer, configPath);
	wsprintfW(buffer, L"%d", cy);
	WritePrivateProfileString(L"app", L"sizey", buffer, configPath);
	// Call Api to move and adjust the left window postion and its
	// height and width

	MoveWindow(g_TreeView->GetHandle(), rect.left,
		rect.top + TOP_POS,
		rect.left + (nleftWnd_width - WIDTH_ADJUST),
		(rect.bottom - (TOP_POS + BOTTOM_POS)),
		FALSE);

	// Call Api to move and adjust the right window postion and its
	// height and width
	MoveWindow(g_ListView->GetHandle(), rect.left + nleftWnd_width + WIDTH_ADJUST,
		rect.top + TOP_POS,
		rect.right - (nleftWnd_width + WIDTH_ADJUST),
		rect.bottom - (TOP_POS + BOTTOM_POS),
		FALSE);

	RECT main;
	GetWindowRect(hwnd, &main);

	int nStatusSize[] = { main.right * 6 / 9, main.right * 6 / 9 + main.right / 12 , -1 };
	SendMessage(m_hStatus, SB_SETPARTS, 3, (LPARAM)&nStatusSize);
	MoveWindow(m_hStatus, 0, 0, main.right, main.bottom, TRUE);

	InvalidateRect(hwnd, &rect, TRUE);
}
void OnDestroy(HWND hwnd)
{
	PostQuitMessage(0);
}
// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}
void OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
	// Parse the menu selections:
	switch (id)
	{
	case IDM_ABOUT:
		DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hwnd, About);
		break;
	case IDM_EXIT:
		DestroyWindow(hwnd);
		break;
	}
}

LRESULT OnNotify(HWND hwnd, int idFrom, NMHDR *pnm)
{
	int nCurSelIndex=-1;
	LPNMTOOLBAR lpnmToolBar = (LPNMTOOLBAR)pnm;

	if (g_bStarted) //Để tránh vòng lặp lẩn quẩn T_T
	{
		LPNMTREEVIEW lpnmTree = (LPNMTREEVIEW)pnm;

		switch (pnm->code)
		{
		case TVN_ITEMEXPANDING:
			g_TreeView->PreloadExpanding(lpnmTree->itemOld.hItem, lpnmTree->itemNew.hItem);
			break;
			//------------------------------------------------------------------------------
		case TVN_SELCHANGED:
			g_TreeView->Expand(g_TreeView->GetCurSel());
			g_ListView->DeleteAll(); //Xóa sạch List View để nạp cái mới
			g_ListView->LoadChild(g_TreeView->GetCurPath(), g_Drive);
			break;
			//------------------------------------------------------------------------------
		case NM_CLICK:
			if (pnm->hwndFrom == g_ListView->GetHandle())
			{
				nCurSelIndex = ListView_GetNextItem(g_ListView->GetHandle(), -1, LVNI_FOCUSED);
			}
			if (nCurSelIndex != -1)
				g_ListView->DisplayInfoCurSel();
			break;
		case NM_DBLCLK:
			if (pnm->hwndFrom == g_ListView->GetHandle())
				g_ListView->LoadCurSel();
			break;
		}//switch
	}//if

	return 0;
}

BOOL OnCreate(HWND hwnd, LPCREATESTRUCT lpCreateStruct)
{
	hcSizeEW = LoadCursor(NULL, IDC_SIZEWE);

	nleftWnd_width = LEFT_WINDOW_WIDTH;

	GetClientRect(hwnd, &rect);
	g_Drive = new CDrive;
	g_Drive->GetSystemDrives();

	m_hStatus = CreateWindowEx(0, STATUSCLASSNAME, NULL, WS_CHILD | WS_VISIBLE | SBARS_SIZEGRIP | SBARS_TOOLTIPS,0, 700, 1000, 20, hwnd, (HMENU)IDC_STATUSBAR, hInst, NULL);

	g_TreeView = new CTree_View;
	g_TreeView->Create(hwnd, IDC_TREEVIEW, hInst, LEFT_WINDOW_WIDTH,
		(rect.bottom - rect.top) - (TOP_POS + BOTTOM_POS), rect.left,
		rect.top + TOP_POS);

	g_TreeView->LoadMyComputer(g_Drive);
	g_TreeView->GetFocus();

	g_ListView = new CListView;
	g_ListView->Create(hwnd, IDC_LISTVIEW, hInst, rect.right - (rect.left + LEFT_WINDOW_WIDTH + SPLITTER_BAR_WIDTH),
		(rect.bottom - rect.top) - (TOP_POS + BOTTOM_POS),
		rect.left + LEFT_WINDOW_WIDTH + SPLITTER_BAR_WIDTH,
		rect.top + TOP_POS);

	g_ListView->LoadMyComputer(g_Drive);
	g_bStarted = TRUE;
	return TRUE;
}