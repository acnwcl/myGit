#pragma once
#include "vector"
namespace UILIB_LIB
{

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										 CMenuUI  �˵��ؼ�
class IMenuListOwnerUI;
class CMenuWnd;

class  CMenuUI:
	public CContainerUI,
	public IMenuListOwnerUI
{
	    friend CMenuWnd;							//��Ԫ��
public:
    CMenuUI();

    LPCTSTR GetClass() const;					//�������
    LPVOID GetInterface(LPCTSTR pstrName);		//��ýӿ�
	void initMenuLayout();


    UINT GetControlFlags() const;				//�ؼ���־

    CStdString GetText() const;					//�ؼ�����
    void SetEnabled(bool bEnable = true);		//���û���

    CStdString GetDropBoxAttributeList();			//�õ��϶������б�
    void SetDropBoxAttributeList(LPCTSTR pstrList);	//�����϶������б�
    SIZE GetDropBoxSize() const;					//�õ��϶����Ӵ�С
    void SetDropBoxSize(SIZE szDropBox);			//�����϶����ӵĴ�С

    int GetCurSel() const;							//�õ����ѡ����
    bool SelectItem(int iIndex);					//ѡ����

    bool SetItemIndex(CControlUI* pControl, int iIndex);	//����ѡ��������
    bool Add(CControlUI* pControl);							//��ӿؼ�
    bool AddAt(CControlUI* pControl, int iIndex);			//�ؼ���λ
    bool Remove(CControlUI* pControl);						//�Ƴ�
    bool RemoveAt(int iIndex);								//�Ƴ�ָ��λ��
    void RemoveAll();										//�Ƴ�ȫ��

	bool Activate();										//�������˵�
	bool ActivateNextMenu(POINT pt,HWND hWndPare,RECT eventrc);								//�����ϴβ˵������� �����¼��˵�


	MenuListInfoUI* GetListInfo();								//�б���Ϣ
	void SetItemFont(int index);							//���ýڵ�����
    void SetItemTextStyle(UINT uStyle);						//���ýڵ��ı�����
    void SetItemTextPadding(RECT rc);						//���ýڵ��ı�λ��
    void SetItemTextColor(DWORD dwTextColor);				//���ýڵ��ı�����ɫ
    void SetItemBkColor(DWORD dwBkColor);					//���ýڵ�ı�����ɫ
	void SetItemBkImage(LPCTSTR pStrImage);
	void SetItemCheckImage(LPCTSTR pStrImage);
	void SetItemNextImage(LPCTSTR pStrImage);
	LPCTSTR GetItemNextImage();
	LPCTSTR GetItemBkImage();
    void SetItemImage(LPCTSTR pStrImage);					//���ýڵ��ͼƬ
    void SetSelectedItemTextColor(DWORD dwTextColor);		//����ѡ�нڵ��ı�������ɫ
    void SetSelectedItemBkColor(DWORD dwBkColor);			//����ѡ�нڵ㱳����ɫ
    void SetSelectedItemImage(LPCTSTR pStrImage);			//����ѡ�нڵ�ͼƬ
    void SetHotItemTextColor(DWORD dwTextColor);			//�������½ڵ��ı���ɫ
    void SetHotItemBkColor(DWORD dwBkColor);				//�������¡�����
    void SetHotItemImage(LPCTSTR pStrImage);
    void SetDisabledItemTextColor(DWORD dwTextColor);
    void SetDisabledItemBkColor(DWORD dwBkColor);
    void SetDisabledItemImage(LPCTSTR pStrImage);
    void SetItemLineColor(DWORD dwLineColor);				//���ýڵ��ߵ���ɫ
    bool IsItemShowHtml();									//�ж��Ƿ���ʾhtml
    void SetItemShowHtml(bool bShowHtml = true);			//���ýڵ���ʾ

    SIZE EstimateSize(SIZE szAvailable);					//���ƴ�С
    void SetPos(RECT rc);									//���ô�С����
    void Event(TEventUI& event);							//�����¼�
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);	//��������

	void DestroyMenu();
	BOOL EnableMenuItem(int labelId,BOOL pValue);						//ʹѡ����ָ�
	BOOL CheckMenuItem(int index,BOOL pValue);
	int GetMenuItemIndex(LPCTSTR pstrName);
	//void UnEnableMenuItem(int index);					//ѡ�������
	//void SetMarkCheckOk(BOOL pValue,int index);			//���ø�ѡ��  ��ȡ�ļ����ø�ѡ���ʶ
	//void SetMarkCheckCancel(BOOL pValue,int index);
    void DoPaint(HDC hDC, const RECT& rcPaint);				//�ؼ��滭

	BOOL ModifyMenuUI(int labelId,UINT nFlags,LPCTSTR lpszNewItem = NULL );
	
	void SetMenuWidth(LPCTSTR pValue);
	int GetMenuWidth();
	SIZE GetMenuCorner();
    void SetMenuCorner(int cx, int cy);

public:
    CMenuWnd* m_pWindow;			//����һ�����ڶ���
	CVerticalLayoutUI* m_pMenuLayout;
	static std::vector<HWND> hWndList;
private:
	HWND hWndParent;				//���˵��ľ��
	int MenuWidth;
    SIZE RoundCorner;
    int m_iCurSel;					//��ǰѡ������
    RECT m_rcTextPadding;			//�ı���λ������
    CStdString m_sDropBoxAttributes;	//�ַ��� �϶���������
    SIZE m_szDropBox;					//�϶����Ӵ�С

    MenuListInfoUI m_ListInfo;				//�б�����Ϣ		TListInfoUIΪ�ṹ
};



/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										 CMenuWnd   ����
class CMenuWnd:
	public CWindowWnd
{
public:
	CMenuWnd();
	void Init(CMenuUI* pOwner,POINT pt);
	void InitNextMenu(CMenuUI* pOwner,POINT pt,HWND hWndPare,RECT eventrc);
    LPCTSTR GetWindowClassName() const;
	HWND GetMainWindowHwnd();
    void OnFinalMessage(HWND hWnd);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
    //HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, const RECT rc, HMENU hMenu = NULL);
    //HWND Create(HWND hwndParent, LPCTSTR pstrName, DWORD dwStyle, DWORD dwExStyle, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT, int cx = CW_USEDEFAULT, int cy = CW_USEDEFAULT, HMENU hMenu = NULL);
  
private:
	CPaintManagerUI m_pm;
	CMenuUI* m_pOwner;
	CMenuUI* m_pEventOwner;
	RECT rcshow;
public:
	CVerticalLayoutUI* m_pLayout;
};


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										 CMenuListElementUI  ����
class CMenuListElementUI : public CControlUI, public IMenuListItemUI
{
public:
    CMenuListElementUI();

    LPCTSTR GetClass() const;
    UINT GetControlFlags() const;
    LPVOID GetInterface(LPCTSTR pstrName);

    void SetEnabled(bool bEnable = true);

    int GetIndex() const;
    void SetIndex(int iIndex);

    IMenuListOwnerUI* GetOwner();
    void SetOwner(CControlUI* pOwner);
    void SetVisible(bool bVisible = true);
	void SetButtonState(UINT state);

    bool IsSelected() const;
    bool Select(bool bSelect = true);
    bool IsExpanded() const;
    bool Expand(bool bExpand = true);

    void Invalidate(); // ֱ��CControl::Invalidate�ᵼ�¹�����ˢ�£���д����ˢ������
    bool Activate();

    void Event(TEventUI& event);
    void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

    void DrawItemBk(HDC hDC, const RECT& rcItem);
	CStdString LabelName;			//�˵���Ŀ����
	int LabelId;
	CStdString NextMenu;			//�¼��˵�   �Ӳ˵�
	BOOL LabelMarkCheck;			//���ø�ѡ �ж�
	CStdString NextMenuMark;		//�¼��˵�ͼ��  ��־

protected:
    int m_iIndex;
    bool m_bSelected;
    UINT m_uButtonState;
    IMenuListOwnerUI* m_pOwner;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//										 CMenuListLabelUI  ����
class CMenuListLabelUI:
      public CMenuListElementUI
{
public:
	CMenuListLabelUI();

	LPCTSTR GetClass() const;
	LPVOID GetInterface(LPCTSTR pstrName);


	void Event(TEventUI& event);
	SIZE EstimateSize(SIZE szAvailable);
	void DoPaint(HDC hDC, const RECT& rcPaint);
	void DrawItemText(HDC hDC, const RECT& rcItem);
	void SetNextMenu(LPCTSTR pstrValue);
	void SetLabelName(LPCTSTR pstrValue);
	CStdString GetLabelName();
	CStdString GetLabelMark();
	void SetLabelId(LPCTSTR pstrValue);
	int GetLabelId();
	void SetLabelMarkCheck(BOOL pstrValue);
	BOOL GetLabelMarkCheck();
	void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);	//��������
	CStdString GetNextMenu();

private:

};


}
