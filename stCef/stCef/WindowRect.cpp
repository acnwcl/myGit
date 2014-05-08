#include "stdafx.h"
#include "WindowRect.h"


CWindowRect::CWindowRect(HWND hWnd)
{
	::GetWindowRect(hWnd, this);
}

CWindowRect::CWindowRect(const CWindow* pWnd)
{
	::GetWindowRect(pWnd->m_hWnd, this);
}

CWindowRect::~CWindowRect()
{
}
