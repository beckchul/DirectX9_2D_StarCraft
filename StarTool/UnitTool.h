#pragma once
#include "afxwin.h"


// CUnitTool 대화 상자입니다.

class CUnitTool : public CDialog
{
	DECLARE_DYNAMIC(CUnitTool)
private:
	map<CString, UNIT*>		m_mapUnitData;
private:
	void Release();
public:
	CUnitTool(CWnd* pParent = NULL);   // 표준 생성자입니다.
	virtual ~CUnitTool();

// 대화 상자 데이터입니다.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_UNITTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 지원입니다.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strUnitName;
	UINT m_iUnitAtt;
	UINT m_iUnitShield;
	UINT m_iUnitHp;
	UINT m_iUnitMakeTime;
	UINT m_iUnitDrop;
	int m_iUnitPeple;
	int m_iUnitMineral;
	int m_iUnitGas;
	int m_iUnitType;
	CListBox m_ListUnitData;
	afx_msg void OnBnClickedButtonUnitAdd();
	afx_msg void OnBnClickedButtonUnitDelete();
	afx_msg void OnBnClickedButtonUnitModify();
	afx_msg void OnBnClickedButtonUnitSave();
	afx_msg void OnBnClickedButtonUnitLoad();
	afx_msg void OnLbnSelchangeListUnitdata();
	afx_msg void OnBnClickedRadio(UINT msg);
	
	int m_iUnitMp;
};
