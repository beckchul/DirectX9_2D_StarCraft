#pragma once


// CMapTool ��ȭ �����Դϴ�.

class CMapTool : public CDialog
{
	DECLARE_DYNAMIC(CMapTool)

public:
	CMapTool(CWnd* pParent = NULL);   // ǥ�� �������Դϴ�.
	virtual ~CMapTool();

// ��ȭ ���� �������Դϴ�.
	enum { IDD = IDD_MAPTOOL };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV �����Դϴ�.

	DECLARE_MESSAGE_MAP()
public:
	int m_MapRadio;
	afx_msg void OnBnClickedRadio(UINT msg);
	afx_msg void OnBnClickedButtonMapsave();
	afx_msg void OnBnClickedButtonMapload();
};
