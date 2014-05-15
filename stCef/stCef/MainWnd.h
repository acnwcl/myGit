#pragma once
#include "Resource.h"

#include "CefClientHandler.h"
#include "CefClientApp.h"


#include "OwnerDrawMenu.h"
#include "UIlib.h"
#include "win.h"

#include "ContainerConUI.h"

#include "Global.h"
#include "Tool.h"

using namespace UILIB_LIB;

extern CStdPtrArray _gSkinWnd;

class CMainWnd : public CWindowWnd, public INotifyUI, public CCoolContextMenu<CMainWnd>
{
public:
	CMainWnd();
	~CMainWnd();
	void Init();
	void OnPrepare();
	UINT GetClassStyle() const;
	LPCTSTR GetWindowClassName() const;

	////////////////////////////////////////////////////////////////////////////////////

	// menu
	void AssociateImage(CMenuItemInfo& mii, MenuItemData * pMI);

	////////////////////////////////////////////////////////////////////////////////////

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	//menu
	LRESULT OnMin(DWORD hWrod, DWORD lWord, HWND hWnd, BOOL& bHandled);
	LRESULT OnExit(DWORD hWrod, DWORD lWord, HWND hWnd, BOOL& bHandled);

	//browser
	LRESULT OnDocumentComplete(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	//ÈÈ¼ü
	LRESULT OnHotKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	BEGIN_MSG_MAP_DIRECT(this)
		MESSAGE_HANDLER_DIRECT(WM_CREATE, OnCreate)
		MESSAGE_HANDLER_DIRECT(WM_CLOSE, OnClose)
		MESSAGE_HANDLER_DIRECT(WM_NCHITTEST, OnNcHitTest)
		MESSAGE_HANDLER_DIRECT(WM_SIZE, OnSize)
		MESSAGE_HANDLER_DIRECT(WM_GETMINMAXINFO, OnGetMinMaxInfo)
		MESSAGE_HANDLER_DIRECT(WM_LBUTTONDOWN, OnLButtonDown)
		//menu
		MESSAGE_HANDLER_DIRECT(WM_INITMENUPOPUP, OnInitMenuPopup)
		MESSAGE_HANDLER_DIRECT(WM_MENUSELECT, OnMenuSelect)
		MESSAGE_HANDLER_DIRECT(WM_DRAWITEM, OnDrawItem)
		MESSAGE_HANDLER_DIRECT(WM_MEASUREITEM, OnMeasureItem)
		COMMAND_ID_HANDLER(ID_MIN, OnMin)
		COMMAND_ID_HANDLER(ID_EXIT, OnExit)
		//browser
		MESSAGE_HANDLER_DIRECT(WM_BROWSERDOCUMENTCOMPLETE, OnDocumentComplete)
		//ÈÈ¼ü
		MESSAGE_HANDLER_DIRECT(WM_HOTKEY, OnHotKey)
	END_MSG_MAP_DIRECT

	////////////////////////////////////////////////////////////////////////////////////

	void OnClick(TNotifyUI& msg);
	void OnSelectChanged(TNotifyUI& msg);
	void OnHeaderClick(TNotifyUI& msg);

	BEGIN_NOTIFY_MAP_DIRECT(this)
		NOTIFY_HANDLER_DIRECT(MST_CLICK, OnClick)
		NOTIFY_HANDLER_DIRECT(MST_SELECTCHANGED, OnSelectChanged)
		NOTIFY_HANDLER_DIRECT(MST_HEADERCLICK, OnHeaderClick)
	END_NOTIFY_MAP_DIRECT

	////////////////////////////////////////////////////////////////////////////////////
	
private:
	CPaintManagerUI m_pm;
	CButtonUI *m_btnTop, *m_btnRestoreTop, *m_btnSetting, *m_btnMin, *m_btnMax, *m_btnRestore, *m_btnClose,*m_btnAddTab,*m_btnGo;
	CHorizontalLayoutUI *m_hlConent,*m_hlTab,*m_hlTool;
	CTabLayoutUI *m_tabView;
	COptionUI *m_op1;
	CEditUI *m_editUrl;
	bool m_bIsTop;
	int m_iOpCount,m_iOpCurrent;

	CMenu m_menu;		// context menu
	HCURSOR m_hCursor;

	CefRefPtr<CefClientHandler> m_spBrowser;
};

