
#include "StdAfx.h"
#include "UIActiveX.h"

namespace UILIB_LIB
{
#define DISPID_SENDMESSAGE 1
#define DISPID_SENDEXE     2
#define DISPID_RELOAD	   3
#define DISPID_ADSHOW      4
#define DISPID_ADCLOSE     5
#define DISPID_SUPERPOWERPLAY 6
#define DISPID_ISOK		   7
#define DISPID_QUERYSTATUS 8
#define DISPID_GETVERSION  9
#define DISPID_NOTICEFUNC 10
/////////////////////////////////////////////////////////////////////////////////////
//
//

class CActiveXCtrl;


/////////////////////////////////////////////////////////////////////////////////////
//
//

class CActiveXWnd : public CWindowWnd
{
public:
    HWND Init(CActiveXCtrl* pOwner, HWND hWndParent);

    LPCTSTR GetWindowClassName() const;
    void OnFinalMessage(HWND hWnd);

    LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

protected:
    void DoVerb(LONG iVerb);

    LRESULT OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);

protected:
    CActiveXCtrl* m_pOwner;
};


/////////////////////////////////////////////////////////////////////////////////////
//
//

class CActiveXEnum : public IEnumUnknown
{
public:
    CActiveXEnum(IUnknown* pUnk) : m_pUnk(pUnk), m_dwRef(1), m_iPos(0)
    {
        m_pUnk->AddRef();
    }
    ~CActiveXEnum()
    {
        m_pUnk->Release();
    }

    LONG m_iPos;
    ULONG m_dwRef;
    IUnknown* m_pUnk;

    STDMETHOD_(ULONG,AddRef)()
    {
        return ++m_dwRef;
    }
    STDMETHOD_(ULONG,Release)()
    {
        LONG lRef = --m_dwRef;
        if( lRef == 0 ) delete this;
        return lRef;
    }
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
    {
        *ppvObject = NULL;
        if( riid == IID_IUnknown ) *ppvObject = static_cast<IEnumUnknown*>(this);
        else if( riid == IID_IEnumUnknown ) *ppvObject = static_cast<IEnumUnknown*>(this);
        if( *ppvObject != NULL ) AddRef();
        return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
    }
    STDMETHOD(Next)(ULONG celt, IUnknown **rgelt, ULONG *pceltFetched)
    {
        if( pceltFetched != NULL ) *pceltFetched = 0;
        if( ++m_iPos > 1 ) return S_FALSE;
        *rgelt = m_pUnk;
        (*rgelt)->AddRef();
        if( pceltFetched != NULL ) *pceltFetched = 1;
        return S_OK;
    }
    STDMETHOD(Skip)(ULONG celt)
    {
        m_iPos += celt;
        return S_OK;
    }
    STDMETHOD(Reset)(void)
    {
        m_iPos = 0;
        return S_OK;
    }
    STDMETHOD(Clone)(IEnumUnknown **ppenum)
    {
        return E_NOTIMPL;
    }
};


/////////////////////////////////////////////////////////////////////////////////////
//
//

class CActiveXFrameWnd : public IOleInPlaceFrame
{
public:
    CActiveXFrameWnd(CActiveXUI* pOwner) : m_dwRef(1), m_pOwner(pOwner), m_pActiveObject(NULL)
    {
    }
    ~CActiveXFrameWnd()
    {
        if( m_pActiveObject != NULL ) m_pActiveObject->Release();
    }

    ULONG m_dwRef;
    CActiveXUI* m_pOwner;
    IOleInPlaceActiveObject* m_pActiveObject;

    // IUnknown
    STDMETHOD_(ULONG,AddRef)()
    {
        return ++m_dwRef;
    }
    STDMETHOD_(ULONG,Release)()
    {
        ULONG lRef = --m_dwRef;
        if( lRef == 0 ) delete this;
        return lRef;
    }
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
    {
        *ppvObject = NULL;
        if( riid == IID_IUnknown ) *ppvObject = static_cast<IOleInPlaceFrame*>(this);
        else if( riid == IID_IOleWindow ) *ppvObject = static_cast<IOleWindow*>(this);
        else if( riid == IID_IOleInPlaceFrame ) *ppvObject = static_cast<IOleInPlaceFrame*>(this);
        else if( riid == IID_IOleInPlaceUIWindow ) *ppvObject = static_cast<IOleInPlaceUIWindow*>(this);
        if( *ppvObject != NULL ) AddRef();
        return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
    }  
    // IOleInPlaceFrameWindow
    STDMETHOD(InsertMenus)(HMENU /*hmenuShared*/, LPOLEMENUGROUPWIDTHS /*lpMenuWidths*/)
    {
        return S_OK;
    }
    STDMETHOD(SetMenu)(HMENU /*hmenuShared*/, HOLEMENU /*holemenu*/, HWND /*hwndActiveObject*/)
    {
        return S_OK;
    }
    STDMETHOD(RemoveMenus)(HMENU /*hmenuShared*/)
    {
        return S_OK;
    }
    STDMETHOD(SetStatusText)(LPCOLESTR /*pszStatusText*/)
    {
        return S_OK;
    }
    STDMETHOD(EnableModeless)(BOOL /*fEnable*/)
    {
        return S_OK;
    }
    STDMETHOD(TranslateAccelerator)(LPMSG /*lpMsg*/, WORD /*wID*/)
    {
		TRACE(_T("AX: CActiveXFrameWnd::TranslateAccelerator"));
        return S_FALSE;
    }
    // IOleWindow
    STDMETHOD(GetWindow)(HWND* phwnd)
    {
        if( m_pOwner == NULL ) return E_UNEXPECTED;
        *phwnd = m_pOwner->GetManager()->GetPaintWindow();
        return S_OK;
    }
    STDMETHOD(ContextSensitiveHelp)(BOOL /*fEnterMode*/)
    {
        return S_OK;
    }
    // IOleInPlaceUIWindow
    STDMETHOD(GetBorder)(LPRECT /*lprectBorder*/)
    {
        return S_OK;
    }
    STDMETHOD(RequestBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
    {
        return INPLACE_E_NOTOOLSPACE;
    }
    STDMETHOD(SetBorderSpace)(LPCBORDERWIDTHS /*pborderwidths*/)
    {
        return S_OK;
    }
    STDMETHOD(SetActiveObject)(IOleInPlaceActiveObject* pActiveObject, LPCOLESTR /*pszObjName*/)
    {
        if( pActiveObject != NULL ) pActiveObject->AddRef();
        if( m_pActiveObject != NULL ) m_pActiveObject->Release();
        m_pActiveObject = pActiveObject;
        return S_OK;
    }
};

/////////////////////////////////////////////////////////////////////////////////////
//

class CActiveXCtrl :
    public IOleClientSite,
    public IOleInPlaceSiteWindowless,
    public IOleControlSite,
    public IObjectWithSite,
    public IOleContainer,
	public IDocHostUIHandler,
	public IDocHostShowUI,
	public IOleControl,
	public IDispatch
{
    friend CActiveXUI;
	friend CActiveXWnd;
public:
	CActiveXCtrl();
	~CActiveXCtrl();
	//iolecontrol
	STDMETHOD(GetControlInfo)( 
		/* [out][in] */ __RPC__inout CONTROLINFO *pCI);

	STDMETHOD(OnMnemonic)( 
		/* [in] */ __RPC__in MSG *pMsg);

	STDMETHOD(OnAmbientPropertyChange)( 
		/* [in] */ DISPID dispID);

	STDMETHOD(FreezeEvents)( 
		/* [in] */ BOOL bFreeze);
	//
   STDMETHOD(GetTypeInfoCount)( 
            /* [out] */ __RPC__out UINT *pctinfo);
        
   STDMETHOD(GetTypeInfo)( 
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ __RPC__deref_out_opt ITypeInfo **ppTInfo);
        
   STDMETHOD(GetIDsOfNames)( 
            /* [in] */ __RPC__in REFIID riid,
            /* [size_is][in] */ __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ __RPC__out_ecount_full(cNames) DISPID *rgDispId);
        
   STDMETHOD(Invoke)( 
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS *pDispParams,
            /* [out] */ VARIANT *pVarResult,
            /* [out] */ EXCEPINFO *pExcepInfo,
            /* [out] */ UINT *puArgErr);
	STDMETHOD(ShowMessage)( 
		/* [in] */ HWND hwnd,
		/* [in] */ 
		__in __nullterminated  LPOLESTR lpstrText,
		/* [in] */ 
		__in __nullterminated  LPOLESTR lpstrCaption,
		/* [in] */ DWORD dwType,
		/* [in] */ 
		__in __nullterminated  LPOLESTR lpstrHelpFile,
		/* [in] */ DWORD dwHelpContext,
		/* [out] */ LRESULT *plResult);

	STDMETHOD(ShowHelp)( 
		/* [in] */ HWND hwnd,
		/* [in] */ 
		__in __nullterminated  LPOLESTR pszHelpFile,
		/* [in] */ UINT uCommand,
		/* [in] */ DWORD dwData,
		/* [in] */ POINT ptMouse,
		/* [out] */ IDispatch *pDispatchObjectHit);
	STDMETHOD(ShowContextMenu)(/* [in] */ DWORD dwID,
            /* [in] */ POINT __RPC_FAR *ppt,
            /* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
            /* [in] */ IDispatch __RPC_FAR *pdispReserved);
	STDMETHOD(GetHostInfo)( 
            /* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo);
	STDMETHOD(ShowUI)( 
            /* [in] */ DWORD dwID,
            /* [in] */ IOleInPlaceActiveObject __RPC_FAR *pActiveObject,
            /* [in] */ IOleCommandTarget __RPC_FAR *pCommandTarget,
            /* [in] */ IOleInPlaceFrame __RPC_FAR *pFrame,
            /* [in] */ IOleInPlaceUIWindow __RPC_FAR *pDoc);
	STDMETHOD(HideUI)(void);
	STDMETHOD(UpdateUI)(void);
	STDMETHOD(EnableModeless)(/* [in] */ BOOL fEnable);
	STDMETHOD(OnDocWindowActivate)(/* [in] */ BOOL fEnable);
	STDMETHOD(OnFrameWindowActivate)(/* [in] */ BOOL fEnable);
	STDMETHOD(ResizeBorder)( 
            /* [in] */ LPCRECT prcBorder,
            /* [in] */ IOleInPlaceUIWindow __RPC_FAR *pUIWindow,
            /* [in] */ BOOL fRameWindow);
	STDMETHOD(TranslateAccelerator)( 
            /* [in] */ LPMSG lpMsg,
            /* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
            /* [in] */ DWORD nCmdID);
	STDMETHOD(GetOptionKeyPath)( 
            /* [out] */ LPOLESTR __RPC_FAR *pchKey,
            /* [in] */ DWORD dw);
	STDMETHOD(GetDropTarget)(
            /* [in] */ IDropTarget __RPC_FAR *pDropTarget,
            /* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget);
    STDMETHOD(GetExternal)( 
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch);
    STDMETHOD(TranslateUrl)( 
            /* [in] */ DWORD dwTranslate,
            /* [in] */ OLECHAR __RPC_FAR *pchURLIn,
            /* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut);
    STDMETHOD(FilterDataObject)( 
            /* [in] */ IDataObject __RPC_FAR *pDO,
            /* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet);
    // IUnknown
    STDMETHOD_(ULONG,AddRef)();
    STDMETHOD_(ULONG,Release)();
    STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject);

    // IObjectWithSite
    STDMETHOD(SetSite)(IUnknown *pUnkSite);
    STDMETHOD(GetSite)(REFIID riid, LPVOID* ppvSite);

    // IOleClientSite
    STDMETHOD(SaveObject)(void);       
    STDMETHOD(GetMoniker)(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk);
    STDMETHOD(GetContainer)(IOleContainer** ppContainer);        
    STDMETHOD(ShowObject)(void);        
    STDMETHOD(OnShowWindow)(BOOL fShow);        
    STDMETHOD(RequestNewObjectLayout)(void);

    // IOleInPlaceSiteWindowless
    STDMETHOD(CanWindowlessActivate)(void);
    STDMETHOD(GetCapture)(void);
    STDMETHOD(SetCapture)(BOOL fCapture);
    STDMETHOD(GetFocus)(void);
    STDMETHOD(SetFocus)(BOOL fFocus);
    STDMETHOD(GetDC)(LPCRECT pRect, DWORD grfFlags, HDC* phDC);
    STDMETHOD(ReleaseDC)(HDC hDC);
    STDMETHOD(InvalidateRect)(LPCRECT pRect, BOOL fErase);
    STDMETHOD(InvalidateRgn)(HRGN hRGN, BOOL fErase);
    STDMETHOD(ScrollRect)(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip);
    STDMETHOD(AdjustRect)(LPRECT prc);
    STDMETHOD(OnDefWindowMessage)(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult);

    // IOleInPlaceSiteEx
    STDMETHOD(OnInPlaceActivateEx)(BOOL *pfNoRedraw, DWORD dwFlags);        
    STDMETHOD(OnInPlaceDeactivateEx)(BOOL fNoRedraw);       
    STDMETHOD(RequestUIActivate)(void);

    // IOleInPlaceSite
    STDMETHOD(CanInPlaceActivate)(void);       
    STDMETHOD(OnInPlaceActivate)(void);        
    STDMETHOD(OnUIActivate)(void);
    STDMETHOD(GetWindowContext)(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo);
    STDMETHOD(Scroll)(SIZE scrollExtant);
    STDMETHOD(OnUIDeactivate)(BOOL fUndoable);
    STDMETHOD(OnInPlaceDeactivate)(void);
    STDMETHOD(DiscardUndoState)( void);
    STDMETHOD(DeactivateAndUndo)( void);
    STDMETHOD(OnPosRectChange)(LPCRECT lprcPosRect);

    // IOleWindow
    STDMETHOD(GetWindow)(HWND* phwnd);
    STDMETHOD(ContextSensitiveHelp)(BOOL fEnterMode);

    // IOleControlSite
    STDMETHOD(OnControlInfoChanged)(void);      
    STDMETHOD(LockInPlaceActive)(BOOL fLock);       
    STDMETHOD(GetExtendedControl)(IDispatch** ppDisp);        
    STDMETHOD(TransformCoords)(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags);       
    STDMETHOD(TranslateAccelerator)(MSG* pMsg, DWORD grfModifiers);
    STDMETHOD(OnFocus)(BOOL fGotFocus);
    STDMETHOD(ShowPropertyFrame)(void);

    // IOleContainer
    STDMETHOD(EnumObjects)(DWORD grfFlags, IEnumUnknown** ppenum);
    STDMETHOD(LockContainer)(BOOL fLock);

    // IParseDisplayName
    STDMETHOD(ParseDisplayName)(IBindCtx* pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut);

protected:
    HRESULT CreateActiveXWnd();

protected:
    LONG m_dwRef;
    CActiveXUI* m_pOwner;
    CActiveXWnd* m_pWindow;
    IUnknown* m_pUnkSite;
    IViewObject* m_pViewObject;
    IOleInPlaceObjectWindowless* m_pInPlaceObject;
    bool m_bLocked;
    bool m_bFocused;
    bool m_bCaptured;
    bool m_bUIActivated;
    bool m_bInPlaceActive;
    bool m_bWindowless;
};

CActiveXCtrl::CActiveXCtrl() : 
m_dwRef(1), 
m_pOwner(NULL), 
m_pWindow(NULL),
m_pUnkSite(NULL), 
m_pViewObject(NULL),
m_pInPlaceObject(NULL),
m_bLocked(false), 
m_bFocused(false),
m_bCaptured(false),
m_bWindowless(true),
m_bUIActivated(false),
m_bInPlaceActive(false)
{
}

CActiveXCtrl::~CActiveXCtrl()
{
    if( m_pWindow != NULL ) {
        ::DestroyWindow(*m_pWindow);
        delete m_pWindow;
    }
    if( m_pUnkSite != NULL ) m_pUnkSite->Release();
    if( m_pViewObject != NULL ) m_pViewObject->Release();
    if( m_pInPlaceObject != NULL ) m_pInPlaceObject->Release();
}
///////////////////////////////disp//////////////////////////////////////////////

STDMETHODIMP CActiveXCtrl::GetTypeInfoCount( UINT* pctinfo )
{
   return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::GetTypeInfo( UINT itinfo,
                                LCID lcid,
                                ITypeInfo** pptinfo )
{
   // Not implemented
   return( E_NOTIMPL );
}

STDMETHODIMP CActiveXCtrl::GetIDsOfNames( REFIID riid,
										 OLECHAR** rgszNames,
										 UINT cNames,
										 LCID lcid,
										 DISPID* rgdispid )
{
	// To make things simple, we only support 1 name at a time
	if ( cNames > 1 )
		return( E_INVALIDARG );

	// Convert the member name to ANSI
	CHAR  szAnsi[128];
	long lLen = WideCharToMultiByte( CP_ACP,
		0,
		rgszNames[0],
		wcslen( rgszNames[0] ),
		szAnsi,
		sizeof( szAnsi ),
		0,
		0 );
	szAnsi[lLen] = '\0';

	// Compare the member name to see if it's one that we have
	// and return the correct DISPID
	if ( strncmp( "listAdShow", szAnsi, 10 ) == 0 )
		rgdispid[0] = DISPID_ADSHOW;
	else if(strncmp("listAdClose",szAnsi,11)==0)
		rgdispid[0]=DISPID_ADCLOSE;
	else if ( strncmp( "SendMessage", szAnsi, 11 ) == 0 )
		rgdispid[0] = DISPID_SENDMESSAGE;
	else if(strncmp("SendExe",szAnsi,7)==0)
		rgdispid[0]=DISPID_SENDEXE;
	else if(strncmp("reload",szAnsi,6)==0)
		rgdispid[0]=DISPID_RELOAD;
	else if(strncmp("SuperPowerPlay",szAnsi,14)==0)
		rgdispid[0]=DISPID_SUPERPOWERPLAY;
	else if(strncmp("isOk",szAnsi,4) == 0)
		rgdispid[0]=DISPID_ISOK;
	else if(strncmp("queryStatus",szAnsi,11) == 0)
		rgdispid[0]=DISPID_QUERYSTATUS;
	else if(strncmp("getClientVersion",szAnsi,16) == 0 )
		rgdispid[0]=DISPID_GETVERSION;
	else if(strncmp("NoticeFunc",szAnsi,10) == 0)
		rgdispid[0]=DISPID_NOTICEFUNC;
	else
		return( DISPID_UNKNOWN );

	return S_OK;
}
_bstr_t bstrErrorUrl;
STDMETHODIMP CActiveXCtrl::Invoke( DISPID dispid,
								  REFIID riid,
                           LCID lcid,
                           WORD wFlags,
                           DISPPARAMS FAR* pDispParams,
                           VARIANT FAR* pvarResult,
                           EXCEPINFO FAR* pExcepInfo,
						   unsigned int FAR* puArgErr )
{
	USES_CONVERSION;
	if (!pDispParams)      
		return E_INVALIDARG;

	switch( dispid )
	{
	case DISPID_ADSHOW:
		::PostMessage(m_pOwner->GetManager()->GetPaintWindow(),WM_USER+200,204,0);
		break;
	case DISPID_ADCLOSE:
		::PostMessage(m_pOwner->GetManager()->GetPaintWindow(),WM_USER+200,205,0);
		break;
	case DISPID_RELOAD:
		{
			CComQIPtr<IWebBrowser2> spWeb;
			m_pOwner->GetControl(IID_IWebBrowser2,(void**)&spWeb);

			if(m_pOwner->GetName() == _T("bhome"))
				spWeb->Navigate(bstrErrorUrl,NULL,NULL,NULL,NULL);
		}
		break;
	case	DISPID_SENDEXE:	   // 新的js函数
		{
			if ( !pDispParams || pDispParams->cArgs != 3 )
				return( DISP_E_BADPARAMCOUNT );

			if (pDispParams->rgvarg[2].vt!=VT_I4)
				return( DISP_E_TYPEMISMATCH );
			UINT msg = 0;
			msg = pDispParams->rgvarg[2].intVal;

			if(pDispParams->rgvarg[1].vt!=VT_BSTR)
				return ( DISP_E_TYPEMISMATCH );
			BSTR sValue;
			sValue = pDispParams->rgvarg[1].bstrVal;

			HRESULT hr = NOERROR;
			CComPtr<IDispatch> spDisp;
			int nLength=0;
			BSTR bstrLength = L"length",bstrIndex = L"IndexOf";
			DISPID dispid;

			if(pDispParams->rgvarg[0].vt!=VT_DISPATCH)
				return ( DISP_E_TYPEMISMATCH );
			spDisp = pDispParams->rgvarg[0].pdispVal;

			BSTR every  =  L"everydayplay";
			int nComp   = wcscmp(every,sValue); 
			BSTR FilmEveryday = L"kuaikanshipin";
			int nComp2  = wcscmp(FilmEveryday,sValue);
			if(nComp != 0 && nComp2 != 0)
			if(wcslen(sValue)>0)
			{	
				CComQIPtr<IWebBrowser2> spWeb;
				m_pOwner->GetControl(IID_IWebBrowser2,(void**)&spWeb);

				CComPtr<IDispatch> pIDisp=NULL;
				CComPtr<IHTMLDocument2>	m_pIHTMLDoc;

				hr=spWeb->get_Document(&pIDisp);

				if(FAILED(hr))		return S_FALSE;

				hr=pIDisp->QueryInterface(IID_IHTMLDocument2, (void**)&m_pIHTMLDoc);
				if(FAILED(hr))		return S_FALSE;

				pIDisp=NULL;
				hr=m_pIHTMLDoc->get_Script(&pIDisp);
				if(FAILED(hr))		return S_FALSE;

				hr=pIDisp->GetIDsOfNames(IID_NULL,&sValue,1,LOCALE_SYSTEM_DEFAULT,&dispid);
				if(FAILED(hr))  return S_FALSE;

				DISPPARAMS dispparams;
				memset(&dispparams,0,sizeof(DISPPARAMS));
				dispparams.cArgs=0;
				dispparams.rgvarg = NULL;
				dispparams.cNamedArgs = 0;
				dispparams.rgdispidNamedArgs = NULL;
				_variant_t vaResult;

				hr = pIDisp->Invoke(dispid,IID_NULL,0,DISPATCH_METHOD,&dispparams,&vaResult,NULL,NULL);	
				return S_OK;
			}

			hr = spDisp->GetIDsOfNames(IID_NULL, &bstrLength, 1,
				LOCALE_USER_DEFAULT, &dispid);
			if ( SUCCEEDED(hr) )//测试收到的数组是否有用
			{
				CComVariant varResult;
				DISPPARAMS prm;
				memset(&prm,0,sizeof(DISPPARAMS));
				prm.cArgs=0;
				hr = spDisp->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT,
					DISPATCH_PROPERTYGET, &prm, &varResult, NULL, NULL);

				if(!(SUCCEEDED(hr)))
					return hr;
				if(varResult.vt == VT_I4)
				{
					nLength = varResult.intVal;
				}
				else
					return (DISP_E_TYPEMISMATCH);
			}  
			else
				return hr;

			if(nLength<=0) return DISP_E_EXCEPTION;
			BSTR* buf=new BSTR[nLength];


			hr = spDisp->GetIDsOfNames(IID_NULL, &bstrIndex, 1,
				LOCALE_USER_DEFAULT, &dispid);

			if ( SUCCEEDED(hr) )//获得数组中的数据
			{
				for(int i=0;i<nLength;i++)
				{
					CComVariant varResult;
					DISPPARAMS prm;
					memset(&prm,0,sizeof(DISPPARAMS));
					prm.cArgs=1;

					VARIANTARG *pArg = new VARIANTARG;
					prm.rgvarg = pArg;
					memset(pArg,0,sizeof(VARIANTARG));
					prm.rgvarg[0].vt = VT_I4;
					prm.rgvarg[0].iVal = i;

					hr = spDisp->Invoke(dispid, IID_NULL, LOCALE_SYSTEM_DEFAULT,
						DISPATCH_METHOD, &prm, &varResult, NULL, NULL);

					if(!(SUCCEEDED(hr)))
					{
						delete pArg;
						return hr;
					}
					if(varResult.vt == VT_BSTR)
					{
						buf[i] = SysAllocString(varResult.bstrVal);
					}
					else
						return (DISP_E_TYPEMISMATCH);

					delete pArg;
				}
			}  
			else
				return hr;

			::SendMessage(m_pOwner->GetManager()->GetPaintWindow(),msg,(WPARAM)nLength,(LPARAM)buf);	

			delete[] buf;	   
		}
		break;
	case DISPID_SENDMESSAGE:	//// All of our methods take 3 parameters
		{
			if ( !pDispParams ||
				pDispParams->cArgs != 3 )
				return( DISP_E_BADPARAMCOUNT );

			// We don't support named arguments
			if ( pDispParams->cNamedArgs > 0 )
				return( DISP_E_NONAMEDARGS );

			// Break out the parameters and coerce them
			// to the proper type
			UINT msg = 0;
			int type = 0;
			BSTR buf[2];
			CComPtr<IDispatch> spDisp;

			if (pDispParams->rgvarg[2].vt!=VT_I4)
				return( DISP_E_TYPEMISMATCH );
			msg = pDispParams->rgvarg[2].intVal;

			if(pDispParams->rgvarg[1].vt!=VT_I4)
				return ( DISP_E_TYPEMISMATCH );
			type = pDispParams->rgvarg[1].iVal;


			//if(pDispParams->rgvarg[1].vt!=VT_BOOL)
			//	return ( DISP_E_TYPEMISMATCH );
			//if(pDispParams->rgvarg[1].boolVal==VARIANT_TRUE)
			//	type = TRUE;
			//else
			//	type = FALSE;

			if(pDispParams->rgvarg[0].vt!=VT_DISPATCH)
				return ( DISP_E_TYPEMISMATCH );
			spDisp = pDispParams->rgvarg[0].pdispVal;

			int nLength=0;
			BSTR bstrLength = L"length",bstrIndex = L"IndexOf";
			HRESULT hr = NOERROR;
			DISPID id;
			hr = spDisp->GetIDsOfNames(IID_NULL, &bstrLength, 1,
				LOCALE_USER_DEFAULT, &id);
			if ( SUCCEEDED(hr) )//测试收到的数组是否有用
			{
				CComVariant varResult;
				DISPPARAMS prm;
				memset(&prm,0,sizeof(DISPPARAMS));
				prm.cArgs=0;
				hr = spDisp->Invoke(id, IID_NULL, LOCALE_SYSTEM_DEFAULT,
					DISPATCH_PROPERTYGET, &prm, &varResult, NULL, NULL);

				if(!(SUCCEEDED(hr)))
					return hr;
				if(varResult.vt == VT_I4)
				{
					nLength = varResult.intVal;
					if(nLength!=2)
						return (DISP_E_TYPEMISMATCH);
				}
				else
					return (DISP_E_TYPEMISMATCH);
			}  
			else
				return hr;

			hr = spDisp->GetIDsOfNames(IID_NULL, &bstrIndex, 1,
				LOCALE_USER_DEFAULT, &id);
			if ( SUCCEEDED(hr) )//获得数组中的数据
			{
				for(int i=0;i<nLength;i++)
				{
					CComVariant varResult;
					DISPPARAMS prm;
					memset(&prm,0,sizeof(DISPPARAMS));
					prm.cArgs=1;

					VARIANTARG *pArg = new VARIANTARG;
					prm.rgvarg = pArg;
					memset(pArg,0,sizeof(VARIANTARG));
					prm.rgvarg[0].vt = VT_I4;
					prm.rgvarg[0].iVal = i;

					hr = spDisp->Invoke(id, IID_NULL, LOCALE_SYSTEM_DEFAULT,
						DISPATCH_METHOD, &prm, &varResult, NULL, NULL);

					if(!(SUCCEEDED(hr)))
					{
						delete pArg;
						return hr;
					}
					if(varResult.vt == VT_BSTR)
					{
						buf[i] = SysAllocString(varResult.bstrVal);
					}
					else
						return (DISP_E_TYPEMISMATCH);
				}
			}  
			else
				return hr;
			::SendMessage(m_pOwner->GetManager()->GetPaintWindow(),msg,(WPARAM)type,(LPARAM)buf);
		}
		break;
	case DISPID_GETVERSION:
		if (pvarResult != NULL)
		{
			VariantInit(pvarResult);
			V_VT(pvarResult) = VT_BSTR;
			//CString strExePath = GetExePath();
			//strExePath += _T("\\EverydayPlay.exe");
			//CString strVersion = GetExeVision(strExePath);strVersion.GetBuffer(strVersion.GetLength())
			pvarResult->bstrVal = SysAllocString(L"4.1.10.10");
			//strVersion.ReleaseBuffer();
		}
		break;
	case DISPID_NOTICEFUNC:
		{
			if ( !pDispParams ||
				pDispParams->cArgs != 2 )
				return( DISP_E_BADPARAMCOUNT );

			// We don't support named arguments
			if ( pDispParams->cNamedArgs > 0 )
				return( DISP_E_NONAMEDARGS );

			if (pDispParams->rgvarg[1].vt!=VT_BSTR)
				return( DISP_E_TYPEMISMATCH );

			if(pDispParams->rgvarg[0].vt!=VT_BSTR)
				return ( DISP_E_TYPEMISMATCH );

			::SendMessage(m_pOwner->GetManager()->GetPaintWindow(),WM_USER+112,(WPARAM)pDispParams->rgvarg[1].bstrVal,(LPARAM)pDispParams->rgvarg[0].bstrVal);
		}
		break;
	case DISPID_SUPERPOWERPLAY:
		{
			if ( !pDispParams ||
				pDispParams->cArgs != 3 )
				return( DISP_E_BADPARAMCOUNT );

			// We don't support named arguments
			if ( pDispParams->cNamedArgs > 0 )
				return( DISP_E_NONAMEDARGS );

			// Break out the parameters and coerce them
			// to the proper type
			UINT msg = 0;
			BOOL type = -1;
			BSTR buf[2];
			CComPtr<IDispatch> spDisp;

			if (pDispParams->rgvarg[2].vt!=VT_I4)
				return( DISP_E_TYPEMISMATCH );
			msg = pDispParams->rgvarg[2].intVal;

			if(pDispParams->rgvarg[1].vt!=VT_BOOL)
				return ( DISP_E_TYPEMISMATCH );
			if(pDispParams->rgvarg[1].boolVal==VARIANT_TRUE)
				type = TRUE;
			else
				type = FALSE;

			if(pDispParams->rgvarg[0].vt!=VT_DISPATCH)
				return ( DISP_E_TYPEMISMATCH );
			spDisp = pDispParams->rgvarg[0].pdispVal;

			int nLength=0;
			BSTR bstrLength = L"length",bstrIndex = L"IndexOf";
			HRESULT hr = NOERROR;
			DISPID id;
			hr = spDisp->GetIDsOfNames(IID_NULL, &bstrLength, 1,
				LOCALE_USER_DEFAULT, &id);
			if ( SUCCEEDED(hr) )//测试收到的数组是否有用
			{
				CComVariant varResult;
				DISPPARAMS prm;
				memset(&prm,0,sizeof(DISPPARAMS));
				prm.cArgs=0;
				hr = spDisp->Invoke(id, IID_NULL, LOCALE_SYSTEM_DEFAULT,
					DISPATCH_PROPERTYGET, &prm, &varResult, NULL, NULL);

				if(!(SUCCEEDED(hr)))
					return hr;
				if(varResult.vt == VT_I4)
				{
					nLength = varResult.intVal;
					if(nLength!=2)
						return (DISP_E_TYPEMISMATCH);
				}
				else
					return (DISP_E_TYPEMISMATCH);
			}  
			else
				return hr;

			hr = spDisp->GetIDsOfNames(IID_NULL, &bstrIndex, 1,
				LOCALE_USER_DEFAULT, &id);
			if ( SUCCEEDED(hr) )//获得数组中的数据
			{
				for(int i=0;i<nLength;i++)
				{
					CComVariant varResult;
					DISPPARAMS prm;
					memset(&prm,0,sizeof(DISPPARAMS));
					prm.cArgs=1;

					VARIANTARG *pArg = new VARIANTARG;
					prm.rgvarg = pArg;
					memset(pArg,0,sizeof(VARIANTARG));
					prm.rgvarg[0].vt = VT_I4;
					prm.rgvarg[0].iVal = i;

					hr = spDisp->Invoke(id, IID_NULL, LOCALE_SYSTEM_DEFAULT,
						DISPATCH_METHOD, &prm, &varResult, NULL, NULL);

					if(!(SUCCEEDED(hr)))
					{
						delete pArg;
						return hr;
					}
					if(varResult.vt == VT_BSTR)
					{
						buf[i] = SysAllocString(varResult.bstrVal);
					}
					else
						return (DISP_E_TYPEMISMATCH);
				}
			}  
			else
				return hr;
			::SendMessage(m_pOwner->GetManager()->GetPaintWindow(),msg,(WPARAM)type,(LPARAM)buf);
		}
		break;
	case DISPID_ISOK:
		::SendMessage(m_pOwner->GetManager()->GetPaintWindow(),WM_USER+101,(WPARAM)0,(LPARAM)0);
		break;
	case DISPID_QUERYSTATUS:
		::SendMessage(m_pOwner->GetManager()->GetPaintWindow(),WM_USER+102,(WPARAM)0,(LPARAM)0);
		break;
   case DISPID_NAVIGATECOMPLETE2:
	   // The parameters for this DISPID:      
	   // [0]: URL navigated to - VT_BYREF|VT_VARIANT      
	   // [1]: An object that evaluates to the top-level or frame      
	   //      WebBrowser object corresponding to the event.       
	   //     
	   if (pDispParams->rgvarg[0].vt == (VT_BYREF|VT_VARIANT))        
	   {            
		   CComVariant varURL(*pDispParams->rgvarg[0].pvarVal);     
		   varURL.ChangeType(VT_BSTR);      
		   TRACE(_T("caption"));
	   }        
	   break;  
   case DISPID_NAVIGATEERROR:
	   //定向到错误
	   {

		   bstrErrorUrl = pDispParams->rgvarg[3].pvarVal->bstrVal;
		   //首先记录出错的url
		   CComQIPtr<IWebBrowser2> spWeb;
		   m_pOwner->GetControl(IID_IWebBrowser2,(void**)&spWeb);

		   IDispatch* pDisp = pDispParams->rgvarg[4].pdispVal;
		   if (!spWeb.IsEqualObject(pDisp))
			   break;

		   _bstr_t sURL = _T("res://");	// 定位文件中的HTML资源
		   TCHAR path[MAX_PATH] = {0};
		   ::GetModuleFileName(NULL,path,MAX_PATH);
		   sURL += path;
		   if(m_pOwner->GetName() == _T("bhome"))
			   sURL += _T("/HTML_ERR");
		   else if(m_pOwner->GetName() == _T("bApply"))
			   sURL += _T("/HTML_APPLYERR");
		   spWeb->Navigate(sURL,NULL,NULL,NULL,NULL);
	   }
	   break;
   //case DISPID_NEWWINDOW3:

	  // ::ShellExecute(NULL,_T("open"),W2T(pDispParams->rgvarg[0].bstrVal),NULL,NULL,SW_SHOWNORMAL);
	  // *pDispParams->rgvarg[3].pboolVal = VARIANT_TRUE;
	  // break;
   default:
	   return( DISP_E_MEMBERNOTFOUND );
   }

   return S_OK;

}
///////////////////////////////disp//////////////////////////////////////////////

///////////////////////////////IOleControl////////////////////////////////////////////
STDMETHODIMP CActiveXCtrl::GetControlInfo( 
		/* [out][in] */ __RPC__inout CONTROLINFO *pCI)
{
	if(!pCI)
	{
		return E_POINTER;
	}
	pCI->hAccel = NULL; //load your accelerators here, if any   
	pCI->cAccel = 0;    
	pCI->dwFlags = 0;

	return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnMnemonic( 
		/* [in] */ __RPC__in MSG *pMsg)
{
	return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::OnAmbientPropertyChange( 
		/* [in] */ DISPID dispID)
{
	return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::FreezeEvents( 
		/* [in] */ BOOL bFreeze)
{
	return E_NOTIMPL;
}
///////////////////////////////IOleControl////////////////////////////////////////////
STDMETHODIMP CActiveXCtrl::ShowMessage(HWND hwnd,
		__in __nullterminated  LPOLESTR lpstrText,
		__in __nullterminated  LPOLESTR lpstrCaption,DWORD dwType,
		__in __nullterminated  LPOLESTR lpstrHelpFile,DWORD dwHelpContext,
		LRESULT *plResult)
{
	return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::ShowHelp(HWND hwnd,__in __nullterminated  LPOLESTR pszHelpFile,
		          UINT uCommand, DWORD dwData,POINT ptMouse,IDispatch *pDispatchObjectHit)
{
	return E_NOTIMPL;
}
// * CImpIDocHostUIHandler::GetHostInfo
// *
// * Purpose: Called at initialization
// *
STDMETHODIMP CActiveXCtrl::GetHostInfo( DOCHOSTUIINFO* pInfo )
{
	pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER;
    pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;

    return S_OK;
}

// * CImpIDocHostUIHandler::ShowUI
// *
// * Purpose: Called when MSHTML.DLL shows its UI
// *
STDMETHODIMP CActiveXCtrl::ShowUI(
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
STDMETHODIMP CActiveXCtrl::HideUI(void)
{
    return S_OK;
}

// * CImpIDocHostUIHandler::UpdateUI
// *
// * Purpose: Called when MSHTML.DLL updates its UI
// *
STDMETHODIMP CActiveXCtrl::UpdateUI(void)
{
	// MFC is pretty good about updating it's UI in it's Idle loop so I don't do anything here
	return S_OK;
}

// * CImpIDocHostUIHandler::EnableModeless
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::EnableModeless
// *
STDMETHODIMP CActiveXCtrl::EnableModeless(BOOL /*fEnable*/)
{
    return E_NOTIMPL;
}

// * CImpIDocHostUIHandler::OnDocWindowActivate
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::OnDocWindowActivate
// *
STDMETHODIMP CActiveXCtrl::OnDocWindowActivate(BOOL /*fActivate*/)
{
    return E_NOTIMPL;
}

// * CImpIDocHostUIHandler::OnFrameWindowActivate
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::OnFrameWindowActivate
// *
STDMETHODIMP CActiveXCtrl::OnFrameWindowActivate(BOOL /*fActivate*/)
{
    return E_NOTIMPL;
}

// * CImpIDocHostUIHandler::ResizeBorder
// *
// * Purpose: Called from MSHTML.DLL's IOleInPlaceActiveObject::ResizeBorder
// *
STDMETHODIMP CActiveXCtrl::ResizeBorder(
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
STDMETHODIMP  CActiveXCtrl::ShowContextMenu(
				DWORD dwID, 
				POINT* pptPosition,
				IUnknown* pCommandTarget,
				IDispatch* pDispatchObjectHit)
{
	if(dwID == CONTEXT_MENU_VSCROLL	|| dwID == CONTEXT_MENU_HSCROLL)
		return E_NOTIMPL;
	m_pOwner->m_pManager->SendNotify(m_pOwner,MST_IEMENU);
	return S_OK;  
}

// * CImpIDocHostUIHandler::TranslateAccelerator
// *
// * Purpose: Called from MSHTML.DLL's TranslateAccelerator routines
// *
STDMETHODIMP  CActiveXCtrl::TranslateAccelerator(LPMSG lpMsg,
            /* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
            /* [in] */ DWORD nCmdID)
{
	 TRACE(_T("AX: CActiveXCtrl::TranslateAccelerator"));
	 if((GetKeyState(VK_CONTROL)&0x80000000)&&(lpMsg->wParam=='A'||lpMsg->wParam=='O'||lpMsg->wParam=='N'||
												lpMsg->wParam=='F'||lpMsg->wParam=='L'||lpMsg->wParam=='M'||
												lpMsg->wParam=='P'))
		 return S_OK;
	 else if((GetKeyState(VK_MENU)&0x80000000)&&(lpMsg->wParam==VK_LEFT||lpMsg->wParam==VK_RIGHT))
		 return S_OK;
	 else if(VK_ESCAPE == lpMsg->wParam)
		 return S_OK;
     return S_FALSE;
}

// * CImpIDocHostUIHandler::GetOptionKeyPath
// *
// * Purpose: Called by MSHTML.DLL to find where the host wishes to store 
// *	its options in the registry
// *
STDMETHODIMP  CActiveXCtrl::GetOptionKeyPath(BSTR* pbstrKey, DWORD)
{

	return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::GetDropTarget( 
            /* [in] */ IDropTarget __RPC_FAR *pDropTarget,
            /* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget)
{
		return S_FALSE;
//    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::GetExternal( 
            /* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch)
{
	// return the IDispatch we have for extending the object Model
	//METHOD_PROLOGUE(CCustomControlSite, DocHostUIHandler)
	if(_tcscmp(m_pOwner->GetClass(), _T("ActiveXUI")) == 0 )
		QueryInterface(IID_IDispatch,(LPVOID*)ppDispatch);
	else
		*ppDispatch = static_cast<IDispatch*>(m_pOwner);
	return S_OK;
}
        
STDMETHODIMP CActiveXCtrl::TranslateUrl( 
            /* [in] */ DWORD dwTranslate,
            /* [in] */ OLECHAR __RPC_FAR *pchURLIn,
            /* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut)
{
    return E_NOTIMPL;
}
        
STDMETHODIMP CActiveXCtrl::FilterDataObject( 
            /* [in] */ IDataObject __RPC_FAR *pDO,
            /* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet)
{
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::QueryInterface(REFIID riid, LPVOID *ppvObject)
{
    *ppvObject = NULL;
    if( riid == IID_IUnknown )                       *ppvObject = static_cast<IOleWindow*>(this);
    else if( riid == IID_IOleClientSite )            *ppvObject = static_cast<IOleClientSite*>(this);
    else if( riid == IID_IOleInPlaceSiteWindowless ) *ppvObject = static_cast<IOleInPlaceSiteWindowless*>(this);
    else if( riid == IID_IOleInPlaceSiteEx )         *ppvObject = static_cast<IOleInPlaceSiteEx*>(this);
	else if( riid == IID_IOleInPlaceSite )           *ppvObject = static_cast<IOleInPlaceSite*>(this);
	else if( riid == IID_IOleWindow )                *ppvObject = static_cast<IOleWindow*>(this);
	else if( riid == IID_IOleControlSite )           *ppvObject = static_cast<IOleControlSite*>(this);
	else if( riid == IID_IOleContainer )             *ppvObject = static_cast<IOleContainer*>(this);
	else if( riid == IID_IObjectWithSite )           *ppvObject = static_cast<IObjectWithSite*>(this);
	else if( riid == IID_IDocHostShowUI )			 *ppvObject = static_cast<IDocHostShowUI*>(this);
	else if( riid == IID_IDocHostUIHandler )		 
	{
		if(_tcscmp(m_pOwner->GetClass(), _T("ActiveXUI")) == 0 )
			*ppvObject = static_cast<IDocHostUIHandler*>(this);
		else
			*ppvObject = static_cast<IDocHostUIHandler*>(m_pOwner);
	}
	else if( m_pOwner->m_clsid == CLSID_WebBrowser && riid == IID_IDispatch )		
		*ppvObject = static_cast<IDispatch*>(this);
	else if( riid == IID_IOleControl )				 *ppvObject = static_cast<IOleControl*>(this);

    if( *ppvObject != NULL ) AddRef();
    return *ppvObject == NULL ? E_NOINTERFACE : S_OK;
}

STDMETHODIMP_(ULONG) CActiveXCtrl::AddRef()
{
    return ++m_dwRef;
}

STDMETHODIMP_(ULONG) CActiveXCtrl::Release()
{
    LONG lRef = --m_dwRef;
    if( lRef == 0 ) delete this;
    return lRef;
}

STDMETHODIMP CActiveXCtrl::SetSite(IUnknown *pUnkSite)
{
    TRACE(_T("AX: CActiveXCtrl::SetSite"));
    if( m_pUnkSite != NULL ) {
        m_pUnkSite->Release();
        m_pUnkSite = NULL;
    }
    if( pUnkSite != NULL ) {
        m_pUnkSite = pUnkSite;
        m_pUnkSite->AddRef();
    }
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetSite(REFIID riid, LPVOID* ppvSite)
{
    TRACE(_T("AX: CActiveXCtrl::GetSite"));
    if( ppvSite == NULL ) return E_POINTER;
    *ppvSite = NULL;
    if( m_pUnkSite == NULL ) return E_FAIL;
    return m_pUnkSite->QueryInterface(riid, ppvSite);
}

STDMETHODIMP CActiveXCtrl::SaveObject(void)
{
    TRACE(_T("AX: CActiveXCtrl::SaveObject"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::GetMoniker(DWORD dwAssign, DWORD dwWhichMoniker, IMoniker** ppmk)
{
    TRACE(_T("AX: CActiveXCtrl::GetMoniker"));
    if( ppmk != NULL ) *ppmk = NULL;
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::GetContainer(IOleContainer** ppContainer)
{
    TRACE(_T("AX: CActiveXCtrl::GetContainer"));
    if( ppContainer == NULL ) return E_POINTER;
    *ppContainer = NULL;
    HRESULT Hr = E_NOTIMPL;
    if( m_pUnkSite != NULL ) Hr = m_pUnkSite->QueryInterface(IID_IOleContainer, (LPVOID*) ppContainer);
    if( FAILED(Hr) ) Hr = QueryInterface(IID_IOleContainer, (LPVOID*) ppContainer);
    return Hr;
}

STDMETHODIMP CActiveXCtrl::ShowObject(void)
{
    TRACE(_T("AX: CActiveXCtrl::ShowObject"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    HDC hDC = ::GetDC(m_pOwner->m_hwndHost);
    if( hDC == NULL ) return E_FAIL;
    if( m_pViewObject != NULL ) m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hDC, (RECTL*) &m_pOwner->m_rcItem, (RECTL*) &m_pOwner->m_rcItem, NULL, NULL);
    ::ReleaseDC(m_pOwner->m_hwndHost, hDC);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnShowWindow(BOOL fShow)
{
    TRACE(_T("AX: CActiveXCtrl::OnShowWindow"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::RequestNewObjectLayout(void)
{
    TRACE(_T("AX: CActiveXCtrl::RequestNewObjectLayout"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::CanWindowlessActivate(void)
{
    TRACE(_T("AX: CActiveXCtrl::CanWindowlessActivate"));
    return S_OK;  // Yes, we can!!
}

STDMETHODIMP CActiveXCtrl::GetCapture(void)
{
    TRACE(_T("AX: CActiveXCtrl::GetCapture"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    return m_bCaptured ? S_OK : S_FALSE;
}

STDMETHODIMP CActiveXCtrl::SetCapture(BOOL fCapture)
{
    TRACE(_T("AX: CActiveXCtrl::SetCapture"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    m_bCaptured = (fCapture == TRUE);
    if( fCapture ) ::SetCapture(m_pOwner->m_hwndHost); else ::ReleaseCapture();
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetFocus(void)
{
    TRACE(_T("AX: CActiveXCtrl::GetFocus"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    return m_bFocused ? S_OK : S_FALSE;
}

STDMETHODIMP CActiveXCtrl::SetFocus(BOOL fFocus)
{
    TRACE(_T("AX: CActiveXCtrl::SetFocus"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    if( fFocus ) m_pOwner->SetFocus();
    m_bFocused = (fFocus == TRUE);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetDC(LPCRECT pRect, DWORD grfFlags, HDC* phDC)
{
    TRACE(_T("AX: CActiveXCtrl::GetDC"));
    if( phDC == NULL ) return E_POINTER;
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    *phDC = ::GetDC(m_pOwner->m_hwndHost);
    if( (grfFlags & OLEDC_PAINTBKGND) != 0 ) {
        CRect rcItem = m_pOwner->GetPos();
        if( !m_bWindowless ) rcItem.ResetOffset();
        ::FillRect(*phDC, &rcItem, (HBRUSH) (COLOR_WINDOW + 1));
    }
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::ReleaseDC(HDC hDC)
{
    TRACE(_T("AX: CActiveXCtrl::ReleaseDC"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    ::ReleaseDC(m_pOwner->m_hwndHost, hDC);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::InvalidateRect(LPCRECT pRect, BOOL fErase)
{
    TRACE(_T("AX: CActiveXCtrl::InvalidateRect"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    if( m_pOwner->m_hwndHost == NULL ) return E_FAIL;
    return ::InvalidateRect(m_pOwner->m_hwndHost, pRect, fErase) ? S_OK : E_FAIL;
}

STDMETHODIMP CActiveXCtrl::InvalidateRgn(HRGN hRGN, BOOL fErase)
{
    TRACE(_T("AX: CActiveXCtrl::InvalidateRgn"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    return ::InvalidateRgn(m_pOwner->m_hwndHost, hRGN, fErase) ? S_OK : E_FAIL;
}

STDMETHODIMP CActiveXCtrl::ScrollRect(INT dx, INT dy, LPCRECT pRectScroll, LPCRECT pRectClip)
{
    TRACE(_T("AX: CActiveXCtrl::ScrollRect"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::AdjustRect(LPRECT prc)
{
    TRACE(_T("AX: CActiveXCtrl::AdjustRect"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnDefWindowMessage(UINT msg, WPARAM wParam, LPARAM lParam, LRESULT* plResult)
{
    TRACE(_T("AX: CActiveXCtrl::OnDefWindowMessage"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    *plResult = ::DefWindowProc(m_pOwner->m_hwndHost, msg, wParam, lParam);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnInPlaceActivateEx(BOOL* pfNoRedraw, DWORD dwFlags)        
{
    TRACE(_T("AX: CActiveXCtrl::OnInPlaceActivateEx"));
    ASSERT(m_pInPlaceObject==NULL);
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    if( m_pOwner->m_pUnk == NULL ) return E_UNEXPECTED;
	::OleLockRunning(m_pOwner->m_pUnk, TRUE, FALSE);
    HWND hWndFrame = m_pOwner->GetManager()->GetPaintWindow();
    HRESULT Hr = E_FAIL;
    if( (dwFlags & ACTIVATE_WINDOWLESS) != 0 ) {
        m_bWindowless = true;
        Hr = m_pOwner->m_pUnk->QueryInterface(IID_IOleInPlaceObjectWindowless, (LPVOID*) &m_pInPlaceObject);
        m_pOwner->m_hwndHost = hWndFrame;
        m_pOwner->GetManager()->AddMessageFilter(m_pOwner);
    }
    if( FAILED(Hr) ) {
        m_bWindowless = false;
        Hr = CreateActiveXWnd();
        if( FAILED(Hr) ) return Hr;
        Hr = m_pOwner->m_pUnk->QueryInterface(IID_IOleInPlaceObject, (LPVOID*) &m_pInPlaceObject);
    }
    if( m_pInPlaceObject != NULL ) {
        CRect rcItem = m_pOwner->m_rcItem;
        if( !m_bWindowless ) rcItem.ResetOffset();
        m_pInPlaceObject->SetObjectRects(&rcItem, &rcItem);
    }
    m_bInPlaceActive = SUCCEEDED(Hr);
    return Hr;
}

STDMETHODIMP CActiveXCtrl::OnInPlaceDeactivateEx(BOOL fNoRedraw)       
{
    TRACE(_T("AX: CActiveXCtrl::OnInPlaceDeactivateEx"));
    m_bInPlaceActive = false;
    if( m_pInPlaceObject != NULL ) {
        m_pInPlaceObject->Release();
        m_pInPlaceObject = NULL;
    }
    if( m_pWindow != NULL ) {
        ::DestroyWindow(*m_pWindow);
        delete m_pWindow;
        m_pWindow = NULL;
    }
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::RequestUIActivate(void)
{
    TRACE(_T("AX: CActiveXCtrl::RequestUIActivate"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::CanInPlaceActivate(void)       
{
    TRACE(_T("AX: CActiveXCtrl::CanInPlaceActivate"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnInPlaceActivate(void)
{
    TRACE(_T("AX: CActiveXCtrl::OnInPlaceActivate"));
    BOOL bDummy = FALSE;
    return OnInPlaceActivateEx(&bDummy, 0);
}

STDMETHODIMP CActiveXCtrl::OnUIActivate(void)
{
    TRACE(_T("AX: CActiveXCtrl::OnUIActivate"));
    m_bUIActivated = true;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetWindowContext(IOleInPlaceFrame** ppFrame, IOleInPlaceUIWindow** ppDoc, LPRECT lprcPosRect, LPRECT lprcClipRect, LPOLEINPLACEFRAMEINFO lpFrameInfo)
{
    TRACE(_T("AX: CActiveXCtrl::GetWindowContext"));
    if( ppDoc == NULL ) return E_POINTER;
    if( ppFrame == NULL ) return E_POINTER;
    if( lprcPosRect == NULL ) return E_POINTER;
    if( lprcClipRect == NULL ) return E_POINTER;
    *ppFrame = new CActiveXFrameWnd(m_pOwner);
    *ppDoc = NULL;
    ACCEL ac = { 0 };
    HACCEL hac = ::CreateAcceleratorTable(&ac, 1);
    lpFrameInfo->cb = sizeof(OLEINPLACEFRAMEINFO);
    lpFrameInfo->fMDIApp = FALSE;
    lpFrameInfo->hwndFrame = m_pOwner->GetManager()->GetPaintWindow();
    lpFrameInfo->haccel = hac;
    lpFrameInfo->cAccelEntries = 1;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::Scroll(SIZE scrollExtant)
{
    TRACE(_T("AX: CActiveXCtrl::Scroll"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::OnUIDeactivate(BOOL fUndoable)
{
    TRACE(_T("AX: CActiveXCtrl::OnUIDeactivate"));
    m_bUIActivated = false;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnInPlaceDeactivate(void)
{
    TRACE(_T("AX: CActiveXCtrl::OnInPlaceDeactivate"));
    return OnInPlaceDeactivateEx(TRUE);
}

STDMETHODIMP CActiveXCtrl::DiscardUndoState(void)
{
    TRACE(_T("AX: CActiveXCtrl::DiscardUndoState"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::DeactivateAndUndo(void)
{
    TRACE(_T("AX: CActiveXCtrl::DeactivateAndUndo"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::OnPosRectChange(LPCRECT lprcPosRect)
{
    TRACE(_T("AX: CActiveXCtrl::OnPosRectChange"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::GetWindow(HWND* phwnd)
{
    TRACE(_T("AX: CActiveXCtrl::GetWindow"));
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    if( m_pOwner->m_hwndHost == NULL ) CreateActiveXWnd();
    if( m_pOwner->m_hwndHost == NULL ) return E_FAIL;
    *phwnd = m_pOwner->m_hwndHost;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::ContextSensitiveHelp(BOOL fEnterMode)
{
    TRACE(_T("AX: CActiveXCtrl::ContextSensitiveHelp"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::OnControlInfoChanged(void)      
{
    TRACE(_T("AX: CActiveXCtrl::OnControlInfoChanged"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::LockInPlaceActive(BOOL fLock)       
{
    TRACE(_T("AX: CActiveXCtrl::LockInPlaceActive"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::GetExtendedControl(IDispatch** ppDisp)        
{
    TRACE(_T("AX: CActiveXCtrl::GetExtendedControl"));
    if( ppDisp == NULL ) return E_POINTER;   
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    if( m_pOwner->m_pUnk == NULL ) return E_UNEXPECTED;
    return m_pOwner->m_pUnk->QueryInterface(IID_IDispatch, (LPVOID*) ppDisp);
}

STDMETHODIMP CActiveXCtrl::TransformCoords(POINTL* pPtlHimetric, POINTF* pPtfContainer, DWORD dwFlags)       
{
    TRACE(_T("AX: CActiveXCtrl::TransformCoords"));
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::TranslateAccelerator(MSG *pMsg, DWORD grfModifiers)
{
    TRACE(_T("AX: CActiveXCtrl::TranslateAccelerator"));
    return S_FALSE;
}

STDMETHODIMP CActiveXCtrl::OnFocus(BOOL fGotFocus)
{
    TRACE(_T("AX: CActiveXCtrl::OnFocus"));
    m_bFocused = (fGotFocus == TRUE);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::ShowPropertyFrame(void)
{
    TRACE(_T("AX: CActiveXCtrl::ShowPropertyFrame"));
    return E_NOTIMPL;
}

STDMETHODIMP CActiveXCtrl::EnumObjects(DWORD grfFlags, IEnumUnknown** ppenum)
{
    TRACE(_T("AX: CActiveXCtrl::EnumObjects"));
    if( ppenum == NULL ) return E_POINTER;
    if( m_pOwner == NULL ) return E_UNEXPECTED;
    *ppenum = new CActiveXEnum(m_pOwner->m_pUnk);
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::LockContainer(BOOL fLock)
{
    TRACE(_T("AX: CActiveXCtrl::LockContainer"));
    m_bLocked = fLock != FALSE;
    return S_OK;
}

STDMETHODIMP CActiveXCtrl::ParseDisplayName(IBindCtx *pbc, LPOLESTR pszDisplayName, ULONG* pchEaten, IMoniker** ppmkOut)
{
    TRACE(_T("AX: CActiveXCtrl::ParseDisplayName"));
    return E_NOTIMPL;
}

HRESULT CActiveXCtrl::CreateActiveXWnd()
{
    if( m_pWindow != NULL ) return S_OK;
    m_pWindow = new CActiveXWnd;
    if( m_pWindow == NULL ) return E_OUTOFMEMORY;
    m_pOwner->m_hwndHost = m_pWindow->Init(this, m_pOwner->GetManager()->GetPaintWindow());
    return S_OK;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

HWND CActiveXWnd::Init(CActiveXCtrl* pOwner, HWND hWndParent)
{
    m_pOwner = pOwner;
    UINT uStyle = WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_CLIPCHILDREN;
    Create(hWndParent, _T("UIActiveX"), uStyle, 0L, 0,0,0,0, NULL);
    return m_hWnd;
}

LPCTSTR CActiveXWnd::GetWindowClassName() const
{
    return _T("ActiveXWnd");
}

void CActiveXWnd::OnFinalMessage(HWND hWnd)
{
    //delete this; // 这里不需要清理，CActiveXUI会清理的
}

void CActiveXWnd::DoVerb(LONG iVerb)
{
    if( m_pOwner == NULL ) return;
    if( m_pOwner->m_pOwner == NULL ) return;
    IOleObject* pUnk = NULL;
    m_pOwner->m_pOwner->GetControl(IID_IOleObject, (LPVOID*) &pUnk);
    if( pUnk == NULL ) return;
    CSafeRelease<IOleObject> RefOleObject = pUnk;
    IOleClientSite* pOleClientSite = NULL;
    m_pOwner->QueryInterface(IID_IOleClientSite, (LPVOID*) &pOleClientSite);
    CSafeRelease<IOleClientSite> RefOleClientSite = pOleClientSite;
    pUnk->DoVerb(iVerb, NULL, pOleClientSite, 0, m_hWnd, &m_pOwner->m_pOwner->GetPos());
}

LRESULT CActiveXWnd::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    LRESULT lRes;
    BOOL bHandled = TRUE;
    switch( uMsg ) {
    case WM_PAINT:         lRes = OnPaint(uMsg, wParam, lParam, bHandled); break;
    case WM_SETFOCUS:      lRes = OnSetFocus(uMsg, wParam, lParam, bHandled); break;
    case WM_KILLFOCUS:     lRes = OnKillFocus(uMsg, wParam, lParam, bHandled); break;
    case WM_ERASEBKGND:    lRes = OnEraseBkgnd(uMsg, wParam, lParam, bHandled); break;
    case WM_MOUSEACTIVATE: lRes = OnMouseActivate(uMsg, wParam, lParam, bHandled); break;
    default:
        bHandled = FALSE;
    }
    if( !bHandled ) return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
    return lRes;
}

LRESULT CActiveXWnd::OnEraseBkgnd(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    if( m_pOwner->m_pViewObject == NULL ) bHandled = FALSE;
    return 1;
}

LRESULT CActiveXWnd::OnMouseActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    IOleObject* pUnk = NULL;
    m_pOwner->m_pOwner->GetControl(IID_IOleObject, (LPVOID*) &pUnk);
    if( pUnk == NULL ) return 0;
    CSafeRelease<IOleObject> RefOleObject = pUnk;
    DWORD dwMiscStatus = 0;
    pUnk->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
    if( (dwMiscStatus & OLEMISC_NOUIACTIVATE) != 0 ) return 0;
    if( !m_pOwner->m_bInPlaceActive ) DoVerb(OLEIVERB_INPLACEACTIVATE);
    bHandled = FALSE;
    return 0;
}

LRESULT CActiveXWnd::OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    m_pOwner->m_bFocused = true;
    if( !m_pOwner->m_bUIActivated ) DoVerb(OLEIVERB_UIACTIVATE);
    return 0;
}

LRESULT CActiveXWnd::OnKillFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    bHandled = FALSE;
    m_pOwner->m_bFocused = false;
    return 0;
}

LRESULT CActiveXWnd::OnPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
{
    PAINTSTRUCT ps = { 0 };
    ::BeginPaint(m_hWnd, &ps);
    ::EndPaint(m_hWnd, &ps);
    return 1;
}


/////////////////////////////////////////////////////////////////////////////////////
//
//

CActiveXUI::CActiveXUI() : m_pUnk(NULL), m_pControl(NULL), m_hwndHost(NULL), m_bCreated(false)
{
}

CActiveXUI::~CActiveXUI()
{
    ReleaseControl();
}

LPCTSTR CActiveXUI::GetClass() const
{
    return _T("ActiveXUI");
}

static void PixelToHiMetric(const SIZEL* lpSizeInPix, LPSIZEL lpSizeInHiMetric)
{
#define HIMETRIC_PER_INCH   2540
#define MAP_PIX_TO_LOGHIM(x,ppli)   MulDiv(HIMETRIC_PER_INCH, (x), (ppli))
#define MAP_LOGHIM_TO_PIX(x,ppli)   MulDiv((ppli), (x), HIMETRIC_PER_INCH)
    int nPixelsPerInchX;    // Pixels per logical inch along width
    int nPixelsPerInchY;    // Pixels per logical inch along height
    HDC hDCScreen = ::GetDC(NULL);
    nPixelsPerInchX = ::GetDeviceCaps(hDCScreen, LOGPIXELSX);
    nPixelsPerInchY = ::GetDeviceCaps(hDCScreen, LOGPIXELSY);
    ::ReleaseDC(NULL, hDCScreen);
    lpSizeInHiMetric->cx = MAP_PIX_TO_LOGHIM(lpSizeInPix->cx, nPixelsPerInchX);
    lpSizeInHiMetric->cy = MAP_PIX_TO_LOGHIM(lpSizeInPix->cy, nPixelsPerInchY);
}

void CActiveXUI::SetVisible(bool bVisible)
{
    CControlUI::SetVisible(bVisible);
    if( m_hwndHost != NULL ) ::ShowWindow(m_hwndHost, IsVisible() ? SW_SHOW : SW_HIDE);
}

void CActiveXUI::SetInternVisible(bool bVisible)
{
    CControlUI::SetInternVisible(bVisible);
    if( m_hwndHost != NULL ) ::ShowWindow(m_hwndHost, IsVisible() ? SW_SHOW : SW_HIDE);
}

void CActiveXUI::SetPos(RECT rc)
{
    CControlUI::SetPos(rc);

    if( !m_bCreated ) DelayedControlCreation();

    if( m_pUnk == NULL ) return;
    if( m_pControl == NULL ) return;

    SIZEL hmSize = { 0 };
    SIZEL pxSize = { 0 };
    pxSize.cx = m_rcItem.right - m_rcItem.left;
    pxSize.cy = m_rcItem.bottom - m_rcItem.top;
    PixelToHiMetric(&pxSize, &hmSize);

    if( m_pUnk != NULL ) {
        m_pUnk->SetExtent(DVASPECT_CONTENT, &hmSize);
    }
    if( m_pControl->m_pInPlaceObject != NULL ) {
        CRect rcItem = m_rcItem;
        if( !m_pControl->m_bWindowless ) rcItem.ResetOffset();
        m_pControl->m_pInPlaceObject->SetObjectRects(&rcItem, &rcItem);
    }
    if( !m_pControl->m_bWindowless ) {
        ASSERT(m_pControl->m_pWindow);
        ::MoveWindow(*m_pControl->m_pWindow, m_rcItem.left, m_rcItem.top, m_rcItem.right - m_rcItem.left, m_rcItem.bottom - m_rcItem.top, TRUE);
    }
}

void CActiveXUI::DoPaint(HDC hDC, const RECT& rcPaint)
{
    if( !::IntersectRect(&m_rcPaint, &rcPaint, &m_rcItem) ) return;

    if( m_pControl != NULL && m_pControl->m_bWindowless && m_pControl->m_pViewObject != NULL )
    {
        m_pControl->m_pViewObject->Draw(DVASPECT_CONTENT, -1, NULL, NULL, NULL, hDC, (RECTL*) &m_rcItem, (RECTL*) &m_rcItem, NULL, NULL); 
    }
}

void CActiveXUI::SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue)
{
    if( _tcscmp(pstrName, _T("clsid")) == 0 ) CreateControl(pstrValue);
    else CControlUI::SetAttribute(pstrName, pstrValue);
}

LRESULT CActiveXUI::MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled)
{
    if( m_pControl == NULL ) return 0;
    ASSERT(m_pControl->m_bWindowless);
    if( !m_pControl->m_bInPlaceActive ) return 0;
    if( m_pControl->m_pInPlaceObject == NULL ) return 0;
    bool bWasHandled = true;
    if( (uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) || uMsg == WM_SETCURSOR )
    {
        // Mouse message only go when captured or inside rect
        DWORD dwHitResult = m_pControl->m_bCaptured ? HITRESULT_HIT : HITRESULT_OUTSIDE;
        if( dwHitResult == HITRESULT_OUTSIDE && m_pControl->m_pViewObject != NULL ) {
            IViewObjectEx* pViewEx = NULL;
            m_pControl->m_pViewObject->QueryInterface(IID_IViewObjectEx, (LPVOID*) &pViewEx);
            if( pViewEx != NULL ) {
                POINT ptMouse = { GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
                pViewEx->QueryHitPoint(DVASPECT_CONTENT, &m_rcItem, ptMouse, 0, &dwHitResult);
                pViewEx->Release();
            }
        }
        if( dwHitResult != HITRESULT_HIT ) return 0;
        if( uMsg == WM_SETCURSOR ) bWasHandled = false;
    }
    else if( uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST )
    {
        // Keyboard messages just go when we have focus
        if( !m_pControl->m_bFocused ) return 0;
    }
    else
    {
        switch( uMsg ) {
        case WM_HELP:
        case WM_CONTEXTMENU:
            bWasHandled = false;
            break;
        default:
            return 0;
        }
    }
    LRESULT lResult = 0;
    HRESULT Hr = m_pControl->m_pInPlaceObject->OnWindowMessage(uMsg, wParam, lParam, &lResult);
    if( Hr == S_OK ) bHandled = bWasHandled;
    return lResult;
}

bool CActiveXUI::CreateControl(LPCTSTR pstrCLSID, bool bDelayedCreate)
{
    CLSID clsid = { 0 };
    OLECHAR szCLSID[100] = { 0 };
#ifndef _UNICODE
    ::MultiByteToWideChar(::GetACP(), 0, pstrCLSID, -1, szCLSID, lengthof(szCLSID) - 1);
#else
    _tcsncpy(szCLSID, pstrCLSID, lengthof(szCLSID) - 1);
#endif
    if( pstrCLSID[0] == '{' ) ::CLSIDFromString(szCLSID, &clsid);
    else ::CLSIDFromProgID(szCLSID, &clsid);
    return CreateControl(clsid, bDelayedCreate);
}

bool CActiveXUI::CreateControl(const CLSID clsid, bool bDelayedCreate)
{
    ASSERT(clsid!=IID_NULL);
    if( clsid == IID_NULL ) return false;
    m_bCreated = false;
    m_clsid = clsid;
    if( !bDelayedCreate ) DelayedControlCreation();
    return true;
}

void CActiveXUI::ReleaseControl()
{
    m_hwndHost = NULL;
    if( m_pUnk != NULL ) {
        IObjectWithSite* pSite = NULL;
        m_pUnk->QueryInterface(IID_IObjectWithSite, (LPVOID*) &pSite);
        if( pSite != NULL ) {
            pSite->SetSite(NULL);
            pSite->Release();
        }
        m_pUnk->Close(OLECLOSE_NOSAVE);
        m_pUnk->SetClientSite(NULL);
        m_pUnk->Release(); 
        m_pUnk = NULL;
    }
    if( m_pControl != NULL ) {
        m_pControl->m_pOwner = NULL;
        m_pControl->Release();
        m_pControl = NULL;
    }
    m_pManager->RemoveMessageFilter(this);
}

bool CActiveXUI::DelayedControlCreation()
{
    ReleaseControl();
    // At this point we'll create the ActiveX control
    m_bCreated = true;
    IOleControl* pOleControl = NULL;
    HRESULT Hr = ::CoCreateInstance(m_clsid, NULL, CLSCTX_ALL, IID_IOleControl, (LPVOID*) &pOleControl);
    ASSERT(SUCCEEDED(Hr));
    if( FAILED(Hr) ) return false;
    pOleControl->QueryInterface(IID_IOleObject, (LPVOID*) &m_pUnk);
    pOleControl->Release();
    if( m_pUnk == NULL ) return false;
    // Create the host too
    m_pControl = new CActiveXCtrl();
    m_pControl->m_pOwner = this;
    // More control creation stuff
    DWORD dwMiscStatus = 0;
    m_pUnk->GetMiscStatus(DVASPECT_CONTENT, &dwMiscStatus);
    IOleClientSite* pOleClientSite = NULL;
    m_pControl->QueryInterface(IID_IOleClientSite, (LPVOID*) &pOleClientSite);
    CSafeRelease<IOleClientSite> RefOleClientSite = pOleClientSite;
    // Initialize control
    if( (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST) != 0 ) m_pUnk->SetClientSite(pOleClientSite);
    IPersistStreamInit* pPersistStreamInit = NULL;
    m_pUnk->QueryInterface(IID_IPersistStreamInit, (LPVOID*) &pPersistStreamInit);
    if( pPersistStreamInit != NULL ) {
        Hr = pPersistStreamInit->InitNew();
        pPersistStreamInit->Release();
    }
    if( FAILED(Hr) ) return false;
    if( (dwMiscStatus & OLEMISC_SETCLIENTSITEFIRST) == 0 ) m_pUnk->SetClientSite(pOleClientSite);
    // Grab the view...
    Hr = m_pUnk->QueryInterface(IID_IViewObjectEx, (LPVOID*) &m_pControl->m_pViewObject);
    if( FAILED(Hr) ) Hr = m_pUnk->QueryInterface(IID_IViewObject2, (LPVOID*) &m_pControl->m_pViewObject);
    if( FAILED(Hr) ) Hr = m_pUnk->QueryInterface(IID_IViewObject, (LPVOID*) &m_pControl->m_pViewObject);
    // Activate and done...
    m_pUnk->SetHostNames(OLESTR("UIActiveX"), NULL);
    if( (dwMiscStatus & OLEMISC_INVISIBLEATRUNTIME) == 0 ) {
        Hr = m_pUnk->DoVerb(OLEIVERB_INPLACEACTIVATE, NULL, pOleClientSite, 0, m_pManager->GetPaintWindow(), &m_rcItem);
        //::RedrawWindow(m_pManager->GetPaintWindow(), &m_rcItem, NULL, RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE | RDW_INTERNALPAINT | RDW_FRAME);
    }
    IObjectWithSite* pSite = NULL;
    m_pUnk->QueryInterface(IID_IObjectWithSite, (LPVOID*) &pSite);
	if( pSite != NULL ) {
		pSite->SetSite(static_cast<IOleClientSite*>(m_pControl));
		pSite->Release();
	}

	IConnectionPointContainer* pCPContainer;    
	// 获取连接点的指针. 
	HRESULT hr = m_pUnk->QueryInterface(IID_IConnectionPointContainer,(void**)&pCPContainer);   
	if (SUCCEEDED(hr))   {      
		// m_pConnectionPoint is defined like this: 
		IConnectionPoint* m_pConnectionPoint;       // Step 2: 选找连接点.      //      
		hr = pCPContainer->FindConnectionPoint(DIID_DWebBrowserEvents2, &m_pConnectionPoint);      
		if (SUCCEEDED(hr))      {         
			// 实现连接点地事件接收
			hr = m_pConnectionPoint->Advise(reinterpret_cast<IUnknown*>(m_pControl), &dwCookie);         
			if (FAILED(hr))         
			{            
				::MessageBox(NULL, _T("Failed to Advise"),_T("C++ Event Sink"), MB_OK); 
			}     
		}      
		pCPContainer->Release();   
	}
	return SUCCEEDED(Hr);
}

HRESULT CActiveXUI::GetControl(const IID iid, LPVOID* ppRet)
{
    ASSERT(ppRet!=NULL);
    ASSERT(*ppRet==NULL);
    if( ppRet == NULL ) return E_POINTER;
    if( m_pUnk == NULL ) return E_PENDING;
    return m_pUnk->QueryInterface(iid, (LPVOID*) ppRet);
}

}
