#pragma once

#include "Include.h"
class CStarToolView;
class CMyForm;
class CMiniView;

class CMainFrame : public CFrameWndEx
{
protected:
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
public:
	CSplitterWnd	m_MainSplitter;
	CSplitterWnd	m_SubSplitter;

	CStarToolView*	m_pStarToolView;
	CMyForm*		m_pMyForm;
	CMiniView*		m_pMiniView;
public:
	CStarToolView* GetStarToolView(void)
	{
		return m_pStarToolView;
	}
	CMyForm* GetMyForm(void)
	{
		return m_pMyForm;
	}
	CMiniView* GetMiniView(void)
	{
		return m_pMiniView;
	}
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL LoadFrame(UINT nIDResource, DWORD dwDefaultStyle = WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, CWnd* pParentWnd = NULL, CCreateContext* pContext = NULL);

public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CMFCMenuBar       m_wndMenuBar;
	CMFCToolBar       m_wndToolBar;
	CMFCStatusBar     m_wndStatusBar;
	CMFCToolBarImages m_UserImages;

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewCustomize();
	afx_msg LRESULT OnToolbarCreateNew(WPARAM wp, LPARAM lp);
	afx_msg void OnApplicationLook(UINT id);
	afx_msg void OnUpdateApplicationLook(CCmdUI* pCmdUI);
	DECLARE_MESSAGE_MAP()

	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
};


