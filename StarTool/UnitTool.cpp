// UnitTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "StarTool.h"
#include "UnitTool.h"
#include "afxdialogex.h"
#include "MainFrm.h"
#include "StarToolView.h"

// CUnitTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CUnitTool, CDialog)

CUnitTool::CUnitTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_UNITTOOL, pParent)
	, m_strUnitName(_T(""))
	, m_iUnitAtt(0)
	, m_iUnitShield(0)
	, m_iUnitHp(0)
	, m_iUnitMakeTime(0)
	, m_iUnitDrop(0)
	, m_iUnitPeple(0)
	, m_iUnitMineral(0)
	, m_iUnitGas(0)
	, m_iUnitType(0)
	, m_iUnitMp(0)
{

}

CUnitTool::~CUnitTool()
{
	Release();
}

void CUnitTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_UNIT_NAME, m_strUnitName);
	DDX_Text(pDX, IDC_EDIT_UNIT_ATT, m_iUnitAtt);
	DDX_Text(pDX, IDC_EDIT_UNIT_SHIED, m_iUnitShield);
	DDX_Text(pDX, IDC_EDIT_UNIT_HP, m_iUnitHp);
	DDX_Text(pDX, IDC_EDIT_UNIT_MAKETIME, m_iUnitMakeTime);
	DDX_Text(pDX, IDC_EDIT_UNIT_DROP, m_iUnitDrop);
	DDX_Text(pDX, IDC_EDIT_UNIT_PEPLE, m_iUnitPeple);
	DDX_Text(pDX, IDC_EDIT_UNIT_MINERAL, m_iUnitMineral);
	DDX_Text(pDX, IDC_EDIT_UNIT_GAS, m_iUnitGas);
	DDX_Control(pDX, IDC_LIST_UNITDATA, m_ListUnitData);
	DDX_Radio(pDX, IDC_RADIO_GROUND, m_iUnitType);
	DDX_Text(pDX, IDC_EDIT_UNIT_MP, m_iUnitMp);
}


BEGIN_MESSAGE_MAP(CUnitTool, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_UNIT_ADD, &CUnitTool::OnBnClickedButtonUnitAdd)
	ON_BN_CLICKED(IDC_BUTTON_UNIT_DELETE, &CUnitTool::OnBnClickedButtonUnitDelete)
	ON_BN_CLICKED(IDC_BUTTON_UNIT_MODIFY, &CUnitTool::OnBnClickedButtonUnitModify)
	ON_BN_CLICKED(IDC_BUTTON_UNIT_SAVE, &CUnitTool::OnBnClickedButtonUnitSave)
	ON_BN_CLICKED(IDC_BUTTON_UNIT_LOAD, &CUnitTool::OnBnClickedButtonUnitLoad)
	ON_LBN_SELCHANGE(IDC_LIST_UNITDATA, &CUnitTool::OnLbnSelchangeListUnitdata)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_GROUND, IDC_RADIO_ARR, OnBnClickedRadio)
END_MESSAGE_MAP()


// CUnitTool 메시지 처리기입니다.
void CUnitTool::Release()
{
	map<CString, UNIT*>::iterator iter = m_mapUnitData.begin();
	map<CString, UNIT*>::iterator iter_end = m_mapUnitData.end();

	for (; iter != iter_end; ++iter)
	{
		delete iter->second;
	}
	m_mapUnitData.clear();
}

void CUnitTool::OnBnClickedRadio(UINT msg)
{
	UpdateData(TRUE);
	CMainFrame* pMainPrame = (CMainFrame*)AfxGetMainWnd();
	pMainPrame->GetStarToolView()->SetTileOption(m_iUnitType);
	UpdateData(FALSE);
}

void CUnitTool::OnBnClickedButtonUnitAdd()
{
	UpdateData(TRUE);

	map<CString, UNIT*>::iterator Find_iter;

	Find_iter = m_mapUnitData.find(m_strUnitName);

	if (Find_iter != m_mapUnitData.end())
	{
		AfxMessageBox(L"중복된 이름입니다!!");
		return;
	}

	UNIT* pUnitData = new UNIT;

	lstrcpy(pUnitData->szName, m_strUnitName);
	pUnitData->iAtt		  = m_iUnitAtt;
	pUnitData->iShield	  = m_iUnitShield;
	pUnitData->iShieldMax = m_iUnitShield;
	pUnitData->iHp		  = m_iUnitHp;
	pUnitData->iHpMax	  = m_iUnitHp;
	pUnitData->iMakeTime  = m_iUnitMakeTime;
	pUnitData->iDrop	  = m_iUnitDrop;
	pUnitData->iPeople	  = m_iUnitPeple;
	pUnitData->iMineral	  = m_iUnitMineral;
	pUnitData->iGas		  = m_iUnitGas;
	pUnitData->eType	  = (eUNIT_TYPE)m_iUnitType;
	pUnitData->iMp		  = m_iUnitMp;

	if(pUnitData->iMp != 0)
		pUnitData->iMpMax	  = 200;
	else
		pUnitData->iMpMax = 0;
	

	if (((CButton*)GetDlgItem(IDC_CHECK_GROUND))->GetCheck())
		pUnitData->bAttGround = true;
	else
		pUnitData->bAttGround = false;

	if (((CButton*)GetDlgItem(IDC_CHECK_ARR))->GetCheck())
		pUnitData->bAttArr = true;
	else
		pUnitData->bAttArr = false;

	m_ListUnitData.AddString(m_strUnitName);
	m_mapUnitData.insert(make_pair(m_strUnitName, pUnitData));
	UpdateData(FALSE);
}


void CUnitTool::OnBnClickedButtonUnitDelete()
{
	UpdateData(TRUE);

	CString strSelectName;

	int iSelectIndex = m_ListUnitData.GetCurSel();

	if (iSelectIndex < 0)
		return;

	m_ListUnitData.GetText(iSelectIndex, strSelectName);

	map<CString, UNIT*>::iterator Find_iter;

	Find_iter = m_mapUnitData.find(strSelectName);

	if (Find_iter == m_mapUnitData.end())
	{
		AfxMessageBox(L"없음!!");
		return;
	}

	delete Find_iter->second;
	m_mapUnitData.erase(strSelectName);

	m_ListUnitData.DeleteString(iSelectIndex);


	UpdateData(FALSE);
}


void CUnitTool::OnBnClickedButtonUnitModify()
{
	UpdateData(TRUE);
	OnBnClickedButtonUnitDelete();
	OnBnClickedButtonUnitAdd();
	UpdateData(FALSE);
}


void CUnitTool::OnBnClickedButtonUnitSave()
{
	HANDLE hFile = CreateFile(L"../Data/UnitData.bat",
								GENERIC_WRITE,
								0,
								NULL,
								CREATE_ALWAYS,
								FILE_ATTRIBUTE_NORMAL,
								NULL);

	DWORD dwByte;

	for (map<CString, UNIT*>::iterator iter = m_mapUnitData.begin();
		iter != m_mapUnitData.end(); ++iter)
	{
		WriteFile(hFile, iter->second, sizeof(UNIT), &dwByte, NULL);
	}

	CloseHandle(hFile);
}


void CUnitTool::OnBnClickedButtonUnitLoad()
{
	UpdateData(TRUE);

	HANDLE hFile = CreateFile(L"../Data/UnitData.bat",
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	DWORD dwByte;
	Release();
	m_ListUnitData.ResetContent();

	while (true)
	{
		UNIT* pUnitData = new UNIT;

		ReadFile(hFile, pUnitData, sizeof(UNIT), &dwByte, NULL);

		if (dwByte == 0)
		{
			delete pUnitData;
			pUnitData = NULL;
			break;
		}

		m_mapUnitData.insert(make_pair(pUnitData->szName, pUnitData));
		m_ListUnitData.AddString(pUnitData->szName);
	}


	CloseHandle(hFile);

	UpdateData(FALSE);
}


void CUnitTool::OnLbnSelchangeListUnitdata()
{
	UpdateData(TRUE);

	CString strSelectName;

	int iSelectIndex = m_ListUnitData.GetCurSel();

	if (iSelectIndex == -1)
		return;

	m_ListUnitData.GetText(iSelectIndex, strSelectName);


	map<CString, UNIT*>::iterator Find_iter;

	Find_iter = m_mapUnitData.find(strSelectName);

	if (Find_iter == m_mapUnitData.end())
	{
		AfxMessageBox(L"없음!!");
		return;
	}

	m_strUnitName	= Find_iter->second->szName;
	m_iUnitAtt		= Find_iter->second->iAtt;
	m_iUnitShield	= Find_iter->second->iShield;
	m_iUnitHp		= Find_iter->second->iHp;
	m_iUnitMakeTime = Find_iter->second->iMakeTime;
	m_iUnitDrop		= Find_iter->second->iDrop;
	m_iUnitPeple	= Find_iter->second->iPeople;
	m_iUnitMineral	= Find_iter->second->iMineral;
	m_iUnitGas		= Find_iter->second->iGas;
	m_iUnitType		= Find_iter->second->eType;
	m_iUnitMp		= Find_iter->second->iMp;


	if (Find_iter->second->bAttGround == true)
		((CButton*)GetDlgItem(IDC_CHECK_GROUND))->SetCheck(TRUE);
	else
		((CButton*)GetDlgItem(IDC_CHECK_GROUND))->SetCheck(FALSE);

	if(Find_iter->second->bAttArr == true)
		((CButton*)GetDlgItem(IDC_CHECK_ARR))->SetCheck(TRUE);
	else
		((CButton*)GetDlgItem(IDC_CHECK_ARR))->SetCheck(FALSE);

	UpdateData(FALSE);
}
