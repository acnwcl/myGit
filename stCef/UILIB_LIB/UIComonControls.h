#ifndef __UICOMMONCONTROLS_H__
#define __UICOMMONCONTROLS_H__

#pragma once

namespace UILIB_LIB
{
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class  CLabelUI : public CControlUI
	{
	public:
		CLabelUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		UINT GetTextStyle();
		void SetTextStyle(UINT uStyle);
		void SetTextColor(DWORD dwTextColor);
		void SetDisabledTextColor(DWORD dwTextColor);
		void SetFont(int index);
		RECT GetTextPadding() const;
		void SetTextPadding(RECT rc);
		bool IsShowHtml();
		void SetShowHtml(bool bShowHtml = true);

		SIZE EstimateSize(SIZE szAvailable);
		void Event(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintText(HDC hDC);

	protected:
		DWORD m_dwTextColor;
		DWORD m_dwDisabledTextColor;
		int m_iFont;
		UINT m_uTextStyle;
		RECT m_rcTextPadding;
		bool m_bShowHtml;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class  CButtonUI : public CLabelUI
	{
	public:
		CButtonUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		bool Activate();
		void SetEnabled(bool bEnable = true);
		void Event(TEventUI& event);

		LPCTSTR GetNormalImage();
		void SetNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetHotImage();
		void SetHotImage(LPCTSTR pStrImage);
		LPCTSTR GetPushedImage();
		void SetPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetFocusedImage();
		void SetFocusedImage(LPCTSTR pStrImage);
		LPCTSTR GetDisabledImage();
		void SetDisabledImage(LPCTSTR pStrImage);

		void SetHotTextColor(DWORD dwColor);
		DWORD GetHotTextColor() const;
		void SetPushedTextColor(DWORD dwColor);
		DWORD GetPushedTextColor() const;
		void SetFocusedTextColor(DWORD dwColor);
		DWORD GetFocusedTextColor() const;
		SIZE EstimateSize(SIZE szAvailable);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);

	protected:
		UINT m_uButtonState;
		DWORD m_dwHotTextColor;
		DWORD m_dwPushedTextColor;
		DWORD m_dwFocusedTextColor;

		CStdString m_sNormalImage;
		CStdString m_sHotImage;
		CStdString m_sPushedImage;
		CStdString m_sFocusedImage;
		CStdString m_sDisabledImage;
		CStdString m_sForeImage;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class  COptionUI : public CButtonUI
	{
	public:
		COptionUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		bool Activate();

		LPCTSTR GetSelectedImage();
		void SetSelectedImage(LPCTSTR pStrImage);
		LPCTSTR GetForeImage();
		void SetForeImage(LPCTSTR pStrImage);
		void SetSelectedTextColor(DWORD dwTextColor);

		bool IsGroup() const;
		void SetGroup(bool bGroup);
		bool IsSelected() const;
		void Selected(bool bSelected);

		SIZE EstimateSize(SIZE szAvailable);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);

		void SetSelectBkColor(DWORD dwColor);
		void PaintBkColor(HDC hDC);

	protected:
		bool m_bSelected;
		bool m_bGroup;
		DWORD m_dwSelectedTextColor;
		DWORD m_dwSelectedBKColor;

		CStdString m_sSelectedImage;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class  CTextUI : public CLabelUI
	{
	public:
		CTextUI();
		~CTextUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		CStdString* GetLinkContent(int iIndex);

		void Event(TEventUI& event);
		SIZE EstimateSize(SIZE szAvailable);

		void PaintText(HDC hDC);

	protected:
		enum { MAX_LINK = 8 };
		int m_nLinks;
		RECT m_rcLinks[MAX_LINK];
		CStdString m_sLinks[MAX_LINK];
		int m_nHoverLink;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class  CProgressUI : public CLabelUI
	{
	public:
		CProgressUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		bool IsHorizontal();
		void SetHorizontal(bool bHorizontal = true);
		int GetMinValue() const;
		void SetMinValue(int nMin);
		int GetMaxValue() const;
		void SetMaxValue(int nMax);
		int GetValue() const;
		void SetValue(int nValue);
		int GetValue2() const;
		void SetValue2(int nValue);
		LPCTSTR GetFgImage() const;
		void SetFgImage(LPCTSTR pStrImage);
		LPCTSTR GetFgImage2() const;
		void SetFgImage2(LPCTSTR pStrImage);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintStatusImage(HDC hDC);

	protected:
		bool m_bHorizontal;
		int m_nMax;
		int m_nMin;
		int m_nValue;
		int m_nValue2;

		CStdString m_sFgImage;
		CStdString m_sFgImageModify;
		CStdString m_sFgImage2;
		CStdString m_sFgImage2Modify;
	};


	/////////////////////////////////////////////////////////////////////////////////////
	//

	class  CSliderUI : public CProgressUI
	{
	public:
		CSliderUI();

		LPCTSTR GetClass() const;
		UINT GetControlFlags() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		void SetEnabled(bool bEnable = true);

		void SetThumbSize(SIZE szXY);
		RECT GetThumbRect() const;
		LPCTSTR GetThumbImage() const;
		void SetThumbImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbHotImage() const;
		void SetThumbHotImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbPushedImage() const;
		void SetThumbPushedImage(LPCTSTR pStrImage);

		void Event(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		void PaintStatusImage(HDC hDC);

	protected:
		SIZE m_szThumb;
		UINT m_uButtonState;

		CStdString m_sThumbImage;
		CStdString m_sThumbHotImage;
		CStdString m_sThumbPushedImage;


		CStdString m_sImageModify;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//
	class CEditUI;

	class CEditWnd : public CWindowWnd
	{
	public:
		CEditWnd();

		void Init(CEditUI* pOwner);

		LPCTSTR GetWindowClassName() const;
		LPCTSTR GetSuperClassName() const;
		void OnFinalMessage(HWND hWnd);

		LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
		LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
		LRESULT OnEditChanged(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

	protected:
		CEditUI* m_pOwner;
	};

	class  CEditUI : public CLabelUI
	{
		friend CEditWnd;
	public:
		CEditUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);
		UINT GetControlFlags() const;

		void SetEnabled(bool bEnable = true);
		void SetText(LPCTSTR pstrText);
		void SetMaxChar(UINT uMax);
		UINT GetMaxChar();
		void SetReadOnly(bool bReadOnly);
		bool IsReadOnly() const;
		void SetPasswordMode(bool bPasswordMode);
		bool IsPasswordMode() const;
		void SetPasswordChar(TCHAR cPasswordChar);
		TCHAR GetPasswordChar() const;

		LPCTSTR GetNormalImage();
		void SetNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetHotImage();
		void SetHotImage(LPCTSTR pStrImage);
		LPCTSTR GetFocusedImage();
		void SetFocusedImage(LPCTSTR pStrImage);
		LPCTSTR GetDisabledImage();
		void SetDisabledImage(LPCTSTR pStrImage);

		void SetVisible(bool bVisible = true);
		void SetInternVisible(bool bVisible = true);
		SIZE EstimateSize(SIZE szAvailable);
		void Event(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void PaintStatusImage(HDC hDC);
		void PaintText(HDC hDC);

	protected:
		CEditWnd* m_pWindow;
		BOOL m_bNumOnly;
		UINT m_uMaxChar;
		bool m_bReadOnly;
		bool m_bPasswordMode;
		TCHAR m_cPasswordChar;
		UINT m_uButtonState;
		CStdString m_sNormalImage;
		CStdString m_sHotImage;
		CStdString m_sFocusedImage;
		CStdString m_sDisabledImage;
	public:
		void SetNumOnly(BOOL bNumOnly = TRUE);
		BOOL GetNumOnly(void);
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class  CScrollbarUI : public CControlUI
	{
	public:
		CScrollbarUI();

		LPCTSTR GetClass() const;
		LPVOID GetInterface(LPCTSTR pstrName);

		CContainerUI* GetOwner() const;
		void SetOwner(CContainerUI* pOwner);

		void SetEnabled(bool bEnable = true);

		bool IsHorizontal();
		void SetHorizontal(bool bHorizontal = true);
		int GetScrollRange() const;
		void SetScrollRange(int nRange);
		int GetScrollPos() const;
		void SetScrollPos(int nPos);
		int GetLineSize() const;
		void SetLineSize(int nSize);

		LPCTSTR GetButton1NormalImage();
		void SetButton1NormalImage(LPCTSTR pStrImage);
		LPCTSTR GetButton1HotImage();
		void SetButton1HotImage(LPCTSTR pStrImage);
		LPCTSTR GetButton1PushedImage();
		void SetButton1PushedImage(LPCTSTR pStrImage);
		LPCTSTR GetButton1DisabledImage();
		void SetButton1DisabledImage(LPCTSTR pStrImage);

		LPCTSTR GetButton2NormalImage();
		void SetButton2NormalImage(LPCTSTR pStrImage);
		LPCTSTR GetButton2HotImage();
		void SetButton2HotImage(LPCTSTR pStrImage);
		LPCTSTR GetButton2PushedImage();
		void SetButton2PushedImage(LPCTSTR pStrImage);
		LPCTSTR GetButton2DisabledImage();
		void SetButton2DisabledImage(LPCTSTR pStrImage);

		LPCTSTR GetThumbNormalImage();
		void SetThumbNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbHotImage();
		void SetThumbHotImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbPushedImage();
		void SetThumbPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetThumbDisabledImage();
		void SetThumbDisabledImage(LPCTSTR pStrImage);

		LPCTSTR GetRailNormalImage();
		void SetRailNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetRailHotImage();
		void SetRailHotImage(LPCTSTR pStrImage);
		LPCTSTR GetRailPushedImage();
		void SetRailPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetRailDisabledImage();
		void SetRailDisabledImage(LPCTSTR pStrImage);

		LPCTSTR GetBkNormalImage();
		void SetBkNormalImage(LPCTSTR pStrImage);
		LPCTSTR GetBkHotImage();
		void SetBkHotImage(LPCTSTR pStrImage);
		LPCTSTR GetBkPushedImage();
		void SetBkPushedImage(LPCTSTR pStrImage);
		LPCTSTR GetBkDisabledImage();
		void SetBkDisabledImage(LPCTSTR pStrImage);

		void SetPos(RECT rc);
		void Event(TEventUI& event);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		void DoPaint(HDC hDC, const RECT& rcPaint);

		void PaintBk(HDC hDC);
		void PaintButton1(HDC hDC);
		void PaintButton2(HDC hDC);
		void PaintThumb(HDC hDC);
		void PaintRail(HDC hDC);

	protected:

		enum {
			DEFAULT_SCROLLBAR_SIZE = 16,
			DEFAULT_TIMERID = 10,
		};

		bool m_bHorizontal;
		int m_nRange;
		int m_nScrollPos;
		int m_nLineSize;
		CContainerUI* m_pOwner;
		POINT ptLastMouse;
		int m_nLastScrollPos;
		int m_nLastScrollOffset;
		int m_nScrollRepeatDelay;

		CStdString m_sBkNormalImage;
		CStdString m_sBkHotImage;
		CStdString m_sBkPushedImage;
		CStdString m_sBkDisabledImage;

		RECT m_rcButton1;
		UINT m_uButton1State;
		CStdString m_sButton1NormalImage;
		CStdString m_sButton1HotImage;
		CStdString m_sButton1PushedImage;
		CStdString m_sButton1DisabledImage;

		RECT m_rcButton2;
		UINT m_uButton2State;
		CStdString m_sButton2NormalImage;
		CStdString m_sButton2HotImage;
		CStdString m_sButton2PushedImage;
		CStdString m_sButton2DisabledImage;

		RECT m_rcThumb;
		UINT m_uThumbState;
		CStdString m_sThumbNormalImage;
		CStdString m_sThumbHotImage;
		CStdString m_sThumbPushedImage;
		CStdString m_sThumbDisabledImage;

		CStdString m_sRailNormalImage;
		CStdString m_sRailHotImage;
		CStdString m_sRailPushedImage;
		CStdString m_sRailDisabledImage;

		CStdString m_sImageModify;
	};
}
#endif // __UICOMMONCONTROLS_H__

