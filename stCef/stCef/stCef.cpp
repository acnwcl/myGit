// uilibTemplate.cpp : 定义应用程序的入口点。
//

#include "stdafx.h"
#include "stCef.h"
#include "MainWnd.h"

#define MAX_LOADSTRING 100

// 全局变量: 
CComModule _Module;
CStdPtrArray _gSkinWnd;
HINSTANCE hInst;								// 当前实例
TCHAR szTitle[MAX_LOADSTRING];					// 标题栏文本
TCHAR szWindowClass[MAX_LOADSTRING];			// 主窗口类名

CAtlString AppGetAppPath()
{
	TCHAR path[MAX_PATH];
	::GetModuleFileName(NULL, (LPWCH)path, MAX_PATH);
	CAtlString returnStringPath = path;
	int index = returnStringPath.ReverseFind(_T('\\'));
	if (index == -1) returnStringPath.Empty();
	else returnStringPath = returnStringPath.Left(index + 1);
	return returnStringPath;
}

const CAtlString GetFullPath()
{
	CAtlString path = AppGetAppPath() + _T("FilmEveryday.exe");
	return path;
}

// Quit the application message loop.
void AppQuitMessageLoop()
{

	CefQuitMessageLoop();
}

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPTSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	//每个进程都会执行｛
	CefMainArgs main_args(hInstance);
	CefRefPtr<CefClientApp> app(new CefClientApp);

	// Execute the secondary process, if any.
	int exit_code = CefExecuteProcess(main_args, app.get());
	if (exit_code >= 0)
		return exit_code;
	//}
	CefSettings cefSettings;
	{
		CefString(&cefSettings.cache_path) = AppGetAppPath() + _T("FilmEveryday_cookies_and_storge");
		cefSettings.persist_session_cookies = true;
	}
	// Initialize CEF.
	CefInitialize(main_args, cefSettings, app.get());

	HACCEL hAccelTable;

	// 初始化全局字符串
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_STCEF, szWindowClass, MAX_LOADSTRING);

	HRESULT Hr = ::OleInitialize(NULL);
	if (FAILED(Hr)) return 0;
	_pAtlModule = &_Module;

	CMainWnd MainWnd;
	MainWnd.Create(NULL, _T("uilibTemplate"), UI_WNDSTYLE_FRAME, WS_EX_WINDOWEDGE);
	MainWnd.CenterWindow();
	MainWnd.ShowWindow();
	MainWnd.SendMessageW(WM_SYSCOMMAND, SC_MAXIMIZE);

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_STCEF));

	//CPaintManagerUI::MessageLoop();
	CefRunMessageLoop();
	CefShutdown();
	
	::CoUninitialize();
	return 0;
}


