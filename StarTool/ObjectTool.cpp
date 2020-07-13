// ObjectTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "StarTool.h"
#include "ObjectTool.h"
#include "afxdialogex.h"
#include "MainFrm.h"


// CObjectTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CObjectTool, CDialog)

CObjectTool::CObjectTool(CWnd* pParent /*=NULL*/)
	: CDialog(IDD_OBJECTTOOL, pParent)
	, m_ObjectRadio(0)
	, m_iObjectX(0)
	, m_iObjectY(0)
	, m_iCount(0)
{

}

CObjectTool::~CObjectTool()
{
	Release();
}

void CObjectTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_OBJECT_MINERAL, m_ObjectRadio);
	DDX_Control(pDX, IDC_LIST_OBJECT, m_ObjListBox);
	DDX_Text(pDX, IDC_EDIT_OBJECT_X, m_iObjectX);
	DDX_Text(pDX, IDC_EDIT_OBJECT_Y, m_iObjectY);
}


BEGIN_MESSAGE_MAP(CObjectTool, CDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_OBJECT_MINERAL, IDC_RADIO_OBJECT_GAS, OnBnClickedRadio)
	ON_BN_CLICKED(IDC_BUTTON_OBJECT_DELETE, &CObjectTool::OnBnClickedButtonObjectDelete)
	ON_BN_CLICKED(IDC_BUTTON_OBJECT_SAVE, &CObjectTool::OnBnClickedButtonObjectSave)
	ON_BN_CLICKED(IDC_BUTTON_OBJECT_LOAD, &CObjectTool::OnBnClickedButtonObjectLoad)
	ON_LBN_SELCHANGE(IDC_LIST_OBJECT, &CObjectTool::OnLbnSelchangeListObject)
END_MESSAGE_MAP()


void CObjectTool::OnBnClickedRadio(UINT msg)
{
	UpdateData(TRUE);
	
	if (m_ObjectRadio == 0)
		m_bMineral = true;
	else
		m_bMineral = false;

	UpdateData(FALSE);
}

void CObjectTool::Release()
{
	map<CString, RESOURCE*>::iterator iter = m_mapObjectList.begin();
	map<CString, RESOURCE*>::iterator iter_end = m_mapObjectList.end();

	for (; iter != iter_end; ++iter)
	{
		delete iter->second;
	}
	m_mapObjectList.clear();
}

void CObjectTool::CreateObject(D3DXVECTOR3 _vPos)
{
	UpdateData(TRUE);
	RESOURCE* pResource = new RESOURCE;

	pResource->vPos = _vPos;

	if (m_bMineral)
	{
		pResource->eType = RESOURCE_MINERAL;
		pResource->vSize.x = TILECX * 2;
		pResource->vSize.y = TILECY;
		pResource->m_iMineral = 50000;
		pResource->m_iGas = 0;
	}
	else
	{
		pResource->eType = RESOURCE_GAS;
		pResource->vSize.x = TILECX * 3;
		pResource->vSize.y = TILECY * 2;
		pResource->m_iMineral = 0;
		pResource->m_iGas = 50000;
	}

	m_iObjectX = S_INT_CAST(_vPos.x);
	m_iObjectY = S_INT_CAST(_vPos.y);
	
	TCHAR szBuf[MIN_STR] = L"";
	_itow_s(m_iCount, szBuf, 10);
	m_ObjListBox.AddString(szBuf);

	m_mapObjectList.insert(make_pair(szBuf, pResource));
	++m_iCount;
	UpdateData(FALSE);
}

void CObjectTool::OnBnClickedButtonObjectDelete()
{
	UpdateData(TRUE);

	CString strSelectName;

	int iSelectIndex = m_ObjListBox.GetCurSel();

	if (iSelectIndex < 0)
		return;

	m_ObjListBox.GetText(iSelectIndex, strSelectName);

	map<CString, RESOURCE*>::iterator Find_iter;

	Find_iter = m_mapObjectList.find(strSelectName);

	if (Find_iter == m_mapObjectList.end())
	{
		AfxMessageBox(L"없음!!");
		return;
	}

	delete Find_iter->second;
	m_mapObjectList.erase(strSelectName);

	m_ObjListBox.DeleteString(iSelectIndex);

	UpdateData(FALSE);
}


void CObjectTool::OnBnClickedButtonObjectSave()
{
	HANDLE hFile = CreateFile(L"../Data/ObjectData.bat",
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	DWORD dwByte;

	for (map<CString, RESOURCE*>::iterator iter = m_mapObjectList.begin();
		iter != m_mapObjectList.end(); ++iter)
	{
		WriteFile(hFile, iter->second, sizeof(RESOURCE), &dwByte, NULL);
	}

	CloseHandle(hFile);
}


void CObjectTool::OnBnClickedButtonObjectLoad()
{
	UpdateData(TRUE);

	HANDLE hFile = CreateFile(L"../Data/ObjectData.bat",
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	m_iCount = 0;
	DWORD dwByte;
	Release();
	m_ObjListBox.ResetContent();

	while (true)
	{
		RESOURCE* pUnitData = new RESOURCE;

		ReadFile(hFile, pUnitData, sizeof(RESOURCE), &dwByte, NULL);

		if (dwByte == 0)
		{
			delete pUnitData;
			pUnitData = NULL;
			break;
		}

		TCHAR szBuf[MIN_STR] = L"";
		_itow_s(m_iCount, szBuf, 10);

		m_mapObjectList.insert(make_pair(szBuf, pUnitData));
		m_ObjListBox.AddString(szBuf);
		++m_iCount;
	}


	CloseHandle(hFile);

	UpdateData(FALSE);
}


void CObjectTool::OnLbnSelchangeListObject()
{
	UpdateData(TRUE);

	CString strSelectName;

	int iSelectIndex = m_ObjListBox.GetCurSel();

	if (iSelectIndex == -1)
		return;

	m_ObjListBox.GetText(iSelectIndex, strSelectName);


	map<CString, RESOURCE*>::iterator Find_iter;

	Find_iter = m_mapObjectList.find(strSelectName);

	if (Find_iter == m_mapObjectList.end())
	{
		AfxMessageBox(L"없음!!");
		return;
	}

	m_iObjectX = S_INT_CAST(Find_iter->second->vPos.x);
	m_iObjectY = S_INT_CAST(Find_iter->second->vPos.y);

	if (Find_iter->second->eType == RESOURCE_MINERAL)
	{
		m_ObjectRadio = 0;
	}
	else
	{
		m_ObjectRadio = 1;
	}

	UpdateData(FALSE);
}
