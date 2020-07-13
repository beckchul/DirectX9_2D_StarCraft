#include "stdafx.h"
#include "StarTool.h"
#include "MainFrm.h"
#include "StarToolDoc.h"
#include "StarToolView.h"
#include "BackGround.h"
#include "MyForm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


IMPLEMENT_DYNCREATE(CStarToolView, CScrollView)

BEGIN_MESSAGE_MAP(CStarToolView, CScrollView)
	ON_COMMAND(ID_FILE_PRINT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, &CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, &CStarToolView::OnFilePrintPreview)
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_RBUTTONDOWN()
END_MESSAGE_MAP()

CStarToolView::CStarToolView()
{
}

CStarToolView::~CStarToolView()
{
}

BOOL CStarToolView::PreCreateWindow(CREATESTRUCT& cs)
{
	return CView::PreCreateWindow(cs);
}

void CStarToolView::OnDraw(CDC*)
{
	CStarToolDoc* pDoc = GetDocument();

	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	Device->Render_Begin();

	m_pBackGround->Render();

	Device->Render_End();

}

void CStarToolView::OnFilePrintPreview()
{
	AFXPrintPreview(this);
}

BOOL CStarToolView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return DoPreparePrinting(pInfo);
}

void CStarToolView::OnBeginPrinting(CDC*, CPrintInfo* )
{
}

void CStarToolView::OnEndPrinting(CDC*, CPrintInfo* )
{
}

void CStarToolView::OnRButtonUp(UINT nFlags, CPoint point)
{
	ClientToScreen(&point);
	OnContextMenu(this, point);
}

void CStarToolView::OnContextMenu(CWnd* pWnd, CPoint point)
{
	theApp.GetContextMenuManager()->ShowPopupMenu(IDR_POPUP_EDIT, point.x, point.y, this, TRUE);
}

#ifdef _DEBUG
void CStarToolView::AssertValid() const
{
	CView::AssertValid();
}

void CStarToolView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CStarToolDoc* CStarToolView::GetDocument() const
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CStarToolDoc)));
	return (CStarToolDoc*)m_pDocument;
}
#endif
void CStarToolView::OnInitialUpdate()
{
	CScrollView::OnInitialUpdate();

	SetScrollSizes(MM_TEXT, CSize(MAPCX, MAPCY));
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();

	RECT rcRect;

	pMainFrame->GetWindowRect(&rcRect);

	SetRect(&rcRect, 0, 0, rcRect.right - rcRect.left, rcRect.bottom - rcRect.top);

	RECT rcMainView;
	GetClientRect(&rcMainView);

	pMainFrame->SetWindowPos(NULL, 0, 0
		, WINCX + rcRect.right - rcMainView.right
		, WINCY + rcRect.bottom - rcMainView.bottom
		, SWP_NOZORDER);

	g_hWnd = m_hWnd;
	m_iTileOption = 0;

	if(FAILED(Device->InitD3D()))
		AfxMessageBox(L"디바이스 생성 실패");

	if (FAILED(TextureMgr->InsertTexture(L"../Texture/UI/Map/Main/MainFrame_0.png", L"Map", TEX_SINGLE)))
	{
		AfxMessageBox(L"단일 이미지 로드 실패!");
	}

	if (FAILED(CTextureMgr::GetInstance()->InsertTexture(L"../Texture/UI/Map/Tool/%d.png"
		, L"Tile", TEX_MULTI, L"Back", 5)))
	{
		AfxMessageBox(L"멀티 텍스쳐 로드 실패!");
	}

	m_pBackGround = new CBackGround;
	m_pBackGround->Initialize();
	m_pBackGround->SetView(this);
}

void CStarToolView::OnDestroy()
{
	CScrollView::OnDestroy();
	SAFE_DELETE(m_pBackGround);
	Device->DestroyInstance();
	TextureMgr->DestroyInstance();
}

void CStarToolView::OnLButtonDown(UINT nFlags, CPoint point)
{
	CScrollView::OnLButtonDown(nFlags, point);

	if (!GetAsyncKeyState('A'))
		return;

	if (point.x >= MAPCX ||
		point.x <= 0 ||
		point.y >= MAPCY - 10 ||
		point.y <= 0)
		return;

	D3DXVECTOR3 vMousePos = D3DXVECTOR3( S_FLOAT_CAST(point.x  + GetScrollPos(0))
									   , S_FLOAT_CAST(point.y  + GetScrollPos(1))
									   , 0.f);
	
	m_pBackGround->TilePicking(vMousePos, m_iTileOption);

	Invalidate(FALSE);
}

void CStarToolView::OnMouseMove(UINT nFlags, CPoint point)
{
	if(GetAsyncKeyState(VK_LBUTTON))
		OnLButtonDown(nFlags, point);
	CScrollView::OnMouseMove(nFlags, point);
}


void CStarToolView::OnRButtonDown(UINT nFlags, CPoint point)
{
	if (point.x >= MAPCX ||
		point.x <= 0 ||
		point.y >= MAPCY - 10 ||
		point.y <= 0)
		return;

	
	D3DXVECTOR3 vMousePos = D3DXVECTOR3(S_FLOAT_CAST(point.x + GetScrollPos(0))
		, S_FLOAT_CAST(point.y + GetScrollPos(1))
		, 0.f);
	CMainFrame* pMainFrame = (CMainFrame*)AfxGetMainWnd();
	pMainFrame->GetMyForm()->GetObjectTool()->CreateObject(vMousePos);


	CScrollView::OnRButtonDown(nFlags, point);
}
