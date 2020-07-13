#pragma once
#include "afxwin.h"


// CObjectTool 대화 상자입니다.

class CObjectTool : public CDialog
{
	DECLARE_DYNAMIC(CObjectTool)

private:
	int			m_iCount;
	bool		m_bMineral;
	map<CString, RESOURCE*> m_mapObjectList;
	void Release();
public:
	void CreateObject(D3DXVECTOR3 _vPos);
public:
	CObjectTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CObjectTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_OBJECTTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
	int m_ObjectRadio;
	afx_msg void OnBnClickedRadio(UINT msg);
public:
	CListBox m_ObjListBox;
	afx_msg void OnBnClickedButtonObjectDelete();
	afx_msg void OnBnClickedButtonObjectSave();
	afx_msg void OnBnClickedButtonObjectLoad();
	afx_msg void OnLbnSelchangeListObject();
	int m_iObjectX;
	int m_iObjectY;
};
