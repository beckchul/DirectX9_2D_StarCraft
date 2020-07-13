#include "stdafx.h"
#include "Probe.h"
#include "ObjMgr.h"
#include "TextureMgr.h"
#include "Device.h"
#include "AStar.h"
#include "KeyMgr.h"
#include "Mouse.h"
#include "BackGround.h"
#include "TimeMgr.h"
#include "Factory.h"
#include "MiniResource.h"
#include "MapResource.h"
#include "DistanceMgr.h"
#include "Effect.h"
#include "Interpace.h"
#include "CreateBuild.h"
#include "Assimilator.h"
#include "CollisionMgr.h"
#include "Fog.h"

CProbe::CProbe()
{
}

CProbe::~CProbe()
{
	Release();
}

HRESULT CProbe::Initialize(void)
{
	list<UNIT*> UnitList = ObjMgr->GetUnitList();

	list<UNIT*>::iterator iter = UnitList.begin();
	list<UNIT*>::iterator iter_end = UnitList.end();

	lstrcpy(m_tInfo.szObjKey, L"Probe");
	lstrcpy(m_tInfo.szStateKey, L"Normal");

	for (; iter != iter_end; ++iter)
	{
		if (!lstrcmp((*iter)->szName, m_tInfo.szObjKey))
		{
			m_tUnitInfo = (*(*iter));
			break;
		}
	}

	m_tUnitInfo.eType = UNIT_GROUND;
	m_bGas2 = false;
	m_bGas = false;
	m_bCreateBuild = false;
	m_pAStar = new CAStar;
	m_fSpeed = 140.f;
	m_bCollision = true;
	m_tInfo.vSize = D3DXVECTOR3(16.f, 16.f, 0.f);
	m_tInfo.vLook = D3DXVECTOR3(1.f, 0.f, 0.f);
	m_tInfo.vDir = D3DXVECTOR3(1.f, 0.f, 0.f);
	m_tFrame = FRAME(0.f, 5.f, 1.f);
	m_dwState = UNIT_NORMAL;
	m_fGetResourceCount = 0;
	m_iGetResource = 0;
	m_eRenderType = RENDER_GROUND;
	m_pTargetNexus = NULL;
	m_bGetResource = false;
	m_bMouseLClick = false;
	m_tMouseLClick = FRAME(0.f, 10.f, 2.f);
	m_tStateCount = FRAME(0.f, 24.f, 24.f);
	m_iStateCount = 0;
	m_iView = 9;
	return S_OK;
}

int CProbe::Update(void)
{
	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
	switch (D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_GROUND_ARMOR))
	{
	case 1:
		m_iArmor = 1;
		break;
	case 2:
		m_iArmor = 2;
		break;
	case 3:
		m_iArmor = 3;
		break;
	}

	switch (D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_SHILD))
	{
	case 1:
		m_iShield = 1;
		break;
	case 2:
		m_iShield = 2;
		break;
	case 3:
		m_iShield = 3;
		break;
	}

	if (m_dwState == UNIT_DEAD)
		m_bDeadCheck = true;
	if (m_bDeadCheck)
	{
		SoundMgr->PlaySound(L"pprdth00.wav", CHANNEL_EFFECT_DEAD, 0.5f, m_tInfo.vPos);
		D_CAST(CInterpace, pInterpace)->EraseUnitList(this);
		ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_PROBE_DEAD), OBJ_EFFECT);
		return 1;
	}

	AI();
	Cloocking();
	TechnologyTree();
	SelectKeyCheck();
	AStarMove();
	DigUpResource();
	Frame();
	CreateBuild();

	list<CGameObject*> pBuildList = ObjMgr->GetList()[OBJ_BUILD];

	list<CGameObject*>::iterator iter = pBuildList.begin();
	list<CGameObject*>::iterator iter_end = pBuildList.end();

	for (; iter != iter_end; ++iter)
	{
		if ((*iter)->GetBuildCreateType() == BUILD_CREATE_CREATE)
			CCollisionMgr::CollisionCreateBuildRectEX(this, (*iter));
	}

	return 0;
}

void CProbe::Render(void)
{
	if (m_eTeamType != TEAM_PLAYER)
	{
		CGameObject* pFog = ObjMgr->GetList()[OBJ_FOG].front();

		vector<PTILE> pTIie = D_CAST(CFog, pFog)->GetTile();

		int iX = S_INT_CAST(m_tInfo.vPos.x / TILECX);
		int iY = S_INT_CAST(m_tInfo.vPos.y / TILECY);

		int iIndex = iX + (TILEEX * iY);

		if (iIndex < 0 || iIndex >= TILEEX * TILEEY)
			return;

		CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();

		if (pTIie[iIndex]->byOption != 2 && !D_CAST(CInterpace, pInterpace)->GetMapLook())
			return;
	}

	if (m_bGas)
		return;
	D3DXMATRIX matScale, matTrans;

	if (m_bSelect)
	{
		CObjectRenderMgr::SelectUnitUIRender(this, 0, 4.f, 0, 20.f);
	}

	const TEX_INFO* pProbetexture = TextureMgr->GetTexture(m_tInfo.szObjKey
		, m_tInfo.szStateKey, S_INT_CAST(m_tFrame.fFrame));

	D3DXMatrixScaling(&matScale, m_tInfo.vLook.x, 1.f, 0.f);
	D3DXMatrixTranslation(&matTrans
		, m_tInfo.vPos.x - m_vScroll.x
		, m_tInfo.vPos.y - m_vScroll.y
		, 0.f);

	matScale *= matTrans;

	float fX = pProbetexture->ImageInfo.Width / 2.f;
	float fY = pProbetexture->ImageInfo.Height / 2.f;


	if (m_eTeamType == TEAM_PLAYER)
	{
		Device->GetSprite()->SetTransform(&matScale);
		Device->GetSprite()->Draw(pProbetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame), 255, 255, 255));
	}
	else if (m_eTeamType == TEAM_ENEMY)
	{
		if (m_eAlphType == ALPH_NOMAL_CLOOCKING && m_dwUnitState != UNIT_MAELSTORM)
		{
			Device->GetSprite()->SetTransform(&matScale);
			Device->GetSprite()->Draw(pProbetexture->pTexture
				, NULL
				, &D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame - 85), 255, 160, 160));
		}
		else
		{
			Device->GetSprite()->SetTransform(&matScale);
			Device->GetSprite()->Draw(pProbetexture->pTexture
				, NULL
				, &D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame), 255, 160, 160));
		}
	}

	if (m_dwUnitState == UNIT_MAELSTORM && m_dwState != UNIT_DEAD)
	{
		const TEX_INFO* pProbetexture = TextureMgr->GetTexture(L"Effect"
			, L"Maelstorm", S_INT_CAST(m_tStateCount.fFrame) + m_iStateCount);

		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y
			, 0.f);

		fX = pProbetexture->ImageInfo.Width / 2.f;
		fY = pProbetexture->ImageInfo.Height / 2.f;

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pProbetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(180, 255, 160, 160));
	}

	if (m_dwUnitState == UNIT_STASISFIELD && m_dwState != UNIT_DEAD)
	{
		const TEX_INFO* pProbetexture = TextureMgr->GetTexture(L"Effect"
			, L"Frozen", S_INT_CAST(m_tStateCount.fFrame) + m_iStasis);

		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y
			, 0.f);

		fX = pProbetexture->ImageInfo.Width / 2.f;
		fY = pProbetexture->ImageInfo.Height / 2.f;

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pProbetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(80, 255, 255, 255));
	}

}

void CProbe::Release(void)
{
}

void CProbe::CreateBuild(void)
{
	if (m_bGas)
		return;

	list<int>* pBestList = m_pAStar->GetBestList();

	if (pBestList->empty() && m_bCreateBuild)
	{
		m_bCreateBuild = false;
		m_eBuildType = BUILD_END;
		m_bAI = false;
		m_bAI2 = false;

		for (int i = 0; i < 9; ++i)
		{
			m_iClickMyInterpace[i] = 0;
		}
		m_vBuildPos.x += m_tBuildInfo.iSizeX * TILECX / 2.f - TILECX / 2.f;
		m_vBuildPos.y += m_tBuildInfo.iSizeY * TILECY / 2.f - TILECY / 2.f;

		CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();

		D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -m_tBuildInfo.iMineral);
		D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -m_tBuildInfo.iGas);

		SoundMgr->PlaySound(L"pshbld00.wav", CHANNEL_EFFECT, 0.5f, m_tInfo.vPos);
		ObjMgr->AddGameObject(CFactory<CCreateBuild>::CreateGameBuildObject(m_vBuildPos, m_tBuildInfo, m_eTeamType), OBJ_BUILD);
	}
}

void CProbe::Cloocking(void)
{
	list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];
	list<CGameObject*> pBuildList = ObjMgr->GetList()[OBJ_BUILD];
	CDistanceMgr::CheckNoamlCloking(this, pUnitList);
	CDistanceMgr::CheckCloking(this, pUnitList, pBuildList);

	if ((m_eAlphType == ALPH_NOMAL_CLOOCKING || m_eAlphType == LOOK_ALPH_NOMAL_CLOOCKING) && m_tAlphCount.fFrame < m_tAlphCount.fMax)
	{
		m_tAlphCount.fFrame += m_tAlphCount.fCount * GET_TIME;

		if (m_tAlphCount.fFrame >= m_tAlphCount.fMax)
		{
			m_tAlphCount.fFrame = m_tAlphCount.fMax;
		}
	}
	else if (m_eAlphType == ALPH_NOMAL && m_tAlphCount.fFrame > 0)
	{
		m_tAlphCount.fFrame -= m_tAlphCount.fCount * GET_TIME;

		if (m_tAlphCount.fFrame <= 0)
		{
			m_tAlphCount.fFrame = 0;
		}
	}
}

void CProbe::SelectKeyCheck(void)
{
	if (m_eTeamType != TEAM_PLAYER)
		return;

	if (m_dwUnitState == UNIT_MAELSTORM || m_dwUnitState == UNIT_STASISFIELD)
	{
		return;
	}
	if (!m_bSelect)
	{
		if (m_eBuildType == BUILD_GROUND ||
			m_eBuildType == BUILD_ARR)
			m_eBuildType = BUILD_END;
		return;
	}

	if (m_bGas)
		return;


	DWORD dwKey = GET_KEY;


	if (dwKey & KEY_RBUTTON && m_bMouseLClick)
	{
		m_bMouseLClick = false;
		if (m_eBuildType != BUILD_END &&
			m_eBuildType != BUILD_GROUND &&
			m_eBuildType != BUILD_ARR)
		{
			m_eBuildType = BUILD_END;
			m_bSelect = false;
			CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
			D_CAST(CInterpace, pInterpace)->NotPortaraits();
			return;
		}
		CGameObject* ptMouse = ObjMgr->GetList()[OBJ_MOUSE].front();
		D3DXVECTOR3 vMouse = D_CAST(CMouse, ptMouse)->GetMousePos();

		// 마우스 인터페이스 클릭 방지
		if (vMouse.x - m_vScroll.x >= 490 && vMouse.y - m_vScroll.y >= 340 &&
			vMouse.x - m_vScroll.x <= 640 && vMouse.y - m_vScroll.y <= 480)
			return;

		if (vMouse.x - m_vScroll.x >= 150 && vMouse.y - m_vScroll.y >= 380 &&
			vMouse.x - m_vScroll.x <= 490 && vMouse.y - m_vScroll.y <= 480)
			return;

		if (vMouse.x - m_vScroll.x >= 0 && vMouse.y - m_vScroll.y >= 320 &&
			vMouse.x - m_vScroll.x <= 150 && vMouse.y - m_vScroll.y <= 480)
			return;

		/////////

		m_bCreateBuild = false;

		if (ptMouse->GetState() != MOUSE_OBJECT)
			ptMouse->SetState(MOUSE_CLICK);

		int iX = S_INT_CAST(vMouse.x / TILECX);
		int iY = S_INT_CAST(vMouse.y / TILECY);

		int iIndex = iX + (TILEEX * iY);

		if (iIndex < 0 || iIndex >= TILEEX * TILEEY)
			return;

		list<CGameObject*> pList = ObjMgr->GetList()[OBJ_RESOURCE];

		list<CGameObject*>::iterator iter = pList.begin();
		list<CGameObject*>::iterator iter_end = pList.end();

		for (; iter != iter_end; ++iter)
		{
			if (D_CAST(CMapResource, (*iter))->GetResourceType() == RESOURCE_GAS)
				continue;

			float fL = (*iter)->GetInfo().vPos.x - ((*iter)->GetInfo().vSize.x / 2.f);
			float fU = (*iter)->GetInfo().vPos.y - ((*iter)->GetInfo().vSize.y / 2.f);
			float fR = (*iter)->GetInfo().vPos.x + ((*iter)->GetInfo().vSize.x / 2.f);
			float fD = (*iter)->GetInfo().vPos.y + ((*iter)->GetInfo().vSize.y / 2.f);

			if (vMouse.x >= fL &&
				vMouse.x <= fR &&
				vMouse.y >= fU &&
				vMouse.y <= fD)
			{
				m_bCollision = false;
				m_dwState = UNIT_DIGUP_RESOURCE_M;
				m_pTarget = (*iter);
				m_pTarget->CleckMineral();
				m_bGas2 = false;

				if (m_bGetResource)
				{
					m_dwState = UNIT_GIVE_NEXUS;
					m_iGetResource = 2;
				}
				break;
			}
			else
			{
				m_bCollision = true;
				m_dwState = UNIT_NORMAL;
				m_pTarget = NULL;
			}
		}

		pList = ObjMgr->GetList()[OBJ_BUILD];

		iter = pList.begin();
		iter_end = pList.end();

		for (; iter != iter_end; ++iter)
		{

			float fL = (*iter)->GetInfo().vPos.x - ((*iter)->GetInfo().vSize.x / 2.f);
			float fU = (*iter)->GetInfo().vPos.y - ((*iter)->GetInfo().vSize.y / 2.f);
			float fR = (*iter)->GetInfo().vPos.x + ((*iter)->GetInfo().vSize.x / 2.f);
			float fD = (*iter)->GetInfo().vPos.y + ((*iter)->GetInfo().vSize.y / 2.f);

			if (vMouse.x < fL ||
				vMouse.x > fR ||
				vMouse.y < fU ||
				vMouse.y > fD)
				continue;

			if ((*iter)->GetBuildType() == BUILD_NEXUS)
			{
				if (m_eTeamType == (*iter)->GetTeamType())
				{
					m_fGetResourceCount = 0.f;
					m_iGetResource = 3;
					m_pTargetNexus = (*iter);
					m_dwState = UNIT_GIVE_NEXUS;
					m_bCollision = false;
					return;
				}
			}

			if ((*iter)->GetBuildType() == BUILD_ASSIMILATOR)
			{
				if (m_eTeamType == (*iter)->GetTeamType())
				{

					m_dwState = UNIT_DIGUP_RESOURCE_G;
					m_pTarget = (*iter);
					m_iGetResource = 0;
					m_bCollision = false;

					if (m_bGetResource)
					{
						m_dwState = UNIT_GIVE_NEXUS;
						m_iGetResource = 2;
					}
					break;
				}
			}
		}

		if (m_dwState == UNIT_GIVE_NEXUS ||
			m_dwState == UNIT_DIGUP_RESOURCE_M ||
			m_dwState == UNIT_DIGUP_RESOURCE_G)
			m_pAStar->StartAStarPos(m_tInfo.vPos, vMouse);
		else
		{
			m_fGetResourceCount = 0.f;
			m_pAStar->StartAStarPos(m_tInfo.vPos, vMouse);
			m_pTarget = NULL;
			m_pTargetNexus = NULL;
		}
	}

	if (dwKey & KEY_LBUTTON && m_bMouseLClick)			// 인터페이스 클릭용
	{
		bool bCheckBuild = false;
		m_bMouseLClick = false;
		m_tMouseLClick.fFrame = 0.f;
		CGameObject* ptMouse = ObjMgr->GetList()[OBJ_MOUSE].front();
		D3DXVECTOR3 vMouse = D_CAST(CMouse, ptMouse)->GetRenderMousePos();
		CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();

		if (m_eBuildType != BUILD_END &&
			m_eBuildType != BUILD_GROUND &&
			m_eBuildType != BUILD_ARR)
		{
			if (D_CAST(CMouse, ptMouse)->GetBuildSave())	// 타일 조건에 따라 건물을 지을 수 있을 때
			{
				m_vBuildPos = D_CAST(CMouse, ptMouse)->GetBuildPos();
				m_bCreateBuild = true;
				D_CAST(CMouse, ptMouse)->SetBuildClick(false);
				m_pAStar->StartAStarPos(m_tInfo.vPos, vMouse + m_vScroll);
				m_eBuildType = BUILD_END;
				return;
			}
			else						// 타일 조건에 맞지 않아 건물을 지을 수 없을 때
			{
				SoundMgr->PlaySound(L"pprerr00.wav", CHANNEL_MAIN, 0.5f);
			}
		}

		if (m_eBuildType == BUILD_END)
		{
			if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 1))
			{
				m_fGetResourceCount = 0.f;
				m_bCollision = true;
				m_dwState = UNIT_NORMAL;
				m_pTarget = NULL;
				m_pTargetNexus = NULL;
				m_pAStar->AStarClear();
				return;
			}
			// 3번째 칸 공격


			// 7번째 칸 지상건물
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 6) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				m_eBuildType = BUILD_GROUND;
				for (int i = 0; i < 9; ++i)
					m_iClickMyInterpace[i] = 0;
				return;
			}
			// 8번째 칸 공중건물
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 7) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				m_eBuildType = BUILD_ARR;
				for (int i = 0; i < 9; ++i)
					m_iClickMyInterpace[i] = 0;
				return;
			}
		}
		else if (m_eBuildType == BUILD_GROUND)
		{
			// 첫번째 칸 넥서스
			if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 0) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				bCheckBuild = CheckBuild(0);

				if (!bCheckBuild)
					return;

				m_eBuildType = BUILD_NEXUS;
				return;
			}
			// 두번째 칸 파일런
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 1) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				bCheckBuild = CheckBuild(1);

				if (!bCheckBuild)
					return;

				m_eBuildType = BUILD_PYLON;
				return;
			}
			// 세번째 칸 가스통
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 2) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				bCheckBuild = CheckBuild(2);

				if (!bCheckBuild)
					return;

				m_eBuildType = BUILD_ASSIMILATOR;
				return;
			}
			// 네번째 칸 게이트웨이
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 3) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				bCheckBuild = CheckBuild(3);

				if (!bCheckBuild)
					return;

				m_eBuildType = BUILD_GATEWAY;
				return;
			}
			// 다섯번째 칸 포지
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 4) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				bCheckBuild = CheckBuild(4);

				if (!bCheckBuild)
					return;
				m_eBuildType = BUILD_FORGE;
				return;
			}
			// 여섯번째 칸 포톤 캐논
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 5) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				bCheckBuild = CheckBuild(5);

				if (!bCheckBuild)
					return;

				m_eBuildType = BUILD_PHOTON_CANNON;
				return;
			}
			// 7번째 칸 사이버네틱스 코어
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 6) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				bCheckBuild = CheckBuild(6);

				if (!bCheckBuild)
					return;
				m_eBuildType = BUILD_CYBERNETICS_CORE;
				return;
			}
			// 8번째 칸 베터리
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 7) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				bCheckBuild = CheckBuild(7);

				if (!bCheckBuild)
					return;
				m_eBuildType = BUILD_SHIELD_BATTERY;
				return;
			}
			//9번째 취소 칸
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 8) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				m_eBuildType = BUILD_END;
				for (int i = 0; i < 9; ++i)
					m_iClickMyInterpace[i] = 0;
				return;
			}
			return;
		}
		else if (m_eBuildType == BUILD_ARR)
		{
			// 첫번째 칸 로보틱스
			if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 0) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				bCheckBuild = CheckBuild(0);

				if (!bCheckBuild)
					return;
				m_eBuildType = BUILD_ROBOTICS_FACILITY;
				return;
			}
			// 두번째 칸 스타게이트
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 1) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				bCheckBuild = CheckBuild(1);

				if (!bCheckBuild)
					return;
				m_eBuildType = BUILD_STARGATE;
				return;
			}
			// 세번째 칸 아둔의 성지
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 2) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				bCheckBuild = CheckBuild(2);

				if (!bCheckBuild)
					return;
				m_eBuildType = BUILD_CITADEL_OF_ADUN;
				return;
			}
			// 네번째 칸 로보틱스 서포터베이
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 3) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				bCheckBuild = CheckBuild(3);

				if (!bCheckBuild)
					return;
				m_eBuildType = BUILD_ROBOTICS_SUPPORT_BAY;
				return;
			}
			// 다섯번째 칸 플릿 비콘
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 4) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				bCheckBuild = CheckBuild(4);

				if (!bCheckBuild)
					return;
				m_eBuildType = BUILD_FLEET_BEACON;
				return;
			}
			// 여섯번째 칸 템플러 아카데미
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 5) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				bCheckBuild = CheckBuild(5);

				if (!bCheckBuild)
					return;
				m_eBuildType = BUILD_TEMPAR_ARCHIVES;
				return;
			}
			// 7번째 칸 옵저버토리
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 6) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				bCheckBuild = CheckBuild(6);

				if (!bCheckBuild)
					return;
				m_eBuildType = BUILD_OBSERVATORY;
				return;
			}
			// 8번째 칸 아비터 트리뷰널
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 7) &&
				D_CAST(CInterpace, pInterpace)->GetOneClick())
			{
				bCheckBuild = CheckBuild(7);

				if (!bCheckBuild)
					return;
				m_eBuildType = BUILD_ARBITER_TRIBUNAL;
				return;
			}
			//9번째 취소 칸
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 8))
			{
				m_eBuildType = BUILD_END;
				for (int i = 0; i < 9; ++i)
					m_iClickMyInterpace[i] = 0;
				return;
			}

			return;
		}
		else
		{
			//9번째 취소 칸
			if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 8))
			{
				m_eBuildType = BUILD_END;
				for (int i = 0; i < 9; ++i)
					m_iClickMyInterpace[i] = 0;
				return;
			}
			return;
		}
	}

	if (!dwKey)
	{
		m_tMouseLClick.fFrame += GET_TIME * m_tMouseLClick.fCount;

		if (m_tMouseLClick.fFrame > m_tMouseLClick.fMax)
			m_bMouseLClick = true;
	}

	if (m_eBuildType == BUILD_END && m_bMouseLClick)
	{
		if (dwKey & KEY_B)
		{
			m_eBuildType = BUILD_GROUND;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_bMouseLClick = false;
			return;
		}
		if (dwKey & KEY_V)
		{
			m_eBuildType = BUILD_ARR;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_bMouseLClick = false;
			return;
		}
		if (dwKey & KEY_S)
		{
			m_fGetResourceCount = 0.f;
			m_bCollision = true;
			m_dwState = UNIT_NORMAL;
			m_pTarget = NULL;
			m_pTargetNexus = NULL;
			m_pAStar->AStarClear();
			m_bMouseLClick = false;
			return;
		}
	}
	else if (m_eBuildType == BUILD_GROUND && m_bMouseLClick)
	{
		bool bCheckBuild = false;

		if (dwKey & KEY_N)
		{
			bCheckBuild = CheckBuild(0);

			if (!bCheckBuild)
				return;

			m_eBuildType = BUILD_NEXUS;
			m_bMouseLClick = false;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			return;
		}
		if (dwKey & KEY_P)
		{
			bCheckBuild = CheckBuild(1);

			if (!bCheckBuild)
				return;

			m_eBuildType = BUILD_PYLON;
			m_bMouseLClick = false;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			return;
		}
		if (dwKey & KEY_A)
		{
			bCheckBuild = CheckBuild(2);

			if (!bCheckBuild)
				return;

			m_eBuildType = BUILD_ASSIMILATOR;
			m_bMouseLClick = false;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			return;
		}
		if (dwKey & KEY_G)
		{
			bCheckBuild = CheckBuild(3);

			if (!bCheckBuild)
				return;

			m_eBuildType = BUILD_GATEWAY;
			m_bMouseLClick = false;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			return;
		}
		if (dwKey & KEY_F)
		{
			bCheckBuild = CheckBuild(4);

			if (!bCheckBuild)
				return;

			m_eBuildType = BUILD_FORGE;

			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_bMouseLClick = false;
			return;
		}
		if (dwKey & KEY_C)
		{
			bCheckBuild = CheckBuild(5);

			if (!bCheckBuild)
				return;

			m_eBuildType = BUILD_PHOTON_CANNON;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_bMouseLClick = false;
			return;
		}
		if (dwKey & KEY_Y)
		{
			bCheckBuild = CheckBuild(6);

			if (!bCheckBuild)
				return;
			m_eBuildType = BUILD_CYBERNETICS_CORE;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_bMouseLClick = false;
			return;
		}
		if (dwKey & KEY_B)
		{
			bCheckBuild = CheckBuild(7);

			if (!bCheckBuild)
				return;
			m_eBuildType = BUILD_SHIELD_BATTERY;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_bMouseLClick = false;
			return;
		}
		if (dwKey & KEY_X)
		{
			m_eBuildType = BUILD_END;
			m_bMouseLClick = false;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			return;
		}
	}
	else if (m_eBuildType == BUILD_ARR && m_bMouseLClick)
	{
		bool bCheckBuild = false;

		if (dwKey & KEY_R)
		{
			bCheckBuild = CheckBuild(0);

			if (!bCheckBuild)
				return;
			m_eBuildType = BUILD_ROBOTICS_FACILITY;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			return;
		}
		if (dwKey & KEY_S)
		{
			bCheckBuild = CheckBuild(1);

			if (!bCheckBuild)
				return;
			m_eBuildType = BUILD_STARGATE;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			return;
		}
		if (dwKey & KEY_C)
		{
			bCheckBuild = CheckBuild(2);

			if (!bCheckBuild)
				return;
			m_eBuildType = BUILD_CITADEL_OF_ADUN;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			return;
		}
		if (dwKey & KEY_B)
		{
			bCheckBuild = CheckBuild(3);

			if (!bCheckBuild)
				return;
			m_eBuildType = BUILD_ROBOTICS_SUPPORT_BAY;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			return;
		}
		if (dwKey & KEY_F)
		{
			bCheckBuild = CheckBuild(4);

			if (!bCheckBuild)
				return;
			m_eBuildType = BUILD_FLEET_BEACON;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			return;
		}
		if (dwKey & KEY_T)
		{
			bCheckBuild = CheckBuild(5);

			if (!bCheckBuild)
				return;
			m_eBuildType = BUILD_TEMPAR_ARCHIVES;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			return;
		}
		if (dwKey & KEY_O)
		{
			bCheckBuild = CheckBuild(6);

			if (!bCheckBuild)
				return;
			m_eBuildType = BUILD_OBSERVATORY;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			return;
		}
		if (dwKey & KEY_A)
		{
			bCheckBuild = CheckBuild(7);

			if (!bCheckBuild)
				return;
			m_eBuildType = BUILD_ARBITER_TRIBUNAL;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			return;
		}
		if (dwKey & KEY_X)
		{
			m_eBuildType = BUILD_END;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			return;
		}
	}
	else if (m_bMouseLClick)
	{
		if (dwKey & KEY_X)
		{
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_eBuildType = BUILD_END;
			return;
		}
	}
}

void CProbe::TechnologyTree(void)
{
	if (!m_bSelect)
		return;

	list<CGameObject*> pBuildList = ObjMgr->GetList()[OBJ_BUILD];

	if (m_eBuildType == BUILD_GROUND)				//지상건물 버튼 클릭시
	{
		for (int i = 3; i < 8; ++i)
			m_iClickMyInterpace[i] = 2;
		list < CGameObject*>::iterator iter = pBuildList.begin();
		list < CGameObject*>::iterator iter_end = pBuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if ((*iter)->GetBuildCreateType() == BUILD_CREATE_CREATE ||
				(*iter)->GetTeamType() != m_eTeamType)
				continue;

			if ((*iter)->GetBuildType() == BUILD_NEXUS)
			{
				m_iClickMyInterpace[3] = 0;
				m_iClickMyInterpace[4] = 0;
			}
			else if ((*iter)->GetBuildType() == BUILD_GATEWAY)
			{
				m_iClickMyInterpace[6] = 0;
				m_iClickMyInterpace[7] = 0;
			}
			else if ((*iter)->GetBuildType() == BUILD_FORGE)
			{
				m_iClickMyInterpace[5] = 0;
			}
		}
	}
	else if (m_eBuildType == BUILD_ARR)
	{
		bool bCheck = false, bCheck2 = false;
		for (int i = 0; i < 8; ++i)
			m_iClickMyInterpace[i] = 2;
		list < CGameObject*>::iterator iter = pBuildList.begin();
		list < CGameObject*>::iterator iter_end = pBuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if ((*iter)->GetBuildCreateType() == BUILD_CREATE_CREATE ||
				(*iter)->GetTeamType() != m_eTeamType)
				continue;

			if ((*iter)->GetBuildType() == BUILD_CYBERNETICS_CORE)
			{
				m_iClickMyInterpace[0] = 0;
				m_iClickMyInterpace[1] = 0;
				m_iClickMyInterpace[2] = 0;
			}
			else if ((*iter)->GetBuildType() == BUILD_ROBOTICS_FACILITY)
			{
				m_iClickMyInterpace[3] = 0;
				m_iClickMyInterpace[6] = 0;
			}
			else if ((*iter)->GetBuildType() == BUILD_STARGATE)
			{
				m_iClickMyInterpace[4] = 0;
				bCheck = true;
			}
			else if ((*iter)->GetBuildType() == BUILD_CITADEL_OF_ADUN)
			{
				m_iClickMyInterpace[5] = 0;
			}
			else if ((*iter)->GetBuildType() == BUILD_TEMPAR_ARCHIVES)
			{
				bCheck2 = true;
			}

			if (bCheck && bCheck2)
			{
				m_iClickMyInterpace[7] = 0;
			}
		}
	}
}

bool CProbe::CheckBuild(int iCount)
{
	if (m_iClickMyInterpace[iCount] == 2)
		return false;

	if (m_iClickMyInterpace[iCount] == 0)	//자원조건 추가 필요
	{
		CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();

		int iMineral = D_CAST(CInterpace, pInterpace)->GetWorldMineral(m_eTeamType);
		int iGas = D_CAST(CInterpace, pInterpace)->GetWorldGas(m_eTeamType);

		if (m_eBuildType == BUILD_GROUND)
		{
			switch (iCount)
			{
			case 0:
				lstrcpy(m_szBuildKey, L"Nexus");
				break;
			case 1:
				lstrcpy(m_szBuildKey, L"Pylon");
				break;
			case 2:
				lstrcpy(m_szBuildKey, L"Assimilator");
				break;
			case 3:
				lstrcpy(m_szBuildKey, L"GateWay");
				break;
			case 4:
				lstrcpy(m_szBuildKey, L"Forge");
				break;
			case 5:
				lstrcpy(m_szBuildKey, L"PhotonCannon");
				break;
			case 6:
				lstrcpy(m_szBuildKey, L"CyberneticsCore");
				break;
			case 7:
				lstrcpy(m_szBuildKey, L"ShieldBattery");
				break;
			}
		}
		else if (m_eBuildType == BUILD_ARR)
		{
			switch (iCount)
			{
			case 0:
				lstrcpy(m_szBuildKey, L"RoboticsFacility");
				break;
			case 1:
				lstrcpy(m_szBuildKey, L"StarGate");
				break;
			case 2:
				lstrcpy(m_szBuildKey, L"CitadelofAdun");
				break;
			case 3:
				lstrcpy(m_szBuildKey, L"RoboticsSupportBay");
				break;
			case 4:
				lstrcpy(m_szBuildKey, L"FleetBeacon");
				break;
			case 5:
				lstrcpy(m_szBuildKey, L"TemplarArchives");
				break;
			case 6:
				lstrcpy(m_szBuildKey, L"Observatory");
				break;
			case 7:
				lstrcpy(m_szBuildKey, L"ArbiterTribunal");
				break;
			}
		}

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		if (m_tBuildInfo.iMineral > iMineral)
		{
			SoundMgr->PlaySound(L"PAdErr00.wav", CHANNEL_UI, 0.5f);
			return false;
		}
		else if (m_tBuildInfo.iGas > iGas)
		{
			SoundMgr->PlaySound(L"PAdErr01.wav", CHANNEL_UI, 0.5f);
			return false;
		}
		else
		{
			return true;
		}
	}
	return false;
}

void CProbe::CheckOneClick(void) //인터페이스 아이콘
{
	if (m_eTeamType != TEAM_PLAYER)
		return;

	if (m_dwUnitState == UNIT_MAELSTORM || m_dwUnitState == UNIT_STASISFIELD)
	{
		return;
	}
	if (!m_bSelect)
		return;

	if (m_eBuildType == BUILD_END)				// 기본 상태 시
	{
		D3DXMATRIX matTrans;

		// 1번째 칸
		const TEX_INFO* pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Move", m_iClickMyInterpace[0]);

		float fX = pInterpacetexture->ImageInfo.Width / 2.f;
		float fY = pInterpacetexture->ImageInfo.Height / 2.f;

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

		// 2번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Stop", m_iClickMyInterpace[1]);

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

		// 3번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Att", m_iClickMyInterpace[2]);

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

		// 5번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Mining", m_iClickMyInterpace[4]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 550.f + fX
			, 397.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));


		// 7번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"LBuild", m_iClickMyInterpace[6]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 505.f + fX
			, 440.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		// 8번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"HBuild", m_iClickMyInterpace[7]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 550.f + fX
			, 440.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	else if (m_eBuildType == BUILD_GROUND)				//지상건물 버튼 클릭시
	{
		D3DXMATRIX matTrans;

		// 1번째 칸
		const TEX_INFO* pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Nexus", m_iClickMyInterpace[0]);

		float fX = pInterpacetexture->ImageInfo.Width / 2.f;
		float fY = pInterpacetexture->ImageInfo.Height / 2.f;

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

		// 2번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Pylon", m_iClickMyInterpace[1]);

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

		// 3번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Assimilator", m_iClickMyInterpace[2]);

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

		// 4번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Gateway", m_iClickMyInterpace[3]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 505.f + fX
			, 397.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		// 5번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Forge", m_iClickMyInterpace[4]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 550.f + fX
			, 397.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		// 6번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Photon Cannon", m_iClickMyInterpace[5]);

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

		// 7번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Cybernetics Core", m_iClickMyInterpace[6]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 505.f + fX
			, 440.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		// 8번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Shield Battery", m_iClickMyInterpace[7]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 550.f + fX
			, 440.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		// 9번째 칸
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
	else if (m_eBuildType == BUILD_ARR)				// 공중 건물 클릭 시
	{
		D3DXMATRIX matTrans;

		// 1번째 칸
		const TEX_INFO* pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Robotics Facility", m_iClickMyInterpace[0]);

		float fX = pInterpacetexture->ImageInfo.Width / 2.f;
		float fY = pInterpacetexture->ImageInfo.Height / 2.f;

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

		// 2번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Stargate", m_iClickMyInterpace[1]);

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

		// 3번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Citadel of Adun", m_iClickMyInterpace[2]);

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

		// 4번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Robotics Support Bay", m_iClickMyInterpace[3]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 505.f + fX
			, 397.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		// 5번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Fleet Beacon", m_iClickMyInterpace[4]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 550.f + fX
			, 397.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		// 6번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Tempar Archives", m_iClickMyInterpace[5]);

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

		// 7번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Observatory", m_iClickMyInterpace[6]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 505.f + fX
			, 440.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		// 8번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Arbiter Tribunal", m_iClickMyInterpace[7]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 550.f + fX
			, 440.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		// 9번째 칸
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
	else
	{
		D3DXMATRIX matTrans;

		// 9번째 칸
		const TEX_INFO* pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Cancel", m_iClickMyInterpace[8]);

		float fX = pInterpacetexture->ImageInfo.Width / 2.f;
		float fY = pInterpacetexture->ImageInfo.Height / 2.f;

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

}


void CProbe::DigUpResource(void)
{
	if (m_dwUnitState == UNIT_MAELSTORM || m_dwUnitState == UNIT_STASISFIELD)
	{
		return;
	}

	if (m_dwState == UNIT_DIGUP_RESOURCE_M && m_bAngle && m_iGetResource == 0)
	{
		if (fabs(m_pTarget->GetPos().x - m_tInfo.vPos.x) > 20 && fabs(m_pTarget->GetPos().y - m_tInfo.vPos.y) > 20)
		{
			m_pAStar->StartAStarPos(m_tInfo.vPos, m_pTarget->GetPos());
			return;
		}
		float fWidth = m_pTarget->GetInfo().vPos.x - m_tInfo.vPos.x;
		float fHeight = m_pTarget->GetInfo().vPos.y - m_tInfo.vPos.y;

		float fDistance = sqrtf((fWidth * fWidth) + (fHeight * fHeight));

		float fAngle = acosf(fWidth / fDistance);

		if (m_pTarget->GetInfo().vPos.y < m_tInfo.vPos.y)
			fAngle *= -1.f;

		m_fAngle = D3DXToDegree(fAngle);
		m_tInfo.vDir = m_pTarget->GetInfo().vPos - m_tInfo.vPos;

		float fDist = D3DXVec3Length(&m_tInfo.vDir);

		D3DXVec3Normalize(&m_tInfo.vDir, &m_tInfo.vDir);

		m_fGetResourceCount += GET_TIME * 25;

		if (m_eBuildType == BUILD_END)
		{
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_iClickMyInterpace[4] = 1;
		}

		if ((S_INT_CAST(m_fGetResourceCount) % 20) == 1)
		{
			if (m_tInfo.vPos.x > m_vScroll.x && m_tInfo.vPos.x < m_vScroll.x + WINCX &&
				m_tInfo.vPos.y > m_vScroll.y && m_tInfo.vPos.y < m_vScroll.y + WINCY)
			{
				D3DXVECTOR3 vTempPos = m_tInfo.vPos + m_tInfo.vDir * 15;
				SoundMgr->PlaySound(L"pprmin00.wav", CHANNEL_EFFECT3, 0.5f, m_tInfo.vPos);
				ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(vTempPos, EFFECT_PROBE), OBJ_EFFECT);
			}
		}
		if (m_fGetResourceCount > 100)
		{
			m_dwState = UNIT_GIVE_NEXUS;

			if (D_CAST(CMapResource, m_pTarget)->GetResourceType() == RESOURCE_MINERAL)
				m_iGetResource = 1;

			m_fGetResourceCount = 0;
		}
		return;
	}

	if (m_dwState == UNIT_DIGUP_RESOURCE_G && m_bAngle && m_iGetResource == 0)
	{
		m_fGetResourceCount += GET_TIME * 25;

		if (m_eBuildType == BUILD_END)
		{
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_iClickMyInterpace[4] = 1;
		}

		if (m_fGetResourceCount > 25)
		{
			m_dwState = UNIT_GIVE_NEXUS;

			if (D_CAST(CAssimilator, m_pTarget)->GetResourceType() == RESOURCE_GAS)
				m_iGetResource = 1;

			m_fGetResourceCount = 0;

			m_bGas = true;
		}
		return;
	}

	if (m_dwState == UNIT_GIVE_NEXUS && m_bGas && m_iGetResource == 1)
	{
		m_fGetResourceCount += GET_TIME * 25;

		if (m_fGetResourceCount > 12.5)
		{
			m_fGetResourceCount = 0.f;
			m_bGas = false;
			m_bGas2 = true;
			m_iGetResource = 2;
			ObjMgr->AddGameObject(CFactory<CMiniResource>::CreateDigUpResource(this, D_CAST(CAssimilator, m_pTarget)->GetResourceType()), OBJ_MINIRESOURCE);
			m_bGetResource = true;
			return;
		}
	}

	else if (m_dwState == UNIT_GIVE_NEXUS && m_iGetResource == 1 && !m_bGas)
	{
		m_iGetResource = 2;
		ObjMgr->AddGameObject(CFactory<CMiniResource>::CreateDigUpResource(this, D_CAST(CMapResource, m_pTarget)->GetResourceType()), OBJ_MINIRESOURCE);
		m_bGetResource = true;
		return;
	}

	if (m_dwState == UNIT_GIVE_NEXUS && m_iGetResource == 2 && m_bAngle)
	{
		if (m_pTargetNexus != NULL)
		{
			m_iGetResource = 3;
			return;
		}

		list<CGameObject*> pList = ObjMgr->GetList()[OBJ_BUILD];
		if (CDistanceMgr::GetLowDistanceNexus(this, pList))
			return;

		m_dwState = UNIT_NORMAL;
		m_bCollision = true;
		return;
	}

	if (m_dwState == UNIT_GIVE_NEXUS && m_iGetResource == 3)
	{
		if (m_pTargetNexus == NULL)
		{
			m_dwState = UNIT_NORMAL;
			m_iGetResource = 2;
			m_bCollision = true;
			return;
		}

		m_pAStar->StartAStarPos(m_tInfo.vPos, m_pTargetNexus->GetPos());
		m_iGetResource = 4;
		return;
	}
	if (m_dwState == UNIT_GIVE_NEXUS && m_iGetResource == 4 && m_bAngle && m_bGetResource)
	{
		if (m_bGas2)
		{
			list<CGameObject*> pList = ObjMgr->GetList()[OBJ_RESOURCE];

			for (list<CGameObject*>::iterator iter = pList.begin();
			iter != pList.end(); ++iter)
			{
				if (m_pTarget == (*iter))
				{
					m_dwState = UNIT_DIGUP_RESOURCE_M;
					m_iGetResource = 5;
					m_bGetResource = false;
					m_pAStar->StartAStarPos(m_tInfo.vPos, m_pTarget->GetPos());
					return;
				}
			}

			if (m_pTarget == NULL)
			{
				list<CGameObject*> pList = ObjMgr->GetList()[OBJ_BUILD];
				CDistanceMgr::GetLowDistanceGas(this, pList);
			}
			if (m_pTarget == NULL)
			{
				m_dwState = UNIT_NORMAL;
				m_bCollision = true;
				return;
			}

			m_dwState = UNIT_DIGUP_RESOURCE_G;
			m_iGetResource = 5;
			m_bGetResource = false;
			m_pAStar->StartAStarPos(m_tInfo.vPos, m_pTarget->GetPos());
			return;
		}
		else
		{
			list<CGameObject*> pList = ObjMgr->GetList()[OBJ_BUILD];
			for (list<CGameObject*>::iterator iter = pList.begin();
			iter != pList.end(); ++iter)
			{
				if (m_pTarget == (*iter))
				{
					m_dwState = UNIT_DIGUP_RESOURCE_G;
					m_iGetResource = 5;
					m_bGetResource = false;
					m_pAStar->StartAStarPos(m_tInfo.vPos, m_pTarget->GetPos());
					return;
				}
			}

			if (m_pTarget == NULL)
			{
				list<CGameObject*> pList = ObjMgr->GetList()[OBJ_RESOURCE];
				CDistanceMgr::GetLowDistanceMineral(this, pList);
			}
			if (m_pTarget == NULL)
			{
				m_dwState = UNIT_NORMAL;
				m_bCollision = true;
				return;
			}

			m_dwState = UNIT_DIGUP_RESOURCE_M;
			m_iGetResource = 5;
			m_bGetResource = false;
			m_pAStar->StartAStarPos(m_tInfo.vPos, m_pTarget->GetPos());
			return;
		}

	}
	else if (m_dwState == UNIT_GIVE_NEXUS && m_iGetResource == 4 && m_bAngle && !m_bGetResource)
	{
		m_iGetResource = 0;
		m_dwState = UNIT_NORMAL;
		m_bCollision = true;
		return;
	}
}

void CProbe::Frame(void)
{
	if (m_fAngle < -168.75 || m_fAngle > 168.75)
	{
		m_tFrame = FRAME(8.f, 5.f, 9.f);
		m_tInfo.vLook.x = -1;
	}
	else if (m_fAngle >= -168.75 && m_fAngle < -146.25)
	{
		m_tFrame = FRAME(6.f, 5.f, 7.f);
		m_tInfo.vLook.x = -1;
	}
	else if (m_fAngle >= -146.25 && m_fAngle < -123.75)
	{
		m_tFrame = FRAME(4.f, 5.f, 5.f);
		m_tInfo.vLook.x = -1;
	}
	else if (m_fAngle >= -123.75 && m_fAngle < -101.25)
	{
		m_tFrame = FRAME(2.f, 5.f, 3.f);
		m_tInfo.vLook.x = -1;
	}
	else if (m_fAngle >= -101.25 && m_fAngle < -78.75)
	{
		m_tFrame = FRAME(0.f, 5.f, 1.f);
		m_tInfo.vLook.x = 1;
	}
	else if (m_fAngle >= -78.75 && m_fAngle < -56.25)
	{
		m_tFrame = FRAME(2.f, 5.f, 3.f);
		m_tInfo.vLook.x = 1;
	}
	else if (m_fAngle >= -56.25 && m_fAngle < -33.75)
	{
		m_tFrame = FRAME(4.f, 5.f, 5.f);
		m_tInfo.vLook.x = 1;
	}
	else if (m_fAngle >= -33.75 && m_fAngle < -11.25)
	{
		m_tFrame = FRAME(6.f, 5.f, 7.f);
		m_tInfo.vLook.x = 1;
	}
	else if (m_fAngle >= -11.25 && m_fAngle < 11.25)
	{
		m_tFrame = FRAME(8.f, 5.f, 9.f);
		m_tInfo.vLook.x = 1;
	}
	else if (m_fAngle >= 11.25 && m_fAngle < 33.75)
	{
		m_tFrame = FRAME(10.f, 5.f, 11.f);
		m_tInfo.vLook.x = 1;
	}
	else if (m_fAngle >= 33.75 && m_fAngle < 56.25)
	{
		m_tFrame = FRAME(12.f, 5.f, 13.f);
		m_tInfo.vLook.x = 1;
	}
	else if (m_fAngle >= 56.25 && m_fAngle < 78.75)
	{
		m_tFrame = FRAME(14.f, 5.f, 15.f);
		m_tInfo.vLook.x = 1;
	}
	else if (m_fAngle >= 78.75 && m_fAngle < 101.25)
	{
		m_tFrame = FRAME(16.f, 5.f, 16.f);
		m_tInfo.vLook.x = 1;
	}
	else if (m_fAngle >= 101.25 && m_fAngle < 123.75)
	{
		m_tFrame = FRAME(14.f, 5.f, 15.f);
		m_tInfo.vLook.x = -1;
	}
	else if (m_fAngle >= 123.75 && m_fAngle < 146.25)
	{
		m_tFrame = FRAME(12.f, 5.f, 13.f);
		m_tInfo.vLook.x = -1;
	}
	else if (m_fAngle >= 146.25 && m_fAngle < 168.75)
	{
		m_tFrame = FRAME(10.f, 5.f, 11.f);
		m_tInfo.vLook.x = -1;
	}

	if (m_dwUnitState == UNIT_MAELSTORM || m_dwUnitState == UNIT_STASISFIELD)
	{
		m_fStateCount += GET_TIME * m_tStateCount.fCount;
		m_AttTarget = NULL;
		m_bUnitAtt = false;
		m_dwState = UNIT_STAND;
		m_dwSubState = UNIT_STAND;

		if (m_fStateCount >= 1.f)
		{
			m_fStateCount = 0.f;
			++m_iStateCount;
			if (m_iStasis < 12)
				++m_iStasis;
			if (m_dwUnitState == UNIT_MAELSTORM)
			{
				if (m_tStateCount.fFrame + m_iStateCount > m_tStateCount.fMax)
				{
					m_iStateCount = 0;
					--m_iStateTime;
				}
			}
			else if (m_dwUnitState == UNIT_STASISFIELD)
			{
				if (m_tStateCount.fFrame + m_iStateCount >= (m_tStateCount.fMax / 2))
				{
					m_iStateCount = 0;
					--m_iStateTime;
				}
			}
		}
		if (m_iStateTime <= 0)
		{
			m_iStasis = 0;
			m_dwUnitState = UNIT_NORMAL;
		}
	}
}

void CProbe::AStarMove(void)
{
	if (m_dwUnitState == UNIT_MAELSTORM || m_dwUnitState == UNIT_STASISFIELD)
	{
		return;
	}
	list<int>* pBestList = m_pAStar->GetBestList();
	if (pBestList->empty())
	{
		m_bAngle = true;
		if (m_eBuildType == BUILD_END)
		{
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_iClickMyInterpace[1] = 1;
		}
		return;
	}
	else
	{
		if (m_eBuildType == BUILD_END)
		{
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_iClickMyInterpace[0] = 1;
		}
		m_bAngle = false;
	}


	const vector<TILE*> pVecTile
		= (ObjMgr->FindGameObject<CBackGround>())->GetTile();

	int iNextTileIndex = pBestList->front();

	////각도
	float fWidth = pVecTile[iNextTileIndex]->vPos.x - m_tInfo.vPos.x;
	float fHeight = pVecTile[iNextTileIndex]->vPos.y - m_tInfo.vPos.y;

	float fDistance = sqrtf((fWidth * fWidth) + (fHeight * fHeight));

	float fAngle = acosf(fWidth / fDistance);

	if (pVecTile[iNextTileIndex]->vPos.y < m_tInfo.vPos.y)
		fAngle *= -1.f;

	m_fAngle = D3DXToDegree(fAngle);

	//방향
	m_tInfo.vDir = pVecTile[iNextTileIndex]->vPos - m_tInfo.vPos;

	float fDist = D3DXVec3Length(&m_tInfo.vDir);

	D3DXVec3Normalize(&m_tInfo.vDir, &m_tInfo.vDir);

	m_tInfo.vPos += m_tInfo.vDir * m_fSpeed * GET_TIME;

	if (fDist < 5.f)
	{
		pBestList->pop_front();
	}
}

void CProbe::AI(void)
{
	if (m_eTeamType == TEAM_PLAYER)
		return;

	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
	int iAICount = D_CAST(CInterpace, pInterpace)->GetAICount();
	FRAME tAIFrame = D_CAST(CInterpace, pInterpace)->GetAIFrame();


	if (iAICount == 10 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"Pylon");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(3552.f, 256.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(3552.f, 256.f, 0.f));
		
		return;
	}


	if (iAICount == 30 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"GateWay");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(3424.f, 240.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(3424.f, 240.f, 0.f));

		return;
	}


	if (iAICount == 45 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"Assimilator");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(3782.f, 96.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(3782.f, 96.f, 0.f));

		return;
	}

	if (iAICount == 60 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"Pylon");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(3680.f, 480.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(3680.f, 480.f, 0.f));

		return;
	}

	if (iAICount == 80 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"CyberneticsCore");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(3824.f, 512.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(3824.f, 512.f, 0.f));

		return;
	}

	if (iAICount == 95 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"Nexus");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(2848.f, 432.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(2860.f, 432.f, 0.f));

		return;
	}

	if (iAICount == 125 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"Pylon");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(2848.f, 608.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(2848.f, 608.f, 0.f));

		return;
	}

	if (iAICount == 160 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"GateWay");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(3520.f, 464.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(3520.f, 464.f, 0.f));

		return;
	}


	if (iAICount == 180 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"CitadelofAdun");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(3728.f, 384.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(3728.f, 384.f, 0.f));

		return;
	}

	if (iAICount == 200 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"Pylon");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(3808.f, 800.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(3808.f, 800.f, 0.f));

		return;
	}

	if (iAICount == 220 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"GateWay");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(3680.f, 720.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(3680.f, 720.f, 0.f));

		return;
	}

	if (iAICount == 240 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"Pylon");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(4000.f, 608.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(4000.f, 608.f, 0.f));

		return;
	}

	if (iAICount == 260 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"RoboticsFacility");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(4016.f, 512.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(4016.f, 512.f, 0.f));

		return;
	}

	if (iAICount == 300 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"TemplarArchives");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(3728.f, 576.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(3728.f, 576.f, 0.f));

		return;
	}

	if (iAICount == 320 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"Forge");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(3618.f, 128.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(3618.f, 128.f, 0.f));

		return;
	}

	if (iAICount == 340 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"PhotonCannon");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(3648.f, 256.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(3648.f, 256.f, 0.f));

		return;
	}

	if (iAICount == 360 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"Observatory");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(3824.f, 672.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(3824.f, 672.f, 0.f));

		return;
	}

	if (iAICount == 380 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"StarGate");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(3936.f, 720.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(3936.f, 720.f, 0.f));

		return;
	}

	if (iAICount == 400 && tAIFrame.fFrame == 0.f)
	{
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") && (*iter)->GetTeamType() == TEAM_ENEMY
				&& (*iter)->GetAI())
			{
				return;
			}
		}

		lstrcpy(m_szBuildKey, L"ArbiterTribunal");

		list<BUILD*> BuildList = ObjMgr->GetBuildList();

		list<BUILD*>::iterator iter = BuildList.begin();
		list<BUILD*>::iterator iter_end = BuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->szName, m_szBuildKey))
			{
				m_tBuildInfo = (*(*iter));
				break;
			}
		}

		m_vBuildPos = D3DXVECTOR3(3920.f, 896.f, 0.f);

		m_bAI = true;
		m_bCreateBuild = true;
		m_pAStar->StartAStarPos(m_tInfo.vPos, D3DXVECTOR3(3920.f, 896.f, 0.f));

		return;
	}

	if (!m_bCreateBuild && iAICount > 0)
	{
		if (m_pTarget == NULL)
		{
			m_dwState = UNIT_DIGUP_RESOURCE_M;
			list<CGameObject*> pList = ObjMgr->GetList()[OBJ_RESOURCE];
			CDistanceMgr::GetLowDistanceMineral(this, pList);
			m_bCollision = false;
			if(!m_bAI2)
			{
				m_bAI2 = true;
				m_pAStar->StartAStarPos(m_tInfo.vPos, m_pTarget->GetPos());
			}
		}
		if (m_pTarget == NULL)
		{
			m_dwState = UNIT_NORMAL;
			m_bCollision = true;
			return;
		}
	}
}