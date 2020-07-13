#pragma once

class CBackGround;
class CStarToolDoc;

class CStarToolView : public CScrollView
{
protected:
	CStarToolView();
	DECLARE_DYNCREATE(CStarToolView)
private:
	int			 m_iTileOption;
public:
	void SetTileOption(int _Option)
	{
		m_iTileOption = _Option;
	}
private:
	CBackGround* m_pBackGround;
public:
	CBackGround* GetBackGround(void)
	{
		return m_pBackGround;
	}
	void ViewInvalidate(bool _true)
	{
		Invalidate(_true);
	}
public:
	CStarToolDoc* GetDocument() const;

public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

public:
	virtual ~CStarToolView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

protected:
	afx_msg void OnFilePrintPreview();
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	DECLARE_MESSAGE_MAP()
public:
	virtual void OnInitialUpdate();
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
};

#ifndef _DEBUG
inline CStarToolDoc* CStarToolView::GetDocument() const
   { return reinterpret_cast<CStarToolDoc*>(m_pDocument); }
#endif

