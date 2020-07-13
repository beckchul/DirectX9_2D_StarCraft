// MapTool.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "StarTool.h"
#include "MapTool.h"
#include "MainFrm.h"
#include "BackGround.h"
#include "StarToolView.h"


// CMapTool 대화 상자입니다.

IMPLEMENT_DYNAMIC(CMapTool, CDialog)

CMapTool::CMapTool(CWnd* pParent /*=NULL*/)
	: CDialog(CMapTool::IDD, pParent)
	, m_MapRadio(0)
{

}

CMapTool::~CMapTool()
{
}

void CMapTool::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Radio(pDX, IDC_RADIO_NOMAL, m_MapRadio);
}


BEGIN_MESSAGE_MAP(CMapTool, CDialog)
	ON_CONTROL_RANGE(BN_CLICKED, IDC_RADIO_NOMAL, IDC_RADIO_HILL, OnBnClickedRadio)
	ON_BN_CLICKED(IDC_BUTTON_MAPSAVE, &CMapTool::OnBnClickedButtonMapsave)
	ON_BN_CLICKED(IDC_BUTTON_MAPLOAD, &CMapTool::OnBnClickedButtonMapload)
END_MESSAGE_MAP()

void CMapTool::OnBnClickedRadio(UINT msg)
{
	UpdateData(TRUE);
	CMainFrame* pMainPrame = (CMainFrame*)AfxGetMainWnd();
	pMainPrame->GetStarToolView()->SetTileOption(m_MapRadio);
	UpdateData(FALSE);
}

void CMapTool::OnBnClickedButtonMapsave()
{
	CMainFrame* pMainPrame = (CMainFrame*)AfxGetMainWnd();
	vector<PTILE> m_vecTile = pMainPrame->GetStarToolView()->GetBackGround()->GetTile();
	HANDLE hFile = CreateFile(L"../Data/MapData.bat", GENERIC_WRITE, NULL, NULL
							, CREATE_ALWAYS
							, FILE_ATTRIBUTE_NORMAL
							, NULL);

	DWORD dwByte;

	for (size_t i = 0; i < m_vecTile.size(); ++i)
	{
		WriteFile(hFile, m_vecTile[i], sizeof(TILE), &dwByte, NULL);
	}

	CloseHandle(hFile);
}

void CMapTool::OnBnClickedButtonMapload()
{
	UpdateData(TRUE);
	vector<PTILE> vecTile;

	CMainFrame* pMainPrame = (CMainFrame*)AfxGetMainWnd();
	pMainPrame->GetStarToolView()->GetBackGround()->TileRelease();

	HANDLE hFile = CreateFile(L"../Data/MapData.bat", GENERIC_READ, NULL, NULL
							, OPEN_EXISTING
							, FILE_ATTRIBUTE_NORMAL
							, NULL);

	DWORD dwByte;

	while (true)
	{
		PTILE pTile = new TILE;

		ReadFile(hFile, pTile, sizeof(TILE), &dwByte, NULL);

		if (dwByte == NULL)
		{
			SAFE_DELETE(pTile);
			break;
		}

		vecTile.push_back(pTile);
	}


	CloseHandle(hFile);

	pMainPrame->GetStarToolView()->GetBackGround()->SetTile(vecTile);

	UpdateData(FALSE);
	Invalidate(TRUE);
}
