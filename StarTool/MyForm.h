#pragma once

#include "MapTool.h"
#include "UnitTool.h"
#include "BuildTool.h"
#include "ObjectTool.h"
#include "afxwin.h"
#include "Include.h"

class CMyForm : public CFormView
{
	DECLARE_DYNCREATE(CMyForm)
private:
	CMapTool	m_MapTool;
	CUnitTool	m_UnitTool;
	CBuildTool	m_BuildTool;
	CObjectTool	m_ObjectTool;
	list<IMAGE_PATH*>	m_listImagePath;
public:
	void Release(void);
public:
	CMapTool* GetMapTool(void)
	{
		return &m_MapTool;
	}
	CObjectTool* GetObjectTool(void)
	{
		return &m_ObjectTool;
	}
protected:
	CMyForm();           
	virtual ~CMyForm();

public:
	enum { IDD = IDD_MYFORM };
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedButtonMaptool();
	afx_msg void OnBnClickedButtonUnittool();
	afx_msg void OnBnClickedButtonBuildtool();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	CListBox m_ListBoxImagePath;
	afx_msg void OnBnClickedButtonSaveImage();
	afx_msg void OnBnClickedButtonLoadImage();
	afx_msg void OnBnClickedButtonObjecttool();
};


