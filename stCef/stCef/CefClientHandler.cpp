#include "StdAfx.h"
#include "CefClientHandler.h"

CefClientHandler::CefClientHandler()
{
}

CefClientHandler::~CefClientHandler()
{
}

// CefLifeSpanHandler
void CefClientHandler::OnAfterCreated(CefRefPtr<CefBrowser> browser)
{
	_ASSERT(browser.get());
	m_vectorBrowser.push_back(browser);
}

// CefLoadHandler
void CefClientHandler::OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode)
{
	REQUIRE_UI_THREAD();

	HWND root_wnd = ::GetAncestor(browser->GetHost()->GetWindowHandle(), GA_ROOT);
	
	::SendMessage(root_wnd, WM_BROWSERDOCUMENTCOMPLETE, 0, (LPARAM)browser->GetHost()->GetWindowHandle());
}

// CefClientHandler
CefRefPtr<CefBrowser> CefClientHandler::GetBrowserByIndex(int index)
{
	_ASSERT(index < m_vectorBrowser.size());
	return m_vectorBrowser.at(index);
}

void CefClientHandler::CloseBrowserByIndex(int index)
{
	GetBrowserByIndex(index)->GetHost()->CloseBrowser(true);
	m_vectorBrowser.erase(m_vectorBrowser.begin() + index);
}

void CefClientHandler::CloseAllBrowser()
{
	for (int i = 0; i < m_vectorBrowser.size(); i++)
	{
		m_vectorBrowser.at(i)->GetHost()->CloseBrowser(true);
	}

	m_vectorBrowser.clear();
}
