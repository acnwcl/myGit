//////////////////////////////////////////////
#pragma once

#include <exdispid.h>
#include <atlwin.h>

const int _nDispatchID = 1;

#define WM_TRAY WM_USER+1
#define WM_CANNAVIGATE WM_USER+2
#define WM_SHUTDOWNSTARTUP WM_USER+3
#define WM_GOTOURL WM_USER+4
#define WM_TVMOST WM_USER+5
#define WM_CHANGESKIN WM_USER+6
#define WM_CLEARCACHE WM_USER+7

#define WM_CANSET WM_USER+101

#define WM_BROWSERTITLECHANGE        (WM_APP)
#define WM_BROWSERDOCUMENTCOMPLETE   (WM_APP + 1)
#define WM_BROWSERSTATUSTEXTCHANGE   (WM_APP + 2)
#define WM_BROWSERLABELCHANGE		 (WM_APP + 3)
#define WM_PLAY						 (WM_APP + 4)
#define WM_GETEXTERNALS				 (WM_APP + 5)
#define WM_TITLE					 (WM_APP + 6)
#define WM_SOURCEURL				 (WM_APP + 7)
#define WM_BROWSERCREATED			 (WM_APP + 8)
#define WM_BROWSERNAVIGATEERROR      (WM_APP + 9)

#define WM_SEARCH					 (WM_APP + 9)
#define WM_OPENURL					 (WM_APP + 10)
#define WM_SHOWCMD					 (WM_APP + 11)

class CDocHostUIHandler : public IDocHostUIHandlerDispatch
{
public:
	CDocHostUIHandler() { m_cRef = 0; }
	virtual ~CDocHostUIHandler() {}

	LONG                m_cRef;

	//IUnknow
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid, void **ppvObject)
	{
		HRESULT result = S_OK;

		if (IsBadWritePtr(ppvObject, sizeof(LPVOID)))
			result = E_INVALIDARG;

		if (result == S_OK)
		{
			*ppvObject = NULL;

			if (IsEqualIID(riid, IID_IUnknown))
				*ppvObject = this;
			//             else if ( IsEqualIID( riid, IID_IOleClientSite ) )
			//                 *ppvObject = (IOleClientSite *) this;
			else if (IsEqualIID(riid, IID_IDocHostUIHandlerDispatch))
				*ppvObject = (IDocHostUIHandler *) this;
			//else if( IsEqualIID( riid, IID_IDispatch) )
			//	*ppvObject = (IDispatch *) this;
			else
				result = E_NOINTERFACE;
		}

		if (result == S_OK)
			this->AddRef();

		return result;
	}

	ULONG STDMETHODCALLTYPE AddRef(){
		InterlockedIncrement(&m_cRef);
		return m_cRef;
	}

	ULONG STDMETHODCALLTYPE Release() {
		ULONG ulRefCount = InterlockedDecrement(&m_cRef);

		return ulRefCount;
	}

	//IDocUIHandle
	HRESULT STDMETHODCALLTYPE ShowContextMenu(
		/* [in] */ DWORD dwID,
		/* [in] */ DWORD x,
		/* [in] */ DWORD y,
		/* [in] */ IUnknown *pcmdtReserved,
		/* [in] */ IDispatch *pdispReserved,
		/* [retval][out] */ HRESULT *dwRetVal)
	{
		*dwRetVal = S_OK;
		return S_OK;
	}

	virtual HRESULT STDMETHODCALLTYPE GetHostInfo(
		/* [out][in] */ DWORD *pdwFlags,
		/* [out][in] */ DWORD *pdwDoubleClick)
	{
		*pdwFlags = DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_SCROLL_NO;
		*pdwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;

		return S_OK;
	}


	virtual HRESULT STDMETHODCALLTYPE ShowUI(
		/* [in] */ DWORD dwID,
		/* [in] */ IUnknown *pActiveObject,
		/* [in] */ IUnknown *pCommandTarget,
		/* [in] */ IUnknown *pFrame,
		/* [in] */ IUnknown *pDoc,
		/* [retval][out] */ HRESULT *dwRetVal)
	{
		return E_NOTIMPL;
	}


	virtual HRESULT STDMETHODCALLTYPE HideUI(void)
	{
		return E_NOTIMPL;
	}


	virtual HRESULT STDMETHODCALLTYPE UpdateUI(void)
	{
		return E_NOTIMPL;
	}


	virtual HRESULT STDMETHODCALLTYPE EnableModeless(
		/* [in] */ VARIANT_BOOL fEnable)
	{
		return E_NOTIMPL;
	}


	virtual HRESULT STDMETHODCALLTYPE OnDocWindowActivate(
		/* [in] */ VARIANT_BOOL fActivate)
	{
		return E_NOTIMPL;
	}


	virtual HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(
		/* [in] */ VARIANT_BOOL fActivate)
	{
		return E_NOTIMPL;
	}


	virtual HRESULT STDMETHODCALLTYPE ResizeBorder(
		/* [in] */ long left,
		/* [in] */ long top,
		/* [in] */ long right,
		/* [in] */ long bottom,
		/* [in] */ IUnknown *pUIWindow,
		/* [in] */ VARIANT_BOOL fFrameWindow)
	{
		return E_NOTIMPL;
	}


	virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(
		/* [in] */ DWORD_PTR hWnd,
		/* [in] */ DWORD nMessage,
		/* [in] */ DWORD_PTR wParam,
		/* [in] */ DWORD_PTR lParam,
		/* [in] */ BSTR bstrGuidCmdGroup,
		/* [in] */ DWORD nCmdID,
		/* [retval][out] */ HRESULT *dwRetVal)
	{
		return E_NOTIMPL;
	}


	virtual HRESULT STDMETHODCALLTYPE GetOptionKeyPath(
		/* [out] */ BSTR *pbstrKey,
		/* [in] */ DWORD dw)
	{
		return E_NOTIMPL;
	}


	virtual HRESULT STDMETHODCALLTYPE GetDropTarget(
		/* [in] */ IUnknown *pDropTarget,
		/* [out] */ IUnknown **ppDropTarget)
	{
		return E_NOTIMPL;
	}


	virtual HRESULT STDMETHODCALLTYPE GetExternal(
		/* [out] */ IDispatch **ppDispatch)
	{
		return E_NOTIMPL;
	}


	virtual HRESULT STDMETHODCALLTYPE TranslateUrl(
		/* [in] */ DWORD dwTranslate,
		/* [in] */ BSTR bstrURLIn,
		/* [out] */ BSTR *pbstrURLOut)
	{
		return E_NOTIMPL;
	}


	virtual HRESULT STDMETHODCALLTYPE FilterDataObject(
		/* [in] */ IUnknown *pDO,
		/* [out] */ IUnknown **ppDORet)
	{
		return E_NOTIMPL;
	}

	//dispatch
	virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(
		/* [out] */ __RPC__out UINT *pctinfo)
	{
		return E_NOTIMPL;
	}

	virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(
		/* [in] */ UINT iTInfo,
		/* [in] */ LCID lcid,
		/* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo)
	{
		return E_NOTIMPL;
	}


	virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(
		/* [in] */ __RPC__in REFIID riid,
		/* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
		/* [range][in] */ UINT cNames,
		/* [in] */ LCID lcid,
		/* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId)
	{
		return E_NOTIMPL;
	}

	virtual /* [local] */ HRESULT STDMETHODCALLTYPE Invoke(
		/* [in] */ DISPID dispIdMember,
		/* [in] */ REFIID riid,
		/* [in] */ LCID lcid,
		/* [in] */ WORD wFlags,
		/* [out][in] */ DISPPARAMS *pDispParams,
		/* [out] */ VARIANT *pVarResult,
		/* [out] */ EXCEPINFO *pExcepInfo,
		/* [out] */ UINT *puArgErr)
	{
		return E_NOTIMPL;
	}

};



#define BEGIN_NOTIFY_MAP_DIRECT(theClass) \
	void Notify(TNotifyUI& msg) \
{ \
if (msg.sType == MST_WINDOWINIT) OnPrepare();

#define NOTIFY_HANDLER_DIRECT(type, func) \
if (msg.sType == type) \
{ \
	return func(msg); \
}

#define END_NOTIFY_MAP_DIRECT \
}


#define BEGIN_MSG_MAP_DIRECT(theClass) \
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam) \
{ \
	LRESULT lResult = 0; \
	BOOL bHandled = TRUE; \
if (uMsg == WM_NCCALCSIZE) return FALSE; \
if (uMsg == WM_NCPAINT)	return FALSE; \
if (uMsg == WM_NCACTIVATE) { if (!::IsIconic(*this)) return (wParam == 0) ? TRUE : FALSE; } \

#define MESSAGE_HANDLER_DIRECT(msg, func) \
if (uMsg == msg) \
{ \
	lResult = func(uMsg, wParam, lParam, bHandled); \
if (bHandled)\
	return lResult; \
}

#define END_MSG_MAP_DIRECT \
if (m_pm.MessageHandler(uMsg, wParam, lParam, lResult)) return lResult; \
	return CWindowWnd::HandleMessage(uMsg, wParam, lParam); \
}

#define CHAIN_MSG_MAP_DIRECT(theChainClass) \
{ \
if (theChainClass::ProcessWindowMessage(m_hWnd, uMsg, wParam, lParam, lResult)) \
	return lResult; \
}

#define BEGIN_DISP_MAP \
	const struct stringdispid* GetDispMap() \
{ \
	static const struct stringdispid dispInfo[] = {


#define DISP_INFO(bstrName,id) {bstrName,ocslen(bstrName), id},

#define END_DISP_MAP \
{ NULL, 0, 0 } }; m_dispCount = GET_ARRSIZE(dispInfo); return dispInfo; }

#define GET_ARRSIZE(p) sizeof(p)/sizeof(p[0])
