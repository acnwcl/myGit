#include "stdafx.h"
#include "MenuUI.h"

namespace UILIB_LIB
{

std::vector<HWND> CMenuUI::hWndList;
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										 CMenuUI  菜单控件

CMenuUI::CMenuUI() : m_pWindow(NULL), m_iCurSel(-1)		//CMenuUI 构造函数
{
	m_szDropBox = CSize(0, 0);			//大小
	MenuWidth=0;
	RoundCorner=CSize(0,0);
	::ZeroMemory(&m_rcTextPadding, sizeof(m_rcTextPadding));//内存填充为零
	m_pMenuLayout=NULL;

	m_ListInfo.nColumns = 0;			//列表控件的行
	m_ListInfo.nFont = -1;					//字体
	m_ListInfo.uTextStyle = DT_VCENTER;			//文本类型
	m_ListInfo.dwTextColor = 0xFF000000;		//文本颜色
	m_ListInfo.dwBkColor = 0;						//背景颜色
	m_ListInfo.dwSelectedTextColor = 0xFF000000;		//选择文本的颜色
	m_ListInfo.dwHotTextColor = 0xFF000000;				//活动文本颜色
	m_ListInfo.dwLineColor = 0;							//线框颜色
	m_ListInfo.bShowHtml=false;
	m_ListInfo.bMultiSelect = false;
	m_ListInfo.dwDisabledTextColor = 0xFFACA899;

	::ZeroMemory(&m_ListInfo.rcTextPadding, sizeof(m_ListInfo.rcTextPadding));		//内存填充为零
	::ZeroMemory(&m_ListInfo.rcColumn, sizeof(m_ListInfo.rcColumn));                //内存填充为零

}

LPCTSTR CMenuUI::GetClass() const
{
	return _T("CMenuUI");				//返回类型
}

LPVOID CMenuUI::GetInterface(LPCTSTR pstrName)			//得到接口
{
	if( _tcscmp(pstrName, _T("ListOwner")) == 0 ) 
		return static_cast<IMenuListOwnerUI*>(this);
	return CContainerUI::GetInterface(pstrName);
}

void CMenuUI::initMenuLayout()
{
	if(m_pMenuLayout!=NULL)
		return;

	m_pMenuLayout = new CVerticalLayoutUI;		//创建垂直布局
	m_pMenuLayout->ApplyAttributeList(this->GetDropBoxAttributeList());		
	for( int i = 0; i < this->GetCount(); i++ )			//m_pOwner 是CMenuUI  循环添加
	{
		m_pMenuLayout->Add(static_cast<CControlUI*>(this->GetItemAt(i)));
	}	
}


UINT CMenuUI::GetControlFlags() const		//得到控件标识
{
	return UIFLAG_TABSTOP;
}


int CMenuUI::GetCurSel() const			//得到当前选择的节点  继承IMenuListOwnerUI
{
	return m_iCurSel;
}

bool CMenuUI::SelectItem(int iIndex)			//选择节点
{
	if( iIndex == m_iCurSel )				//当前所选择的节点
		return true;
	if( m_iCurSel >= 0 )				//如果当前选择的节点序号大于零
	{
		CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);		//查找当前节点
		if( !pControl )
			return false;
		IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));	//获得列表的接口
		if( pListItem != NULL ) 
			pListItem->Select(false);			//如果接口所对应的控件不存在 则不选择
		m_iCurSel = -1;						    //当前选择节点的序号 被清楚
	}

	if( m_items.GetSize() == 0 )
		return false;
	if( iIndex < 0 )
		iIndex = 0;
	if( iIndex >= m_items.GetSize() )			//节点序号的判断
		iIndex = m_items.GetSize() - 1;

	CControlUI* pControl = static_cast<CControlUI*>(m_items[iIndex]);		//匹配相应的节点控件

	if( !pControl || !pControl->IsVisible() || !pControl->IsEnabled() )			//判断控件不存在 控件不可活动 不可显示 则都会返回假
		return false;
	IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));		//得到相应列表节点对应的接口对象
	if( pListItem == NULL )
		return false;
	m_iCurSel = iIndex;			//改变当前选择节点
	pControl->SetFocus();			//节点控件获得焦点
	pListItem->Select(true);		//选择为真
	if( m_pManager != NULL )		//管理对象存在  则发送自定义事件
		m_pManager->SendNotify(this, MST_ITEMSELECT);
	Invalidate();		//重绘
	return true;
}

bool CMenuUI::SetItemIndex(CControlUI* pControl, int iIndex)			//设置节点的序号
{
	int iOrginIndex = GetItemIndex(pControl);		//获得当前节点控件的序号
	if (iOrginIndex == -1) 
		return false;

	if (!CContainerUI::SetItemIndex(pControl, iIndex))		//容器设置节点的序号
		return false;

	// The list items should know about us			该清单项目应了解我们
	IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));	//获得清单的接口
	if( pListItem != NULL )			//设置列表的序号
	{
		pListItem->SetIndex(GetCount()); // 本来是GetCount() - 1的，不过后面有减一
	}

	for(int i = iOrginIndex; i < GetCount(); ++i)		//枚举列表
	{
		CControlUI* p = GetItemAt(i);
		pListItem = static_cast<IListItemUI*>(p->GetInterface(_T("ListItem")));
		if( pListItem != NULL )
		{
			pListItem->SetIndex(pListItem->GetIndex() - 1);		//设置相应节点的序号
		}
	}
	return true;
}

bool CMenuUI::Add(CControlUI* pControl)			//添加控件
{
	IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));		//获得列表控件
	if( pListItem != NULL )			//列表控件存在
	{
		pListItem->SetOwner(this);		//设置获得相应的控件
		pListItem->SetIndex(m_items.GetSize());		//设置相应控件的序号
	}
	return CContainerUI::Add(pControl);			//添加控件
}

bool CMenuUI::AddAt(CControlUI* pControl, int iIndex)			//添加置顶控件
{
	if (!CContainerUI::AddAt(pControl, iIndex)) 
		return false;

	// The list items should know about us  该清单项目应了解我们
	IListItemUI* pListItem = static_cast<IListItemUI*>(pControl->GetInterface(_T("ListItem")));

	if( pListItem != NULL ) 
	{
		pListItem->SetOwner(this);
		pListItem->SetIndex(iIndex);
	}

	for(int i = iIndex + 1; i < GetCount(); ++i)  //循环枚举 设置增加节点之后的序号
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

bool CMenuUI::Remove(CControlUI* pControl)		//移除控件
{
	int iIndex = GetItemIndex(pControl);

	if (iIndex == -1)
		return false;

	if (!CContainerUI::RemoveAt(iIndex))
		return false;

	for(int i = iIndex; i < GetCount(); ++i)		//循环枚举 设置增加节点之后的序号
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

bool CMenuUI::RemoveAt(int iIndex)			//移除指定控件
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

void CMenuUI::RemoveAll()			//移除全部控件
{
	m_iCurSel = -1;
	CContainerUI::RemoveAll();
}

void CMenuUI::Event(TEventUI& event)			//自定义事件
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

SIZE CMenuUI::EstimateSize(SIZE szAvailable)			//估计大小		默认大小
{
	if( m_cxyFixed.cy == 0 )
		return CSize(m_cxyFixed.cx, m_pManager->GetDefaultFontInfo().tmHeight + 12);
	return CControlUI::EstimateSize(szAvailable);			//调用父类
}

bool CMenuUI::Activate()			//活动
{
	if( !CControlUI::Activate() ) 
		return false;
	if( m_pWindow )
		return true;
	m_pWindow = new CMenuWnd();	//建立某窗口
	ASSERT(m_pWindow);
	POINT pt;
	::GetCursorPos(&pt);
	m_pWindow->Init(this,pt);			//窗口初始化

	Invalidate();
	return true;
}

bool CMenuUI::ActivateNextMenu(POINT pt,HWND hWndPare,RECT eventrc)			//活动
{
	if( !CControlUI::Activate() ) 
		return false;
	if( m_pWindow )
		return true;
	m_pWindow = new CMenuWnd();	//建立某窗口
	ASSERT(m_pWindow);
	::SendMessage(this->m_pWindow->GetHWND(), WM_PAINT, TRUE, 0L);		//发送绘图消息
	m_pWindow->InitNextMenu(this,pt,hWndPare,eventrc);			//窗口初始化

	Invalidate();
	return true;
}

CStdString CMenuUI::GetText() const			//获得控件内容
{
	if( m_iCurSel < 0 ) 
		return _T("");
	CControlUI* pControl = static_cast<CControlUI*>(m_items[m_iCurSel]);
	return pControl->GetText();
}

void CMenuUI::SetEnabled(bool bEnable)		//设置控件活动与否
{
	CContainerUI::SetEnabled(bEnable);
}

void CMenuUI::SetPos(RECT rc)			//设置位置区域
{
	// Put all elements out of sight  推出了视线的所有元素
	//RECT rcNull = { 0 };
	//for( int i = 0; i < m_items.GetSize(); i++ )
	//	static_cast<CControlUI*>(m_items[i])->SetPos(rcNull);
	// Position this control  该控制的位置
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

void CMenuUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)		//设置属性
{

	if( _tcscmp(pstrName, _T("itemfont")) == 0 )
		m_ListInfo.nFont = _ttoi(pstrValue);
	else if( _tcscmp(pstrName, _T("itemalign")) == 0 )			//设置控件排列位置
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
			pstrValue = ::CharNext(pstrValue);		//获得下一个字符
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

void CMenuUI::DoPaint(HDC hDC, const RECT& rcPaint)		//控件绘画
{
	CControlUI::DoPaint(hDC, rcPaint);
}

CStdString CMenuUI::GetDropBoxAttributeList()			//获得下拉框属性
{
	return m_sDropBoxAttributes;
}

void CMenuUI::SetDropBoxAttributeList(LPCTSTR pstrList)		//设置下拉框属性
{
	m_sDropBoxAttributes = pstrList;
}

SIZE CMenuUI::GetDropBoxSize() const			//得到下拉框大小
{
	return m_szDropBox;
}

void CMenuUI::SetDropBoxSize(SIZE szDropBox)		//设置下拉框的大小
{
	m_szDropBox = szDropBox;
}

MenuListInfoUI* CMenuUI::GetListInfo()			//获得列表信息    继承listUI
{
	return &m_ListInfo;
}

void CMenuUI::SetItemFont(int index)			//设置节点的字体
{
	m_ListInfo.nFont = index;
	Invalidate();
}

void CMenuUI::SetItemTextStyle(UINT uStyle)			//设置节点文本的类型
{
	m_ListInfo.uTextStyle = uStyle;
	Invalidate();
}

void CMenuUI::SetItemTextPadding(RECT rc)		//设置节点文本的位置
{
	m_ListInfo.rcTextPadding = rc;
	Invalidate();
}

void CMenuUI::SetItemTextColor(DWORD dwTextColor)		//设置节点文本的颜色
{
	m_ListInfo.dwTextColor = dwTextColor;
	Invalidate();
}

void CMenuUI::SetItemBkColor(DWORD dwBkColor)		//设置节点的背景颜色
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

void CMenuUI::SetItemImage(LPCTSTR pStrImage)			//设置节点的图片
{
	m_ListInfo.sImage = pStrImage;
}

void CMenuUI::SetSelectedItemTextColor(DWORD dwTextColor)		//设置 已经选择节点的文本的颜色
{
	m_ListInfo.dwSelectedTextColor = dwTextColor;
}

void CMenuUI::SetSelectedItemBkColor(DWORD dwBkColor)		//设置 已经选择节点的背景颜色
{
	m_ListInfo.dwSelectedBkColor = dwBkColor;
}

void CMenuUI::SetSelectedItemImage(LPCTSTR pStrImage)			//设置已经选择节点的图片
{
	m_ListInfo.sSelectedImage = pStrImage;
}

void CMenuUI::SetHotItemTextColor(DWORD dwTextColor)			//设置最新的节点的文本颜色
{
	m_ListInfo.dwHotTextColor = dwTextColor;
}

void CMenuUI::SetHotItemBkColor(DWORD dwBkColor)				//设置最新的节点的背景颜色
{
	m_ListInfo.dwHotBkColor = dwBkColor;
}

void CMenuUI::SetHotItemImage(LPCTSTR pStrImage)			//设置最新节点的图片
{
	m_ListInfo.sHotImage = pStrImage;
}

void CMenuUI::SetDisabledItemTextColor(DWORD dwTextColor)		//设置失去焦点的节点的文本颜色
{
	m_ListInfo.dwDisabledTextColor = dwTextColor;
}

void CMenuUI::SetDisabledItemBkColor(DWORD dwBkColor)			//设置失去焦点的节点的背景颜色
{
	m_ListInfo.dwDisabledBkColor = dwBkColor;
}

void CMenuUI::SetDisabledItemImage(LPCTSTR pStrImage)			//设置失去焦点的节点的图片
{
	m_ListInfo.sDisabledImage = pStrImage;
}

void CMenuUI::SetItemLineColor(DWORD dwLineColor)			//设置节点线的颜色
{
	m_ListInfo.dwLineColor = dwLineColor;
}

bool CMenuUI::IsItemShowHtml()				// 设置节点是否显示html
{
	return m_ListInfo.bShowHtml;
}

void CMenuUI::SetItemShowHtml(bool bShowHtml)			// 设置节点是否显示html
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
MF_STRING  修改菜单项的字符串
MF_POPUP   修改菜单项的子菜单 为NULL则为没有
MF_CHANGE  修改菜单的labelId
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
//										 CMenuWnd   主窗体

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
void CMenuWnd::Init(CMenuUI* pOwner,POINT pt)		//初始化
{
	m_pOwner = pOwner;		//控件赋值
	m_pLayout = NULL;		//垂直布局对象为空

	// Position the popup window in absolute space		定位在绝对空间弹出窗口
	SIZE szDrop = m_pOwner->GetDropBoxSize();			//得到下拉框的大小

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
		rc.bottom-=49*fixed/2-2*(fixed/2-1);					//加上横线宽度

	CStdString sNextImage = m_pOwner->GetItemNextImage();
	if(-1==sNextImage.Find(_T("dest='")))
	{
		CStdString dest;
		dest.Format(_T(" dest='%d,%d,%d,%d'"),rc.right-rc.left-12,7,rc.right-rc.left-7,16);
		sNextImage+=dest;
		m_pOwner->SetAttribute(_T("itemnextimage"),sNextImage);
	}

	MONITORINFO oMonitor = {};			//此结构包含有关屏幕信息。
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);	//该MonitorFromWindow函数检索处理的显示屏具有最大的交集与一个指定的窗口边界矩形区域。
	CRect rcWork = oMonitor.rcWork;
	rcWork.Offset(-rcWork.left, -rcWork.top);			//将指定值的最小和最大为每个维度。

	if( rc.bottom > rcWork.bottom )			//控件底盘大于屏幕底盘
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
	Create(NULL, NULL, WS_POPUP, WS_EX_PALETTEWINDOW, rc);		//创建相关窗口 创建下拉窗口
	m_pOwner->hWndList.push_back(this->GetHWND());

	m_pOwner->hWndParent=this->GetHWND();
	// HACK: Don't deselect the parent's caption		不要取消选择父母的句柄
	HWND hWndParent =GetMainWindowHwnd();
	::ShowWindow(m_hWnd, SW_SHOW);							//显示相关窗口
	::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);		//发送绘图消息
}


void CMenuWnd::InitNextMenu(CMenuUI* pOwner,POINT pt,HWND hWndPare,RECT eventrc)		//初始化下级菜单
{
	m_pOwner = pOwner;		//控件赋值
	m_pLayout = NULL;		//垂直布局对象为空
	RECT eventMenuRc=eventrc;

	// Position the popup window in absolute space		定位在绝对空间弹出窗口
	SIZE szDrop = m_pOwner->GetDropBoxSize();			//得到下拉框的大小


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
		rc.bottom-=49*fixed/2-2*(fixed/2-1);					//加上横线宽度
	}
	else
	{
		rc.bottom-=2;
	}


	MONITORINFO oMonitor = {};			//此结构包含有关屏幕信息。
	oMonitor.cbSize = sizeof(oMonitor);
	::GetMonitorInfo(::MonitorFromWindow(*this, MONITOR_DEFAULTTOPRIMARY), &oMonitor);	//该MonitorFromWindow函数检索处理的显示屏具有最大的交集与一个指定的窗口边界矩形区域。
	CRect rcWork = oMonitor.rcWork;
	rcWork.Offset(-rcWork.left, -rcWork.top);			//将指定值的最小和最大为每个维度。

	if( rc.bottom > rcWork.bottom )			//控件底盘大于屏幕底盘
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

	::SendMessage(GetMainWindowHwnd(), WM_PAINT, TRUE, 0L);		//发送绘图消息
	::SendMessage(this->m_pOwner->hWndParent, WM_PAINT, TRUE, 0L);		//发送绘图消息
	Create(NULL, NULL, WS_POPUP, WS_EX_PALETTEWINDOW, rc);		//创建相关窗口 创建下拉窗口
	m_pOwner->hWndList.push_back(m_hWnd);

	m_pOwner->hWndParent=hWndPare;
	// HACK: Don't deselect the parent's caption		不要取消选择父母的句柄
	HWND hWndParent =GetMainWindowHwnd();
	::ShowWindow(m_hWnd, SW_SHOW);							//显示相关窗口
	::SendMessage(hWndParent, WM_NCACTIVATE, TRUE, 0L);		//发送绘图消息
}


LPCTSTR CMenuWnd::GetWindowClassName() const			//返回窗口的类型
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

void CMenuWnd::OnFinalMessage(HWND hWnd)				//在接到调用的最后一条消息（通常WM_NCDESTROY）。	销毁下级子菜单
{
	m_pOwner->m_pWindow = NULL;
	m_pOwner->Invalidate();
	m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent());			//设置管理关系
	m_pOwner->SetPos(m_pOwner->GetPos());			//设置控件位置

	if(	m_pEventOwner!=NULL)
	{
		m_pEventOwner->DestroyMenu();
		m_pEventOwner->m_pWindow=NULL;
		m_pEventOwner=NULL;
	}
	delete this;
}

LRESULT CMenuWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)			//窗口处理消息
{
	if( uMsg == WM_CREATE )				//窗口创建
	{
		m_pm.Init(m_hWnd);

		m_pLayout = new CVerticalLayoutUI;		//创建垂直布局
		m_pm.UseParentResource(m_pOwner->GetManager());  //界面管理   使用父类资源
		m_pLayout->SetManager(&m_pm, NULL);						//设置管理关系
		m_pm.SetSkn(m_pOwner->GetManager()->GetSkn());
		LPCTSTR pDefaultAttributes = m_pOwner->GetManager()->GetDefaultAttributeList(_T("VerticalLayoutUI"));  //得到默认垂直布局属性

		if( pDefaultAttributes )				//根据默认垂直属性 申请属性列表
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
		ItemBkImageDest.Format(_T(" dest='%d,%d,%d,%d'"),-5,-5,imagecx,imagecy);		//动态设置窗口背景大小
		strItemBkImage.Append(ItemBkImageDest.GetData());

		m_pLayout->SetInset(CRect(2, 2, 2, 0));			//设置相关属性
		//m_pLayout->SetBkColor(0xFFFFFFF);
		//m_pLayout->SetBorderColor(0x00EFEEE5);
		//CStdString strItemBkImage1=m_pOwner->GetItemBkImage();
		m_pLayout->SetBkImage(strItemBkImage.GetData());
		m_pLayout->SetBorderSize(2);

		m_pLayout->SetAutoDestroy(false);
		m_pLayout->ApplyAttributeList(m_pOwner->GetDropBoxAttributeList());

		for( int i = 0; i < m_pOwner->GetCount(); i++ )			//m_pOwner 是CMenuUI  循环添加
		{
			m_pLayout->Add(static_cast<CControlUI*>(m_pOwner->GetItemAt(i)));
		}
		m_pm.AttachDialog(m_pLayout);	//添加到Hash表中        
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
	else if( uMsg == WM_PAINT )				//鼠标左键UP事件
	{
		TRACE(_T("WM_PAINT"));
	}
	else if( uMsg == WM_KEYDOWN )					//键盘按下
	{
		switch( wParam ) 
		{
		case VK_ESCAPE:							//esc
			//     m_pOwner->SelectItem(m_iOldSel);
		case VK_RETURN:							//回车
			PostMessage(WM_KILLFOCUS);
			break;
		default:								//其他按键
			TEventUI event;						//自定义事件
			event.Type = UIEVENT_KEYDOWN;
			event.chKey = (TCHAR)wParam;
			m_pOwner->Event(event);				//执行事件
			return 0;
		}
	}
	else if( uMsg == WM_KILLFOCUS )					//失去焦点
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
	else if(uMsg == WM_NEXTMENUUI)			//在同一个菜单下 只生成一个子菜单
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
		::PostMessage(GetMainWindowHwnd(), WM_PAINT, TRUE, 0L);		//发送绘图消息

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
		m_pOwner->SetManager(m_pOwner->GetManager(), m_pOwner->GetParent());			//设置管理关系
		m_pOwner->SetPos(m_pOwner->GetPos());			//设置控件位置
		m_pOwner->SetFocus();				//控件获得焦点
		::DestroyWindow(m_pOwner->hWndParent);
		return 0;		
	}
	else if(uMsg==WM_CHECKMENU)				//判断复选按钮
	{
		int LabelListIndex=(int)wParam;
		BOOL labelmarkcheck=(BOOL)lParam;

		CMenuListLabelUI* LabelList=NULL;
		LabelList=static_cast<CMenuListLabelUI*>(m_pLayout->GetItemAt(LabelListIndex));
		::SendMessage(GetMainWindowHwnd(),WM_DOMAINWINDOWHANDLE,WPARAM(LabelList->GetLabelId()),0);   //  向主窗口发送消息
		if(LabelList!=NULL&&labelmarkcheck)
			LabelList->SetLabelMarkCheck(false);
		else
			LabelList->SetLabelMarkCheck(true);
		LabelList=NULL;

		m_pOwner->m_pMenuLayout=NULL;
		m_pOwner->initMenuLayout();
	}
	else if(uMsg==WM_DOMAINWINDOWHANDLE)		//响应退出菜单消息
	{
		if(GetMainWindowHwnd())
			::PostMessage(GetMainWindowHwnd(),WM_USER+200, wParam, lParam);		//向主窗口发送销毁消息
	}
	//else if(uMsg==WM_SETOPTIONUNENABLE)			//禁用选择项
	//{
	//	int LabelListIndex=(int)wParam;
	//	CMenuListLabelUI* LabelList=NULL;
	//	LabelList=static_cast<CMenuListLabelUI*>(m_pLayout->GetItemAt(LabelListIndex));
	//	LabelList->SetEnabled(false);
	//	LabelList=NULL;
	//}
	//else if(uMsg==WM_RESUMEOPTIONENABLE)			//恢复选择项
	//{
	//	int LabelListIndex=(int)wParam;
	//	CMenuListLabelUI* LabelList=NULL;
	//	LabelList=static_cast<CMenuListLabelUI*>(m_pLayout->GetItemAt(LabelListIndex));
	//	LabelList->SetEnabled(true);
	//	LabelList=NULL;
	//}
	//else if(uMsg==WM_CLOSEMENU)					//向主窗口发送隐藏菜单窗体
	//{
	//	::SendMessage(m_pOwner->GetManager()->GetPaintWindow(),WM_HIDEMENU,0,0);
	//}

	LRESULT lRes = 0;
	if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) )			//界面管理消息处理
		return lRes;
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam);				//窗口消息处理
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//											 CMenuListLabelUI  窗体



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
			::PostMessage(m_pManager->GetPaintWindow(),WM_DESTROYMENU,WPARAM(this->GetIndex()),0);		//向窗体发送隐藏消息
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
//										 CMenuListElementUI  窗体
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
	//一个重要的转折：列表项将不其直接父，但“连接”列表发送事件。实际上，列表可能会嵌入在其路径中的项目中的几个组件，但的按键等需求到实际的清单。
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