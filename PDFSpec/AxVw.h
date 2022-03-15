// AxVw.h : CAxVw クラスのインターフェイス
//

#pragma once

#include <atlcoll.h>
#include <algorithm>
#include <cpp/poppler-document.h>
#include <cpp/poppler-page.h>
#include <cpp/poppler-page-renderer.h>

#define E_errOpenFile MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,2001)
#define E_errBadCatalog MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,2002)
#define E_errDamaged MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,2003)
#define E_errEncrypted MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,2004)
#define E_errHighlightFile MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,2005)
#define E_errBadPrinter MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,2006)
#define E_errPrinting MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,2007)
#define E_errPermission MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,2008)
#define E_errBadPageNum MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,2009)
#define E_errFileIO MAKE_HRESULT(SEVERITY_ERROR,FACILITY_ITF,2010)

class CPvRender {
public:
	virtual CBitmap *GetThumb(int iPage, int cx) = NULL;
};

class CRenderInf;
class CPDFRef : public IUnknown {
public:
	LONG locks;
	poppler::document*ref_pdfdoc;

	CPDFRef(poppler::document*ref_pdfdoc)
		: locks(0)
		, ref_pdfdoc(ref_pdfdoc)
	{

	}
	virtual ~CPDFRef() {
		delete ref_pdfdoc;
	}

    virtual HRESULT STDMETHODCALLTYPE QueryInterface( 
        /* [in] */ REFIID riid,
		/* [iid_is][out] */ __RPC__deref_out void __RPC_FAR *__RPC_FAR *ppvObject) {
			if (ppvObject == NULL)
				return E_POINTER;
			if (riid == IID_IUnknown) {
				*ppvObject = static_cast<IUnknown *>(this);
			}
			else {
				*ppvObject = NULL;
				return E_NOINTERFACE;
			}
			AddRef();
			return S_OK;
	}

	virtual ULONG STDMETHODCALLTYPE AddRef( void) {
		return InterlockedIncrement(&locks);
	}

	virtual ULONG STDMETHODCALLTYPE Release( void) {
		ULONG x = InterlockedDecrement(&locks);
		if (x == 0)
			delete this;
		return x;
	}
};

class CPPSummary { // pdf page summary
public:
	CRect mediaBox;
	int rotate;

	CPPSummary() {
		rotate = 0;
	}
};

// CAxVw ウィンドウ

class CAxVw : public CWnd, public CPvRender
{
// コンストラクション
public:
	CAxVw();

// 属性
public:
	CString m_strUrl;
	CString m_errorMessage;

protected:
	CComPtr<CPDFRef> m_prefpdf;
	poppler::document*m_pdfdoc;
	int m_iPage;
	float m_fZoom;
	typedef enum {
		ftZoom,
		ftW,
		ftWH,
	} FitMode;
	FitMode m_ft;
	SCROLLINFO m_hsc, m_vsc;
	std::unique_ptr<CRenderInf> m_renderAll;
	std::unique_ptr<CRenderInf> m_renderPart;
	CBitmap m_bmMask10;
	CWinThread *m_threadRenderer;

	CBitmap m_bmPrev, m_bmNext, m_bmAbout, m_bmMag, m_bmMagBtn, m_bmMove, m_bmMoveBtn, m_bmZoomVal, m_bmPageDisp, m_bmPrt;
	CRect m_rcPaint, m_rcPrev, m_rcNext, m_rcDisp, m_rcAbout, m_rcFitWH, m_rcFitW, m_rcPrt;
	CRect m_rcMMSel, m_rcZoomVal, m_rcCmdBar;
	bool m_toolZoom;
	CPoint m_ptBegin, m_ptScrollFrm;
	bool m_fDrag;
	CScrollBar m_sbH, m_sbV;
	bool m_fFitOnSmall;
	CAutoPtrArray<CBitmap> m_pThumbs;
	HCURSOR m_hcZoomIn, m_hcZoomOut;
	CArray<CPPSummary> m_pps;
	bool m_canPrintThisPDF;

// 操作
public:
	HRESULT LoadPDF(LPCTSTR psz);
	HRESULT LoadTruePDF(LPCTSTR psz);
	bool MovePage(int iDelta, bool force = false);
	bool SetPage(int iPage, bool force = false);
	void Setft(FitMode ft);
	void Zoom(float fDelta);
	void UpdScroll();
	void UnloadPDF();

	int CntPages();

	CRect GetPageRect(int top) const;

	void LayoutClient();

	void Zoomat(bool fIn, CPoint at);
	void ZoomatR(float zf, CPoint at);
	void SetzoomR(float zf);

	CPoint GetCenterPos() const;
	CPoint GetAbsPosAt(CPoint pt) const;
	void SetCenterAt(CPoint pt, CPoint clientpt);

	CPoint GetScrollOff() const {
		return CPoint(m_hsc.nPos, m_vsc.nPos);
	}
	void Setzf(float zf) {
		m_fZoom = zf;
		SetFit(ftZoom);
		InvalidateRect(m_rcZoomVal,false);
	}
	void SetFit(FitMode fit) {
		Setft(fit);
	}
	float Getzf() const;

	int Newxp(int v) {
		return (std::max)(m_hsc.nMin, (std::min)(m_hsc.nMax - (int)m_hsc.nPage + 1, v));
	}
	int Newyp(int v) const {
		return (std::max)(m_vsc.nMin, (std::min)(m_vsc.nMax - (int)m_vsc.nPage + 1, v));
	}
	CSize GetZoomedSize();

	bool IsActive() {
		return GetFocus() == this;
	}

	class Fitrect {
	public:
		static CRect Fit(CRect rcMax, CSize rcBox) {
            if (rcMax.Height() == 0 || rcBox.cy == 0)
                return rcMax;
            float frMax = rcMax.Width() / (float)rcMax.Height();
            float frBox = rcBox.cx / (float)rcBox.cy;
            float centerx = ((float)rcMax.left + rcMax.right) / 2;
			float centery = ((float)rcMax.top + rcMax.bottom) / 2;
            if (frMax >= frBox) {
                // 縦長
                float v = (float)rcBox.cx * rcMax.Height() / rcBox.cy / 2;
                return CRect(
                    int(centerx - v),
                    int(rcMax.top),
                    int(centerx + v),
                    int(rcMax.bottom)
                    );
            }
            else {
                // 横長
                float v = (float)rcBox.cy * rcMax.Width() / rcBox.cx / 2;
                return CRect(
                    int(rcMax.left),
                    int(centery - v),
                    int(rcMax.right),
                    int(centery + v)
                    );
            }
        }
	};

	CBitmap *GetThumb(int iPage, int cx);

	void FillHatch(CDC &dc, CRect rc);

	bool IsPDFReady() const {
		return m_pdfdoc != NULL;
	}

// オーバーライド
protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 実装
public:
	virtual ~CAxVw();

protected:
	bool WaitRendererThreadDone(DWORD milliseconds = 1) {
		if (m_threadRenderer != NULL) {
			if (WaitForSingleObject(m_threadRenderer->m_hThread, milliseconds) == WAIT_OBJECT_0) {
				delete m_threadRenderer;
				m_threadRenderer = NULL;
			}
		}
		return m_threadRenderer == NULL;
	}

	// 生成された、メッセージ割り当て関数
protected:
	afx_msg void OnPaint();
	afx_msg BOOL OnPageUp(UINT nID);
	afx_msg void OnUpdatePageUp(CCmdUI *pUI);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnSelCmd(UINT nID);
	afx_msg void OnUpdateSelCmd(CCmdUI *pUI);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg LRESULT OnMouseHWheel(WPARAM, LPARAM);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDblClk(UINT nFlags, CPoint point);
	afx_msg BOOL OnPageSel(UINT nID);
	afx_msg void OnUpdatePageSel(CCmdUI *pUI);
	afx_msg int OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg LRESULT OnSetRenderInf(WPARAM, LPARAM);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
};
