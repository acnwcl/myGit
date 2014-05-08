#include "stdafx.h"
#include "ClientRect.h"


CClientRect::CClientRect(HWND hWnd)
{
	::GetClientRect(hWnd, this);
}

CClientRect::CClientRect(const CWindow* pWnd)
{
	::GetClientRect(pWnd->m_hWnd, this);
}

CClientRect::~CClientRect()
{
}
