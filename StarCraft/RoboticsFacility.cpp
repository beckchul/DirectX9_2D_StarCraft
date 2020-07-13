#include "stdafx.h"
#include "RoboticsFacility.h"
#include "ObjMgr.h"
#include "Device.h"
#include "TextureMgr.h"
#include "BackGround.h"
#include "KeyMgr.h"
#include "Interpace.h"
#include "Mouse.h"
#include "TimeMgr.h"
#include "Factory.h"
#include "Shuttle.h"
#include "Reaver.h"
#include "Observer.h"
#include "Effect.h"

CRoboticsFacility::CRoboticsFacility()
{
}


CRoboticsFacility::~CRoboticsFacility()
{
	Release();
}

HRESULT CRoboticsFacility::Initialize(void)
{
	list<BUILD*> BuildList = ObjMgr->GetBuildList();

	list<BUILD*>::iterator iter = BuildList.begin();
	list<BUILD*>::iterator iter_end = BuildList.end();

	lstrcpy(m_tInfo.szObjKey, L"Building");
	lstrcpy(m_tInfo.szStateKey, L"RoboticsFacility");

	for (; iter != iter_end; ++iter)
	{
		if (!lstrcmp((*iter)->szName, m_tInfo.szStateKey))
		{
			m_tBuildInfo = (*(*iter));
			break;
		}
	}
	m_tInfo.vSize.x = S_FLOAT_CAST(m_tBuildInfo.iSizeX * TILECX) + 40;
	m_tInfo.vSize.y = S_FLOAT_CAST(m_tBuildInfo.iSizeY * TILECY);
	m_tClick = FRAME(0.f, 10.f, 2.f);
	m_bKeyInput = false;
	m_bCreateUnit = false;
	m_eBuildCreateType = BUILD_CREATE_NOMAL;
	m_eBuildType = BUILD_ROBOTICS_FACILITY;
	m_vStartingPoint = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_eRenderType = RENDER_GROUND;
	SettingTile();
	m_iView = 10;
	return S_OK;
}

int CRoboticsFacility::Update(void)
{
	if (m_dwState == UNIT_DEAD)
		m_bDeadCheck = true;

	if (m_bDeadCheck)
	{
		SoundMgr->PlaySound(L"explo4.wav", CHANNEL_EFFECT_CREATE, 0.9f, m_tInfo.vPos);
		ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_BUILD_DEAD), OBJ_EFFECT);
		ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_BUILD_DEAD2), OBJ_EFFECT);
		SettingTile();
		return 1;
	}

	Function();
	CreateUnit(true);


	if (m_eTeamType != TEAM_PLAYER)
	{
		m_vStartingPoint = D3DXVECTOR3(2920.f + rand() % 200 - 100.f, 900.f + rand() % 100 - 50.f, 0.f);
		CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
		int iAICount = D_CAST(CInterpace, pInterpace)->GetAICount();

		FRAME tAIFrame = D_CAST(CInterpace, pInterpace)->GetAIFrame();

		if (iAICount == 370 && tAIFrame.fFrame == 0.f)
		{
			CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
			int iMineral = D_CAST(CInterpace, pInterpace)->GetWorldMineral(m_eTeamType);
			int iGas = D_CAST(CInterpace, pInterpace)->GetWorldGas(m_eTeamType);
			int iHumun = D_CAST(CInterpace, pInterpace)->GetHumanCount(m_eTeamType);
			int iHumunMax = D_CAST(CInterpace, pInterpace)->GetHumanMaxCount(m_eTeamType);

			if (iMineral >= 25)
			{
				if (iGas >= 75)
				{
					if (iHumunMax - iHumun >= 1)
					{
						if (m_UnitCreateList.size() < 5)
						{
							m_UnitCreateList.push_back(L"O");
							D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -25);
							D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -75);
						}

						m_eBuildCreateType = BUILD_CREATE_UNIT;
					}
				}
			}
		}

		if (iAICount == 470 && tAIFrame.fFrame == 0.f)
		{
			CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
			int iMineral = D_CAST(CInterpace, pInterpace)->GetWorldMineral(m_eTeamType);
			int iGas = D_CAST(CInterpace, pInterpace)->GetWorldGas(m_eTeamType);
			int iHumun = D_CAST(CInterpace, pInterpace)->GetHumanCount(m_eTeamType);
			int iHumunMax = D_CAST(CInterpace, pInterpace)->GetHumanMaxCount(m_eTeamType);

			if (iMineral >= 25)
			{
				if (iGas >= 75)
				{
					if (iHumunMax - iHumun >= 1)
					{
						if (m_UnitCreateList.size() < 5)
						{
							m_UnitCreateList.push_back(L"O");
							D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -25);
							D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -75);
						}

						m_eBuildCreateType = BUILD_CREATE_UNIT;
					}
				}
			}
		}
	}

	return 0;
}

void CRoboticsFacility::Render(void)
{
	D3DXMATRIX matScale, matTrans;

	if (m_bSelect)
	{
		CObjectRenderMgr::SelectBuildUIRender(this, 6, 0.f, 6, 40.f);
	}
	const TEX_INFO* pBuildtexture = TextureMgr->GetTexture(L"Shad"
		, L"Robotics Facility", 0);

	float fX = pBuildtexture->ImageInfo.Width / 2.f;
	float fY = pBuildtexture->ImageInfo.Height / 2.f;

	D3DXMatrixTranslation(&matTrans
		, m_tInfo.vPos.x - m_vScroll.x
		, m_tInfo.vPos.y - m_vScroll.y
		, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pBuildtexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(230, 255, 255, 255));

	pBuildtexture = TextureMgr->GetTexture(L"Building"
		, L"Robotics Facility", 0);

	D3DXMatrixTranslation(&matTrans
		, m_tInfo.vPos.x - m_vScroll.x
		, m_tInfo.vPos.y - m_vScroll.y
		, 0.f);

	fX = pBuildtexture->ImageInfo.Width / 2.f;
	fY = pBuildtexture->ImageInfo.Height / 2.f;

	Device->GetSprite()->SetTransform(&matTrans);
	if (m_eTeamType == TEAM_PLAYER)
	{
		Device->GetSprite()->Draw(pBuildtexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	else
	{
		Device->GetSprite()->Draw(pBuildtexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 160, 160));
	}
}

void CRoboticsFacility::Release(void)
{
}

void CRoboticsFacility::Function(void)
{
	if (!m_bSelect)
		return;

	DWORD dwKey = GET_KEY;

	CGameObject* ptMouse = ObjMgr->GetList()[OBJ_MOUSE].front();
	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
	D3DXVECTOR3 vMouse = D_CAST(CMouse, ptMouse)->GetRenderMousePos();
	int iMineral = D_CAST(CInterpace, pInterpace)->GetWorldMineral(m_eTeamType);
	int iGas = D_CAST(CInterpace, pInterpace)->GetWorldGas(m_eTeamType);
	int iHumun = D_CAST(CInterpace, pInterpace)->GetHumanCount(m_eTeamType);
	int iHumunMax = D_CAST(CInterpace, pInterpace)->GetHumanMaxCount(m_eTeamType);

	if (m_eBuildCreateType != BUILD_CREATE_NOMAL)
	{
		if (dwKey & KEY_LBUTTON && D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 8) && !m_bKeyInput)
		{
			m_bKeyInput = true;

			list<TCHAR*>::iterator iter = m_UnitCreateList.begin();

			if (!lstrcmp((*iter), L"S"))
			{
				D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 200);
			}
			else if (!lstrcmp((*iter), L"V"))
			{
				D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 200);
				D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 100);
			}
			else if (!lstrcmp((*iter), L"O"))
			{
				D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 25);
				D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 75);
			}


			if (m_UnitCreateList.size() > 1)
				m_UnitCreateList.pop_back();
			else
			{
				m_UnitCreateList.pop_back();
				m_eBuildCreateType = BUILD_CREATE_NOMAL;
				m_bCreateUnit = false;
			}
			return;
		}
		if (dwKey & KEY_X && !m_bKeyInput)
		{
			m_bKeyInput = true;

			list<TCHAR*>::iterator iter = m_UnitCreateList.begin();

			if (!lstrcmp((*iter), L"S"))
			{
				D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 200);
			}
			else if (!lstrcmp((*iter), L"V"))
			{
				D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 200);
				D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 100);
			}
			else if (!lstrcmp((*iter), L"O"))
			{
				D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 25);
				D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 75);
			}

			if (m_UnitCreateList.size() > 1)
				m_UnitCreateList.pop_back();
			else
			{
				m_UnitCreateList.pop_back();
				m_eBuildCreateType = BUILD_CREATE_NOMAL;
				m_bCreateUnit = false;
			}
			return;
		}
	}

	// ���콺 üũ
	if (dwKey & KEY_LBUTTON && !m_bKeyInput)
	{
		m_bKeyInput = true;
		// ù��° ĭ
		if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 0))
		{
			if (iMineral >= 200)
			{
				if (iHumunMax - iHumun >= 2)
				{
					if (m_UnitCreateList.size() < 5)
					{
						m_UnitCreateList.push_back(L"S");
						D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -200);
					}

					m_eBuildCreateType = BUILD_CREATE_UNIT;
				}
				else
				{
					//�α� �� ����
				}
			}
			else
			{
				//�̳׶� ����
			}
			return;
		}
		// �ι�° ĭ
		else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 1) &&
			m_iClickMyInterpace[1] == 0)
		{
			if (iMineral >= 200)
			{
				if (iGas >= 100)
				{
					if (iHumunMax - iHumun >= 4)
					{
						if (m_UnitCreateList.size() < 5)
						{
							m_UnitCreateList.push_back(L"V");
							D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -200);
							D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -100);
						}

						m_eBuildCreateType = BUILD_CREATE_UNIT;
					}
					else
					{
						//�α� �� ����
					}
				}
				else
				{
					// ���� ����
				}
			}
			else
			{
				//�̳׶� ����
			}
			return;
		}
		// ����° ĭ
		else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 2) &&
			m_iClickMyInterpace[2] == 0)
		{
			if (iMineral >= 25)
			{
				if (iGas >= 75)
				{
					if (iHumunMax - iHumun >= 1)
					{
						if (m_UnitCreateList.size() < 5)
						{
							m_UnitCreateList.push_back(L"O");
							D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -25);
							D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -75);
						}

						m_eBuildCreateType = BUILD_CREATE_UNIT;
					}
					else
					{
						//�α� �� ����
					}
				}
				else
				{
					// ���� ����
				}
			}
			else
			{
				//�̳׶� ����
			}
			return;
		}
		//������° ĭ
		else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 5))
		{
			D_CAST(CMouse, ptMouse)->SettingMouseRallyPoint(this);
			return;
		}
	}

	if (dwKey & KEY_S && !m_bKeyInput)
	{
		m_bKeyInput = true;

		if (iMineral >= 200)
		{
			if (iHumunMax - iHumun >= 2)
			{
				if (m_UnitCreateList.size() < 5)
				{
					m_UnitCreateList.push_back(L"S");
					D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -200);
				}

				m_eBuildCreateType = BUILD_CREATE_UNIT;
			}
			else
			{
				//�α� �� ����
			}
		}
		else
		{
			//�̳׶� ����
		}
		return;
	}
	if (dwKey & KEY_V && !m_bKeyInput&&
		m_iClickMyInterpace[1] == 0)
	{
		m_bKeyInput = true;

		if (iMineral >= 200)
		{
			if (iGas >= 100)
			{
				if (iHumunMax - iHumun >= 4)
				{
					if (m_UnitCreateList.size() < 5)
					{
						m_UnitCreateList.push_back(L"V");
						D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -200);
						D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -100);
					}

					m_eBuildCreateType = BUILD_CREATE_UNIT;
				}
				else
				{
					//�α� �� ����
				}
			}
			else
			{
				// ���� ����
			}
		}
		else
		{
			//�̳׶� ����
		}
	}

	if (dwKey & KEY_O && !m_bKeyInput&&
		m_iClickMyInterpace[2] == 0)
	{
		m_bKeyInput = true;

		if (iMineral >= 25)
		{
			if (iGas >= 75)
			{
				if (iHumunMax - iHumun >= 2)
				{
					if (m_UnitCreateList.size() < 5)
					{
						m_UnitCreateList.push_back(L"O");
						D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -25);
						D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -75);
					}

					m_eBuildCreateType = BUILD_CREATE_UNIT;
				}
				else
				{
					//�α� �� ����
				}
			}
			else
			{
				// ���� ����
			}
		}
		else
		{
			//�̳׶� ����
		}
		return;
	}
	if (dwKey & KEY_RBUTTON && !m_bKeyInput)
	{
		m_vStartingPoint = D_CAST(CMouse, ptMouse)->GetMousePos();
		ptMouse->SetState(MOUSE_CLICK);
	}
	if (dwKey & KEY_R && !m_bKeyInput)
	{
		m_bKeyInput = true;
		D_CAST(CMouse, ptMouse)->SettingMouseRallyPoint(this);
		return;
	}
	if (!dwKey)
	{
		m_tClick.fFrame += GET_TIME * m_tClick.fCount;

		if (m_tClick.fFrame > m_tClick.fMax)
			m_bKeyInput = false;
	}
}

void CRoboticsFacility::CheckOneClick(void)
{
	D3DXMATRIX matTrans;
	float fX = 0.f, fY = 0.f;
	const TEX_INFO* pInterpacetexture = NULL;

	list<CGameObject*> pBuildList = ObjMgr->GetList()[OBJ_BUILD];

	list < CGameObject*>::iterator iter = pBuildList.begin();
	list < CGameObject*>::iterator iter_end = pBuildList.end();

	m_iClickMyInterpace[1] = 2;
	m_iClickMyInterpace[2] = 2;

	for (; iter != iter_end; ++iter)
	{
		if ((*iter)->GetBuildType() == BUILD_ROBOTICS_SUPPORT_BAY &&
			(*iter)->GetBuildCreateType() != BUILD_CREATE_CREATE)
		{
			m_iClickMyInterpace[1] = 0;
		}

		if ((*iter)->GetBuildType() == BUILD_OBSERVATORY &&
			(*iter)->GetBuildCreateType() != BUILD_CREATE_CREATE)
		{
			m_iClickMyInterpace[2] = 0;
		}
	}

	if (m_eBuildCreateType != BUILD_CREATE_NOMAL)
	{
		// 9��° ĭ
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Cancel", m_iClickMyInterpace[8]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 595.f + fX
			, 440.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	// 1��° ĭ
	pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"Shuttle", m_iClickMyInterpace[0]);

	fX = pInterpacetexture->ImageInfo.Width / 2.f;
	fY = pInterpacetexture->ImageInfo.Height / 2.f;

	D3DXMatrixTranslation(&matTrans
		, 505.f + fX
		, 357.f + fY
		, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pInterpacetexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));

	// 2��° ĭ
	pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"Reaver", m_iClickMyInterpace[1]);

	fX = pInterpacetexture->ImageInfo.Width / 2.f;
	fY = pInterpacetexture->ImageInfo.Height / 2.f;

	D3DXMatrixTranslation(&matTrans
		, 550.f + fX
		, 357.f + fY
		, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pInterpacetexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));

	// 3��° ĭ
	pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"Observer", m_iClickMyInterpace[2]);

	fX = pInterpacetexture->ImageInfo.Width / 2.f;
	fY = pInterpacetexture->ImageInfo.Height / 2.f;

	D3DXMatrixTranslation(&matTrans
		, 595.f + fX
		, 357.f + fY
		, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pInterpacetexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));



	// 6��° ĭ
	pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"Rally", m_iClickMyInterpace[5]);

	fX = pInterpacetexture->ImageInfo.Width / 2.f;
	fY = pInterpacetexture->ImageInfo.Height / 2.f;

	D3DXMatrixTranslation(&matTrans
		, 595.f + fX
		, 397.f + fY
		, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pInterpacetexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CRoboticsFacility::CreateUnit(bool Check)
{
	if (Check) // ���� ���� �ð� ���
	{
		if (!m_bCreateUnit)
		{
			list<TCHAR*>::iterator iter = m_UnitCreateList.begin();

			if (iter == m_UnitCreateList.end())
			{
				m_eBuildCreateType = BUILD_CREATE_NOMAL;
				return;
			}

			TCHAR szBuf[5] = L"";
			lstrcpy(szBuf, (*iter));
			/*if (!lstrcmp(szBuf, L"S"))
			{
				m_tCreateTime = FRAME(0.f, 1.f, 25.f);
			}
			if (!lstrcmp(szBuf, L"V"))
			{
				m_tCreateTime = FRAME(0.f, 1.f, 43.75f);
			}
			if (!lstrcmp(szBuf, L"O"))
			{
				m_tCreateTime = FRAME(0.f, 1.f, 25.f);
			}*/

			m_tCreateTime = FRAME(0.f, 1.f, 12.5f);
			m_iFrameFirst = S_INT_CAST(m_tFrame.fFrame);
			m_bCreateUnit = true;
		}
		else
		{
			m_tCreateTime.fFrame += GET_TIME * m_tCreateTime.fCount;
			m_tFrame.fFrame += GET_TIME * m_tFrame.fCount;

			if (m_tFrame.fFrame > m_tFrame.fCount)
			{
				m_tFrame.fFrame = S_FLOAT_CAST(m_iFrameFirst);
			}

			if (m_tCreateTime.fFrame > m_tCreateTime.fMax)
			{
				m_bCreateUnit = false;
				m_tCreateTime.fFrame = m_tCreateTime.fMax;



				list<TCHAR*>::iterator iter = m_UnitCreateList.begin();

				if (iter == m_UnitCreateList.end())
					return;

				TCHAR szBuf[5] = L"";
				lstrcpy(szBuf, (*iter));
				if (!lstrcmp(szBuf, L"S"))
				{
					if (m_eTeamType == TEAM_PLAYER)
					{
						ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_PING), OBJ_EFFECT);
						SoundMgr->PlaySound(L"pshrdy00.wav", CHANNEL_MAIN, 0.9f);
					}
					if (m_vStartingPoint.x != NULL && m_vStartingPoint.y != NULL)
					{
						ObjMgr->AddGameObject(CFactory<CShuttle>::CreateGameObjectRallyAir
						(D3DXVECTOR3(m_tInfo.vPos.x - m_tInfo.vSize.x / 2.f, m_tInfo.vPos.y + m_tInfo.vSize.y / 2.f, 0.f)
							, m_vStartingPoint, UNIT_MOVE, m_eTeamType), OBJ_UNIT);
					}
					else
					{
						ObjMgr->AddGameObject(CFactory<CShuttle>::CreateGameObject
						(D3DXVECTOR3(m_tInfo.vPos.x - m_tInfo.vSize.x / 2.f, m_tInfo.vPos.y + m_tInfo.vSize.y / 2.f, 0.f), m_eTeamType), OBJ_UNIT);
					}
				}
				if (!lstrcmp(szBuf, L"V"))
				{
					if (m_eTeamType == TEAM_PLAYER)
					{
						ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_PING), OBJ_EFFECT);
						SoundMgr->PlaySound(L"ptrrdy00.wav", CHANNEL_MAIN, 0.9f);
					}
					if (m_vStartingPoint.x != NULL && m_vStartingPoint.y != NULL)
					{
						ObjMgr->AddGameObject(CFactory<CReaver>::CreateGameObjectRallyGround
						(D3DXVECTOR3(m_tInfo.vPos.x - m_tInfo.vSize.x / 2.f, m_tInfo.vPos.y + m_tInfo.vSize.y / 2.f, 0.f)
							, m_vStartingPoint, UNIT_MOVE, m_eTeamType), OBJ_UNIT);
					}
					else
					{
						ObjMgr->AddGameObject(CFactory<CReaver>::CreateGameObject
						(D3DXVECTOR3(m_tInfo.vPos.x - m_tInfo.vSize.x / 2.f, m_tInfo.vPos.y + m_tInfo.vSize.y / 2.f, 0.f), m_eTeamType), OBJ_UNIT);
					}
				}
				if (!lstrcmp(szBuf, L"O"))
				{
					if (m_eTeamType == TEAM_PLAYER)
					{
						ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_PING), OBJ_EFFECT);
						//SoundMgr->PlaySound(L"pwirdy00.wav", CHANNEL_MAIN, 0.9f);
					}
					if (m_vStartingPoint.x != NULL && m_vStartingPoint.y != NULL)
					{
						ObjMgr->AddGameObject(CFactory<CObserver>::CreateGameObjectRallyAir
						(D3DXVECTOR3(m_tInfo.vPos.x - m_tInfo.vSize.x / 2.f, m_tInfo.vPos.y + m_tInfo.vSize.y / 2.f, 0.f)
							, m_vStartingPoint, UNIT_MOVE, m_eTeamType), OBJ_UNIT);
					}
					else
					{
						ObjMgr->AddGameObject(CFactory<CObserver>::CreateGameObject
						(D3DXVECTOR3(m_tInfo.vPos.x - m_tInfo.vSize.x / 2.f, m_tInfo.vPos.y + m_tInfo.vSize.y / 2.f, 0.f), m_eTeamType), OBJ_UNIT);
					}
				}
				

				m_UnitCreateList.pop_front();
			}
		}
	}
	else	// �������̽� �����ִ°�
	{
		D3DXMATRIX matScale, matTrans;
		float fX = 0.f, fY = 0.f;
		const TEX_INFO* pBigWireTexture = TextureMgr->GetTexture(L"Produce"
			, L"Frame", 0);

		fX = pBigWireTexture->ImageInfo.Width / 2.f;
		fY = pBigWireTexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 230.f + fX
			, 395.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pBigWireTexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		pBigWireTexture = TextureMgr->GetTexture(L"Produce"
			, L"Bar", 0);

		fX = pBigWireTexture->ImageInfo.Width / 2.f;
		fY = pBigWireTexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, fX + 270.f
			, fY + 418.f
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pBigWireTexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		pBigWireTexture = TextureMgr->GetTexture(L"Produce"
			, L"Bar", 1);

		fX = pBigWireTexture->ImageInfo.Width / 2.f;
		fY = pBigWireTexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 270.f
			, 418.f
			, 0.f);


		float fPercent = m_tCreateTime.fFrame / m_tCreateTime.fMax;

		RECT rc = { NULL, NULL
			, S_LONG_CAST(fPercent * fX * 2), S_LONG_CAST(fY * 2) };

		if (fPercent > 1.f)
			fPercent = 1.f;

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pBigWireTexture->pTexture
			, &rc
			, NULL //&D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));


		list<TCHAR*>::iterator iter = m_UnitCreateList.begin();
		list<TCHAR*>::iterator iter_end = m_UnitCreateList.end();

		int iCount = 0;
		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter), L"S"))
			{
				pBigWireTexture = TextureMgr->GetTexture(L"Produce"
					, L"Icon", 9);
			}
			if (!lstrcmp((*iter), L"V"))
			{
				pBigWireTexture = TextureMgr->GetTexture(L"Produce"
					, L"Icon", 10);
			}
			if (!lstrcmp((*iter), L"O"))
			{
				pBigWireTexture = TextureMgr->GetTexture(L"Produce"
					, L"Icon", 11);
			}

			fX = pBigWireTexture->ImageInfo.Width / 2.f;
			fY = pBigWireTexture->ImageInfo.Height / 2.f;

			switch (iCount)
			{
			case 0:
				D3DXMatrixTranslation(&matTrans
					, 232.f
					, 397.f
					, 0.f);
				break;
			case 1:
				D3DXMatrixTranslation(&matTrans
					, 232.f
					, 436.f
					, 0.f);
				break;
			case 2:
				D3DXMatrixTranslation(&matTrans
					, 271.f
					, 436.f
					, 0.f);
				break;
			case 3:
				D3DXMatrixTranslation(&matTrans
					, 310.f
					, 436.f
					, 0.f);
				break;
			case 4:
				D3DXMatrixTranslation(&matTrans
					, 349.f
					, 436.f
					, 0.f);
				break;
			}

			Device->GetSprite()->SetTransform(&matTrans);
			Device->GetSprite()->Draw(pBigWireTexture->pTexture
				, NULL
				, NULL 
				, NULL
				, D3DCOLOR_ARGB(255, 255, 255, 255));

			++iCount;
		}
	}
}

void CRoboticsFacility::SettingTile(void)
{
	int iX = S_INT_CAST(m_tInfo.vPos.x / TILECX);
	int iY = S_INT_CAST(m_tInfo.vPos.y / TILECY);

	int iIndex = iX + (TILEEX * iY);

	if (iIndex < 0 || iIndex >= TILEEX * TILEEY)
		return;

	CGameObject* pObj = ObjMgr->GetList()[OBJ_BACKGROUND].front();

	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - 1);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex + 1);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX - 1);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX + 1);
}

