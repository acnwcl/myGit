#include "stdafx.h"
#include "MainWnd.h"


CMainWnd::CMainWnd()
{
	m_btnTop = NULL;
	m_btnRestoreTop = NULL;
	m_btnSetting = NULL;
	m_btnMin = NULL;
	m_btnMax = NULL;
	m_btnRestore = NULL;
	m_btnClose = NULL;
	m_btnAddTab = NULL;
	m_btnGo = NULL;

	m_hlConent = NULL;
	m_hlTab = NULL;
	m_hlTool = NULL;

	m_tabView = NULL;
	m_op1 = NULL;
	m_editUrl = NULL;

	m_iOpCount = 0;
	m_iOpCurrent = 0;

	m_bIsTop = false;

	// menu
	m_PngImageList = NULL;

	m_spBrowser = new CefClientHandler;
}
CMainWnd::~CMainWnd()
{
}

void CMainWnd::Init()
{
	m_btnTop = static_cast<CButtonUI*>(m_pm.FindControl(_T("top")));
	m_btnRestoreTop = static_cast<CButtonUI*>(m_pm.FindControl(_T("restoretop")));
	m_btnSetting = static_cast<CButtonUI*>(m_pm.FindControl(_T("setting")));
	m_btnMin = static_cast<CButtonUI*>(m_pm.FindControl(_T("min")));
	m_btnMax = static_cast<CButtonUI*>(m_pm.FindControl(_T("max")));
	m_btnRestore = static_cast<CButtonUI*>(m_pm.FindControl(_T("restore")));
	m_btnClose = static_cast<CButtonUI*>(m_pm.FindControl(_T("close")));
	m_btnAddTab = static_cast<CButtonUI*>(m_pm.FindControl(_T("addtab")));
	m_btnGo = static_cast<CButtonUI*>(m_pm.FindControl(_T("go")));

	m_hlConent = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("content")));
	m_hlTab = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("tab")));
	m_hlTool = static_cast<CHorizontalLayoutUI*>(m_pm.FindControl(_T("tool")));

	m_tabView = static_cast<CTabLayoutUI*>(m_pm.FindControl(_T("tabView")));
	m_op1 = static_cast<COptionUI*>(m_pm.FindControl(_T("op1")));
	m_editUrl = static_cast<CEditUI*>(m_pm.FindControl(_T("url")));

	// menu
	GetSystemSettings();
	m_PngImageList = GetImage(IDB_MENU);

	m_pm.SendNotify(m_btnAddTab, MST_CLICK);

	// 注册热键
	RegisterHotKey(m_hWnd, 0x01, MOD_ALT, 67); // atl + c
}
void CMainWnd::OnPrepare(void)//完成时
{
}
UINT CMainWnd::GetClassStyle() const
{
	return CS_DBLCLKS;
}
LPCTSTR CMainWnd::GetWindowClassName() const
{
	return _T("tem");
}

//////////////////////////////////////////////////////////////////////////////////// command message

LRESULT CMainWnd::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	CDialogBuilder builder;

	CTool tool;
	CAtlString path = tool.GetAppPath();
	path.Append(_T("skin\\skin.sk"));

	m_pm.Init(m_hWnd);
	_gSkinWnd.Add(&m_pm);
	CControlUI* pRoot = builder.Create(path.GetBuffer(), _T("main.xml"), (UINT)0, NULL, &m_pm);
	m_pm.AttachDialog(pRoot);
	m_pm.AddNotifier(this);

	Init();

	return 1;
}
LRESULT CMainWnd::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	m_spBrowser->CloseAllBrowser();
	CefQuitMessageLoop();
	return 1;
}
LRESULT CMainWnd::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
	::ScreenToClient(*this, &pt);

	RECT rcClient;
	::GetClientRect(*this, &rcClient);

	if (!::IsZoomed(*this)) {
		RECT rcSizeBox = { 2, 2, 3, 3 };
		if (pt.y > rcClient.bottom - 10 && pt.x > rcClient.right - 10) return HTBOTTOMRIGHT;
		else if (pt.y < rcClient.top + rcSizeBox.top) {
			if (pt.x < rcClient.left + rcSizeBox.left) return HTTOPLEFT;
			if (pt.x > rcClient.right - rcSizeBox.right) return HTTOPRIGHT;
			return HTTOP;
		}
		else if (pt.y > rcClient.bottom - rcSizeBox.bottom) {
			if (pt.x < rcClient.left + rcSizeBox.left) return HTBOTTOMLEFT;
			return HTBOTTOM;
		}
		if (pt.x < rcClient.left + rcSizeBox.left) return HTLEFT;
		if (pt.x > rcClient.right - rcSizeBox.right) return HTRIGHT;
	}

	if (::PtInRect(&rcClient, pt))
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
		if (pControl && _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 &&
			_tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
			_tcscmp(pControl->GetClass(), _T("TextUI")) != 0)
			return HTCAPTION;
	}

	return HTCLIENT;
}
LRESULT CMainWnd::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	if (IsZoomed(m_hWnd))
	{
		m_btnMax->SetVisible(false);
		m_btnRestore->SetVisible();
	}
	else
	{
		m_btnMax->SetVisible();
		m_btnRestore->SetVisible(false);
	}

	bHandled = FALSE;
	return 0;
}
LRESULT CMainWnd::OnGetMinMaxInfo(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	//MONITORINFO oMonitor = {};
	//oMonitor.cbSize = sizeof(oMonitor);
	//::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);

	//LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;

	//lpMMI->ptMaxPosition.x = oMonitor.rcWork.left;
	//lpMMI->ptMaxPosition.y = oMonitor.rcWork.top;

	//// normal wnd max size
	//lpMMI->ptMaxSize.x = oMonitor.rcWork.right - oMonitor.rcWork.left;
	//lpMMI->ptMaxSize.y = oMonitor.rcWork.bottom - oMonitor.rcWork.top;

	////normal wnd min size
	//lpMMI->ptMinTrackSize.x = 1000;
	//lpMMI->ptMinTrackSize.y = 700;


	//bHandled = TRUE;
	return 0;
}
LRESULT CMainWnd::OnLButtonDown(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = FALSE;
	return 1;
}

// menu
void CMainWnd::AssociateImage(CMenuItemInfo& mii, MenuItemData * pMI)
{
	switch (mii.wID)
	{
	case ID_MIN:
		pMI->iImage = 1;
		break;
	case ID_EXIT:
		pMI->iImage = 2;
		break;
	default:
		pMI->iImage = -1;
		break;
	}
}
LRESULT CMainWnd::OnExit(DWORD hWrod, DWORD lWord, HWND hWnd, BOOL& bHandled)
{
	SendMessage(WM_CLOSE);
	return 0;
}
LRESULT CMainWnd::OnMin(DWORD hWrod, DWORD lWord, HWND hWnd, BOOL& bHandled)
{
	m_pm.SendNotify(m_btnMin, MST_CLICK);
	return 0;
}

// borwser
LRESULT CMainWnd::OnDocumentComplete(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	bHandled = TRUE;
	CContainerConUI* pView = (CContainerConUI*)m_tabView->GetItemAt(m_iOpCurrent);
	ATLASSERT(pView);

	if (!pView->IsAttached())//如果browser未加入container
		pView->SetWindow((HWND)lParam);

	return 1;
}

// 热键
LRESULT CMainWnd::OnHotKey(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
	switch (wParam)
	{
	case 1:
		SendMessage(WM_CLOSE);
		break;
	}
	return 1;
}

//////////////////////////////////////////////////////////////////////////////////// notify message

void CMainWnd::OnClick(TNotifyUI& msg)
{
	if (msg.pSender == m_btnTop)	// 置顶
	{
		m_bIsTop = true;
		m_btnTop->SetVisible(false);
		m_btnRestoreTop->SetVisible();
		SetWindowPos(m_hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}
	else if (msg.pSender == m_btnRestoreTop)	// 取消置顶
	{
		m_bIsTop = false;
		m_btnTop->SetVisible();
		m_btnRestoreTop->SetVisible(false);
		SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_SHOWWINDOW);
	}
	else if (msg.pSender == m_btnSetting)	// 菜单
	{
		if (m_menu.m_hMenu != NULL)
			return;

		// Cursor's horizontal and vertical position
		/*WTL::CPoint pt(GET_X_LPARAM(msg.lParam), GET_Y_LPARAM(msg.lParam)); */
		WTL::CPoint pt(msg.ptMouse.x, msg.ptMouse.y);

		// Convert client coordinates to screen coordinates 
		ClientToScreen(m_hWnd, &pt);

		// Load the menu resource 
		if (!m_menu.LoadMenu(IDR_POPUP_RIGHT_MENU_MAIN))
		{
			ATLTRACE(_T("Menu resource was not loaded successfully!\n"));
			return;
		}

		// TrackPopupMenu cannot display the menu bar so get 
		// a handle to the first shortcut menu. 
		CMenuHandle menuPopup = m_menu.GetSubMenu(0);
		if (menuPopup.m_hMenu == NULL)
		{
			ATLTRACE(_T("Submenu was not retrieved successfully!\n"));
			return;
		}

		// Display the shortcut menu
		if (!menuPopup.TrackPopupMenu(TPM_LEFTALIGN | TPM_TOPALIGN | TPM_LEFTBUTTON,
			pt.x, pt.y, m_hWnd))
		{
			ATLTRACE(_T("Context menu was not displayed successfully!\n"));
			return;
		}

		// Destroy the menu and free any memory that the menu occupies
		menuPopup.DestroyMenu();
		m_menu.DestroyMenu();
		m_menu.m_hMenu = NULL;
	}
	else if (msg.pSender == m_btnMin)	// 最小化
	{
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
	}
	else if (msg.pSender == m_btnMax)	// 最大化
	{
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
	}
	else if (msg.pSender == m_btnRestore)	// 还原最大化
	{
		SendMessage(WM_SYSCOMMAND, SC_RESTORE);
	}
	else if (msg.pSender == m_btnAddTab)	// 添加标签
	{
		// 添加标签
		COptionUI *op = new COptionUI;
		CAtlString name;
		name.Format(_T("新标签 %d"), m_iOpCount);
		op->SetName(name);
		op->SetText(name);
		op->SetBkColor(0xFF123456);
		op->SetTextColor(0xFFffffff);
		op->SetMaxWidth(100);
		op->SetSelectBkColor(0xFF0000ff);
		op->SetSelectedTextColor(0xFFff0000);
		op->SetBorderSize(1);
		op->SetBorderColor(0xFF00ff00);
		RECT rc;
		rc.left = 5;
		rc.top = 10;
		rc.right = 0;
		rc.bottom = 0;
		op->SetPadding(rc);
		op->SetGroup(true);
		m_hlTab->AddAt(op, m_iOpCount);
		op->Selected(true);

		// 添加浏览器容器
		CContainerConUI *con = new CContainerConUI;
		ATLASSERT(con);
		con->SetBkColor(0xFFffffff);
		
		ATLASSERT(m_tabView->AddAt(con,m_iOpCount));
		ATLASSERT(m_tabView->SelectItem(m_iOpCount));

		m_iOpCount++;
	}
	else if (msg.pSender == m_btnGo)	// 链接 url
	{
		CefWindowInfo info;
		RECT rc = { 0, 0, 0, 0 };
		info.SetAsChild(m_hWnd, rc);

		CefBrowserSettings setting;

		ATLASSERT(CefBrowserHost::CreateBrowser(info, m_spBrowser.get(), "www.baidu.com", setting));
	}
	else if (msg.pSender == m_btnClose)		// 退出
	{
		SendMessage(WM_CLOSE);
	}
}
void CMainWnd::OnSelectChanged(TNotifyUI& msg)
{
	m_iOpCurrent = m_hlTab->GetItemIndex(msg.pSender);
	m_tabView->SelectItem(m_iOpCurrent);
}
void CMainWnd::OnHeaderClick(TNotifyUI& msg)
{

}
