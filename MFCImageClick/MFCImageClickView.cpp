
// MFCImageClickView.cpp: CMFCImageClickView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
// SHARED_HANDLERS는 미리 보기, 축소판 그림 및 검색 필터 처리기를 구현하는 ATL 프로젝트에서 정의할 수 있으며
// 해당 프로젝트와 문서 코드를 공유하도록 해 줍니다.
#ifndef SHARED_HANDLERS
#include "MFCImageClick.h"
#include <gdiplus.h>
using namespace Gdiplus;
#pragma comment(lib, "gdiplus.lib")

#endif

#include "MFCImageClickDoc.h"
#include "MFCImageClickView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CMFCImageClickView

IMPLEMENT_DYNCREATE(CMFCImageClickView, CView)

BEGIN_MESSAGE_MAP(CMFCImageClickView, CView)
	// 표준 인쇄 명령입니다.
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CMFCImageClickView::OnFilePrintPreview)
	ON_WM_CONTEXTMENU()
	ON_WM_RBUTTONUP()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_WM_KEYDOWN()



END_MESSAGE_MAP()

// CMFCImageClickView 생성/소멸
void CMFCImageClickView::OnLButtonDown(UINT nFlags, CPoint point)
{
	int imgX = (int)((point.x - m_offsetX) / m_zoom);
	int imgY = (int)((point.y - m_offsetY) / m_zoom);

	CString str;
	str.Format(_T("Image 좌표: (%d, %d)"), imgX, imgY);

	AfxMessageBox(str);

	CStdioFile file;
	CFileException ex;
	CString filePath = _T("좌표.txt");

	if (file.Open(filePath, CFile::modeWrite | CFile::modeNoTruncate | CFile::modeCreate | CFile::typeText, &ex)) {
		file.SeekToEnd();
		file.WriteString(str + _T("\r\n"));  // ← 줄바꿈 명확하게

		file.Close();
	}
	else {
		TCHAR error[512];
		ex.GetErrorMessage(error, 512);
		AfxMessageBox(error);
	}

	CView::OnLButtonDown(nFlags, point);
}





BOOL CMFCImageClickView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// Ctrl + 휠 → 확대/축소
	if (GetKeyState(VK_CONTROL) & 0x8000) {
		if (zDelta > 0)
			m_zoom *= 1.25f;
		else
			m_zoom /= 1.25f;  // 축소 25%


		if (m_zoom < 0.1f) m_zoom = 0.1f;
		if (m_zoom > 30.0f) m_zoom = 30.0f;
	}
	// Shift + 휠 → 좌우 이동
	else if (GetKeyState(VK_SHIFT) & 0x8000) {
		if (zDelta > 0)
			m_offsetX += 60;  // 오른쪽 이동
		else
			m_offsetX -= 60;  // 왼쪽 이동
	}
	// 일반 휠 → 상하 이동
	else {
		if (zDelta > 0)
			m_offsetY += 60;  // 위로 이동
		else
			m_offsetY -= 60;  // 아래로 이동
	}

	Invalidate();
	return TRUE;
}
void CMFCImageClickView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	const int moveAmount = 30;

	switch (nChar)
	{
	case VK_LEFT:
	case 'A':
		m_offsetX += moveAmount;
		break;
	case VK_RIGHT:
	case 'D':
		m_offsetX -= moveAmount;
		break;
	case VK_UP:
	case 'W':
		m_offsetY += moveAmount;
		break;
	case VK_DOWN:
	case 'S':
		m_offsetY -= moveAmount;
		break;
	}

	Invalidate(); // 다시 그리기
	CView::OnKeyDown(nChar, nRepCnt, nFlags);
}





CMFCImageClickView::CMFCImageClickView() noexcept
{
	m_pImage = nullptr;
	m_gdiplusToken = 0;
}



CMFCImageClickView::~CMFCImageClickView()
{
	if (m_pImage) delete m_pImage;
	GdiplusShutdown(m_gdiplusToken);
}

BOOL CMFCImageClickView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: CREATESTRUCT cs를 수정하여 여기에서
	//  Window 클래스 또는 스타일을 수정합니다.
	cs.style |= WS_TABSTOP;  // 포커스 받을 수 있도록


	return CView::PreCreateWindow(cs);

}

// CMFCImageClickView 그리기

void CMFCImageClickView::OnDraw(CDC* pDC)
{
	if (m_pImage) {
		Graphics graphics(pDC->GetSafeHdc());
		graphics.SetInterpolationMode(InterpolationModeNearestNeighbor);

		int width = (int)(m_pImage->GetWidth() * m_zoom);
		int height = (int)(m_pImage->GetHeight() * m_zoom);

		graphics.DrawImage(m_pImage, m_offsetX, m_offsetY, width, height);
	}
}



void CMFCImageClickView::OnInitialUpdate()
{
	CView::OnInitialUpdate();

	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&m_gdiplusToken, &gdiplusStartupInput, nullptr);

	// 파일 열기 대화상자 띄우기
	CFileDialog dlg(TRUE, _T("jpg"), NULL, OFN_FILEMUSTEXIST,
		_T("JPEG Files (*.jpg;*.jpeg)|*.jpg;*.jpeg|All Files (*.*)|*.*||"));

	if (dlg.DoModal() == IDOK)
	{
		CString path = dlg.GetPathName();
		m_pImage = Gdiplus::Image::FromFile(path);

		if (!m_pImage || m_pImage->GetLastStatus() != Ok)
		{
			AfxMessageBox(_T("이미지 로딩 실패!"));
		}
		else
		{
			Invalidate(); // 강제로 다시 그리기 요청
		}
	}
}






// CMFCImageClickView 인쇄


void CMFCImageClickView::OnFilePrintPreview()
{
#ifndef SHARED_HANDLERS
	AFXPrintPreview(this);
#endif
}

BOOL CMFCImageClickView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// 기본적인 준비
	return DoPreparePrinting(pInfo);
}

void CMFCImageClickView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄하기 전에 추가 초기화 작업을 추가합니다.
}

void CMFCImageClickView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: 인쇄 후 정리 작업을 추가합니다.
}

void CMFCImageClickView::OnRButtonUp(UINT /* nFlags */, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CMFCImageClickView::OnContextMenu(CWnd* /* pWnd */, CPoint point)
{
#ifndef SHARED_HANDLERS
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
#endif
}


// CMFCImageClickView 진단

#ifdef _DEBUG
void CMFCImageClickView::AssertValid() const
{
	CView::AssertValid();
}

void CMFCImageClickView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CMFCImageClickDoc* CMFCImageClickView::GetDocument() const // 디버그되지 않은 버전은 인라인으로 지정됩니다.
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CMFCImageClickDoc)));
	return (CMFCImageClickDoc*)m_pDocument;
}
#endif //_DEBUG


// CMFCImageClickView 메시지 처리기
