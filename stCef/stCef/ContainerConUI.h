#pragma once

#include "uilib.h"
using namespace UILIB_LIB;

//��Ҫ����setwindow �� chromium ����
//isAttached�ж��Ƿ�����
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

