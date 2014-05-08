#pragma once

#include "uilib.h"
using namespace UILIB_LIB;

//需要调用setwindow 和 chromium 连接
//isAttached判断是否连接
class CContainerConUI :
	public CControlUI
{
public:
	CContainerConUI(void);
	~CContainerConUI(void);

	void SetWindow(HWND hWnd);
	void SetWindow(CWindow window);

	bool IsAttached();

	virtual void SetPos(RECT rc);
	virtual void SetVisible(bool bVisible = true);
	virtual void Event(TEventUI& event);
	virtual void DoPaint(HDC hDC, const RECT& rcPaint);

private:
	bool m_bAttached;
	//chromium window
	CWindow m_window;
};

