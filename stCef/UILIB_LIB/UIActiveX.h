#ifndef __UIACTIVEX_H__
#define __UIACTIVEX_H__

#pragma once

#include <MsHtmHst.h>
#include <ExDisp.h>

namespace UILIB_LIB
{
	/////////////////////////////////////////////////////////////////////////////////////
	//

	class CActiveXCtrl;
	//struct IOleObject;

	template< class T >
	class CSafeRelease
	{
	public:
		CSafeRelease(T* p) : m_p(p) { };
		~CSafeRelease() { if (m_p != NULL) m_p->Release(); };
		T* Detach() { T* t = m_p; m_p = NULL; return t; };
		T* m_p;
	};

	/////////////////////////////////////////////////////////////////////////////////////
	//

	class  CActiveXUI : public CControlUI
		, public IMessageFilterUI
		, public IDispatch
		, public IDocHostUIHandler
	{
		friend CActiveXCtrl;
	public:
		CActiveXUI();
		virtual ~CActiveXUI();

		LPCTSTR GetClass() const;

		bool CreateControl(const CLSID clsid, bool bDelayedCreate = false);
		bool CreateControl(LPCTSTR pstrCLSID, bool bDelayedCreate = false);
		HRESULT GetControl(const IID iid, LPVOID* ppRet);

		void SetVisible(bool bVisible = true);
		void SetInternVisible(bool bVisible = true);
		void SetPos(RECT rc);
		void DoPaint(HDC hDC, const RECT& rcPaint);

		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);

		LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);


		// interface IDocHostUIHandler
		virtual HRESULT STDMETHODCALLTYPE ShowContextMenu(
			/* [in] */ DWORD dwID,
			/* [in] */ POINT __RPC_FAR *ppt,
			/* [in] */ IUnknown __RPC_FAR *pcmdtReserved,
			/* [in] */ IDispatch __RPC_FAR *pdispReserved)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE GetHostInfo(
			/* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo)
		{
			// 		if (pInfo != NULL)
			// 		{
			// 			pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER;
			// 		}
			if (pInfo != NULL)
			{
				pInfo->cbSize = sizeof(DOCHOSTUIINFO);
				pInfo->dwFlags = DOCHOSTUIFLAG_NO3DBORDER/*|DOCHOSTUIFLAG_SCROLL_NO*/;
				pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
				return S_OK;

			}
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE ShowUI(
			/* [in] */ DWORD dwID,
			/* [in] */ IOleInPlaceActiveObject __RPC_FAR *pActiveObject,
			/* [in] */ IOleCommandTarget __RPC_FAR *pCommandTarget,
			/* [in] */ IOleInPlaceFrame __RPC_FAR *pFrame,
			/* [in] */ IOleInPlaceUIWindow __RPC_FAR *pDoc)
		{
			return S_FALSE;
		}

		virtual HRESULT STDMETHODCALLTYPE HideUI(void)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE UpdateUI(void)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE EnableModeless(
			/* [in] */ BOOL fEnable)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE OnDocWindowActivate(
			/* [in] */ BOOL fActivate)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE OnFrameWindowActivate(
			/* [in] */ BOOL fActivate)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE ResizeBorder(
			/* [in] */ LPCRECT prcBorder,
			/* [in] */ IOleInPlaceUIWindow __RPC_FAR *pUIWindow,
			/* [in] */ BOOL fRameWindow)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE TranslateAccelerator(
			/* [in] */ LPMSG lpMsg,
			/* [in] */ const GUID __RPC_FAR *pguidCmdGroup,
			/* [in] */ DWORD nCmdID)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE GetOptionKeyPath(
			/* [out] */ LPOLESTR __RPC_FAR *pchKey,
			/* [in] */ DWORD dw)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE GetDropTarget(
			/* [in] */ IDropTarget __RPC_FAR *pDropTarget,
			/* [out] */ IDropTarget __RPC_FAR *__RPC_FAR *ppDropTarget)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE GetExternal(
			/* [out] */ IDispatch __RPC_FAR *__RPC_FAR *ppDispatch)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE TranslateUrl(
			/* [in] */ DWORD dwTranslate,
			/* [in] */ OLECHAR __RPC_FAR *pchURLIn,
			/* [out] */ OLECHAR __RPC_FAR *__RPC_FAR *ppchURLOut)
		{
			return S_OK;
		}

		virtual HRESULT STDMETHODCALLTYPE FilterDataObject(
			/* [in] */ IDataObject __RPC_FAR *pDO,
			/* [out] */ IDataObject __RPC_FAR *__RPC_FAR *ppDORet)
		{
			return S_OK;
		}

		// IDispatch
		virtual HRESULT STDMETHODCALLTYPE GetTypeInfoCount(__RPC__out UINT *pctinfo)
		{
			return S_OK;
		}
		virtual HRESULT STDMETHODCALLTYPE GetTypeInfo(UINT iTInfo, LCID lcid, __RPC__deref_out_opt ITypeInfo **ppTInfo)
		{
			return S_OK;
		}
		virtual HRESULT STDMETHODCALLTYPE GetIDsOfNames(__RPC__in REFIID riid, __RPC__in_ecount_full(cNames) LPOLESTR *rgszNames, UINT cNames, LCID lcid, __RPC__out_ecount_full(cNames) DISPID *rgDispId)
		{
			return S_OK;
		}
		virtual HRESULT STDMETHODCALLTYPE Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams, VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr)
		{
			return S_OK;
		}

		// IUnknown
		STDMETHOD_(ULONG, AddRef)()
		{
			return S_OK;
		}
		STDMETHOD_(ULONG, Release)()
		{
			return S_OK;
		}
		STDMETHOD(QueryInterface)(REFIID riid, LPVOID *ppvObject)
		{
			return S_OK;
		}

	protected:
		void ReleaseControl();
		virtual bool DelayedControlCreation();

	protected:
		CLSID m_clsid;
		bool m_bCreated;
		IOleObject* m_pUnk;
		CActiveXCtrl* m_pControl;
		HWND m_hwndHost;
		DWORD dwCookie;

	};
}
#endif // __UIACTIVEX_H__

