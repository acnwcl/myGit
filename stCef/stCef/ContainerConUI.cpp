#include "StdAfx.h"
#include "ContainerConUI.h"

CContainerConUI::CContainerConUI(void)
{
	//m_window is not a window;
	m_bAttached = false;
	
	DWORD dwSize = 0;
	SetBkColor(0xFF000000);
}


CContainerConUI::~CContainerConUI(void)
{
	if(m_window.m_hWnd)
		m_window.Detach();
}


void CContainerConUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	__super::DoPaint(hDC,rcPaint);
	if( !::IntersectRect( &m_rcPaint, &rcPaint, &m_rcItem ) ) 
		return;
}

void CContainerConUI::Event(TEventUI& event)
{
	__super::Event(event);
}

void CContainerConUI::SetWindow(HWND hWnd)
{
	//不用检查hWnd,m_window自动检查
	//if(hWnd == NULL || !::IsWindow(hWnd))
	//	return ;
	if(IsAttached())return;
	m_window.Attach(hWnd);
	if(IsVisible())
	{
		m_window.ShowWindow(SW_SHOW);
		WTL::CRect rect = GetPos();
		rect.left++;
		rect.right--;
		m_window.MoveWindow(&rect);
	}
	m_bAttached = true;

}

void CContainerConUI::SetWindow(CWindow window)
{
	if(IsAttached())return;
	m_window = window;
	m_bAttached = true;
}

bool CContainerConUI::IsAttached()
{
	return m_bAttached;
}

void CContainerConUI::SetPos(RECT rc)
{
	__super::SetPos(rc);
	WTL::CRect rect = GetPos();
	rect.left++;
	rect.right--;
	if(m_window.m_hWnd!=NULL && m_window.IsWindow())
		m_window.MoveWindow(&rect);
}

void CContainerConUI::SetVisible(bool bVisible)
{
	if(m_window.IsWindow())
		m_window.ShowWindow( bVisible ? SW_SHOW : SW_HIDE);
	__super::SetVisible(bVisible);
}
