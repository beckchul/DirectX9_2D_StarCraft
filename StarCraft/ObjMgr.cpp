#include "stdafx.h"
#include "ObjMgr.h"
#include "GameObject.h"
#include "TextureMgr.h"

IMPLEMENT_SINGLETON(CObjMgr);

CObjMgr::CObjMgr()
{
}

CObjMgr::~CObjMgr()
{
	Release();
	DataRelease();
}

void CObjMgr::AddGameObject(CGameObject * pObj, eOBJ_TYPE eType)
{
	m_ObjList[eType].push_back(pObj);
}

void CObjMgr::LoadData(void)
{
	LoadUnitInfo();
	LoadBuildInfo();
	LoadResourceInfo();

	CoInitialize(NULL);

	MSXML2::IXMLDOMDocument2Ptr pDoc;

	pDoc.CreateInstance(__uuidof(DOMDocument40));

	pDoc->async = VARIANT_FALSE;
	pDoc->validateOnParse = VARIANT_TRUE;
	pDoc->resolveExternals = VARIANT_TRUE;

	if (pDoc->load((_variant_t)"../Data/ImagePath.xml") == E_FAIL)
	{
		ERRBOX(L"Xml Load Failed");
		return;
	}

	MSXML2::IXMLDOMNodeListPtr pElementList = NULL;

	CString strTemp = L"";

	pElementList = pDoc->selectNodes(L"//IMAGE");

	int iCount = 0;
	int iTotal = 0;

	iTotal = pElementList->Getlength();

	for (int i = 0; i < iTotal; ++i)
	{
		IMAGE_PATH* pData = new IMAGE_PATH;
		CString strTemp = L"";

		strTemp
			= pElementList->Getitem(i)->selectNodes(L"//OBJKEY")->Getitem(i)->Gettext().operator LPCSTR();
		lstrcpy(pData->szObjKey, strTemp);

		strTemp
			= pElementList->Getitem(i)->selectNodes(L"//STATEKEY")->Getitem(i)->Gettext().operator LPCSTR();
		lstrcpy(pData->szStateKey, strTemp);
		strTemp
			= pElementList->Getitem(i)->selectNodes(L"//PATH")->Getitem(i)->Gettext().operator LPCSTR();
		lstrcpy(pData->szPath, strTemp);

		pData->iCount
			= atoi(pElementList->Getitem(i)->selectNodes(L"//COUNT")->Getitem(i)->Gettext());

		m_ImagePath.push_back(pData);

		TextureMgr->InsertTexture(pData->szPath, pData->szObjKey, TEX_MULTI,
			pData->szStateKey, pData->iCount);
	}
}

void CObjMgr::LoadUnitInfo(void)
{
	HANDLE hFile = CreateFile(L"../Data/UnitData.bat",
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	DWORD dwByte;

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

		m_UnitInfoList.push_back(pUnitData);
	}
	CloseHandle(hFile);
}

void CObjMgr::LoadBuildInfo(void)
{
	HANDLE hFile = CreateFile(L"../Data/BuildData.bat",
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	DWORD dwByte;

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
		m_BuildInfoList.push_back(pBuildData);
	}
	CloseHandle(hFile);
}

void CObjMgr::LoadResourceInfo(void)
{
	HANDLE hFile = CreateFile(L"../Data/ObjectData.bat",
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	DWORD dwByte;

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

		m_ResourceInfoList.push_back(pUnitData);
	}
	CloseHandle(hFile);
}


HRESULT CObjMgr::Initialize(void)
{
	return S_OK;
}

int CObjMgr::Update(void)
{
	for (int i = 0; i < OBJ_END; ++i)
	{
		OBJITER iter = m_ObjList[i].begin();
		OBJITER iter_end = m_ObjList[i].end();

		for (iter; iter != iter_end; )
		{
			if ((*iter)->Update() == 1)
			{
				SAFE_DELETE(*iter);

				iter = m_ObjList[i].erase(iter);
			}
			else
				++iter;
		}
	}

	return 0;
}

void CObjMgr::Render(void)
{
	for (int i = 0; i < OBJ_END; ++i)
	{
		OBJITER iter = m_ObjList[i].begin();
		OBJITER iter_end = m_ObjList[i].end();

		for (iter; iter != iter_end; ++iter)
		{
			m_vecRender[(*iter)->GetRenderType()].push_back(*iter);
		}
	}
	for (int i = 0; i < RENDER_END; ++i)
	{
		sort(m_vecRender[i].begin(), m_vecRender[i].end(), CObjMgr::CompareY);
	}
	for (int i = 0; i < RENDER_END; ++i)
	{

		for (size_t j = 0; j < m_vecRender[i].size(); ++j)
		{
			if(i== RENDER_BACKGROUND || i == RENDER_INTERPACE || i == RENDER_MOUSE || i == RENDER_FOG ||
				(m_vecRender[i][j]->GetInfo().vPos.x > m_vecRender[i][j]->GetScrollPos().x - 80 &&
				m_vecRender[i][j]->GetInfo().vPos.x < m_vecRender[i][j]->GetScrollPos().x + WINCX + 80 && 
				m_vecRender[i][j]->GetInfo().vPos.y > m_vecRender[i][j]->GetScrollPos().y - 80 && 
				m_vecRender[i][j]->GetInfo().vPos.y < m_vecRender[i][j]->GetScrollPos().y + WINCY + 80))
			m_vecRender[i][j]->Render();
		}
			

		m_vecRender[i].clear();
	}
}

void CObjMgr::Release(void)
{
	for (int i = 0; i < OBJ_END; ++i)
	{
		OBJITER iter = m_ObjList[i].begin();
		OBJITER iter_end = m_ObjList[i].end();

		for (iter; iter != iter_end; ++iter)
		{
			Safe_Delete(*iter);
		}
		list<CGameObject*> Temp;
		m_ObjList[i].swap(Temp);
	}
}

void CObjMgr::DataRelease(void)
{
	for (list<IMAGE_PATH*>::iterator iter = m_ImagePath.begin();
		iter != m_ImagePath.end(); ++iter)
	{
		Safe_Delete(*iter);
	}
	m_ImagePath.clear();

	for (list<UNIT*>::iterator iter = m_UnitInfoList.begin();
		iter != m_UnitInfoList.end(); ++iter)
	{
		Safe_Delete(*iter);
	}
	m_ImagePath.clear();

	for (list<BUILD*>::iterator iter = m_BuildInfoList.begin();
		iter != m_BuildInfoList.end(); ++iter)
	{
		Safe_Delete(*iter);
	}
	m_ImagePath.clear();

	for (list<RESOURCE*>::iterator iter = m_ResourceInfoList.begin();
		iter != m_ResourceInfoList.end(); ++iter)
	{
		Safe_Delete(*iter);
	}
	m_ImagePath.clear();
}

bool CObjMgr::CompareY(CGameObject* pSour, CGameObject* pDest)
{
	return pSour->GetPos().y < pDest->GetPos().y;
}