#pragma once
class CClientRect : public CRect
{
public:
	CClientRect(HWND hWnd);
	CClientRect(const CWindow* pWnd);
	~CClientRect();
};

