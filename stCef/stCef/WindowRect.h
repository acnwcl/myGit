#pragma once
class CWindowRect : public CRect
{
public:
	CWindowRect(HWND hWnd);
	CWindowRect(const CWindow* pWnd);
	~CWindowRect();
};

