// BuildTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "StarTool.h"
#include "BuildTool.h"
#include "afxdialogex.h"


// CBuildTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CBuildTool, CDialog)

CBuildTool::CBuildTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_BUILDTOOL, pParent)
	, m_strBuildName(_T(""))
	, m_iBuildShield(0)
	, m_iBuildHp(0)
	, m_iBuildMineral(0)
	, m_iBuildGas(0)
	, m_iBuildTime(0)
	, m_iBuildMp(0)
	, m_iBuildSizeX(0)
	, m_iBuildSizeY(0)
{

}

CBuildTool::~CBuildTool()
{
	Release();
}

void CBuildTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT_BUILD_NAME, m_strBuildName);
	DDX_Text(pDX, IDC_EDIT_BUILD_SHILD, m_iBuildShield);
	DDX_Text(pDX, IDC_EDIT_BUILD_HP, m_iBuildHp);
	DDX_Text(pDX, IDC_EDIT_BUILD_MINERAL, m_iBuildMineral);
	DDX_Text(pDX, IDC_EDIT_BUILD_GAS, m_iBuildGas);
	DDX_Text(pDX, IDC_EDIT_BUILD_MAKETIME, m_iBuildTime);
	DDX_Text(pDX, IDC_EDIT_BUILD_MP, m_iBuildMp);
	DDX_Text(pDX, IDC_EDIT_BUILD_SIZE_X, m_iBuildSizeX);
	DDX_Text(pDX, IDC_EDIT_BUILD_SIZE_Y, m_iBuildSizeY);
	DDX_Control(pDX, IDC_LIST_BUILDLIST, m_BuildList);
}


BEGIN_MESSAGE_MAP(CBuildTool, CDialog)
	ON_BN_CLICKED(IDC_BUTTON_BUILD_ADD, &CBuildTool::OnBnClickedButtonBuildAdd)
	ON_BN_CLICKED(IDC_BUTTON_BUILD_DELETE, &CBuildTool::OnBnClickedButtonBuildDelete)
	ON_BN_CLICKED(IDC_BUTTON_BUILD_MODIFY, &CBuildTool::OnBnClickedButtonBuildModify)
	ON_BN_CLICKED(IDC_BUTTON_BUILD_SAVE, &CBuildTool::OnBnClickedButtonBuildSave)
	ON_BN_CLICKED(IDC_BUTTON_BUILD_LOAD, &CBuildTool::OnBnClickedButtonBuildLoad)
	ON_LBN_SELCHANGE(IDC_LIST_BUILDLIST, &CBuildTool::OnLbnSelchangeListBuildlist)
END_MESSAGE_MAP()


void CBuildTool::Release(void)
{
	map<CString, BUILD*>::iterator iter		= m_mapBiuldData.begin();
	map<CString, BUILD*>::iterator iter_end = m_mapBiuldData.end();

	for (; iter != iter_end; ++iter)
	{
		delete iter->second;
	}
	m_mapBiuldData.clear();
}

void CBuildTool::OnBnClickedButtonBuildAdd()
{
	UpdateData(TRUE);

	map<CString, BUILD*>::iterator Find_iter;

	Find_iter = m_mapBiuldData.find(m_strBuildName);

	if (Find_iter != m_mapBiuldData.end())
	{
		AfxMessageBox(L"중복된 이름입니다!!");
		return;
	}

	BUILD* pBuildData = new BUILD;

	lstrcpy(pBuildData->szName, m_strBuildName);
	pBuildData->iHp			= m_iBuildHp;
	pBuildData->iHpMax		= m_iBuildHp;
	pBuildData->iShield		= m_iBuildShield;
	pBuildData->iShieldMax  = m_iBuildShield;
	pBuildData->iMineral	= m_iBuildMineral;
	pBuildData->iGas		= m_iBuildGas;
	pBuildData->iMp			= m_iBuildMp;
	pBuildData->iMpMax		= m_iBuildMp;
	pBuildData->iSizeX		= m_iBuildSizeX;
	pBuildData->iSizeY		= m_iBuildSizeY;
	pBuildData->iMakeTime	= m_iBuildTime;
	m_BuildList.AddString(m_strBuildName);
	m_mapBiuldData.insert(make_pair(m_strBuildName, pBuildData));

	UpdateData(FALSE);
}


void CBuildTool::OnBnClickedButtonBuildDelete()
{
	UpdateData(TRUE);

	CString strSelectName;

	int iSelectIndex = m_BuildList.GetCurSel();

	if (iSelectIndex < 0)
		return;

	m_BuildList.GetText(iSelectIndex, strSelectName);

	map<CString, BUILD*>::iterator Find_iter;

	Find_iter = m_mapBiuldData.find(strSelectName);

	if (Find_iter == m_mapBiuldData.end())
	{
		AfxMessageBox(L"없음!!");
		return;
	}

	delete Find_iter->second;
	m_mapBiuldData.erase(strSelectName);

	m_BuildList.DeleteString(iSelectIndex);

	UpdateData(FALSE);
}


void CBuildTool::OnBnClickedButtonBuildModify()
{
	UpdateData(TRUE);
	OnBnClickedButtonBuildDelete();
	OnBnClickedButtonBuildAdd();
	UpdateData(FALSE);
}

void CBuildTool::OnLbnSelchangeListBuildlist()
{
	UpdateData(TRUE);

	CString strSelectName;

	int iSelectIndex = m_BuildList.GetCurSel();

	if (iSelectIndex == -1)
		return;

	m_BuildList.GetText(iSelectIndex, strSelectName);


	map<CString, BUILD*>::iterator Find_iter;

	Find_iter = m_mapBiuldData.find(strSelectName);

	if (Find_iter == m_mapBiuldData.end())
	{
		AfxMessageBox(L"없음!!");
		return;
	}

	m_strBuildName	= Find_iter->second->szName;
	m_iBuildHp		= Find_iter->second->iHp;
	m_iBuildShield	= Find_iter->second->iShield;
	m_iBuildMineral = Find_iter->second->iMineral;
	m_iBuildGas		= Find_iter->second->iGas;
	m_iBuildMp		= Find_iter->second->iMp;
	m_iBuildTime	= Find_iter->second->iMakeTime;
	m_iBuildSizeX	= Find_iter->second->iSizeX;
	m_iBuildSizeY	= Find_iter->second->iSizeY;

	UpdateData(FALSE);
}

void CBuildTool::OnBnClickedButtonBuildSave()
{
	HANDLE hFile = CreateFile(L"../Data/BuildData.bat",
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	DWORD dwByte;

	for (map<CString, BUILD*>::iterator iter = m_mapBiuldData.begin();
		iter != m_mapBiuldData.end(); ++iter)
	{
		WriteFile(hFile, iter->second, sizeof(BUILD), &dwByte, NULL);
	}

	CloseHandle(hFile);
}


void CBuildTool::OnBnClickedButtonBuildLoad()
{
	UpdateData(TRUE);

	HANDLE hFile = CreateFile(L"../Data/BuildData.bat",
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	DWORD dwByte;
	Release();
	m_BuildList.ResetContent();

	while (true)
	{
		BUILD* pBuildData = new BUILD;

		ReadFile(hFile, pBuildData, sizeof(BUILD), &dwByte, NULL);

		if (dwByte == 0)
		{
			delete pBuildData;
			pBuildData = NULL;
			break;
		}

		m_mapBiuldData.insert(make_pair(pBuildData->szName, pBuildData));
		m_BuildList.AddString(pBuildData->szName);
	}


	CloseHandle(hFile);

	UpdateData(FALSE);
}



