#pragma once
#include "include\cef_client.h"
#include <vector>

#include "util.h"
#include "win.h"

class CefClientHandler : public CefClient,
	public CefLifeSpanHandler,
	public CefLoadHandler
{
public:
	CefClientHandler();
	~CefClientHandler();

	// CefClient
	virtual CefRefPtr<CefLifeSpanHandler> GetLifeSpanHandler(){ return this; }
	virtual CefRefPtr<CefLoadHandler> GetLoadHandler() { return this; }

	// CefLifeSpanHandler
	virtual void OnAfterCreated(CefRefPtr<CefBrowser> browser);

	// CefLoadHandler
	virtual void OnLoadEnd(CefRefPtr<CefBrowser> browser, CefRefPtr<CefFrame> frame, int httpStatusCode);
	
public:
	// CefClientHandler
	std::vector<CefRefPtr<CefBrowser>> m_vectorBrowser;

	CefRefPtr<CefBrowser> GetBrowserByIndex(int index);
	void CloseBrowserByIndex(int index);
	void CloseAllBrowser();

public:
	// CefBase
	IMPLEMENT_REFCOUNTING(CefClientHandler);
	IMPLEMENT_LOCKING(CefClientHandler);
};

