// MiniView.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "StarTool.h"
#include "MiniView.h"
#include "MainFrm.h"
#include "StarToolView.h"
#include "BackGround.h"

IMPLEMENT_DYNCREATE(CMiniView, CView)

CMiniView::CMiniView()
{

}

CMiniView::~CMiniView()
{
}

BEGIN_MESSAGE_MAP(CMiniView, CView)
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()

void CMiniView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();

	CMainFrame* pMainPrame = (CMainFrame*)AfxGetMainWnd();
	
	Device->Render_Begin();

	pMainPrame->GetStarToolView()->GetBackGround()->MiniView();

	Device->Render_End(m_hWnd);
}


#ifdef _DEBUG
void CMiniView::AssertValid() const
{
	CView::AssertValid();
}

#ifndef _WIN32_WCE
void CMiniView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif
#endif

void CMiniView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CMainFrame* pMainPrame = (CMainFrame*)AfxGetMainWnd();

	pMainPrame->GetStarToolView()->SetScrollPos(0, point.x * 16);
	pMainPrame->GetStarToolView()->SetScrollPos(1, point.y * 16);

	int iX = pMainPrame->GetStarToolView()->GetScrollPos(0);

	CView::OnLButtonDown(nFlags, point);

	pMainPrame->GetStarToolView()->ViewInvalidate(true);
}
