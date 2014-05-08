#include "stdafx.h"
#include "MenuUI.h"

namespace UILIB_LIB
{

std::vector<HWND> CMenuUI::hWndList;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										 CMenuUI  �˵��ؼ�

CMenuUI::CMenuUI() : m_pWindow(NULL), m_iCurSel(-1)		//CMenuUI ���캯��
{
	m_szDropBox = CSize(0, 0);			//��С
	MenuWidth=0;
	RoundCorner=CSize(0,0);
	::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));//�ڴ����Ϊ��
	m_pMenuLayout=NULL;

	m_ListInfo.nColumns = 0;			//�б�ؼ�����
	m_ListInfo.nFont = -1;					//����
	m_ListInfo.uTextStyle = DT_VCENTER;			//�ı�����
	m_ListInfo.dwTextColor = 0xFF000000;		//�ı���ɫ
	m_ListInfo.dwBkColor = 0;						//������ɫ
	m_ListInfo.dwSelectedTextColor = 0xFF000000;		//ѡ���ı�����ɫ
	m_ListInfo.dwHotTextColor = 0xFF000000;				//��ı���ɫ
	m_ListInfo.dwLineColor = 0;							//�߿���ɫ
	m_ListInfo.bShowHtml=false;
	m_ListInfo.bMultiSelect = false;
	m_ListInfo.dwDisabledTextColor = 0xFFACA899;

	::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));		//�ڴ����Ϊ��
	::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));                //�ڴ����Ϊ��

}

LPCTSTR CMenuUI::GetClass() const
{
	return _T("CMenuUI");				//��������
}

LPVOID CMenuUI::GetInterface(LPCTSTR pstrName)			//�õ��ӿ�
{
	if( _tcscmp(pstrName, _T("ListOwner")) == 0 ) 
		return static_cast<IMenuListOwnerUI*>(this);
	return CContainerUI::GetInterface(pstrName);
}

void CMenuUI::initMenuLayout()
{
	if(m_pMenuLayout!=NULL)
		return;

	m_pMenuLayout = new CVerticalLayoutUI;		//������ֱ����
	m_pMenuLayout->ApplyAttributeList(this->GetDropBoxAttributeList());		
	for( int i = 0; i < this->GetCount(); i++ )			//m_pOwner ��CMenuUI  ѭ�����
	{
		m_pMenuLayout->Add(static_cast<CControlUI*>(this->GetItemAt(i)));
	}	
}


UINT CMenuUI::GetControlFlags() const		//�õ��ؼ���ʶ
{
	return UIFLAG_TABSTOP;
}


int CMenuUI::GetCurSel() const			//�õ���ǰѡ��Ľڵ�  �̳�IMenuListOwnerUI
{
	return m_iCurSel;
}

bool CMenuUI::SelectItem(int iIndex)			//ѡ��ڵ�
{
	if( iIndex == m_iCurSel )				//��ǰ��ѡ��Ľڵ�
		return true;
	if( m_iCurSel >= 0 )				//�����ǰѡ��Ľڵ���Ŵ�����
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);		//���ҵ�ǰ�ڵ�
		if( !pControl )
			return false;
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));	//����б�Ľӿ�
		if( pListItem != NULL ) 
			pListItem->Select(false);			//����ӿ�����Ӧ�Ŀؼ������� ��ѡ��
		m_iCurSel = -1;						    //��ǰѡ��ڵ����� �����
	}

	if( m_items.GetSize() == 0 )
		return false;
	if( iIndex < 0 )
		iIndex = 0;
	if( iIndex >= m_items.GetSize() )			//�ڵ���ŵ��ж�
		iIndex = m_items.GetSize() - 1;

	CControlUI* pControl = static_cast<CControlUI*>(m_items[iIndex]);		//ƥ����Ӧ�Ľڵ�ؼ�

	if( !pControl || !pControl->IsVisible() || !pControl->IsEnabled() )			//�жϿؼ������� �ؼ����ɻ ������ʾ �򶼻᷵�ؼ�
		return false;
	IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));		//�õ���Ӧ�б�ڵ��Ӧ�Ľӿڶ���
	if( pListItem == NULL )
		return false;
	m_iCurSel = iIndex;			//�ı䵱ǰѡ��ڵ�
	pControl->SetFocus();			//�ڵ�ؼ���ý���
	pListItem->Select(true);		//ѡ��Ϊ��
	if( m_pManager != NULL )		//����������  �����Զ����¼�
		m_pManager->SendNotify(this, MST_ITEMSELECT);
	Invalidate();		//�ػ�
	return true;
}

bool CMenuUI::SetItemIndex(CControlUI* pControl, int iIndex)			//���ýڵ�����
{
	int iOrginIndex = GetItemIndex(pControl);		//��õ�ǰ�ڵ�ؼ������
	if (iOrginIndex == -1) 
		return false;

	if (!CContainerUI::SetItemIndex(pControl, iIndex))		//�������ýڵ�����
		return false;

	// The list items should know about us			���嵥��ĿӦ�˽�����
	IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));	//����嵥�Ľӿ�
	if( pListItem != NULL )			//�����б�����
	{
		pListItem->SetIndex(GetCount()); // ������GetCount() - 1�ģ����������м�һ
	}

	for(int i = iOrginIndex; i < GetCount(); ++i)		//ö���б�
	{
		CControlUI* p = GetItemAt(i);
		pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
		if( pListItem != NULL )
		{
			pListItem->SetIndex(pListItem->GetIndex() - 1);		//������Ӧ�ڵ�����
		}
	}
	return true;
}

bool CMenuUI::Add(CControlUI* pControl)			//��ӿؼ�
{
	IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));		//����б�ؼ�
	if( pListItem != NULL )			//�б�ؼ�����
	{
		pListItem->SetOwner(this);		//���û����Ӧ�Ŀؼ�
		pListItem->SetIndex(m_items.GetSize());		//������Ӧ�ؼ������
	}
	return CContainerUI::Add(pControl);			//��ӿؼ�
}

bool CMenuUI::AddAt(CControlUI* pControl, int iIndex)			//����ö��ؼ�
{
	if (!CContainerUI::AddAt(pControl, iIndex)) 
		return false;

	// The list items should know about us  ���嵥��ĿӦ�˽�����
	IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));

	if( pListItem != NULL ) 
	{
		pListItem->SetOwner(this);
		pListItem->SetIndex(iIndex);
	}

	for(int i = iIndex + 1; i < GetCount(); ++i)  //ѭ��ö�� �������ӽڵ�֮������
	{
		CControlUI* p = GetItemAt(i);
		pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
		if( pListItem != NULL )
		{
			pListItem->SetIndex(pListItem->GetIndex() + 1);
		}
	}
	return true;
}

bool CMenuUI::Remove(CControlUI* pControl)		//�Ƴ��ؼ�
{
	int iIndex = GetItemIndex(pControl);

	if (iIndex == -1)
		return false;

	if (!CContainerUI::RemoveAt(iIndex))
		return false;

	for(int i = iIndex; i < GetCount(); ++i)		//ѭ��ö�� �������ӽڵ�֮������
	{
		CControlUI* p = GetItemAt(i);
		IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) 
		{
			pListItem->SetIndex(pListItem->GetIndex() - 1);
		}
	}
	return true;
}

bool CMenuUI::RemoveAt(int iIndex)			//�Ƴ�ָ���ؼ�
{
	if (!CContainerUI::RemoveAt(iIndex)) 
		return false;

	for(int i = iIndex; i < GetCount(); ++i)
	{
		CControlUI* p = GetItemAt(i);
		IListItemUI* pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
		if( pListItem != NULL ) 
		{
			pListItem->SetIndex(pListItem->GetIndex() - 1);
		}
	}
	return true;
}

void CMenuUI::RemoveAll()			//�Ƴ�ȫ���ؼ�
{
	m_iCurSel = -1;
	CContainerUI::RemoveAll();
}

void CMenuUI::Event(TEventUI& event)			//�Զ����¼�
{
	if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND )
	{
		if( m_pParent != NULL )

			m_pParent->Event(event);
		else 
			CContainerUI::Event(event);
		return;
	}
	if( event.Type == UIEVENT_SETFOCUS ) 
	{
		Invalidate();
	}
	if( event.Type == UIEVENT_KILLFOCUS ) 
	{
		Invalidate();
	}
	if( event.Type == UIEVENT_BUTTONDOWN )
	{
		return;
	}
	if( event.Type == UIEVENT_BUTTONUP )
	{
		return;
	}
	if( event.Type == UIEVENT_MOUSEMOVE )
	{
		return;
	}
	if( event.Type == UIEVENT_KEYDOWN )
	{
		switch( event.chKey )
		{
		case VK_F4:
			//  Activate();
			return;
		case VK_UP:
			//     SelectItem(FindSelectable(m_iCurSel - 1, false));
			return;
		case VK_DOWN:
			//     SelectItem(FindSelectable(m_iCurSel + 1, true));
			return;
		case VK_PRIOR:
			//     SelectItem(FindSelectable(m_iCurSel - 1, false));
			return;
		case VK_NEXT:
			//    SelectItem(FindSelectable(m_iCurSel + 1, true));
			return;
		case VK_HOME:
			//    SelectItem(FindSelectable(0, false));
			return;
		case VK_END:
			//    SelectItem(FindSelectable(GetCount() - 1, true));
			return;
		}
	}
	if( event.Type == UIEVENT_SCROLLWHEEL )
	{
		return;
	}
	if( event.Type == UIEVENT_MOUSEENTER )
	{

		return;
	}
	if( event.Type == UIEVENT_MOUSELEAVE )
	{
		return;
	}
	CControlUI::Event(event);
}

SIZE CMenuUI::EstimateSize(SIZE szAvailable)			//���ƴ�С		Ĭ�ϴ�С
{
	if( m_cxyFixed.cy == 0 )
		return CSize(m_cxyFixed.cx, m_pManager->GetDefaultFontInfo().tmHeight + 12);
	return CControlUI::EstimateSize(szAvailable);			//���ø���
}

bool CMenuUI::Activate()			//�
{
	if( !CControlUI::Activate() ) 
		return false;
	if( m_pWindow )
		return true;
	m_pWindow = new CMenuWnd();	//����ĳ����
	ASSERT(m_pWindow);
	POINT pt;
	::GetCursorPos(&pt);
	m_pWindow->Init(this,pt);			//���ڳ�ʼ��

	Invalidate();
	return true;
}

bool CMenuUI::ActivateNextMenu(POINT pt,HWND hWndPare,RECT eventrc)			//�
{
	if( !CControlUI::Activate() ) 
		return false;
	if( m_pWindow )
		return true;
	m_pWindow = new CMenuWnd();	//����ĳ����
	ASSERT(m_pWindow);
	::SendMessage(this->m_pWindow->GetHWND(), WM_PAINT, TRUE, 0L);		//���ͻ�ͼ��Ϣ
	m_pWindow->InitNextMenu(this,pt,hWndPare,eventrc);			//���ڳ�ʼ��

	Invalidate();
	return true;
}

CStdString CMenuUI::GetText() const			//��ÿؼ�����
{
	if( m_iCurSel < 0 ) 
		return _T("");
	CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
	return pControl->GetText();
}

void CMenuUI::SetEnabled(bool bEnable)		//���ÿؼ�����
{
	CContainerUI::SetEnabled(bEnable);
}

void CMenuUI::SetPos(RECT rc)			//����λ������
{
	// Put all elements out of sight  �Ƴ������ߵ�����Ԫ��
	//RECT rcNull = { 0 };
	//for( int i = 0; i < m_items.GetSize(); i++ )
	//	static_cast<CControlUI*>(m_items[i])->SetPos(rcNull);
	// Position this control  �ÿ��Ƶ�λ��
	CControlUI::SetPos(rc);	
}

void CMenuUI::SetMenuWidth(LPCTSTR pstrValue)
{
  LPTSTR pstr = NULL;
  MenuWidth=_tcstol(pstrValue, &pstr, 10);
  ASSERT(pstr);  
}

int CMenuUI::GetMenuWidth()
{
	return MenuWidth;
}

SIZE CMenuUI::GetMenuCorner() 
{
    return RoundCorner;
}

void CMenuUI::SetMenuCorner(int cx, int cy)
{
    RoundCorner.cx = cx;
    RoundCorner.cy = cy;
}

void CMenuUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)		//��������
{

	if( _tcscmp(pstrName, _T("itemfont")) == 0 )
		m_ListInfo.nFont = _ttoi(pstrValue);
	else if( _tcscmp(pstrName, _T("itemalign")) == 0 )			//���ÿؼ�����λ��
	{
		if( _tcsstr(pstrValue, _T("left")) != NULL ) 
		{
			m_ListInfo.uTextStyle &= ~(DT_CENTER | DT_RIGHT);
			m_ListInfo.uTextStyle |= DT_LEFT;
		}
		if( _tcsstr(pstrValue, _T("center")) != NULL ) 
		{
			m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_RIGHT);
			m_ListInfo.uTextStyle |= DT_CENTER;
		}
		if( _tcsstr(pstrValue, _T("right")) != NULL )
		{
			m_ListInfo.uTextStyle &= ~(DT_LEFT | DT_CENTER);
			m_ListInfo.uTextStyle |= DT_RIGHT;
		}
	}
	if( _tcscmp(pstrName, _T("itemtextpadding")) == 0 ) 
	{
		RECT rcTextPadding = { 0 };
		LPTSTR pstr = NULL;
		rcTextPadding.left = _tcstol(pstrValue, &pstr, 10); 
		ASSERT(pstr);    
		rcTextPadding.top = _tcstol(pstr + 1, &pstr, 10);   
		ASSERT(pstr);    
		rcTextPadding.right = _tcstol(pstr + 1, &pstr, 10);  
		ASSERT(pstr);    
		rcTextPadding.bottom = _tcstol(pstr + 1, &pstr, 10); 
		ASSERT(pstr);    
		SetItemTextPadding(rcTextPadding);
	}
	else if( _tcscmp(pstrName, _T("itemtextcolor")) == 0 ) 
	{
		if( *pstrValue == _T('#')) 
			pstrValue = ::CharNext(pstrValue);		//�����һ���ַ�
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetItemTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, _T("itembkcolor")) == 0 )
	{
		if( *pstrValue == _T('#'))
			pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetItemBkColor(clrColor);
	}
	else if(_tcscmp(pstrName, _T("itembkimage")) == 0)
	{
		SetItemBkImage(pstrValue);
	}
	else if(_tcscmp(pstrName,_T("itemcheckimage")) == 0)
	{
		CStdString value = pstrValue;
		if(-1==value.Find(_T("dest='")))
			value+=_T(" dest='5,7,14,15'");
		SetItemCheckImage(value);
	}
	else if(_tcscmp(pstrName,_T("itemnextimage")) == 0)
	{
		SetItemNextImage(pstrValue);
	}
	else if( _tcscmp(pstrName, _T("itemimage")) == 0 ) 
		SetItemImage(pstrValue);
	else if( _tcscmp(pstrName, _T("itemselectedtextcolor")) == 0 ) 
	{
		if( *pstrValue == _T('#'))
			pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetSelectedItemTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, _T("itemselectedbkcolor")) == 0 ) 
	{
		if( *pstrValue == _T('#'))
			pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetSelectedItemBkColor(clrColor);
	}
	else if( _tcscmp(pstrName, _T("itemselectedimage")) == 0 )
		SetSelectedItemImage(pstrValue);
	else if( _tcscmp(pstrName, _T("itemhottextcolor")) == 0 )
	{
		if( *pstrValue == _T('#'))
			pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetHotItemTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, _T("itemhotbkcolor")) == 0 ) 
	{
		if( *pstrValue == _T('#')) 
			pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetHotItemBkColor(clrColor);
	}
	else if( _tcscmp(pstrName, _T("itemhotimage")) == 0 ) 
		SetHotItemImage(pstrValue);
	else if( _tcscmp(pstrName, _T("itemdisabledtextcolor")) == 0 ) 
	{
		if( *pstrValue == _T('#')) 
			pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetDisabledItemTextColor(clrColor);
	}
	else if( _tcscmp(pstrName, _T("itemdisabledbkcolor")) == 0 )
	{
		if( *pstrValue == _T('#'))
			pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetDisabledItemBkColor(clrColor);
	}
	else if( _tcscmp(pstrName, _T("itemdisabledimage")) == 0 )
		SetDisabledItemImage(pstrValue);
	else if( _tcscmp(pstrName, _T("itemlinecolor")) == 0 ) 
	{
		if( *pstrValue == _T('#')) 
			pstrValue = ::CharNext(pstrValue);
		LPTSTR pstr = NULL;
		DWORD clrColor = _tcstoul(pstrValue, &pstr, 16);
		SetItemLineColor(clrColor);
	}
	else if( _tcscmp(pstrName, _T("itemshowhtml")) == 0 )
		SetItemShowHtml(_tcscmp(pstrValue, _T("true")) == 0);
	else if(_tcscmp(pstrName, _T("menucorner")) == 0)
	{
		LPTSTR pstr = NULL;
		int cx = _tcstol(pstrValue, &pstr, 10);  ASSERT(pstr);    
		int cy = _tcstol(pstr + 1, &pstr, 10);    ASSERT(pstr); 
		SetMenuCorner(cx, cy);	
	}
	else if(_tcscmp(pstrName, _T("menuwidth")) == 0 )
	{
		SetMenuWidth(pstrValue);
	}
	else 
		CContainerUI::SetAttribute(pstrName, pstrValue);
}

void CMenuUI::DoPaint(HDC hDC, const RECT& rcPaint)		//�ؼ��滭
{
	CControlUI::DoPaint(hDC, rcPaint);
}

CStdString CMenuUI::GetDropBoxAttributeList()			//�������������
{
	return m_sDropBoxAttributes;
}

void CMenuUI::SetDropBoxAttributeList(LPCTSTR pstrList)		//��������������
{
	m_sDropBoxAttributes = pstrList;
}

SIZE CMenuUI::GetDropBoxSize() const			//�õ��������С
{
	return m_szDropBox;
}

void CMenuUI::SetDropBoxSize(SIZE szDropBox)		//����������Ĵ�С
{
	m_szDropBox = szDropBox;
}

MenuListInfoUI* CMenuUI::GetListInfo()			//����б���Ϣ    �̳�listUI
{
	return &m_ListInfo;
}

void CMenuUI::SetItemFont(int index)			//���ýڵ������
{
	m_ListInfo.nFont = index;
	Invalidate();
}

void CMenuUI::SetItemTextStyle(UINT uStyle)			//���ýڵ��ı�������
{
	m_ListInfo.uTextStyle = uStyle;
	Invalidate();
}

void CMenuUI::SetItemTextPadding(RECT rc)		//���ýڵ��ı���λ��
{
	m_ListInfo.rcTextPadding = rc;
	Invalidate();
}

void CMenuUI::SetItemTextColor(DWORD dwTextColor)		//���ýڵ��ı�����ɫ
{
	m_ListInfo.dwTextColor = dwTextColor;
	Invalidate();
}

void CMenuUI::SetItemBkColor(DWORD dwBkColor)		//���ýڵ�ı�����ɫ
{
	m_ListInfo.dwBkColor = dwBkColor;
}

void CMenuUI::SetItemBkImage(LPCTSTR pstrValue)
{
	m_ListInfo.sBkImage=pstrValue;
}

void CMenuUI::SetItemCheckImage(LPCTSTR pStrImage)
{
	m_ListInfo.sCheckImage = pStrImage;
}

LPCTSTR CMenuUI::GetItemNextImage()
{
	return m_ListInfo.sNextImage;
}

void CMenuUI::SetItemNextImage(LPCTSTR pStrImage)
{
	m_ListInfo.sNextImage = pStrImage;
}

LPCTSTR CMenuUI::GetItemBkImage()
{
	return m_ListInfo.sBkImage;
}

void CMenuUI::SetItemImage(LPCTSTR pStrImage)			//���ýڵ��ͼƬ
{
	m_ListInfo.sImage = pStrImage;
}

void CMenuUI::SetSelectedItemTextColor(DWORD dwTextColor)		//���� �Ѿ�ѡ��ڵ���ı�����ɫ
{
	m_ListInfo.dwSelectedTextColor = dwTextColor;
}

void CMenuUI::SetSelectedItemBkColor(DWORD dwBkColor)		//���� �Ѿ�ѡ��ڵ�ı�����ɫ
{
	m_ListInfo.dwSelectedBkColor = dwBkColor;
}

void CMenuUI::SetSelectedItemImage(LPCTSTR pStrImage)			//�����Ѿ�ѡ��ڵ��ͼƬ
{
	m_ListInfo.sSelectedImage = pStrImage;
}

void CMenuUI::SetHotItemTextColor(DWORD dwTextColor)			//�������µĽڵ���ı���ɫ
{
	m_ListInfo.dwHotTextColor = dwTextColor;
}

void CMenuUI::SetHotItemBkColor(DWORD dwBkColor)				//�������µĽڵ�ı�����ɫ
{
	m_ListInfo.dwHotBkColor = dwBkColor;
}

void CMenuUI::SetHotItemImage(LPCTSTR pStrImage)			//�������½ڵ��ͼƬ
{
	m_ListInfo.sHotImage = pStrImage;
}

void CMenuUI::SetDisabledItemTextColor(DWORD dwTextColor)		//����ʧȥ����Ľڵ���ı���ɫ
{
	m_ListInfo.dwDisabledTextColor = dwTextColor;
}

void CMenuUI::SetDisabledItemBkColor(DWORD dwBkColor)			//����ʧȥ����Ľڵ�ı�����ɫ
{
	m_ListInfo.dwDisabledBkColor = dwBkColor;
}

void CMenuUI::SetDisabledItemImage(LPCTSTR pStrImage)			//����ʧȥ����Ľڵ��ͼƬ
{
	m_ListInfo.sDisabledImage = pStrImage;
}

void CMenuUI::SetItemLineColor(DWORD dwLineColor)			//���ýڵ��ߵ���ɫ
{
	m_ListInfo.dwLineColor = dwLineColor;
}

bool CMenuUI::IsItemShowHtml()				// ���ýڵ��Ƿ���ʾhtml
{
	return m_ListInfo.bShowHtml;
}

void CMenuUI::SetItemShowHtml(bool bShowHtml)			// ���ýڵ��Ƿ���ʾhtml
{
	if( m_ListInfo.bShowHtml == bShowHtml ) return;
	m_ListInfo.bShowHtml = bShowHtml;
	Invalidate();
}

void CMenuUI::DestroyMenu()
{
	::DestroyWindow(this->m_pWindow->GetHWND());
	this->m_pWindow=NULL;
}

BOOL CMenuUI::EnableMenuItem(int labelId,BOOL pValue)
{
	// BOOL bHandled = FALSE;
	if(labelId == -1)
		for(int i=0;i<this->GetCount();i++)
		{
			CMenuListLabelUI* LabelList=NULL;
			LabelList=static_cast<CMenuListLabelUI*>(this->GetItemAt(i));
			if(LabelList==NULL)
				return FALSE;
			if(_tcscmp(LabelList->GetClass(), _T("MenuListLabelUI")) != 0)
				continue;
			CStdString NextName = LabelList->GetNextMenu();
			LabelList->SetEnabled(pValue);
			if(!NextName.IsEmpty())
			{
				CMenuUI* pNextMenu=NULL;
				pNextMenu=static_cast<CMenuUI*>(m_pManager->FindControl(NextName));
				if(pNextMenu!=NULL)
				{
					pNextMenu->EnableMenuItem(-1,pValue);
				}
			}	
		}
	else
		for(int i=0;i<this->GetCount();i++)
		{
			CMenuListLabelUI* LabelList=NULL;
			LabelList=static_cast<CMenuListLabelUI*>(this->GetItemAt(i));
			if(LabelList==NULL)
				return FALSE;
			if(_tcscmp(LabelList->GetClass(), _T("MenuListLabelUI")) != 0)
				continue;
			CStdString NextName = LabelList->GetNextMenu();
			if(NextName.IsEmpty()&&labelId==LabelList->GetLabelId())
			{
				//bHandled=TRUE;
				LabelList->SetEnabled(pValue);
				return TRUE;
				//	return;
			}
			else if(!NextName.IsEmpty())
			{
				CMenuUI* pNextMenu=NULL;
				pNextMenu=static_cast<CMenuUI*>(m_pManager->FindControl(NextName));
				if(pNextMenu!=NULL)
				{
					if(TRUE == pNextMenu->EnableMenuItem(labelId,pValue/*,bHandled*/))
						return TRUE;
				}
			}	
		}
		return FALSE;
}

BOOL CMenuUI::CheckMenuItem(int labelId,BOOL pValue)
{
	if(labelId == -1)
		for(int i=0;i<this->GetCount();i++)
		{
			CMenuListLabelUI* LabelList=NULL;
			LabelList=static_cast<CMenuListLabelUI*>(this->GetItemAt(i));
			if(LabelList==NULL)
				return FALSE;
			if(_tcscmp(LabelList->GetClass(), _T("MenuListLabelUI")) != 0)
				continue;
			CStdString NextName = LabelList->GetNextMenu();
			LabelList->SetLabelMarkCheck(pValue);
			if(!NextName.IsEmpty())
			{
				CMenuUI* pNextMenu=NULL;
				pNextMenu=static_cast<CMenuUI*>(m_pManager->FindControl(NextName));
				if(pNextMenu!=NULL)
				{
					pNextMenu->CheckMenuItem(-1,pValue);
				}
			}	
		}
	else
		for(int i=0;i<this->GetCount();i++)
		{
			CMenuListLabelUI* LabelList=NULL;
			LabelList=static_cast<CMenuListLabelUI*>(this->GetItemAt(i));
			if(LabelList==NULL)
				return FALSE;
			//if(LabelList->GetClass() != _T("MenuListLabelUI"))
			//	continue;
			if(_tcscmp(LabelList->GetClass(), _T("MenuListLabelUI")) != 0)
				continue;
			CStdString NextName = LabelList->GetNextMenu();
			if(NextName.IsEmpty()&&labelId==LabelList->GetLabelId())
			{
				//bHandled=TRUE;
				LabelList->SetLabelMarkCheck(pValue);
				return TRUE;
				//	return;
			}
			else if(!NextName.IsEmpty())
			{
				CMenuUI* pNextMenu=NULL;
				pNextMenu=static_cast<CMenuUI*>(m_pManager->FindControl(NextName));
				if(pNextMenu!=NULL)
				{
					if(TRUE == pNextMenu->CheckMenuItem(labelId,pValue/*,bHandled*/))
						return TRUE;
				}
			}	
		}
		return FALSE;
}

/*
MF_STRING  �޸Ĳ˵�����ַ���
MF_POPUP   �޸Ĳ˵�����Ӳ˵� ΪNULL��Ϊû��
MF_CHANGE  �޸Ĳ˵���labelId
*/
BOOL CMenuUI::ModifyMenuUI(int labelId,UINT nFlags,LPCTSTR lpszNewItem)
{
	for(int i=0;i<this->GetCount();i++)
	{
		CMenuListLabelUI* LabelList=NULL;
		LabelList=static_cast<CMenuListLabelUI*>(this->GetItemAt(i));
		if(LabelList==NULL)
			return FALSE;
		//if(LabelList->GetClass() != _T("MenuListLabelUI"))
		//	continue;
		if(_tcscmp(LabelList->GetClass(), _T("MenuListLabelUI")) != 0)
			continue;
		CStdString NextName = LabelList->GetNextMenu();
		if(NextName.IsEmpty()&&labelId==LabelList->GetLabelId())
		{
			//bHandled=TRUE;
			if(nFlags == MF_STRING)
				LabelList->SetText(lpszNewItem);
			else if(nFlags == MF_POPUP)
				LabelList->SetNextMenu(lpszNewItem);
			else if(nFlags == MF_CHANGE)
				LabelList->SetLabelId(lpszNewItem);
			//	return;
		}
		else if(!NextName.IsEmpty())
		{
			CMenuUI* pNextMenu=NULL;
			pNextMenu=static_cast<CMenuUI*>(m_pManager->FindControl(NextName));
			if(pNextMenu!=NULL)
			{
				pNextMenu->ModifyMenuUI(labelId,nFlags,lpszNewItem);
			}
		}	
	}
	return TRUE;
}
int CMenuUI::GetMenuItemIndex(LPCTSTR pstrName)
{
	initMenuLayout();
	if(m_pMenuLayout!=NULL)
	{
		CMenuListLabelUI* LabelList=NULL;
		int index=0;

		for(int i=0;i<m_pMenuLayout->GetCount();i++)
		{
			LabelList=static_cast<CMenuListLabelUI*>(m_pMenuLayout->GetItemAt(i));
			CStdString strname=LabelList->GetClass();
			if(_tcscmp(strname.GetData(), _T("ContainerUI")) == 0)
				index--;
			if(_tcscmp(LabelList->GetLabelName().GetData(),pstrName)==0)
				return index;
			index++;
			LabelList=NULL;
		}
	}
	return NULL;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										 CMenuWnd   ������

CMenuWnd::CMenuWnd()
{
	m_pEventOwner=NULL;
}
//HWND CMenuWnd::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu)
//{
//    return Create(hwndParent, pstrName, dwStyle, dwExStyle, rc.left, rc.top, rc.right - rc.left, rc.bottom - rc.top, hMenu);
//}
//
//HWND CMenuWnd::Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x, int y, int cx, int cy, HMENU hMenu)
//{
//    if( GetSuperClassName() != NULL && !RegisterSuperclass() ) return NULL;
//    if( GetSuperClassName() == NULL && !RegisterWindowClass() ) return NULL;
//	m_hWnd = ::CreateWindowEx(dwExStyle, _T("#32768"), pstrName, dwStyle, x, y, cx, cy, hwndParent, 0, CPaintManagerUI::GetResourceInstance(), this);
//    ASSERT(m_hWnd!=NULL);
//    return m_hWnd;
//}
void CMenuWnd::Init(CMenuUI* pOwner,POINT pt)		//��ʼ��
{
	m_pOwner = pOwner;		//�ؼ���ֵ
	m_pLayout = NULL;		//��ֱ���ֶ���Ϊ��

	// Position the popup window in absolute space		��λ�ھ��Կռ䵯������
	SIZE szDrop = m_pOwner->GetDropBoxSize();			//�õ�������Ĵ�С

	RECT rc={0};
	rc.top=pt.y;
	rc.left=pt.x;
	rc.bottom=0;
	rc.right=pt.x;
	int fixed=0;


	SIZE cXY=CSize(0,0); 
	cXY.cy = pOwner->m_pManager->GetDefaultFontInfo().tmHeight +13;	
	rc.bottom=rc.top+pOwner->GetCount()*cXY.cy+16.5;

	MenuListInfoUI* pInfo = m_pOwner->GetListInfo();

	int x = 0;
	for(int i=0;i<pOwner->GetCount();i++)
	{
		CControlUI* pControl = static_cast<CControlUI*>(pOwner->GetItemAt(i));
		RECT rcText = { 0, 0, 9999, cXY.cy };
		if( pInfo->bShowHtml ) {
			int nLinks = 0;
			CRenderEngine::DrawHtmlText(pOwner->m_pManager->GetPaintDC(), pOwner->m_pManager, rcText, pControl->GetText(), 0, NULL, NULL, nLinks, DT_SINGLELINE | DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
		}
		else {
			CRenderEngine::DrawText(pOwner->m_pManager->GetPaintDC(), pOwner->m_pManager, rcText, pControl->GetText(), 0, pInfo->nFont, DT_SINGLELINE | DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
		}
		x = rcText.right - rcText.left + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right;      
		if(cXY.cx <= x)
			cXY.cx = x;
	}
	cXY.cx += 30;

	for(int i=0;i<pOwner->GetCount();i++)
	{

		CControlUI* pControl = static_cast<CControlUI*>(pOwner->GetItemAt(i));
		CStdString strtype=pControl->GetClass();
		if(_tcscmp(strtype.GetData(), _T("ContainerUI")) == 0)
			fixed+=1;
	}

	if(m_pOwner->GetMenuWidth()!=0)
		cXY.cx=m_pOwner->GetMenuWidth();
	//else
	//	cXY.cx=szDrop.cx;

	rc.right+=cXY.cx;
	if(fixed!=0)
		rc.bottom-=49*fixed/2-2*(fixed/2-1);					//���Ϻ��߿��

	CStdString sNextImage = m_pOwner->GetItemNextImage();
	if(-1==sNextImage.Find(_T("dest='")))
	{
		CStdString dest;
		dest.Format(_T(" dest='%d,%d,%d,%d'"),rc.right-rc.left-12,7,rc.right-rc.left-7,16);
		sNextImage+=dest;
		m_pOwner->SetAttribute(_T("itemnextimage"),sNextImage);
	}

	MONITORINFO oMonitor = {};			//�˽ṹ�����й���Ļ��Ϣ��
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);	//��MonitorFromWindow���������������ʾ���������Ľ�����һ��ָ���Ĵ��ڱ߽��������
	CRect rcWork = oMonitor.rcWork;
	rcWork.Offset(-rcWork.left, -rcWork.top);			//��ָ��ֵ����С�����Ϊÿ��ά�ȡ�

	if( rc.bottom > rcWork.bottom )			//�ؼ����̴�����Ļ����
	{
		int Heigth=rc.bottom-rc.top-5;
		rc.top-=Heigth;
		rc.bottom-=Heigth;
	}
	if(rc.right>rcWork.right)
	{
		int Width=rc.right-rc.left;
		rc.left-=Width;
		rc.right-=Width;			
	}
	rc.bottom-=9;

	m_pOwner->hWndList.clear();
	Create(NULL, NULL, WS_POPUP, WS_EX_PALETTEWINDOW, rc);		//������ش��� ������������
	m_pOwner->hWndList.push_back(this->GetHWND());

	m_pOwner->hWndParent=this->GetHWND();
	// HACK: Don't deselect the parent's caption		��Ҫȡ��ѡ��ĸ�ľ��
	HWND hWndParent =GetMainWindowHwnd();
	::ShowWindow(m_hWnd, SW_SHOW);							//��ʾ��ش���
	::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);		//���ͻ�ͼ��Ϣ
}


void CMenuWnd::InitNextMenu(CMenuUI* pOwner,POINT pt,HWND hWndPare,RECT eventrc)		//��ʼ���¼��˵�
{
	m_pOwner = pOwner;		//�ؼ���ֵ
	m_pLayout = NULL;		//��ֱ���ֶ���Ϊ��
	RECT eventMenuRc=eventrc;

	// Position the popup window in absolute space		��λ�ھ��Կռ䵯������
	SIZE szDrop = m_pOwner->GetDropBoxSize();			//�õ�������Ĵ�С


	RECT rc={0};
	rc.top=pt.y;
	rc.left=pt.x+3;
	rc.bottom=0;
	rc.right=pt.x;
	int fixed=0;

	SIZE cXY=CSize(0,0); 
	cXY.cy = pOwner->m_pManager->GetDefaultFontInfo().tmHeight +13;	
	rc.bottom=rc.top+pOwner->GetCount()*cXY.cy+16.5;

	MenuListInfoUI* pInfo = m_pOwner->GetListInfo();

	int x = 0;
	for(int i=0;i<pOwner->GetCount();i++)
	{
		CControlUI* pControl = static_cast<CControlUI*>(pOwner->GetItemAt(i));
		RECT rcText = { 0, 0, 9999, cXY.cy };
		if( pInfo->bShowHtml ) {
			int nLinks = 0;
			CRenderEngine::DrawHtmlText(pOwner->m_pManager->GetPaintDC(), pOwner->m_pManager, rcText, pControl->GetText(), 0, NULL, NULL, nLinks, DT_SINGLELINE | DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
		}
		else {
			CRenderEngine::DrawText(pOwner->m_pManager->GetPaintDC(), pOwner->m_pManager, rcText, pControl->GetText(), 0, pInfo->nFont, DT_SINGLELINE | DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
		}
		x = rcText.right - rcText.left + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right;      
		if(cXY.cx <= x)
			cXY.cx = x;
	}
	cXY.cx += 30;

	for(int i=1;i<pOwner->GetCount();i++)
	{
		CControlUI* pControl = static_cast<CControlUI*>(pOwner->GetItemAt(i));
		CStdString strtype=pControl->GetClass();
		if(_tcscmp(strtype.GetData(), _T("ContainerUI")) == 0)
			fixed+=1;	 
	}

	if(m_pOwner->GetMenuWidth()!=0)
		cXY.cx=m_pOwner->GetMenuWidth();
	
	rc.right+=cXY.cx;
	if(fixed!=0)
	{
		rc.bottom-=49*fixed/2-2*(fixed/2-1);					//���Ϻ��߿��
	}
	else
	{
		rc.bottom-=2;
	}


	MONITORINFO oMonitor = {};			//�˽ṹ�����й���Ļ��Ϣ��
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);	//��MonitorFromWindow���������������ʾ���������Ľ�����һ��ָ���Ĵ��ڱ߽��������
	CRect rcWork = oMonitor.rcWork;
	rcWork.Offset(-rcWork.left, -rcWork.top);			//��ָ��ֵ����С�����Ϊÿ��ά�ȡ�

	if( rc.bottom > rcWork.bottom )			//�ؼ����̴�����Ļ����
	{
		int Heigth=rc.bottom-rc.top-30;
		rc.top-=Heigth;
		rc.bottom-=Heigth;
	}

	if(rc.right>rcWork.right)
	{
		int Width=eventMenuRc.right-eventMenuRc.left+rc.right-rc.left+6;
		rc.left-=Width;
		rc.right-=Width;			
	}
	rc.bottom-=9;

	::SendMessage(GetMainWindowHwnd(), WM_PAINT, TRUE, 0L);		//���ͻ�ͼ��Ϣ
	::SendMessage(this->m_pOwner->hWndParent, WM_PAINT, TRUE, 0L);		//���ͻ�ͼ��Ϣ
	Create(NULL, NULL, WS_POPUP, WS_EX_PALETTEWINDOW, rc);		//������ش��� ������������
	m_pOwner->hWndList.push_back(m_hWnd);

	m_pOwner->hWndParent=hWndPare;
	// HACK: Don't deselect the parent's caption		��Ҫȡ��ѡ��ĸ�ľ��
	HWND hWndParent =GetMainWindowHwnd();
	::ShowWindow(m_hWnd, SW_SHOW);							//��ʾ��ش���
	::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);		//���ͻ�ͼ��Ϣ
}


LPCTSTR CMenuWnd::GetWindowClassName() const			//���ش��ڵ�����
{
	return _T("CMenuWnd");
}

HWND CMenuWnd::GetMainWindowHwnd()
{
	HWND hWndParent=m_pOwner->GetManager()->GetPaintWindow();
	if(!hWndParent)
		return NULL;
	return hWndParent;
}

void CMenuWnd::OnFinalMessage(HWND hWnd)				//�ڽӵ����õ����һ����Ϣ��ͨ��WM_NCDESTROY����	�����¼��Ӳ˵�
{
	m_pOwner->m_pWindow = NULL;
	m_pOwner->Invalidate();
	m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent());			//���ù����ϵ
	m_pOwner->SetPos(m_pOwner->GetPos());			//���ÿؼ�λ��

	if(	m_pEventOwner!=NULL)
	{
		m_pEventOwner->DestroyMenu();
		m_pEventOwner->m_pWindow=NULL;
		m_pEventOwner=NULL;
	}
	delete this;
}

LRESULT CMenuWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)			//���ڴ�����Ϣ
{
	if( uMsg == WM_CREATE )				//���ڴ���
	{
		m_pm.Init(m_hWnd);

		m_pLayout = new CVerticalLayoutUI;		//������ֱ����
		m_pm.UseParentResource(m_pOwner->GetManager());  //�������   ʹ�ø�����Դ
		m_pLayout->SetManager(&m_pm, NULL);						//���ù����ϵ
		m_pm.SetSkn(m_pOwner->GetManager()->GetSkn());
		LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("VerticalLayoutUI"));  //�õ�Ĭ�ϴ�ֱ��������

		if( pDefaultAttributes )				//����Ĭ�ϴ�ֱ���� ���������б�
		{
			m_pLayout->ApplyAttributeList(pDefaultAttributes);
		}

		CREATESTRUCT createstruct;										
		memcpy(&createstruct,(void *)lParam,sizeof(CREATESTRUCT));
		int imagecx=createstruct.cx+5;
		int imagecy=createstruct.cy+5;

		CStdString strItemBkImage=m_pOwner->GetItemBkImage();
		CStdString ItemBkImageDest;
		ItemBkImageDest.IsEmpty();
		ItemBkImageDest.Format(_T(" dest='%d,%d,%d,%d'"),-5,-5,imagecx,imagecy);		//��̬���ô��ڱ�����С
		strItemBkImage.Append(ItemBkImageDest.GetData());

		m_pLayout->SetInset(CRect(2, 2, 2, 0));			//�����������
		//m_pLayout->SetBkColor(0xFFFFFFF);
		//m_pLayout->SetBorderColor(0x00EFEEE5);
		//CStdString strItemBkImage1=m_pOwner->GetItemBkImage();
		m_pLayout->SetBkImage(strItemBkImage.GetData());
		m_pLayout->SetBorderSize(2);

		m_pLayout->SetAutoDestroy(false);
		m_pLayout->ApplyAttributeList(m_pOwner->GetDropBoxAttributeList());

		for( int i = 0; i < m_pOwner->GetCount(); i++ )			//m_pOwner ��CMenuUI  ѭ�����
		{
			m_pLayout->Add(static_cast<CControlUI*>(m_pOwner->GetItemAt(i)));
		}
		m_pm.AttachDialog(m_pLayout);	//��ӵ�Hash����        
		return 0;
	}
	else if(uMsg==WM_DESTROY)
	{
		for(std::vector<HWND>::iterator it=m_pOwner->hWndList.begin(); it!=m_pOwner->hWndList.end(); )
		{
			if(* it == m_hWnd)
			{
				it = m_pOwner->hWndList.erase(it);
				break;
			}
			++it;
		}
	}	
	else if(uMsg==WM_SIZE)
	{
		SIZE szRoundCorner = m_pOwner->GetMenuCorner();
		if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
			RECT rcClient;
			::GetClientRect(*this, &rcClient);
			RECT rc = { rcClient.left, rcClient.top + szRoundCorner.cx, rcClient.right, rcClient.bottom };
			HRGN hRgn1 = ::CreateRectRgnIndirect( &rc );
			HRGN hRgn2 = ::CreateRoundRectRgn(rcClient.left, rcClient.top, rcClient.right + 1, rcClient.bottom+1 , szRoundCorner.cx,szRoundCorner.cy);
			::CombineRgn( hRgn1, hRgn1, hRgn2, RGN_OR );
			::SetWindowRgn(*this, hRgn1, TRUE);
			::SetWindowRgn(*this, hRgn2, TRUE);
			::DeleteObject(hRgn1);
			::DeleteObject(hRgn2);
		}
	}
	else if( uMsg == WM_PAINT )				//������UP�¼�
	{
		TRACE(_T("WM_PAINT"));
	}
	else if( uMsg == WM_KEYDOWN )					//���̰���
	{
		switch( wParam ) 
		{
		case VK_ESCAPE:							//esc
			//     m_pOwner->SelectItem(m_iOldSel);
		case VK_RETURN:							//�س�
			PostMessage(WM_KILLFOCUS);
			break;
		default:								//��������
			TEventUI event;						//�Զ����¼�
			event.Type = UIEVENT_KEYDOWN;
			event.chKey = (TCHAR)wParam;
			m_pOwner->Event(event);				//ִ���¼�
			return 0;
		}
	}
	else if( uMsg == WM_KILLFOCUS )					//ʧȥ����
	{
		HWND hWnd=NULL;
		hWnd=(HWND)wParam ;


		if(hWnd==this->GetHWND())
			return 0;
		if(m_pEventOwner!=NULL&&m_pEventOwner->m_pWindow->GetHWND()==hWnd)
			return 0;
		if(hWnd==m_pOwner->GetManager()->GetPaintWindow())
		{
			PostMessage(WM_DESTROYMENU,0,0);
			return 0;
		}
		if(hWnd==NULL)
		{
			PostMessage(WM_DESTROYMENU,0,0);
			return 0;
		}

		std::vector<HWND>::iterator it=m_pOwner->hWndList.begin();
		for(; it!=m_pOwner->hWndList.end(); )
		{
			if(* it == hWnd)
			{
				return 0;
			}
			++it;
		}

		if(it==m_pOwner->hWndList.end())
		{
			PostMessage(WM_DESTROYMENU,0,0);
			return 0;
		}

	}
	else if(uMsg == WM_NEXTMENUUI)			//��ͬһ���˵��� ֻ����һ���Ӳ˵�
	{
		POINT pt; 
		CStdString str=(LPCTSTR)wParam;	
		CMenuUI* menuui=static_cast<CMenuUI*>(m_pOwner->GetManager()->FindControl(str.GetData()));

		if(m_pEventOwner!=NULL)
		{
			if(menuui==m_pEventOwner)
				return 0;
		}

		if(menuui!=NULL&&m_pEventOwner==NULL)
		{
			m_pEventOwner=menuui;
		}
		else
		{
			m_pEventOwner->DestroyMenu();
			m_pEventOwner=NULL;
			m_pEventOwner=menuui;	
		}
		::PostMessage(GetMainWindowHwnd(), WM_PAINT, TRUE, 0L);		//���ͻ�ͼ��Ϣ

		RECT rc={0};
		::GetWindowRect(this->GetHWND(),&rc);

		RECT rc1;
		memcpy(&rc1,(void *)lParam,sizeof(RECT));

		pt.x=rc.right;
		pt.y=rc.top+rc1.top;
		HWND hWnd=m_pOwner->hWndParent;
		menuui->ActivateNextMenu(pt,hWnd,rc);	
	}
	else if(uMsg==WM_DESTROYMENUUI)
	{
		::SetFocus(m_pOwner->m_pWindow->GetHWND());
		if(m_pEventOwner!=NULL)
		{
			m_pEventOwner->DestroyMenu();
			m_pEventOwner=NULL;
		}
	}
	else if(uMsg==WM_DESTROYMENU)
	{
		m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent());			//���ù����ϵ
		m_pOwner->SetPos(m_pOwner->GetPos());			//���ÿؼ�λ��
		m_pOwner->SetFocus();				//�ؼ���ý���
		::DestroyWindow(m_pOwner->hWndParent);
		return 0;		
	}
	else if(uMsg==WM_CHECKMENU)				//�жϸ�ѡ��ť
	{
		int LabelListIndex=(int)wParam;
		BOOL labelmarkcheck=(BOOL)lParam;

		CMenuListLabelUI* LabelList=NULL;
		LabelList=static_cast<CMenuListLabelUI*>(m_pLayout->GetItemAt(LabelListIndex));
		::SendMessage(GetMainWindowHwnd(),WM_DOMAINWINDOWHANDLE,WPARAM(LabelList->GetLabelId()),0);   //  �������ڷ�����Ϣ
		if(LabelList!=NULL&&labelmarkcheck)
			LabelList->SetLabelMarkCheck(false);
		else
			LabelList->SetLabelMarkCheck(true);
		LabelList=NULL;

		m_pOwner->m_pMenuLayout=NULL;
		m_pOwner->initMenuLayout();
	}
	else if(uMsg==WM_DOMAINWINDOWHANDLE)		//��Ӧ�˳��˵���Ϣ
	{
		if(GetMainWindowHwnd())
			::PostMessage(GetMainWindowHwnd(),WM_USER+200, wParam, lParam);		//�������ڷ���������Ϣ
	}
	//else if(uMsg==WM_SETOPTIONUNENABLE)			//����ѡ����
	//{
	//	int LabelListIndex=(int)wParam;
	//	CMenuListLabelUI* LabelList=NULL;
	//	LabelList=static_cast<CMenuListLabelUI*>(m_pLayout->GetItemAt(LabelListIndex));
	//	LabelList->SetEnabled(false);
	//	LabelList=NULL;
	//}
	//else if(uMsg==WM_RESUMEOPTIONENABLE)			//�ָ�ѡ����
	//{
	//	int LabelListIndex=(int)wParam;
	//	CMenuListLabelUI* LabelList=NULL;
	//	LabelList=static_cast<CMenuListLabelUI*>(m_pLayout->GetItemAt(LabelListIndex));
	//	LabelList->SetEnabled(true);
	//	LabelList=NULL;
	//}
	//else if(uMsg==WM_CLOSEMENU)					//�������ڷ������ز˵�����
	//{
	//	::SendMessage(m_pOwner->GetManager()->GetPaintWindow(),WM_HIDEMENU,0,0);
	//}

	LRESULT lRes = 0;
	if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) )			//���������Ϣ����
		return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);				//������Ϣ����
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//											 CMenuListLabelUI  ����



CMenuListLabelUI::CMenuListLabelUI()
{
	NextMenu.Empty();
	LabelMarkCheck=false;
	LabelId=0;
}

LPCTSTR CMenuListLabelUI::GetClass() const
{
	return _T("MenuListLabelUI");
}

LPVOID CMenuListLabelUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("MenuListLabelUI")) == 0 )
		return static_cast<CMenuListLabelUI*>(this);
	return CMenuListElementUI::GetInterface(pstrName);
}

CStdString CMenuListLabelUI::GetNextMenu()
{
	return NextMenu;
}

void CMenuListLabelUI::Event(TEventUI& event)
{
	if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) 
	{
		if( m_pOwner != NULL )
			m_pOwner->Event(event);
		else 
			CMenuListElementUI::Event(event);
		return;
	}

	if( event.Type == UIEVENT_BUTTONDOWN )
	{
		return;
	}
	if( event.Type == UIEVENT_MOUSEMOVE ) 
	{
		return;
	}
	if( event.Type == UIEVENT_BUTTONUP )
	{		
		if(IsEnabled())
		{
			CStdString strnext=this->NextMenu;
			if(!strnext.IsEmpty())
				return;

			CStdString strname=this->GetLabelName();
			::SendMessage(m_pManager->GetPaintWindow(),WM_DOMAINWINDOWHANDLE,WPARAM(this->GetLabelId()),0);			

			this->SetButtonState(UISTATE_DISABLED);
			::PostMessage(m_pManager->GetPaintWindow(),WM_DESTROYMENU,WPARAM(this->GetIndex()),0);		//���巢��������Ϣ
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSEENTER )
	{
		if( IsEnabled() )
		{
			m_uButtonState |= UISTATE_HOT;
			Invalidate();
		}

		if( IsEnabled() ) 
		{
			CStdString str=this->NextMenu;
			if(!str.IsEmpty())
			{				  
				POINT pt=event.ptMouse; 
				CMenuListLabelUI* label=static_cast<CMenuListLabelUI*>(m_pManager->FindControl(pt));
				RECT rc;
				rc=label->GetPos();
				::SendMessage(m_pManager->GetPaintWindow(),WM_PAINT,0,0);
				::SendMessage(m_pManager->GetPaintWindow(),WM_NEXTMENUUI,WPARAM(str.GetData()),LPARAM(&rc));
			}
			else
			{		
				::SendMessage(m_pManager->GetPaintWindow(),WM_PAINT,0,0);
				::PostMessage(m_pManager->GetPaintWindow(),WM_DESTROYMENUUI,0,0);
			}
			//Select();
		}
		return;
	}
	if( event.Type == UIEVENT_MOUSELEAVE )
	{
		if( (m_uButtonState & UISTATE_HOT) != 0 )
		{
			m_uButtonState &= ~UISTATE_HOT;
			Invalidate();
		}
		return;
	}
	CMenuListElementUI::Event(event);
}

SIZE CMenuListLabelUI::EstimateSize(SIZE szAvailable)
{
	if( m_pOwner == NULL ) return CSize(0, 0);

	MenuListInfoUI* pInfo = m_pOwner->GetListInfo();
	SIZE cXY = m_cxyFixed;
	if( cXY.cy == 0 && m_pManager != NULL ) {
		cXY.cy = m_pManager->GetDefaultFontInfo().tmHeight + 13;
		cXY.cy += pInfo->rcTextPadding.top + pInfo->rcTextPadding.bottom;
	}

	if( cXY.cx == 0 && m_pManager != NULL ) {
		RECT rcText = { 0, 0, 9999, cXY.cy };
		if( pInfo->bShowHtml ) {
			int nLinks = 0;
			CRenderEngine::DrawHtmlText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, NULL, NULL, nLinks, DT_SINGLELINE | DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
		}
		else {
			CRenderEngine::DrawText(m_pManager->GetPaintDC(), m_pManager, rcText, m_sText, 0, pInfo->nFont, DT_SINGLELINE | DT_CALCRECT | pInfo->uTextStyle & ~DT_RIGHT & ~DT_CENTER);
		}
		cXY.cx = rcText.right - rcText.left + pInfo->rcTextPadding.left + pInfo->rcTextPadding.right;        
	}

	return cXY;
}

void CMenuListLabelUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
	if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) )
		return;
	DrawItemBk(hDC, m_rcItem);
	DrawItemText(hDC, m_rcItem);
	__super::DoPaint(hDC,rcPaint);
}

void CMenuListLabelUI::DrawItemText(HDC hDC, const RECT& rcItem)
{
	if( m_sText.IsEmpty() ) return;

	if( m_pOwner == NULL ) return;
	MenuListInfoUI* pInfo = m_pOwner->GetListInfo();
	DWORD iTextColor = pInfo->dwTextColor;
	if( (m_uButtonState & UISTATE_HOT) != 0 ) 
	{
		iTextColor = pInfo->dwHotTextColor;
	}
	if( IsSelected() )
	{
		iTextColor = pInfo->dwSelectedTextColor;
	}
	if( !IsEnabled() ) 
	{
		iTextColor = pInfo->dwDisabledTextColor;
	}

	int nLinks = 0;

	RECT rcText = rcItem;
	rcText.left += pInfo->rcTextPadding.left;
	rcText.right -= pInfo->rcTextPadding.right;
	rcText.top += pInfo->rcTextPadding.top;
	rcText.bottom -= pInfo->rcTextPadding.bottom;

	if( pInfo->bShowHtml )
		CRenderEngine::DrawHtmlText(hDC, m_pManager, rcText, m_sText, iTextColor, \
		NULL, NULL, nLinks, DT_SINGLELINE | pInfo->uTextStyle);
	else
		CRenderEngine::DrawText(hDC, m_pManager, rcText, m_sText, iTextColor, \
		pInfo->nFont, DT_SINGLELINE | pInfo->uTextStyle);


	if(!NextMenu.IsEmpty())
	{
		if(!pInfo->sNextImage.IsEmpty()) 
		{
			if(!DrawImage(hDC,(LPCTSTR)pInfo->sNextImage)) pInfo->sNextImage.Empty();
			else return ;
		}
	}
	if( LabelMarkCheck )
	{
		if(!pInfo->sCheckImage.IsEmpty())
		{
			if(!DrawImage(hDC,(LPCTSTR)pInfo->sCheckImage) ) pInfo->sCheckImage.Empty();
			else return ;
		}
	}
}

void CMenuListLabelUI::SetNextMenu(LPCTSTR pstrValue)
{
	if(pstrValue == NULL)
		NextMenu.Empty();
	else
		NextMenu=pstrValue;
}

void CMenuListLabelUI::SetLabelName(LPCTSTR pstrValue)
{
	LabelName=pstrValue;
}

CStdString CMenuListLabelUI::GetLabelName()
{
	return LabelName;
}

void CMenuListLabelUI::SetLabelId(LPCTSTR pstrValue)
{
	LPTSTR pstr = NULL;
	LabelId=_tcstol(pstrValue, &pstr, 10);
	ASSERT(pstr);  
}

int CMenuListLabelUI::GetLabelId()
{
	return LabelId;
}

void CMenuListLabelUI::SetLabelMarkCheck(BOOL pstrValue)
{
	LabelMarkCheck=pstrValue;
}

BOOL CMenuListLabelUI::GetLabelMarkCheck()
{
	return LabelMarkCheck;
}

void CMenuListLabelUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("name")) == 0)
		SetLabelName(pstrValue);
	else if( _tcscmp(pstrName, _T("nextmenu")) == 0 )
		SetNextMenu(pstrValue);
	else if(_tcscmp(pstrName, _T("labelid")) == 0 )
		SetLabelId(pstrValue);
	else if(_tcscmp(pstrName, _T("labelmarkcheck")) == 0 )
		SetLabelMarkCheck(_tcscmp(pstrValue, _T("true")) == 0);
	else if(_tcscmp(pstrName,_T("ftimage")) == 0)
	{
		CStdString value = pstrValue;
		if(-1==value.Find(_T("dest='")))
			value+=_T(" dest='3,2,18,17'");
		CControlUI::SetAttribute(pstrName, value);
	}
	else
		CControlUI::SetAttribute(pstrName, pstrValue);
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										 CMenuListElementUI  ����
CMenuListElementUI::CMenuListElementUI() : 
m_iIndex(-1),
m_pOwner(NULL), 
m_bSelected(false),
m_uButtonState(0)
{
}

LPCTSTR CMenuListElementUI::GetClass() const
{
	return _T("MenuListElementUI");
}

UINT CMenuListElementUI::GetControlFlags() const
{
	return UIFLAG_WANTRETURN;
}

LPVOID CMenuListElementUI::GetInterface(LPCTSTR pstrName)
{
	if( _tcscmp(pstrName, _T("ListItem")) == 0 ) return static_cast<IMenuListItemUI*>(this);
	return CControlUI::GetInterface(pstrName);
}

IMenuListOwnerUI* CMenuListElementUI::GetOwner()
{
	return m_pOwner;
}

void CMenuListElementUI::SetOwner(CControlUI* pOwner)
{
	m_pOwner = static_cast<IMenuListOwnerUI*>(pOwner->GetInterface(_T("ListOwner")));
}

void CMenuListElementUI::SetVisible(bool bVisible)
{
	CControlUI::SetVisible(bVisible);
	if( !IsVisible() && m_bSelected)
	{
		m_bSelected = false;
		if( m_pOwner != NULL ) m_pOwner->SelectItem(-1);
	}
}

void CMenuListElementUI::SetButtonState(UINT state)
{
	m_uButtonState=state;
}

void CMenuListElementUI::SetEnabled(bool bEnable)
{
	CControlUI::SetEnabled(bEnable);
	if( !IsEnabled() ) {
		m_uButtonState = 0;
	}
}

int CMenuListElementUI::GetIndex() const
{
	return m_iIndex;
}

void CMenuListElementUI::SetIndex(int iIndex)
{
	m_iIndex = iIndex;
}

void CMenuListElementUI::Invalidate()
{
	if( !IsVisible() ) return;

	if( GetParent() ) {
		CContainerUI* pParentContainer = static_cast<CContainerUI*>(GetParent()->GetInterface(_T("Container")));
		if( pParentContainer ) {
			RECT rc = pParentContainer->GetPos();
			RECT rcInset = pParentContainer->GetInset();
			rc.left += rcInset.left;
			rc.top += rcInset.top;
			rc.right -= rcInset.right;
			rc.bottom -= rcInset.bottom;
			CScrollbarUI* pVerticalScrollbar = pParentContainer->GetVerticalScrollbar();
			if( pVerticalScrollbar && pVerticalScrollbar->IsVisible() ) rc.right -= pVerticalScrollbar->GetFixedWidth();
			CScrollbarUI* pHorizontalScrollbar = pParentContainer->GetHorizontalScrollbar();
			if( pHorizontalScrollbar && pHorizontalScrollbar->IsVisible() ) rc.bottom -= pHorizontalScrollbar->GetFixedHeight();

			RECT invalidateRc = m_rcItem;
			if( !::IntersectRect(&invalidateRc, &m_rcItem, &rc) ) 
			{
				return;
			}

			CControlUI* pParent = GetParent();
			RECT rcTemp;
			RECT rcParent;
			while( pParent = pParent->GetParent() )
			{
				rcTemp = invalidateRc;
				rcParent = pParent->GetPos();
				if( !::IntersectRect(&invalidateRc, &rcTemp, &rcParent) ) 
				{
					return;
				}
			}

			if( m_pManager != NULL ) m_pManager->Invalidate(invalidateRc);
		}
		else {
			CControlUI::Invalidate();
		}
	}
	else {
		CControlUI::Invalidate();
	}
}

bool CMenuListElementUI::Activate()
{
	if( !CControlUI::Activate() ) return false;
	if( m_pManager != NULL ) m_pManager->SendNotify(this, MST_ITEMACTIVATE);
	return true;
}

bool CMenuListElementUI::IsSelected() const
{
	return m_bSelected;
}

bool CMenuListElementUI::Select(bool bSelect)
{
	if( !IsEnabled() ) return false;
	if( bSelect == m_bSelected ) return true;
	m_bSelected = bSelect;
	if( bSelect && m_pOwner != NULL ) m_pOwner->SelectItem(m_iIndex);
	Invalidate();

	return true;
}

bool CMenuListElementUI::IsExpanded() const
{
	return false;
}

bool CMenuListElementUI::Expand(bool /*bExpand = true*/)
{
	return false;
}

void CMenuListElementUI::Event(TEventUI& event)
{
	if( !IsMouseEnabled() && event.Type > UIEVENT__MOUSEBEGIN && event.Type < UIEVENT__MOUSEEND ) {
		if( m_pOwner != NULL ) m_pOwner->Event(event);
		else CControlUI::Event(event);
		return;
	}

	if( event.Type == UIEVENT_DBLCLICK )
	{
		if( IsEnabled() ) {
			Activate();
			Invalidate();
		}
		return;
	}
	if( event.Type == UIEVENT_KEYDOWN && IsEnabled() )
	{
		if( event.chKey == VK_RETURN ) {
			Activate();
			Invalidate();
			return;
		}
	}
	// An important twist: The list-item will send the event not to its immediate
	// parent but to the "attached" list. A list may actually embed several components
	// in its path to the item, but key-presses etc. needs to go to the actual list.
	//һ����Ҫ��ת�ۣ��б������ֱ�Ӹ����������ӡ��б����¼���ʵ���ϣ��б���ܻ�Ƕ������·���е���Ŀ�еļ�����������İ���������ʵ�ʵ��嵥��
	if( m_pOwner != NULL ) m_pOwner->Event(event); else CControlUI::Event(event);
}

void CMenuListElementUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
	if( _tcscmp(pstrName, _T("selected")) == 0 ) Select();
	else CControlUI::SetAttribute(pstrName, pstrValue);
}

void CMenuListElementUI::DrawItemBk(HDC hDC, const RECT& rcItem)
{
	ASSERT(m_pOwner);
	if( m_pOwner == NULL ) return;
	MenuListInfoUI* pInfo = m_pOwner->GetListInfo();
	DWORD iBackColor = pInfo->dwBkColor;
	if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		iBackColor = pInfo->dwHotBkColor;
	}
	/* if( IsSelected() ) {
	iBackColor = pInfo->dwSelectedBkColor;
	}
	if( !IsEnabled() ) {
	iBackColor = pInfo->dwDisabledBkColor;
	}*/

	if ( iBackColor != 0 ) {
		CRenderEngine::DrawColor(hDC, m_rcItem, iBackColor);
	}

	if( !IsEnabled() ) {
		if( !pInfo->sDisabledImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)pInfo->sDisabledImage) ) pInfo->sDisabledImage.Empty();
			else return;
		}
	}
	if( IsSelected() ) {
		if( !pInfo->sSelectedImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)pInfo->sSelectedImage) )
				pInfo->sSelectedImage.Empty();
			else return;
		}
	}
	if( (m_uButtonState & UISTATE_HOT) != 0 ) {
		if( !pInfo->sHotImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)pInfo->sHotImage) ) pInfo->sHotImage.Empty();
			else return;
		}
	}

	if( !m_sBkImage.IsEmpty() ) {
		if( !DrawImage(hDC, (LPCTSTR)m_sBkImage) ) m_sBkImage.Empty();
	}

	if( m_sBkImage.IsEmpty() ) {
		if( !pInfo->sImage.IsEmpty() ) {
			if( !DrawImage(hDC, (LPCTSTR)pInfo->sImage) ) pInfo->sImage.Empty();
			else return;
		}
	}

	if ( pInfo->dwLineColor != 0 ) {
		RECT rcLine = { m_rcItem.left, m_rcItem.bottom - 1, m_rcItem.right, m_rcItem.bottom - 1 };
		CRenderEngine::DrawLine(hDC, rcLine, 1, pInfo->dwLineColor);
	}
}

}