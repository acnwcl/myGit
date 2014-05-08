#pragma once
#include "include\cef_app.h"

class CefClientApp : public CefApp,
	public CefBrowserProcessHandler,
	public CefRenderProcessHandler
{
public:
	CefClientApp();
	~CefClientApp();

	// CefBase
	IMPLEMENT_REFCOUNTING(CefClientApp);
	IMPLEMENT_LOCKING(CefClientApp);

	// CefApp
	virtual CefRefPtr<CefBrowserProcessHandler> GetBrowserProcessHandler() { return this; }
	virtual CefRefPtr<CefRenderProcessHandler> GetRenderProcessHandler() { return this; }
};

