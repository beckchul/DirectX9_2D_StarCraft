#pragma once
#include "afxwin.h"


// CBuildTool ��ȭ �����Դϴ�.

class CBuildTool : public CDialog
{
	DECLARE_DYNAMIC(CBuildTool)
private:
	map<CString, BUILD*>	m_mapBiuldData;
private:
	void Release(void);
public:
	CBuildTool(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CBuildTool();

// ��ȭ ���� �������Դϴ�.
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_BUILDTOOL };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	CString m_strBuildName;
	int m_iBuildShield;
	int m_iBuildHp;
	int m_iBuildMineral;
	int m_iBuildGas;
	int m_iBuildTime;
	int m_iBuildMp;
	int m_iBuildSizeX;
	int m_iBuildSizeY;
	CListBox m_BuildList;
	afx_msg void OnBnClickedButtonBuildAdd();
	afx_msg void OnBnClickedButtonBuildDelete();
	afx_msg void OnBnClickedButtonBuildModify();
	afx_msg void OnBnClickedButtonBuildSave();
	afx_msg void OnBnClickedButtonBuildLoad();
	afx_msg void OnLbnSelchangeListBuildlist();
};
