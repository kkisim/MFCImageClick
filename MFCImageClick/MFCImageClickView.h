
// MFCImageClickView.h: CMFCImageClickView 클래스의 인터페이스
//

#pragma once


class CMFCImageClickView : public CView
{
protected:
	void OnLButtonDown(UINT nFlags, CPoint point);
	BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);

	// serialization에서만 만들어집니다.
	CMFCImageClickView() noexcept;
	DECLARE_DYNCREATE(CMFCImageClickView)

// 특성입니다.
public:
	CMFCImageClickDoc* GetDocument() const;

// 작업입니다.
public:

// 재정의입니다.
public:
	virtual void OnDraw(CDC* pDC);  // 이 뷰를 그리기 위해 재정의되었습니다.
	void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
private:
	Gdiplus::Image* m_pImage;
	ULONG_PTR m_gdiplusToken;
	float m_zoom = 1.0f;
	int m_offsetX = 0;
	int m_offsetY = 0;




// 구현입니다.
public:
	virtual ~CMFCImageClickView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 생성된 메시지 맵 함수
protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // MFCImageClickView.cpp의 디버그 버전
inline CMFCImageClickDoc* CMFCImageClickView::GetDocument() const
   { return reinterpret_cast<CMFCImageClickDoc*>(m_pDocument); }
#endif

