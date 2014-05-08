#include "StdAfx.h"
#include "UIHtmlCtrl.h"

namespace UILIB_LIB
{

	CHtmlCtrlUI::CHtmlCtrlUI()
	{
		m_clsid = CLSID_WebBrowser;
	}

	CHtmlCtrlUI::~CHtmlCtrlUI()
	{
	}

	LPCTSTR CHtmlCtrlUI::GetClass() const
	{
		return _T("HtmlCtrlUI");
	}

	bool CHtmlCtrlUI::DelayedControlCreation()
	{
		if (!CActiveXUI::DelayedControlCreation())
			return false;
		GetControl(IID_IWebBrowser2, (LPVOID*)&m_pWebBrowser2);
		return true;
	}

	void CHtmlCtrlUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
	{
		CControlUI::SetAttribute(pstrName, pstrValue);
	}

	STDMETHODIMP CHtmlCtrlUI::GetTypeInfoCount(UINT *iTInfo)
	{
		*iTInfo = 0;
		return S_OK;
	}

	STDMETHODIMP CHtmlCtrlUI::GetTypeInfo(UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP CHtmlCtrlUI::GetIDsOfNames(REFIID riid, OLECHAR **rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP CHtmlCtrlUI::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS* pDispParams, VARIANT* pVarResult, EXCEPINFO* pExcepInfo, UINT* puArgErr)
	{
		if ((riid != IID_NULL))
			return E_INVALIDARG;

		switch (dispIdMember)
		{
		case DISPID_BEFORENAVIGATE2:
			BeforeNavigate2(
				pDispParams->rgvarg[6].pdispVal,
				pDispParams->rgvarg[5].pvarVal,
				pDispParams->rgvarg[4].pvarVal,
				pDispParams->rgvarg[3].pvarVal,
				pDispParams->rgvarg[2].pvarVal,
				pDispParams->rgvarg[1].pvarVal,
				pDispParams->rgvarg[0].pboolVal);
			break;
		case DISPID_COMMANDSTATECHANGE:
			break;
		case DISPID_NAVIGATECOMPLETE2:
			NavigateComplete2(
				pDispParams->rgvarg[1].pdispVal,
				pDispParams->rgvarg[0].pvarVal);
			break;
		case DISPID_NAVIGATEERROR:
			NavigateError(
				pDispParams->rgvarg[4].pdispVal,
				pDispParams->rgvarg[3].pvarVal,
				pDispParams->rgvarg[2].pvarVal,
				pDispParams->rgvarg[1].pvarVal,
				pDispParams->rgvarg[0].pboolVal);
			break;
		case DISPID_STATUSTEXTCHANGE:
			break;
			//  	case DISPID_NEWWINDOW2:
			//  		break;
		case DISPID_NEWWINDOW3:
			NewWindow3(
				pDispParams->rgvarg[4].ppdispVal,
				pDispParams->rgvarg[3].pboolVal,
				pDispParams->rgvarg[2].uintVal,
				pDispParams->rgvarg[1].bstrVal,
				pDispParams->rgvarg[0].bstrVal);
			break;
		default:
			return E_NOTIMPL;
		}
		return S_OK;
	}

	STDMETHODIMP CHtmlCtrlUI::QueryInterface(REFIID riid, LPVOID *ppvObject)
	{
		if (riid == IID_IDispatch)	*ppvObject = static_cast<IDispatch*>(this);

		if (*ppvObject != NULL) AddRef();
		return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
	}

	STDMETHODIMP_(ULONG) CHtmlCtrlUI::AddRef()
	{
		InterlockedIncrement(&m_dwRef);
		return m_dwRef;
	}

	STDMETHODIMP_(ULONG) CHtmlCtrlUI::Release()
	{
		ULONG ulRefCount = InterlockedDecrement(&m_dwRef);
		return ulRefCount;
	}

	//ä¯ÀÀÆ÷·½·¨
	void CHtmlCtrlUI::Navigate2(LPCTSTR lpszUrl)
	{
		if (lpszUrl == NULL)
			return;

		if (m_pWebBrowser2)
		{
			VARIANT v;
			v.vt = VT_BSTR;
			v.bstrVal = T2BSTR(lpszUrl);
			HRESULT hr = m_pWebBrowser2->Navigate2(&v, NULL, NULL, NULL, NULL);
		}
	}

	void CHtmlCtrlUI::Refresh()
	{
		if (m_pWebBrowser2)
		{
			m_pWebBrowser2->Refresh();
		}
	}


	// IDocHostUIHandler

	// * CImpIDocHostUIHandler::GetHostInfo
	// *
	// * Purpose: Called at initialization
	// *
	STDMETHODIMP CHtmlCtrlUI::GetHostInfo(DOCHOSTUIINFO* pInfo)
	{
		pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER;
		pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;

		return S_OK;
	}

	// * CImpIDocHostUIHandler::ShowUI
	// *
	// * Purpose: Called when MSHTML.DLL shows its UI
	// *
	STDMETHODIMP CHtmlCtrlUI::ShowUI(
		DWORD dwID,
		IOleInPlaceActiveObject * /*pActiveObject*/,
		IOleCommandTarget * pCommandTarget,
		IOleInPlaceFrame * /*pFrame*/,
		IOleInPlaceUIWindow * /*pDoc*/)
	{
		// We've already got our own UI in place so just return S_OK
		return S_OK;
	}

	// * CImpIDocHostUIHandler::HideUI
	// *
	// * Purpose: Called when MSHTML.DLL hides its UI
	// *
	STDMETHODIMP CHtmlCtrlUI::HideUI(void)
	{
		return S_OK;
	}

	// * CImpIDocHostUIHandler::UpdateUI
	// *
	// * Purpose: Called when MSHTML.DLL updates its UI
	// *
	STDMETHODIMP CHtmlCtrlUI::UpdateUI(void)
	{
		// MFC is pretty good about updating it's UI in it's Idle loop so I don't do anything here
		return S_OK;
	}

	// * CImpIDocHostUIHandler::EnableModeless
	// *
	// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::EnableModeless
	// *
	STDMETHODIMP CHtmlCtrlUI::EnableModeless(BOOL /*fEnable*/)
	{
		return E_NOTIMPL;
	}

	// * CImpIDocHostUIHandler::OnDocWindowActivate
	// *
	// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::OnDocWindowActivate
	// *
	STDMETHODIMP CHtmlCtrlUI::OnDocWindowActivate(BOOL /*fActivate*/)
	{
		return E_NOTIMPL;
	}

	// * CImpIDocHostUIHandler::OnFrameWindowActivate
	// *
	// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::OnFrameWindowActivate
	// *
	STDMETHODIMP CHtmlCtrlUI::OnFrameWindowActivate(BOOL /*fActivate*/)
	{
		return E_NOTIMPL;
	}

	// * CImpIDocHostUIHandler::ResizeBorder
	// *
	// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::ResizeBorder
	// *
	STDMETHODIMP CHtmlCtrlUI::ResizeBorder(
		LPCRECT /*prcBorder*/,
		IOleInPlaceUIWindow* /*pUIWindow*/,
		BOOL /*fRameWindow*/)
	{
		return E_NOTIMPL;
	}

	// * CImpIDocHostUIHandler::ShowContextMenu
	// *
	// * Purpose: Called when MSHTML.DLL would normally display its context menu
	// *
	STDMETHODIMP  CHtmlCtrlUI::ShowContextMenu(
		DWORD dwID,
		POINT* pptPosition,
		IUnknown* pCommandTarget,
		IDispatch* pDispatchObjectHit)
	{
		if (dwID == CONTEXT_MENU_VSCROLL || dwID == CONTEXT_MENU_HSCROLL)
			return E_NOTIMPL;
		m_pManager->SendNotify(this, MST_IEMENU);
		return S_OK;
	}

	// * CImpIDocHostUIHandler::TranslateAccelerator
	// *
	// * Purpose: Called from MSHTML.DLL's TranslateAccelerator routines
	// *
	STDMETHODIMP  CHtmlCtrlUI::TranslateAccelerator(LPMSG lpMsg,
		/* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
		/* [in] */ DWORD nCmdID)
	{
		TRACE(_T("AX: CHtmlCtrlUI::TranslateAccelerator"));
		if ((GetKeyState(VK_CONTROL) & 0x80000000) && (lpMsg->wParam == 'A' || lpMsg->wParam == 'O' || lpMsg->wParam == 'N' ||
			lpMsg->wParam == 'F' || lpMsg->wParam == 'L' || lpMsg->wParam == 'M' ||
			lpMsg->wParam == 'P'))
			return S_OK;
		else if ((GetKeyState(VK_MENU) & 0x80000000) && (lpMsg->wParam == VK_LEFT || lpMsg->wParam == VK_RIGHT))
			return S_OK;
		else if (VK_ESCAPE == lpMsg->wParam)
			return S_OK;
		return S_FALSE;
	}

	// * CImpIDocHostUIHandler::GetOptionKeyPath
	// *
	// * Purpose: Called by MSHTML.DLL to find where the host wishes to store 
	// *	its options in the registry
	// *
	STDMETHODIMP  CHtmlCtrlUI::GetOptionKeyPath(BSTR* pbstrKey, DWORD)
	{

		return E_NOTIMPL;
	}

	STDMETHODIMP CHtmlCtrlUI::GetDropTarget(
		/* [in] */ IDropTarget __RPC_FAR *pDropTarget,
		/* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget)
	{
		return S_FALSE;
		//    return E_NOTIMPL;
	}

	STDMETHODIMP CHtmlCtrlUI::GetExternal(
		/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch)
	{
		// return the IDispatch we have for extending the object Model
		//METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
		QueryInterface(IID_IDispatch, (LPVOID*)ppDispatch);
		return S_OK;
	}

	STDMETHODIMP CHtmlCtrlUI::TranslateUrl(
		/* [in] */ DWORD dwTranslate,
		/* [in] */ OLECHAR __RPC_FAR *pchURLIn,
		/* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut)
	{
		return E_NOTIMPL;
	}

	STDMETHODIMP CHtmlCtrlUI::FilterDataObject(
		/* [in] */ IDataObject __RPC_FAR *pDO,
		/* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet)
	{
		return E_NOTIMPL;
	}

	/// DWebBrowserEvents2
	void CHtmlCtrlUI::BeforeNavigate2(IDispatch *pDisp, VARIANT *&url, VARIANT *&Flags, VARIANT *&TargetFrameName, VARIANT *&PostData, VARIANT *&Headers, VARIANT_BOOL *&Cancel)
	{
		return;
	}

	void CHtmlCtrlUI::NavigateError(IDispatch *pDisp, VARIANT * &url, VARIANT *&TargetFrameName, VARIANT *&StatusCode, VARIANT_BOOL *&Cancel)
	{
		return;
	}

	void CHtmlCtrlUI::NavigateComplete2(IDispatch *pDisp, VARIANT *&url)
	{
		return;
	}

	void CHtmlCtrlUI::ProgressChange(LONG nProgress, LONG nProgressMax)
	{
		return;
	}

	void CHtmlCtrlUI::NewWindow3(IDispatch **pDisp, VARIANT_BOOL *&Cancel, DWORD dwFlags, BSTR bstrUrlContext, BSTR bstrUrl)
	{
		return;
	}

}
