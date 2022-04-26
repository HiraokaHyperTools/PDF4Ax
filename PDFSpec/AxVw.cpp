// AxVw.cpp : CAxVw クラスの実装
//

#include "pch.h"
#include "res/PDFSpecResource.h"
#include "AxVw.h"

#undef max
#undef min

#include <algorithm>

#include "rijndael-alg-fst.h"

#include "PaperSizeUtil.h"
#include "FitRect3.h"

#define CAIRO_WIN32_STATIC_BUILD
#include <cairo-win32.h>
#include <cpp/poppler-page-printer.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define WM_SET_RENDERINF (WM_APP+0x0010)

namespace {
	bool GetTempPathName(TCHAR tctmp[MAX_PATH], LPCTSTR pszPrefix = NULL) {
		TCHAR tcdir[MAX_PATH] = { 0 };
		GetTempPath(256, tcdir);
		ZeroMemory(tctmp, sizeof(tctmp));
		if (GetTempFileName(tcdir, pszPrefix, 0, tctmp))
			return true;
		return false;
	}
};

// CPvMenu

CMutex s_lockpdf;

class CPvMenu : public CMenu {
	CPvRender& pv;
	int cxThumb;
public:

	CPvMenu(CPvRender& pv, int cxThumb = 150) : pv(pv), cxThumb(cxThumb) {
	}

	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) {
		DRAWITEMSTRUCT& di = *lpDrawItemStruct;

		CDC* pDC = CDC::FromHandle(di.hDC);
		if (pDC == NULL)
			return;

		bool fSel = 0 != (di.itemState & ODS_SELECTED);

		CRect rc = di.rcItem;

		COLORREF clrBk = GetSysColor(fSel ? COLOR_HIGHLIGHT : COLOR_WINDOW);
		CBrush br(clrBk);
		pDC->FillRect(rc, &br);

		rc.right = rc.left + 26;
		rc.DeflateRect(1, 1);
		int iPage = di.itemID - IDC_PAGE1;
		CString str; str.Format(_T("%d"), iPage + 1);

		pDC->SetTextColor(GetSysColor(fSel ? COLOR_HIGHLIGHTTEXT : COLOR_WINDOWTEXT));
		pDC->SetBkColor(clrBk);

		pDC->DrawText(str, rc, DT_SINGLELINE | DT_TOP | DT_LEFT);

		CBitmap* pic = pv.GetThumb(iPage, cxThumb);
		if (pic != NULL) {
			CDC dcMem;
			VERIFY(dcMem.CreateCompatibleDC(pDC));
			CBitmap* pOrg = dcMem.SelectObject(pic);
			pDC->BitBlt(rc.left + 26, rc.top, cxThumb - 2, cxThumb - 2, &dcMem, 0, 0, SRCCOPY);
			dcMem.SelectObject(pOrg);
		}
	}
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct) {
		lpMeasureItemStruct->itemHeight = cxThumb + 2;
		lpMeasureItemStruct->itemWidth = cxThumb + 26 + 2;
	}
};

// CRenderInf

class CRenderInf {
public:
	int iPage; // in. zero based
	double dpi; // in
	CSize sizeIn; // in
	CRect rcPartial; // in
	poppler::image imageOut; // out
	BITMAPINFO bi; // out
	bool partial; // out
	poppler::document* pdfdoc; // in
	CComPtr<IUnknown> prefcnt;

	HWND hwndCb; // in
	UINT nMsg; // in
};

// CAxVw

CAxVw::CAxVw() {
	m_pdfdoc = NULL;
	m_threadRenderer = NULL;
}

CAxVw::~CAxVw() {
	UnloadPDF();
}

#ifndef WM_MOUSEHWHEEL
#define WM_MOUSEHWHEEL 0x020E
#endif // WM_MOUSEHWHEEL

BEGIN_MESSAGE_MAP(CAxVw, CWnd)
	ON_WM_PAINT()
	ON_COMMAND_EX_RANGE(ID_PAGE_UP, ID_ZOOM_MINUS, OnPageUp)
	ON_UPDATE_COMMAND_UI_RANGE(ID_PAGE_UP, ID_ZOOM_MINUS, OnUpdatePageUp)
	ON_COMMAND_EX_RANGE(IDC_MAG, IDC_FITWH, OnSelCmd)
	ON_UPDATE_COMMAND_UI_RANGE(IDC_MAG, IDC_FITWH, OnUpdateSelCmd)
	ON_COMMAND_EX_RANGE(IDC_PAGE1, IDC_PAGE1000, OnPageSel)
	ON_UPDATE_COMMAND_UI_RANGE(IDC_PAGE1, IDC_PAGE1000, OnUpdatePageSel)
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_LBUTTONDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_MESSAGE(WM_MOUSEHWHEEL, OnMouseHWheel)

	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_HSCROLL()
	ON_WM_VSCROLL()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_RBUTTONDBLCLK()
	ON_WM_MOUSEACTIVATE()
	ON_WM_SETCURSOR()
	ON_MESSAGE(WM_SET_RENDERINF, OnSetRenderInf)
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	ON_WM_TIMER()
END_MESSAGE_MAP()

// CAxVw メッセージ ハンドラ

BOOL CAxVw::PreCreateWindow(CREATESTRUCT& cs)
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS,
		::LoadCursor(NULL, IDC_ARROW), NULL, NULL);
	// reinterpret_cast<HBRUSH>(COLOR_WINDOW+1)

	return TRUE;
}

CRect CAxVw::GetPageRect(int top) const {
	if (top < m_pps.GetSize()) {
		const CPPSummary& pps = m_pps[top];
		switch (pps.rotate) {
		case 90:
		case 270:
			return CRect(
				int(pps.mediaBox.top),
				int(pps.mediaBox.left),
				int(pps.mediaBox.bottom),
				int(pps.mediaBox.right)
			);
		default:
			return CRect(
				int(pps.mediaBox.left),
				int(pps.mediaBox.top),
				int(pps.mediaBox.right),
				int(pps.mediaBox.bottom)
			);
		}
	}
	return CRect();
}

UINT DrawPDFProc(LPVOID lpv) {
	CSingleLock lck(&s_lockpdf);
	std::unique_ptr<CRenderInf> inf(reinterpret_cast<CRenderInf*>(lpv));

	double dpi = inf->dpi;
	int iPage = inf->iPage;
	CSize sizeIn = inf->sizeIn;
	CRect rcOut = inf->rcPartial;
	if (sizeIn.cx * sizeIn.cy * 3 > 1024 * 1024 * 20) {
		inf->partial = true;
	}
	else {
		inf->partial = false;

		rcOut = CRect(CPoint(0, 0), inf->sizeIn);
	}

	std::unique_ptr<poppler::page> pageOut(inf->pdfdoc->create_page(iPage));

	poppler::page_renderer renderer;
	renderer.set_image_format(poppler::image::format_bgr24);
	renderer.set_render_hint(poppler::page_renderer::antialiasing);
	renderer.set_render_hint(poppler::page_renderer::text_antialiasing);

	poppler::image imageOut(
		renderer.render_page(pageOut.get(), dpi, dpi, rcOut.left, rcOut.top, rcOut.Width(), rcOut.Height())
	);

	lck.Unlock();

	int pcx = imageOut.width();
	int pcy = imageOut.height();
	BITMAPINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bi.bmiHeader.biWidth = pcx;
	bi.bmiHeader.biHeight = -pcy;
	bi.bmiHeader.biBitCount = 24;
	bi.bmiHeader.biPlanes = 1;

	const int dstPitch = (3 * pcx + 3) & (~3);

	ASSERT(imageOut.bytes_per_row() >= dstPitch);

#if 0
	int dstPitch = (3 * pcx + 3) & (~3);

	for (int y = 0; y < pcy; y++) {
		BYTE* pRow = reinterpret_cast<BYTE*>(imageOut.data()) + imageOut.bytes_per_row() * y;
		for (int x = 0; x < pcx; x++, pRow += 4) {
			BYTE tmp = pRow[2];
			pRow[2] = pRow[0];
			pRow[0] = tmp;
		}
	}
#endif

	inf->dpi = dpi;
	inf->imageOut = imageOut;
	inf->bi = bi;
	if (::PostMessage(inf->hwndCb, inf->nMsg, 0, reinterpret_cast<LPARAM>(inf.get()))) {
		inf.release();
	}
	else {

	}

	return 0;
}

void CAxVw::OnPaint()
{
	CPaintDC dc(this);

	CRect rc = m_rcPaint;

	if (m_pdfdoc == NULL) {
		CBrush br1;
		br1.CreateStockObject(WHITE_BRUSH);
		dc.FillRect(rc, &br1);
	}
	else if (dc.RectVisible(m_rcPaint)) {
		CSize size = GetZoomedSize();
		int cx = size.cx;
		int cy = size.cy;

		int xp = -m_hsc.nPos;
		int yp = -m_vsc.nPos;

		if (cx < m_rcPaint.Width()) {
			xp = (m_rcPaint.Width() - cx) / 2;
		}
		if (cy < m_rcPaint.Height()) {
			yp = (m_rcPaint.Height() - cy) / 2;
		}

		int vx = std::max(0, -xp);
		int vy = std::max(0, -yp);

		int dx = std::max(0, xp - vx);
		int dy = std::max(0, yp - vy);

		double scale = Getzf();

		double dpi = 72 * scale;

		bool need = false;
		bool drawwip = false;
		bool clear = false;

		int tx = std::max(m_hsc.nPos, 0);
		int ty = std::max(m_vsc.nPos, 0);
		int tcx = std::min(cx, rc.Width());
		int tcy = std::min(cy, rc.Height());
		CRect rcPartial(tx, ty, tx + tcx, ty + tcy);

		if (m_renderPart.get() != NULL && m_renderPart->iPage == m_iPage && m_renderPart->rcPartial == rcPartial) {
			poppler::image& bitmap = m_renderPart->imageOut;
			int pcx = bitmap.width();
			int pcy = bitmap.height();

			int state = dc.SaveDC();
			dc.IntersectClipRect(m_rcPaint);
			dc.SetStretchBltMode(HALFTONE);
			dc.SetBrushOrg(xp, yp);
			SetDIBitsToDevice(
				dc, xp + rcPartial.left, yp + rcPartial.top, rcPartial.Width(), rcPartial.Height(), 0, 0, 0, rcPartial.Height(),
				bitmap.data(), &m_renderPart->bi, DIB_RGB_COLORS
			);
			if (state != 0) dc.RestoreDC(state);
		}
		else if (m_renderAll.get() != NULL && m_renderAll->iPage == m_iPage) {
			poppler::image& bitmap = m_renderAll->imageOut;
			int pcx = bitmap.width();
			int pcy = bitmap.height();

			int state = dc.SaveDC();
			dc.IntersectClipRect(m_rcPaint);
			dc.SetStretchBltMode(HALFTONE);
			dc.SetBrushOrg(xp, yp);
			StretchDIBits(
				dc, xp, yp, cx, cy, 0, 0, pcx, pcy, bitmap.data(), &m_renderAll->bi, DIB_RGB_COLORS, SRCCOPY
			);
			if (state != 0) dc.RestoreDC(state);

			if (fabs(m_renderAll->dpi - dpi) > 1) {
				need = true;
			}
			if (m_renderAll->iPage != m_iPage) {
				need = true;
				drawwip = true;
			}
		}
		else {
			clear = true;
			need = true;
			drawwip = true;
		}

		if (need) {
			if (WaitRendererThreadDone(1)) {
				CRenderInf* inf = new CRenderInf();
				inf->sizeIn = size;
				inf->rcPartial = rcPartial;
				inf->iPage = m_iPage;
				inf->dpi = dpi;
				inf->pdfdoc = m_pdfdoc;
				inf->hwndCb = *this;
				inf->nMsg = WM_SET_RENDERINF;

				m_threadRenderer = AfxBeginThread(DrawPDFProc, inf, 0, 0, CREATE_SUSPENDED);
				m_threadRenderer->m_bAutoDelete = false;
				m_threadRenderer->ResumeThread();
			}
		}

		if (drawwip) {
			CRect rc(dx, dy, dx + cx, dy + cy);
			CRect rc2;
			if (rc2.IntersectRect(rc, m_rcPaint))
				rc = rc2;
			if (clear) {
				CBrush br0;
				br0.CreateStockObject(WHITE_BRUSH);
				dc.FillRect(rc, &br0);
			}
			else {
#if 0
				CDC dcMem;
				dcMem.CreateCompatibleDC(&dc);
				CBitmap* pOrg = dcMem.SelectObject(&m_bmMask10);
				for (int by = rc.top; by < rc.bottom; by += 64) {
					for (int bx = rc.left; bx < rc.right; bx += 64) {
						dc.BitBlt(bx, by, std::min((int)rc.right - bx, 64), std::min((int)rc.bottom - by, 64), &dcMem, 0, 0, SRCPAINT);
					}
				}
				dcMem.SelectObject(pOrg);
#endif
			}
			CFont font;
			font.CreateStockObject(DEFAULT_GUI_FONT);
			LOGFONT lf;
			if (0 != font.GetLogFont(&lf)) {
				lf.lfHeight *= 3;
				font.DeleteObject();
				font.CreateFontIndirect(&lf);
			}
			{
				CFont* pOrg = dc.SelectObject(&font);
				dc.DrawText(_T("お待ちください..."), rc, DT_SINGLELINE | DT_CENTER | DT_VCENTER);
				dc.SelectObject(pOrg);
			}
		}

		int state = dc.SaveDC();
		dc.ExcludeClipRect(dx, dy, dx + cx, dy + cy);
		CBrush br;
		br.CreateSysColorBrush(COLOR_3DDKSHADOW);
		dc.FillRect(m_rcPaint, &br);
		dc.RestoreDC(state);

		dc.ExcludeClipRect(m_rcPaint);
	}

	bool fHatch = !IsActive();

	if (dc.RectVisible(m_rcPrev)) {
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap* pOrg = dcMem.SelectObject(&m_bmPrev);
		dc.BitBlt(m_rcPrev.left, m_rcPrev.top, m_rcPrev.Width(), m_rcPrev.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pOrg);
		if (fHatch) FillHatch(dc, m_rcPrev);
		dc.ExcludeClipRect(m_rcPrev);
	}
	if (dc.RectVisible(m_rcNext)) {
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap* pOrg = dcMem.SelectObject(&m_bmNext);
		dc.BitBlt(m_rcNext.left, m_rcNext.top, m_rcNext.Width(), m_rcNext.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pOrg);
		if (fHatch) FillHatch(dc, m_rcNext);
		dc.ExcludeClipRect(m_rcNext);
	}
	if (dc.RectVisible(m_rcDisp)) {
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap* pOrg = dcMem.SelectObject(&m_bmPageDisp);
		dc.BitBlt(m_rcDisp.left, m_rcDisp.top, m_rcDisp.Width(), m_rcDisp.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pOrg);

		dc.SelectStockObject(DEFAULT_GUI_FONT);
		CString str;
		if (m_iPage < CntPages())
			str.Format(_T("%u/%u"), 1 + m_iPage, CntPages());

		//COLORREF lastBkClr = dc.SetBkColor(GetSysColor(COLOR_WINDOW));
		UINT lastMode = dc.SetTextAlign(TA_CENTER | TA_TOP);
		CSize size = dc.GetTextExtent(str);
		CPoint pt = m_rcDisp.CenterPoint() - CPoint(0, size.cy / 2);
		dc.ExtTextOut(pt.x, pt.y, ETO_CLIPPED, m_rcDisp, str, str.GetLength(), NULL);
		dc.SetTextAlign(lastMode);
		//dc.SetBkColor(lastBkClr);

		if (fHatch) FillHatch(dc, m_rcDisp);
		dc.ExcludeClipRect(m_rcDisp);
	}
	if (dc.RectVisible(m_rcAbout)) {
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap* pOrg = dcMem.SelectObject(&m_bmAbout);
		dc.BitBlt(m_rcAbout.left, m_rcAbout.top, m_rcAbout.Width(), m_rcAbout.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pOrg);
		if (fHatch) FillHatch(dc, m_rcAbout);
		dc.ExcludeClipRect(m_rcAbout);
	}
	if (dc.RectVisible(m_rcPrt)) {
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap* pOrg = dcMem.SelectObject(&m_bmPrt);
		dc.BitBlt(m_rcPrt.left, m_rcPrt.top, m_rcPrt.Width(), m_rcPrt.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pOrg);
		if (fHatch) FillHatch(dc, m_rcPrt);
		dc.ExcludeClipRect(m_rcPrt);
	}
	if (dc.RectVisible(m_rcMMSel)) {
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap* pOrg = dcMem.SelectObject(m_toolZoom ? &m_bmMagBtn : &m_bmMoveBtn);
		dc.BitBlt(m_rcMMSel.left, m_rcMMSel.top, m_rcMMSel.Width(), m_rcMMSel.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pOrg);
		if (fHatch) FillHatch(dc, m_rcMMSel);
		dc.ExcludeClipRect(m_rcMMSel);
	}
	if (dc.RectVisible(m_rcZoomVal)) {
		CDC dcMem;
		dcMem.CreateCompatibleDC(&dc);
		CBitmap* pOrg = dcMem.SelectObject(&m_bmZoomVal);
		dc.BitBlt(m_rcZoomVal.left, m_rcZoomVal.top, m_rcZoomVal.Width(), m_rcZoomVal.Height(), &dcMem, 0, 0, SRCCOPY);
		dcMem.SelectObject(pOrg);

		dc.SelectStockObject(DEFAULT_GUI_FONT);
		CString str;
		str.Format(_T("%u %%"), (int)(100 * Getzf()));

		//COLORREF lastBkClr = dc.SetBkColor(GetSysColor(COLOR_WINDOW));
		UINT lastMode = dc.SetTextAlign(TA_CENTER | TA_TOP);
		COLORREF lastTextClr = dc.SetTextColor(RGB(0, 0, 0));
		CSize size = dc.GetTextExtent(str);
		CPoint pt = m_rcZoomVal.CenterPoint() - CPoint(0, size.cy / 2);
		dc.ExtTextOut(pt.x, pt.y, ETO_CLIPPED, m_rcZoomVal, str, str.GetLength(), NULL);
		dc.SetTextColor(lastTextClr);
		dc.SetTextAlign(lastMode);
		//dc.SetBkColor(lastBkClr);

		if (fHatch) FillHatch(dc, m_rcZoomVal);
		dc.ExcludeClipRect(m_rcZoomVal);
	}

	{
		CBrush br;
		br.CreateSysColorBrush(COLOR_WINDOW);
		CRect rc;
		GetClientRect(rc); //dc.GetClipBox(rc);
		dc.FillRect(rc, &br);
	}
}

void CAxVw::UnloadPDF() {
	WaitRendererThreadDone(INFINITE);

	m_pdfdoc = NULL;
	m_prefpdf.Release();

	m_strUrl.Empty();

	m_canPrintThisPDF = false;
}

HRESULT CAxVw::LoadPDF(LPCTSTR newVal) {
	bool maybePDF = false;
	bool maybeItsPDF = false;

	CFile fUnk;
	if (!fUnk.Open(newVal, CFile::modeRead | CFile::shareDenyWrite)) {
		m_errorMessage.Format(IDS_STRING2001);
		return E_errOpenFile;
	}

	BYTE buff[4];
	if (fUnk.Read(buff, 4) != 4) {
		m_errorMessage.Format(IDS_STRING2003);
		return E_errDamaged;
	}

	if (memcmp(buff, "%PDF", 4) == 0) {
		return LoadTruePDF(newVal);
	}

	TCHAR tcTmpfp[MAX_PATH] = { 0 };
	if (!GetTempPathName(tcTmpfp)) {
		m_errorMessage.Format(IDS_STRING2010);
		return E_errFileIO;
	}

	CFile fTmp;
	if (!fTmp.Open(tcTmpfp, CFile::modeCreate | CFile::modeReadWrite | CFile::shareDenyWrite)) {
		m_errorMessage.Format(IDS_STRING2010);
		return E_errFileIO;
	}

	fUnk.SeekToBegin();
	int t = 0;
	u32 rk[100];
	int nr = rijndaelKeySetupDec(rk, reinterpret_cast<const u8*>(static_cast<LPCSTR>(CW2A(L"*?x" L"l%\"" L"G|`W],a$" L"2y"))), 128);
	while (true) {
		BYTE buffIn[16], buffOut[16];
		if (fUnk.Read(buffIn, 16) != 16)
			break;
		rijndaelDecrypt(rk, nr, buffIn, buffOut);
		if (t == 0 && memcmp(buffOut, "%PDF", 4) != 0) {
			m_errorMessage.Format(IDS_STRING2003);
			return E_errDamaged;
		}
		maybeItsPDF = true;
		fTmp.Write(buffOut, 16);
		++t;
	}
	fTmp.Close();

	return LoadTruePDF(tcTmpfp);
}

class ErrorCatcher {
public:
	static CStringA text;

	static void Empty(int, char*, va_list)
	{

	}

	static void Capture(int pos, char* msg, va_list args)
	{
		if (pos >= 0) {
			text.AppendFormat("Error (%d): ", pos);
		}
		else {
			text.AppendFormat("Error: ");
		}
		text.AppendFormatV(msg, args);
		text.Append("\n");
	}
};

CStringA ErrorCatcher::text;

HRESULT CAxVw::LoadTruePDF(LPCTSTR newVal) {
	UnloadPDF();

	m_errorMessage.Empty();

	DWORD atts = GetFileAttributes(newVal);
	if (atts == 0xFFFFFFFFU || (atts & FILE_ATTRIBUTE_DIRECTORY) != 0) {
		m_errorMessage.Format(IDS_STRING2010);
		return E_errFileIO;
	}

	CStringW str = newVal;

	ErrorCatcher::text.Empty();
	//setErrorFunction(ErrorCatcher::Capture);

	{
		CSingleLock lck(&s_lockpdf);

		m_pdfdoc = poppler::document::load_from_file(std::string(CW2AEX(str, 65001)));
	}

	//setErrorFunction(ErrorCatcher::Empty);

	int errorCode = 1;//TODO m_pdfdoc->getErrorCode();
	if (m_pdfdoc == NULL) {
		UnloadPDF();
		m_errorMessage = ErrorCatcher::text;
		return MAKE_HRESULT(SEVERITY_ERROR, FACILITY_ITF, 2000 + errorCode);
	}

	m_prefpdf.Release();
	m_prefpdf = new CPDFRef(m_pdfdoc);

	m_iPage = 0;
	m_ft = ftWH;
	m_fZoom = 1;

	m_pps.RemoveAll();
	int cx = std::min(1000, m_pdfdoc->pages());
	for (int x = 0; x < cx; x++) {
		CPPSummary pps;
		std::unique_ptr<poppler::page> page(m_pdfdoc->create_page(x));
		if (page != NULL) {
			poppler::rectf prc = page->page_rect(poppler::page_box_enum::media_box);
			pps.mediaBox.SetRect((int)prc.left(), (int)prc.top(), (int)prc.right(), (int)prc.bottom());
			pps.rotate = 0;//TODO page->getRotate();
		}
		m_pps.Add(pps);
	}

	m_canPrintThisPDF = m_pdfdoc->has_permission(poppler::perm_print);

	LayoutClient();

	SetPage(0, true);

	m_strUrl = newVal;
	return S_OK;
}

int CAxVw::CntPages() {
	return m_pps.GetSize();
}

void CAxVw::OnUpdatePageUp(CCmdUI* pUI) {
	switch (pUI->m_nID) {
	case ID_PAGE_UP:
		pUI->Enable(m_iPage != 0);
		break;
	case ID_PAGE_DOWN:
		pUI->Enable(m_iPage < CntPages() - 1);
		break;
	case ID_PAGE_FITW:
		pUI->SetRadio(m_ft == ftW);
		break;
	case ID_PAGE_FITWH:
		pUI->SetRadio(m_ft == ftWH);
		break;
	}
}

BOOL CAxVw::OnPageUp(UINT nID) {
	switch (nID) {
	case ID_PAGE_UP:
		MovePage(-1);
		return true;
	case ID_PAGE_DOWN:
		MovePage(1);
		return true;
	case ID_PAGE_FITW:
		Setft(ftW);
		return true;
	case ID_PAGE_FITWH:
		Setft(ftWH);
		return true;
	case ID_ZOOM_PLUS:
		Zoom(1.5f);
		return true;
	case ID_ZOOM_MINUS:
		Zoom(1 / 1.5f);
		return true;
	}

	return false;
}

int CAxVw::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (false
		|| !m_sbH.Create(WS_CHILDWINDOW | WS_VISIBLE | SBS_HORZ | SBS_BOTTOMALIGN, CRect(), this, IDC_HORZ)
		|| !m_sbV.Create(WS_CHILDWINDOW | WS_VISIBLE | SBS_VERT | SBS_RIGHTALIGN, CRect(), this, IDC_VERT))
		return -1;

	if (false
		|| !m_bmPrev.LoadBitmap(IDB_PREV)
		|| !m_bmNext.LoadBitmap(IDB_NEXT)
		|| !m_bmAbout.LoadBitmap(IDB_ABOUT)
		|| !m_bmMag.LoadBitmap(IDB_MAG)
		|| !m_bmMagBtn.LoadBitmap(IDB_MAGBTN)
		|| !m_bmMove.LoadBitmap(IDB_MOVE)
		|| !m_bmMoveBtn.LoadBitmap(IDB_MOVEBTN)
		|| !m_bmZoomVal.LoadBitmap(IDB_ZOOMVAL)
		|| !m_bmPageDisp.LoadBitmap(IDB_PAGE_DISP)
		|| !m_bmMask10.LoadBitmap(IDB_MASK10)
		|| !m_bmPrt.LoadBitmap(IDB_PRT)
		)
		return -1;

	if (false
		|| (m_hcZoomIn = AfxGetApp()->LoadCursor(IDC_ZOOM_IN)) == NULL
		|| (m_hcZoomOut = AfxGetApp()->LoadCursor(IDC_ZOOM_OUT)) == NULL
		)
		return -1;

	ZeroMemory(&m_hsc, sizeof(m_hsc));
	ZeroMemory(&m_vsc, sizeof(m_vsc));

	m_hsc.cbSize = sizeof(m_hsc);
	m_hsc.fMask = SIF_DISABLENOSCROLL | SIF_POS | SIF_RANGE | SIF_PAGE;

	m_vsc.cbSize = sizeof(m_hsc);
	m_vsc.fMask = SIF_DISABLENOSCROLL | SIF_POS | SIF_RANGE | SIF_PAGE;

	m_toolZoom = true;
	m_fDrag = false;
	m_fFitOnSmall = true;
	m_iPage = 0;
	m_pThumbs.RemoveAll();

	m_threadRenderer = NULL;

	LayoutClient();
	return 0;
}

bool CAxVw::MovePage(int iDelta, bool force) {
	int i = m_iPage + iDelta;
	i = std::min(i, CntPages() - 1);
	i = std::max(0, i);

	return SetPage(i, force);
}

bool CAxVw::SetPage(int i, bool force) {
	if (i == m_iPage && !force)
		return false;

	m_iPage = i;
	m_hsc.nPos = 0;
	m_vsc.nPos = 0;
	if (m_hWnd != NULL) {
		Invalidate();
		UpdScroll();
	}
	return true;
}

void CAxVw::Setft(FitMode ft) {
	switch (ft) {
	case ftW:
	case ftWH:
	case ftZoom:
		m_ft = ft;
		UpdScroll();
		InvalidateRect(m_rcZoomVal);
		InvalidateRect(m_rcMMSel);
		InvalidateRect(m_rcPaint);
		break;
	}
}

void CAxVw::Zoom(float fDelta) {
	m_ft = ftZoom;
	m_fZoom *= fDelta;
	Invalidate();
	UpdScroll();
}

void CAxVw::UpdScroll() {
	CSize size = GetZoomedSize();

	m_hsc.nMax = size.cx;
	m_vsc.nMax = size.cy;

	{
		SCROLLINFO& si = m_hsc;
		int& newPos = si.nPos;

		newPos = std::min(newPos, si.nMax - (int)si.nPage);
		newPos = std::max(newPos, si.nMin);
	}

	{
		SCROLLINFO& si = m_vsc;
		int& newPos = si.nPos;

		newPos = std::min(newPos, si.nMax - (int)si.nPage);
		newPos = std::max(newPos, si.nMin);
	}

	m_hsc.nPage = m_rcPaint.Width();
	m_sbH.SetScrollInfo(&m_hsc);

	m_vsc.nPage = m_rcPaint.Height();
	m_sbV.SetScrollInfo(&m_vsc);
}

void CAxVw::LayoutClient() {
	CRect rc;
	GetClientRect(&rc);
	int curx = 0;
	const int cyBar = 24;
	const int cxBMPrev = 32;
	const int cxBMNext = 32;
	const int cxBMDisp = 55;

	m_rcCmdBar.left = curx;
	m_rcCmdBar.top = rc.bottom - cyBar;
	m_rcCmdBar.bottom = rc.bottom;

	{
		m_rcMMSel.left = curx;
		m_rcMMSel.bottom = rc.bottom;
		m_rcMMSel.right = (curx += 24);
		m_rcMMSel.top = rc.bottom - cyBar;

		m_rcZoomVal.left = curx;
		m_rcZoomVal.bottom = rc.bottom;
		m_rcZoomVal.right = (curx += 48);
		m_rcZoomVal.top = rc.bottom - cyBar;

		m_rcPrev.left = curx;
		m_rcPrev.bottom = rc.bottom;
		m_rcPrev.right = curx = (curx += cxBMPrev);
		m_rcPrev.top = rc.bottom - cyBar;

		m_rcDisp.left = curx;
		m_rcDisp.bottom = rc.bottom;
		m_rcDisp.right = curx = (curx += cxBMDisp);
		m_rcDisp.top = rc.bottom - cyBar;

		m_rcNext.left = curx;
		m_rcNext.bottom = rc.bottom;
		m_rcNext.right = curx = (curx += cxBMNext);
		m_rcNext.top = rc.bottom - cyBar;

		if (m_canPrintThisPDF) {
			m_rcPrt.left = curx;
			m_rcPrt.bottom = rc.bottom;
			m_rcPrt.right = (curx += 24);
			m_rcPrt.top = rc.bottom - cyBar;
		}
		else {
			m_rcPrt.SetRectEmpty();
		}

		m_rcAbout.left = curx;
		m_rcAbout.bottom = rc.bottom;
		m_rcAbout.right = (curx += 24);
		m_rcAbout.top = rc.bottom - cyBar;
	}

	m_rcCmdBar.right = curx;

	int cex = (rc.Width() - curx) / 2;
	if (cex > 0) {
		m_rcMMSel.OffsetRect(cex, 0);
		m_rcZoomVal.OffsetRect(cex, 0);
		m_rcPrev.OffsetRect(cex, 0);
		m_rcDisp.OffsetRect(cex, 0);
		m_rcNext.OffsetRect(cex, 0);
		m_rcPrt.OffsetRect(cex, 0);
		m_rcAbout.OffsetRect(cex, 0);

		m_rcCmdBar.OffsetRect(cex, 0);
	}

	rc.bottom -= cyBar;

	m_rcPaint = rc;

	UpdScroll();
}

void CAxVw::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	LayoutClient();
}

void CAxVw::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	SCROLLINFO& si = m_hsc;
	const int nBar = SB_HORZ;

	if (pScrollBar == &m_sbH) {
		int newPos = si.nPos;
		switch (nSBCode) {
		case SB_LEFT:
			newPos = si.nMin;
			break;
		case SB_RIGHT:
			newPos = si.nMax;
			break;
		case SB_LINELEFT:
			newPos--;
			break;
		case SB_LINERIGHT:
			newPos++;
			break;
		case SB_PAGELEFT:
			newPos -= si.nPage;
			break;
		case SB_PAGERIGHT:
			newPos += si.nPage;
			break;
		case SB_THUMBPOSITION:
			newPos = nPos;
			break;
		case SB_THUMBTRACK:
			newPos = nPos;
			break;
		}

		newPos = std::max(newPos, si.nMin);
		newPos = std::min(newPos, si.nMax - (int)si.nPage);

		if (newPos != si.nPos) {
			si.nPos = newPos;
			pScrollBar->SetScrollInfo(&si);
			Invalidate();
		}
	}

	CWnd::OnHScroll(nSBCode, nPos, pScrollBar);
}

void CAxVw::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) {
	SCROLLINFO& si = m_vsc;
	const int nBar = SB_VERT;

	if (pScrollBar == &m_sbV) {
		int newPos = si.nPos;
		switch (nSBCode) {
		case SB_LEFT:
			newPos = si.nMin;
			break;
		case SB_RIGHT:
			newPos = si.nMax;
			break;
		case SB_LINELEFT:
			newPos--;
			break;
		case SB_LINERIGHT:
			newPos++;
			break;
		case SB_PAGELEFT:
			newPos -= si.nPage;
			break;
		case SB_PAGERIGHT:
			newPos += si.nPage;
			break;
		case SB_THUMBPOSITION:
			newPos = nPos;
			break;
		case SB_THUMBTRACK:
			newPos = nPos;
			break;
		}

		newPos = std::max(newPos, si.nMin);
		newPos = std::min(newPos, si.nMax - (int)si.nPage);

		if (newPos != si.nPos) {
			si.nPos = newPos;
			pScrollBar->SetScrollInfo(&si);
			Invalidate();
		}
	}

	CWnd::OnVScroll(nSBCode, nPos, pScrollBar);
}

BOOL CAxVw::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) {
	if (zDelta != 0) {
		SCROLLINFO& si = m_vsc;
		const int nBar = SB_VERT;

		int newPos = si.nPos - zDelta;

		newPos = std::max(newPos, si.nMin);
		newPos = std::min(newPos, si.nMax - (int)si.nPage);

		if (newPos != si.nPos) {
			si.nPos = newPos;
			m_sbV.SetScrollInfo(&si);
			Invalidate();
		}
		else {
			bool nextPage = zDelta < 0;

			bool pageChanged = false;

			if (nextPage) {
				// go to next page
				if (m_iPage + 1 < CntPages()) {
					m_iPage++;
					pageChanged = true;
				}
			}
			else {
				// go to previous page
				if (m_iPage > 0) {
					m_iPage--;
					pageChanged = true;
				}
			}

			if (pageChanged) {
				LayoutClient();
				InvalidateRect(m_rcPaint, false);
				InvalidateRect(m_rcDisp, false);

				Setft(ftWH);
			}
		}
	}

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}

void CAxVw::OnLButtonDown(UINT nFlags, CPoint point) {
	if (IsActive()) {
		if (false) {}
		if (m_toolZoom && m_rcPaint.PtInRect(point)) {
			bool fShift = (0x8000 & GetKeyState(VK_SHIFT)) != 0;
			Zoomat(!fShift, point);
		}
		else if (!m_toolZoom && m_rcPaint.PtInRect(point)) {
			m_ptBegin = point;
			m_ptScrollFrm = CPoint(m_hsc.nPos, m_vsc.nPos);
			m_fDrag = true;

			SetCapture();
		}
		else if (m_rcPrev.PtInRect(point)) {
			if (m_iPage > 0) {
				m_iPage--;
				LayoutClient();
				InvalidateRect(m_rcPaint, false);
				InvalidateRect(m_rcDisp, false);
			}
		}
		else if (m_rcNext.PtInRect(point)) {
			if (m_iPage + 1 < CntPages()) {
				m_iPage++;
				LayoutClient();
				InvalidateRect(m_rcPaint, false);
				InvalidateRect(m_rcDisp, false);
			}
		}
		else if (m_rcAbout.PtInRect(point)) {
			CWnd* p = GetOwner();
			if (p)
				p->OnCmdMsg(ID_APP_ABOUT, 0, NULL, NULL);
		}
		else if (m_rcPrt.PtInRect(point)) {
			CSingleLock lck(&s_lockpdf);
			TCHAR tcTmpfp[MAX_PATH] = { 0 };
			if (!IsPDFReady()) {
				return;
			}
			if (!m_canPrintThisPDF) {
				AfxMessageBox(IDS_PRINTING_IS_NOT_ALLOWED);
				return;
			}
			OnFilePrint();
		}
		else if (m_rcMMSel.PtInRect(point)) {
			CMenu aSel;
			VERIFY(aSel.CreatePopupMenu());
			VERIFY(aSel.AppendMenu(MF_BITMAP, IDC_MAG, &m_bmMag));
			VERIFY(aSel.AppendMenu(MF_BITMAP, IDC_MOVE, &m_bmMove));
			CPoint ptMenu = m_rcMMSel.TopLeft();
			ClientToScreen(&ptMenu);
			aSel.TrackPopupMenu(TPM_LEFTALIGN, ptMenu.x, ptMenu.y, GetParentFrame());
		}
		else if (m_rcZoomVal.PtInRect(point)) {
			CMenu aSel;
			VERIFY(aSel.LoadMenu(IDR_MENU_P));
			CMenu* pMenu = aSel.GetSubMenu(0);
			CPoint ptMenu = m_rcZoomVal.TopLeft();
			ClientToScreen(&ptMenu);
			if (pMenu != NULL)
				pMenu->TrackPopupMenu(TPM_LEFTALIGN, ptMenu.x, ptMenu.y, GetParentFrame());
		}
		else if (m_rcDisp.PtInRect(point)) {
			CPvMenu aSel(*this);
			VERIFY(aSel.CreatePopupMenu());
			int cy = CntPages();
			for (int y = 0; y < cy; y++) {
				CString str; str.Format(_T("%d"), 1 + y);
#if 1
				aSel.AppendMenu(MF_STRING | ((y != 0 && (0 == (y % 50))) ? MF_MENUBREAK : 0), IDC_PAGE1 + y, str);
#else
				aSel.AppendMenu(MF_OWNERDRAW, IDC_PAGE1 + y, str);
#endif
			}
			CPoint ptMenu(m_rcDisp.left, m_rcDisp.bottom);
			ClientToScreen(&ptMenu);
			aSel.TrackPopupMenu(TPM_LEFTALIGN, ptMenu.x, ptMenu.y, GetParentFrame());
		}
	}

	CWnd::OnLButtonDown(nFlags, point);
}

BOOL CAxVw::OnSelCmd(UINT nID) {
	switch (nID) {
	case IDC_MAG: m_toolZoom = true; break;
	case IDC_MOVE: m_toolZoom = false; break;

	case IDC_P6: SetzoomR(0.06f); return true;
	case IDC_P12: SetzoomR(0.12f); return true;
	case IDC_P25: SetzoomR(0.25f); return true;
	case IDC_P50: SetzoomR(0.5f); return true;
	case IDC_P100: SetzoomR(1); return true;
	case IDC_P200: SetzoomR(2); return true;
	case IDC_P400: SetzoomR(4); return true;
	case IDC_P800: SetzoomR(8); return true;
	case IDC_P1600: SetzoomR(16); return true;

	case IDC_FITW: Setft(ftW); return true;
	case IDC_FITWH: Setft(ftWH); return true;

	default: return false;
	}
	InvalidateRect(m_rcMMSel, false);
	return true;
}

void CAxVw::OnUpdateSelCmd(CCmdUI* pUI) {
	switch (pUI->m_nID) {
	case IDC_MAG: pUI->SetCheck(m_toolZoom); break;
	case IDC_MOVE: pUI->SetCheck(!m_toolZoom); break;
	case IDC_FITW: pUI->SetRadio(m_ft == ftW); break;
	case IDC_FITWH: pUI->SetRadio(m_ft == ftWH); break;
	}
}

void CAxVw::SetzoomR(float zf) {
	CPoint posat = GetAbsPosAt(m_rcPaint.CenterPoint() + GetScrollOff());
	Setzf(zf);
	LayoutClient();
	SetCenterAt(posat, m_rcPaint.CenterPoint());
	InvalidateRect(m_rcPaint, false);
}

void CAxVw::Zoomat(bool fIn, CPoint mouseat) {
	float zf = Getzf();
	ZoomatR(fIn ? zf * 2 : zf / 2, mouseat);
}

void CAxVw::ZoomatR(float zf, CPoint mouseat) {
	CPoint clientpt = mouseat;
	CPoint posat = GetAbsPosAt(GetScrollOff() + clientpt);
	Setzf(std::max(0.0625f, std::min(16.0f, zf)));
	LayoutClient();
	SetCenterAt(posat, clientpt);
	InvalidateRect(m_rcPaint, false);
}

CPoint CAxVw::GetCenterPos() const {
	CPoint pt = m_rcPaint.CenterPoint();
	float zf = Getzf();
	return CPoint(
		(int)(pt.x / zf),
		(int)(pt.y / zf)
	);
}

CPoint CAxVw::GetAbsPosAt(CPoint pt) const {
	float zf = Getzf();
	return CPoint(
		(int)(pt.x / zf),
		(int)(pt.y / zf)
	);
}

void CAxVw::SetCenterAt(CPoint pt, CPoint clientpt) {
	float zf = Getzf();
	{
		int xp = Newxp((int)(pt.x * zf - m_rcPaint.Width() / 2 + (m_rcPaint.Width() / 2 - clientpt.x)));
		if (xp != m_hsc.nPos)
			m_sbH.SetScrollPos(m_hsc.nPos = xp);
	}
	{
		int yp = Newyp((int)(pt.y * zf - m_rcPaint.Height() / 2 + (m_rcPaint.Height() / 2 - clientpt.y)));
		if (yp != m_vsc.nPos)
			m_sbV.SetScrollPos(m_vsc.nPos = yp);
	}
}

CSize CAxVw::GetZoomedSize() {
	CRect rc = GetPageRect(m_iPage);
	if (!rc.IsRectEmpty()) {
		CSize size = rc.Size();
		switch (m_ft) {
		case ftZoom:
			return CSize((int)(size.cx * m_fZoom), (int)(size.cy * m_fZoom));
		case ftW:
			if (m_fFitOnSmall || m_rcPaint.Width() < size.cx) {
				return CSize(
					m_rcPaint.Width(),
					(size.cx != 0)
					? (int)(size.cy * (m_rcPaint.Width() / (float)size.cx))
					: 0
				);
			}
			return size;
		case ftWH:
			if (m_fFitOnSmall || m_rcPaint.Width() < size.cx || m_rcPaint.Height() < size.cy) {
				return Fitrect::Fit(m_rcPaint, size).Size();
			}
			return size;
		}
	}
	return CSize(0, 0);
}

float CAxVw::Getzf() const {
	CRect rc = GetPageRect(m_iPage);
	if (!rc.IsRectEmpty()) {
		CSize size = rc.Size();
		switch (m_ft) {
		default:
		case ftZoom:
			return m_fZoom;
		case ftW:
			if (m_fFitOnSmall || m_rcPaint.Width() < size.cx) {
				return (size.cx != 0)
					? (m_rcPaint.Width() / (float)size.cx)
					: 1
					;
			}
			return 1;
		case ftWH:
			if (m_fFitOnSmall || m_rcPaint.Width() < size.cx || m_rcPaint.Height() < size.cy) {
				CSize newSize = Fitrect::Fit(m_rcPaint, size).Size();
				float fx = (size.cx != 0) ? ((float)newSize.cx / size.cx) : 0;
				float fy = (size.cy != 0) ? ((float)newSize.cy / size.cy) : 0;
				if (fx != 0) return fx;
				if (fy != 0) return fy;
				return 1;
			}
			return 1;
		}
	}
	return 1;
}

void CAxVw::OnLButtonUp(UINT nFlags, CPoint point) {
	if (m_fDrag) {
		m_fDrag = false;
		if (GetCapture() == this)
			ReleaseCapture();
	}

	CWnd::OnLButtonUp(nFlags, point);
}

void CAxVw::OnMouseMove(UINT nFlags, CPoint point) {
	if (m_fDrag && 0 != (nFlags & MK_LBUTTON) && GetCapture() == this) {
		CPoint pt = m_ptScrollFrm + m_ptBegin - point;

		bool moved = false;

		int xp = Newxp(pt.x);
		if (xp != m_hsc.nPos)
			m_sbH.SetScrollPos(m_hsc.nPos = xp), moved = true;

		int yp = Newyp(pt.y);
		if (yp != m_vsc.nPos)
			m_sbV.SetScrollPos(m_vsc.nPos = yp), moved = true;

		if (moved)
			InvalidateRect(m_rcPaint, false);
	}

	CWnd::OnMouseMove(nFlags, point);
}

// http://msdn.microsoft.com/en-us/library/ms645614(VS.85).aspx
LRESULT CAxVw::OnMouseHWheel(WPARAM wp, LPARAM) {
	short zDelta = HIWORD(wp);
	{
		int oldpos = m_sbH.GetScrollPos();
		int newpos = oldpos;

		newpos = Newxp(newpos + zDelta);

		if (oldpos != newpos) {
			m_hsc.nPos = newpos;
			m_sbH.SetScrollPos(newpos);
			this->InvalidateRect(m_rcPaint, false);
			return 1;
		}
	}
	return 0;
}

void CAxVw::OnRButtonDown(UINT nFlags, CPoint point) {
	if (IsActive()) {
		if (m_toolZoom && m_rcPaint.PtInRect(point)) {
			Zoomat(false, point);
			return;
		}
	}

	CWnd::OnRButtonDown(nFlags, point);
}

void CAxVw::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	OnLButtonDown(nFlags, point);
	OnLButtonUp(nFlags, point);

	//	CWnd::OnLButtonDblClk(nFlags, point);
}

void CAxVw::OnRButtonDblClk(UINT nFlags, CPoint point)
{
	OnRButtonDown(nFlags, point);
	OnRButtonUp(nFlags, point);

	//	CWnd::OnRButtonDblClk(nFlags, point);
}

BOOL CAxVw::OnPageSel(UINT nID) {
	SetPage(nID - IDC_PAGE1);
	return true;
}

void CAxVw::OnUpdatePageSel(CCmdUI* pUI) {
	pUI->SetRadio(m_iPage == (pUI->m_nID - IDC_PAGE1));
}

CBitmap* CAxVw::GetThumb(int iPage, int cx) {
	if (m_pThumbs.GetCount() <= (size_t)iPage || m_pThumbs[iPage] == NULL) {
		CSingleLock lck(&s_lockpdf);

		CRect rcDraw(0, 0, cx, cx);
		CRect rcPage = GetPageRect(iPage);
		float scale = (float)cx / std::max(rcPage.Height(), rcPage.Width());

		double dpi = 72 * scale;

		const int cy = cx;

		CRect rcDst(0, 0,
			int(rcPage.Width() * scale),
			int(rcPage.Height() * scale)
		);

		int slx = (cx - rcDst.Width()) / 2;
		int sly = (cy - rcDst.Height()) / 2;

		std::unique_ptr<poppler::page> pageOut(m_pdfdoc->create_page(iPage));

		poppler::page_renderer renderer;
		renderer.set_image_format(poppler::image::format_bgr24);

		poppler::image imageOut(
			renderer.render_page(pageOut.get(), dpi, dpi, slx, sly, rcDst.Width(), rcDst.Height())
		);

		lck.Unlock();

		int pcx = imageOut.width();
		int pcy = imageOut.height();
		BITMAPINFO bi;
		ZeroMemory(&bi, sizeof(bi));
		bi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		bi.bmiHeader.biWidth = pcx;
		bi.bmiHeader.biHeight = pcy;
		bi.bmiHeader.biBitCount = 24;
		bi.bmiHeader.biPlanes = 1;

		int dstPitch = (3 * pcx + 3) & (~3);

		void* pvBits = NULL;
		HANDLE h = CreateDIBSection(NULL, &bi, DIB_RGB_COLORS, &pvBits, NULL, 0);
		if (h != NULL) {
			for (int y = 0; y < pcy; y++) {
				const BYTE* pSrc = reinterpret_cast<const BYTE*>(imageOut.const_data() + imageOut.bytes_per_row() * y);
				BYTE* pDst = reinterpret_cast<PBYTE>(pvBits) + dstPitch * (pcy - y - 1);
				for (int x = 0; x < pcx; x++, pSrc += 3, pDst += 3) {
					pDst[0] = pSrc[0];
					pDst[1] = pSrc[1];
					pDst[2] = pSrc[2];
				}
			}

			CBitmap* pBM = new CBitmap();
			pBM->Attach(h);
			CAutoPtr<CBitmap> autoBM(pBM);
			m_pThumbs.SetAtGrow(iPage, autoBM);
		}
	}

	if (m_pThumbs.GetCount() <= (size_t)iPage)
		return NULL;

	return static_cast<CBitmap*>(m_pThumbs[iPage]);
}

int CAxVw::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message) {
	int r = CWnd::OnMouseActivate(pDesktopWnd, nHitTest, message);
	if (r == MA_ACTIVATE) {
		if (GetFocus() != this) {
			SetFocus();
			r = MA_ACTIVATEANDEAT;
		}
	}
	return r;
}

BOOL CAxVw::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) {
	if (nHitTest == HTCLIENT) {
		DWORD xy = GetMessagePos();
		CPoint pt(GET_X_LPARAM(xy), GET_Y_LPARAM(xy));
		ScreenToClient(&pt);
		if (m_rcPaint.PtInRect(pt)) {
			bool fShift = (0x8000 & GetKeyState(VK_SHIFT)) != 0;
			if (m_toolZoom) {
				SetCursor(fShift ? m_hcZoomOut : m_hcZoomIn);
				return true;
			}
		}
	}
	return CWnd::OnSetCursor(pWnd, nHitTest, message);
}

LRESULT CAxVw::OnSetRenderInf(WPARAM, LPARAM lParam) {
	CRenderInf* inf = reinterpret_cast<CRenderInf*>(lParam);
	if (inf->partial) {
		m_renderPart.reset(inf);
	}
	else {
		m_renderAll.reset(inf);
	}
	InvalidateRect(m_rcPaint);
	return 0;
}

void CAxVw::FillHatch(CDC& dc, CRect rc) {
	CBitmap bm;
	bm.LoadBitmap(IDB_MASKS50);
	ASSERT(bm.m_hObject != NULL);
	CBrush br(&bm);
	ASSERT(br.m_hObject != NULL);
	CBrush* pbr = dc.SelectObject(&br);
	dc.BitBlt(rc.left, rc.top, rc.Width(), rc.Height(), &dc, rc.left, rc.top, 0x00FC008A); // PSo
	dc.SelectObject(pbr);
}

void CAxVw::OnKillFocus(CWnd* pNewWnd) {
	CWnd::OnKillFocus(pNewWnd);
	InvalidateRect(m_rcCmdBar);
}

void CAxVw::OnSetFocus(CWnd* pOldWnd) {
	CWnd::OnSetFocus(pOldWnd);
	InvalidateRect(m_rcCmdBar);
}

class CPrintOptsPage : public CPropertyPage, public PrintOpts {
	DECLARE_DYNAMIC(CPrintOptsPage);
	DECLARE_MESSAGE_MAP();

public:
	enum { IDD = IDD_PRINT_OPTS };

	CPrintOptsPage()
		: CPropertyPage(IDD, IDS_PDF4AX_PRINT_PROP_CAPTION)
	{
	}

	virtual BOOL OnInitDialog() {
		__super::OnInitDialog();

		UpdateData(false);
		return true;
	}

	void DoDataExchange(CDataExchange* pDX) {
		__super::DoDataExchange(pDX);
		DDX_Check(pDX, IDC_CENTERING, m_bCentering);
		DDX_Check(pDX, IDC_IGNORE_MARGIN, m_bIgnoreMargin);
		DDX_Check(pDX, IDC_AUTO_PAPERSIZE, m_bAutoPaperSize);
	}
};

BEGIN_MESSAGE_MAP(CPrintOptsPage, CPropertyPage)
END_MESSAGE_MAP()

IMPLEMENT_DYNAMIC(CPrintOptsPage, CPropertyPage);

void CAxVw::OnFilePrint() {
	CPrintDialogEx dlg(
		PD_ALLPAGES | PD_PAGENUMS | PD_USEDEVMODECOPIES | PD_HIDEPRINTTOFILE | PD_NOSELECTION,
		this
	);

	CPrintOptsPage page;
	page.LoadFromReg();
	HPROPSHEETPAGE hPage = CreatePropertySheetPage(&page.GetPSP());

	PRINTPAGERANGE ranges[13] = { 0 };
	ranges[0].nFromPage = dlg.m_pdex.nMinPage = 1;
	ranges[0].nToPage = dlg.m_pdex.nMaxPage = CntPages();
	dlg.m_pdex.lpPageRanges = ranges;
	dlg.m_pdex.nPageRanges = 1;
	dlg.m_pdex.nMaxPageRanges = 13;
	dlg.m_pdex.nPropertyPages = 1;
	dlg.m_pdex.lphPropertyPages = &hPage;
	int result = dlg.DoModal();
	if (result != S_OK) {
		return;
	}
	if (dlg.m_pdex.dwResultAction == PD_RESULT_CANCEL) {
		return;
	}

	page.SaveToReg();

	if (dlg.m_pdex.dwResultAction != PD_RESULT_PRINT) {
		return;
	}

	m_printState.reset(new CAxVw::PrintState());

	DEVMODE* devmode = dlg.GetDevMode();

	m_printState.get()->devmode.SetSize(devmode->dmSize + devmode->dmDriverExtra);
	memcpy(m_printState.get()->devmode.GetData(), devmode, m_printState.get()->devmode.GetSize());

	auto& targetPages = m_printState.get()->targetPages;

	if (dlg.m_pdex.Flags & PD_CURRENTPAGE) {
		targetPages.AddPages(1 + m_iPage, 1 + m_iPage);
	}
	else if (dlg.m_pdex.Flags & PD_PAGENUMS) {
		targetPages.AddRanges(ranges, dlg.m_pdex.nPageRanges);
	}
	else {
		targetPages.AddPages(dlg.m_pdex.nMinPage, dlg.m_pdex.nMaxPage);
	}

	m_printState.get()->opts = page;

	CDC& printer = m_printState.get()->printer;
	printer.m_bPrinting = true;
	printer.Attach(dlg.CreatePrinterDC());

	SetTimer(1000, 100, NULL);

	m_dlgPrint.DoModal();
}

void CAxVw::OnTimer(UINT_PTR nIDEvent) {
	if (nIDEvent == 1000) {
		KillTimer(nIDEvent);
		if (m_dlgPrint.GetSafeHwnd() != NULL) {
			if ((bool)m_printState) {
				m_dlgPrint.m_strPrintPos.Format(_T("%d / %d")
					, (std::max)(1, m_printState.get()->getCurPage())
					, 0 + m_printState.get()->getMaxPage()
				);
			}
			m_dlgPrint.UpdateData(false);

			if (PrintNextPage()) {
				SetTimer(1000, 100, NULL);
			}
			else {
				m_dlgPrint.EndDialog(0);
				m_printState.reset(nullptr);
			}
		}
		else {
			m_printState.reset(nullptr);
		}
	}
}

bool CAxVw::PrintNextPage() {
	if (!(bool)m_printState) {
		return false;
	}

	DOCINFO& di = m_printState.get()->di;
	DEVMODE* devmode = reinterpret_cast<DEVMODE*>(m_printState.get()->devmode.GetData());
	PaperSizeLite& defaultPaperSize = m_printState.get()->defaultPaperSize;
	CDC& printer = m_printState.get()->printer;

	if (m_printState.get()->isFirstPage()) {
		std::wstring title(reinterpret_cast<LPCWSTR>(m_pdfdoc->get_title().c_str()));
		if (title.empty()) {
			title = m_title;
		}
		if (title.empty()) {
			title = L"PDF document";
		}

		ZeroMemory(&di, sizeof(di));
		di.cbSize = sizeof(di);
		di.lpszDocName = title.c_str();

		defaultPaperSize.CopyFrom(*devmode);

		devmode->dmFields |= DM_PAPERSIZE | DM_ORIENTATION;

		m_printState.get()->printer.StartDoc(&di);
		m_printState.get()->startDocActive = true;
	}

	if (m_printState.get()->isPrintingDone()) {
		printer.EndDoc();
		m_printState.get()->startDocActive = false;
		return false;
	}

	int iPage = m_printState.get()->getTargetPage();

	struct cairo_deleter {
		void operator()(cairo_t* ptr) const {
			cairo_destroy(ptr);
		}
	};

	struct cairo_surface_deleter {
		void operator()(cairo_surface_t* ptr) const {
			cairo_surface_destroy(ptr);
		}
	};

	std::unique_ptr<cairo_surface_t, cairo_surface_deleter> surface(cairo_win32_printing_surface_create(printer.m_hDC));

	std::unique_ptr<poppler::page> pageOut(m_pdfdoc->create_page(iPage - 1));

	{
		auto mediaBox = m_pps[iPage - 1].mediaBox;

		float mmWidth = mediaBox.Width() * 0.352777f;
		float mmHeight = mediaBox.Height() * 0.352777f;

		PaperSizeLite guessed;
		if (m_printState.get()->opts.m_bAutoPaperSize && PaperSizeUtil::Guess(mmWidth, mmHeight, guessed)) {
			guessed.CopyTo(*devmode);
		}
		else {
			defaultPaperSize.CopyTo(*devmode);
			devmode->dmOrientation = (mmWidth <= mmHeight) ? DMORIENT_PORTRAIT : DMORIENT_LANDSCAPE;
		}

		printer.ResetDC(devmode);
		int x_dpi = printer.GetDeviceCaps(LOGPIXELSX);
		int y_dpi = printer.GetDeviceCaps(LOGPIXELSY);
		int ox = printer.GetDeviceCaps(PHYSICALOFFSETX);
		int oy = printer.GetDeviceCaps(PHYSICALOFFSETY);
		int cx = printer.GetDeviceCaps(PHYSICALWIDTH);
		int cy = printer.GetDeviceCaps(PHYSICALHEIGHT);

		int pixelWidth = int(mediaBox.Width() / 72.0f * x_dpi);
		int pixelHeight = int(mediaBox.Height() / 72.0f * y_dpi);

		CRect rcMax(-ox, -oy, cx, cy);
		CRect rcPrint = m_printState.get()->opts.m_bIgnoreMargin
			? rcMax
			: CRect(0, 0, cx - ox - ox, cy - oy - oy);
		CRect rcDraw = m_printState.get()->opts.m_bCentering
			? FitRect3::Fit(
				rcPrint,
				CSize(
					pixelWidth,
					pixelHeight
				),
				0,
				0
			)
			: FitRect3::Fit(
				rcPrint,
				CSize(
					pixelWidth,
					pixelHeight
				),
				-1,
				-1
			);

		float xScale = (rcDraw.Width() * 1.0f / (std::max)(1, pixelWidth));
		float yScale = (rcDraw.Height() * 1.0f / (std::max)(1, pixelHeight));
		float xyScale = (std::min)(xScale, yScale);

		XFORM xform;
		xform.eM11 = x_dpi / 72.0 * xyScale;
		xform.eM12 = 0;
		xform.eM21 = 0;
		xform.eM22 = y_dpi / 72.0 * xyScale;
		xform.eDx = rcDraw.left;
		xform.eDy = rcDraw.top;
		printer.SetGraphicsMode(GM_ADVANCED);
		printer.SetWorldTransform(&xform);

		printer.StartPage();

		std::unique_ptr<cairo_t, cairo_deleter> renderer(cairo_create(surface.get()));

		cairo_surface_set_fallback_resolution(surface.get(), 72, 72);

		poppler::page_printer print;
		print.print_page(pageOut.get(), printer.GetSafeHdc(), renderer.get());

		cairo_surface_finish(surface.get());

		printer.EndPage();
	}

	m_printState.get()->moveToNextPage();
	return true;
}
