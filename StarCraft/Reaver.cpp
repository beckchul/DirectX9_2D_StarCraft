#include "stdafx.h"
#include "Reaver.h"
#include "TextureMgr.h"
#include "Device.h"
#include "ObjMgr.h"
#include "TimeMgr.h"
#include "AStar.h"
#include "KeyMgr.h"
#include "Mouse.h"
#include "BackGround.h"
#include "Interpace.h"
#include "AttEffect.h"
#include "Factory.h"
#include "Scarab.h"
#include "Effect.h"

CReaver::CReaver()
{
}


CReaver::~CReaver()
{
	Release();
}



HRESULT CReaver::Initialize(void)
{
	list<UNIT*> UnitList = ObjMgr->GetUnitList();

	list<UNIT*>::iterator iter = UnitList.begin();
	list<UNIT*>::iterator iter_end = UnitList.end();

	lstrcpy(m_tInfo.szObjKey, L"Reaver");
	lstrcpy(m_tInfo.szStateKey, L"Move");

	for (; iter != iter_end; ++iter)
	{
		if (!lstrcmp((*iter)->szName, m_tInfo.szObjKey))
		{
			m_tUnitInfo = (*(*iter));
			break;
		}
	}
	m_tUnitInfo.eType = UNIT_GROUND;
	m_dwState = UNIT_STAND;
	m_pAStar = new CAStar;
	m_bClickKey = true;
	m_tInfo.vSize = D3DXVECTOR3(42.f, 42.f, 0.f);
	m_tInfo.vLook = D3DXVECTOR3(1.f, 0.f, 0.f);
	m_tClick = FRAME(0.f, 10.f, 2.f);
	m_tFrame = FRAME(8.f, 5.f, 9.f);
	m_fSpeed = 35;
	m_fFrameTime = 0.f;
	m_iFrameCount = 0;
	m_eRenderType = RENDER_GROUND;
	m_iView = 11;
	m_iScarabCount = 0;
	m_bCreateUnit = false;
	m_iScarabCountMax = 5;
	m_tAttDelay = FRAME(0.f, 1.f, 3.f);
	return S_OK;
}

int CReaver::Update(void)
{
	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();

	if (D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_REVER_COUNT) == 1)
		m_iScarabCountMax = 10;

	AStarMove();

	if (m_dwState == UNIT_DEAD || m_tUnitInfo.iHp <= 0)
		m_bDeadCheck = true;

	if (m_bDeadCheck)
	{
		ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_ARCHON_DEAD), OBJ_EFFECT);
		SoundMgr->PlaySound(L"ptrdth00.wav", CHANNEL_EFFECT_DEAD, 0.5f, m_tInfo.vPos);
		D_CAST(CInterpace, pInterpace)->EraseUnitList(this);
		return 1;
	}
	Cloocking();
	SelectKeyCheck();
	Frame();
	CreateUnit(true);
	Function();
	return 0;
}

void CReaver::Render(void)
{
	D3DXMATRIX matScale, matTrans;

	if (m_bSelect)
	{
		CObjectRenderMgr::SelectUnitUIRender(this, 3, 7.f, 4, 30.f);
	}

	const TEX_INFO* pReavertexture = TextureMgr->GetTexture(m_tInfo.szObjKey
		, m_tInfo.szStateKey, S_INT_CAST(m_tFrame.fFrame) + S_INT_CAST(m_iFrameCount * 17));

	D3DXMatrixScaling(&matScale, m_tInfo.vLook.x, 1.f, 0.f);
	D3DXMatrixTranslation(&matTrans
		, m_tInfo.vPos.x - m_vScroll.x
		, m_tInfo.vPos.y - m_vScroll.y
		, 0.f);

	matScale *= matTrans;

	float fX = pReavertexture->ImageInfo.Width / 2.f;
	float fY = pReavertexture->ImageInfo.Height / 2.f;

	if (m_eTeamType == TEAM_PLAYER)
	{
		Device->GetSprite()->SetTransform(&matScale);
		Device->GetSprite()->Draw(pReavertexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame), 255, 255, 255));
	}
	else if (m_eTeamType == TEAM_ENEMY)
	{
		if (m_eAlphType == ALPH_NOMAL_CLOOCKING)
		{
			Device->GetSprite()->SetTransform(&matScale);
			Device->GetSprite()->Draw(pReavertexture->pTexture
				, NULL
				, &D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame - 85), 255, 160, 160));
		}
		else
		{
			Device->GetSprite()->SetTransform(&matScale);
			Device->GetSprite()->Draw(pReavertexture->pTexture
				, NULL
				, &D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame), 255, 160, 160));
		}
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

void CReaver::Release(void)
{
}

void CReaver::SelectKeyCheck(void)
{
	if (m_dwUnitState == UNIT_STASISFIELD)
		return;
	if (!m_bSelect)
		return;
	DWORD dwKey = GET_KEY;
	CGameObject* ptMouse = ObjMgr->GetList()[OBJ_MOUSE].front();
	D3DXVECTOR3 vMouse = D_CAST(CMouse, ptMouse)->GetMousePos();
	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
	int iMineral = D_CAST(CInterpace, pInterpace)->GetWorldMineral(m_eTeamType);

	if (dwKey & KEY_RBUTTON && m_bClickKey)
	{
		m_bClickKey = false;

		
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

		ptMouse->SetState(MOUSE_CLICK);
		m_dwState = UNIT_MOVE;
		m_pAStar->StartAStarPos(m_tInfo.vPos, vMouse);
	}

	if (dwKey & KEY_R && m_bClickKey && m_iScarabCount < m_iScarabCountMax && m_UnitCreateList.size() < 5)
	{
		m_bClickKey = false;
		if (iMineral >= 15)
		{
			m_eBuildCreateType = BUILD_CREATE_UNIT;
			D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -15);
			m_UnitCreateList.push_back(L"R");
		}
		else
		{

		}
	}

	if (dwKey & KEY_X && m_bClickKey)
	{
		m_bClickKey = false;
		if (m_UnitCreateList.size() >= 1)
		{
			m_UnitCreateList.pop_back();
			D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 15);

			if (m_UnitCreateList.empty())
				m_eBuildCreateType = BUILD_CREATE_NOMAL;
		}

	}

	if (dwKey & KEY_A)
	{
		ptMouse->SetState(MOUSE_ATTACK_POINT_NOMAL);
	}

	if (dwKey & KEY_P)
	{
		ptMouse->SetState(MOUSE_ATTACK_POINT_PATOL);
	}

	if (dwKey & KEY_LBUTTON && (ptMouse->GetState() == MOUSE_ATTACK_POINT_NOMAL || ptMouse->GetSubState() == MOUSE_ATTACK_POINT_NOMAL))
	{
		m_vPatrolEndPoint = vMouse;
		m_dwSubState = UNIT_ATTACK;
		m_dwState = UNIT_MOVE;
		m_pAStar->StartAStarPos(m_tInfo.vPos, vMouse);
		ptMouse->SetState(MOUSE_CLICK);
		ptMouse->SetSubState(MOUSE_ATTACK_POINT_NOMAL);
	}

	if (dwKey & KEY_LBUTTON && (ptMouse->GetState() == MOUSE_ATTACK_POINT_PATOL || ptMouse->GetSubState() == MOUSE_ATTACK_POINT_PATOL))
	{
		m_vPatrolStartPoint = m_tInfo.vPos;
		m_vPatrolEndPoint = vMouse;
		m_dwSubState = UNIT_PATROL;
		m_dwState = UNIT_MOVE;
		m_pAStar->StartAStarPos(m_tInfo.vPos, m_vPatrolEndPoint);
		ptMouse->SetState(MOUSE_CLICK);
		ptMouse->SetSubState(MOUSE_ATTACK_POINT_PATOL);
		m_bPatrol = false;
	}

	if (dwKey & KEY_H)
	{
		m_dwState = UNIT_STAND;
		m_dwSubState = UNIT_HOLD;
		m_pAStar->AStarClear();
	}

	if (dwKey & KEY_S)
	{
		m_bUnitAtt = false;
		m_AttTarget = NULL;
		m_dwState = UNIT_STAND;
		m_dwSubState = UNIT_STAND;
		m_pAStar->AStarClear();
		for (int i = 0; i < 9; ++i)
			m_iClickMyInterpace[i] = 0;
		m_iClickMyInterpace[1] = 1;
	}

	if (!dwKey)
	{
		m_tClick.fFrame += GET_TIME * m_tClick.fCount;

		if (m_tClick.fFrame > m_tClick.fMax)
			m_bClickKey = true;
	}
}

void CReaver::CheckOneClick(void)
{
	if (m_dwUnitState == UNIT_STASISFIELD)
		return;
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

	// 4번째 칸
	pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"Patrol", m_iClickMyInterpace[3]);

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
		, L"Hold", m_iClickMyInterpace[4]);

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

	if (m_iScarabCount < m_iScarabCountMax && m_UnitCreateList.size() < 5)
	{
		// 7번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"ReverScarup", 0);

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
	}
}

void CReaver::CreateUnit(bool Check)
{
	if (Check)
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
			if (!lstrcmp(szBuf, L"R"))
			{
				m_tCreateTime = FRAME(0.f, 1.f, 3.5f);
			}
			m_iFrameFirst = S_INT_CAST(m_tFrame.fFrame);
			m_bCreateUnit = true;
		}
		else
		{
			m_tCreateTime.fFrame += GET_TIME * m_tCreateTime.fCount;

			m_tFrame.fFrame += GET_TIME * m_tFrame.fCount;

			if (m_tFrame.fFrame > m_tFrame.fMax)
			{
				m_tFrame.fFrame = 1.f;
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
				if (!lstrcmp(szBuf, L"R"))
				{
					if (m_iScarabCount < m_iScarabCountMax)
						++m_iScarabCount;
				}

				m_UnitCreateList.pop_front();
			}
		}
	}
	else
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




		float fPercent = m_tCreateTime.fFrame / m_tCreateTime.fMax;

		if (fPercent > 1.f)
			fPercent = 1.f;

		RECT rc = { NULL, NULL
			, S_LONG_CAST(fPercent * fX * 2), S_LONG_CAST(fY * 2) };
		D3DXMatrixTranslation(&matTrans
			, 270.f
			, 418.f
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pBigWireTexture->pTexture
			, &rc
			, NULL
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));


		list<TCHAR*>::iterator iter = m_UnitCreateList.begin();
		list<TCHAR*>::iterator iter_end = m_UnitCreateList.end();

		int iCount = 0;
		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter), L"R"))
			{
				pBigWireTexture = TextureMgr->GetTexture(L"Produce"
					, L"Icon", 39);
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
				, NULL //&D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(255, 255, 255, 255));

			++iCount;
		}
	}
}
void CReaver::Function(void)
{
	if (m_dwUnitState == UNIT_STASISFIELD)
		return;
	if (m_dwState == UNIT_DEAD)
		return;

	list<int>* pBestList = m_pAStar->GetBestList();

	if (m_dwSubState != UNIT_MOVE)
	{
		list<CGameObject*> pObjList = ObjMgr->GetList()[OBJ_BUILD];
		CDistanceMgr::GetLowDistanceAttTargetBuild(this, pObjList);
		pObjList = ObjMgr->GetList()[OBJ_UNIT];
		CDistanceMgr::GetLowDistanceAttTargetGround(this, pObjList);
	}


	if ((m_AttTarget == NULL || m_AttTarget->GetState() == UNIT_DEAD) &&
		m_dwSubState == UNIT_MOVE && pBestList->empty())
	{
		m_bUnitAtt = false;
		m_dwState = UNIT_STAND;
		return;
	}
	if (m_dwSubState != UNIT_HOLD && m_AttTarget != NULL && m_AttTarget->GetState() != UNIT_DEAD && m_bUnitAtt)
	{
		m_pAStar->StartAStarPos(m_tInfo.vPos, m_AttTarget->GetInfo().vPos);
		m_dwState = UNIT_MOVE;
	}

	if (m_bUnitAtt && m_AttTarget != NULL &&
		fabs(m_AttTarget->GetPos().x - m_tInfo.vPos.x)  < fabs(m_AttTarget->GetInfo().vSize.x / 2.f - m_tInfo.vSize.x / 2.f) + 270.f &&
		fabs(m_AttTarget->GetPos().y - m_tInfo.vPos.y)  < fabs(m_AttTarget->GetInfo().vSize.y / 2.f - m_tInfo.vSize.y / 2.f) + 270.f)
	{
		m_pAStar->AStarClear();

		m_dwState = UNIT_ATTACK;

		float fWidth = m_AttTarget->GetPos().x - m_tInfo.vPos.x;
		float fHeight = m_AttTarget->GetPos().y - m_tInfo.vPos.y;

		float fDistance = sqrtf((fWidth * fWidth) + (fHeight * fHeight));

		float fAngle = acosf(fWidth / fDistance);

		if (m_AttTarget->GetPos().y < m_tInfo.vPos.y)
			fAngle *= -1.f;

		m_fAngle = D3DXToDegree(fAngle);
	}
	else if (m_bUnitAtt)
	{
		if (m_dwSubState == UNIT_HOLD)
		{
			m_dwState = UNIT_HOLD;
		}
	}

	if (m_dwSubState == UNIT_PATROL)
	{
		if (m_dwState != UNIT_ATTACK)
		{
			if (pBestList->empty())
			{
				if (!m_bPatrol)
				{
					m_pAStar->StartAStarPos(m_tInfo.vPos, m_vPatrolStartPoint);
					m_bPatrol = true;
				}
				else
				{
					m_pAStar->StartAStarPos(m_tInfo.vPos, m_vPatrolEndPoint);
					m_bPatrol = false;
				}
			}
		}
	}
	if (m_dwSubState == UNIT_ATTACK)
	{

		if (pBestList->empty() && (m_AttTarget == NULL || m_AttTarget->GetState() == UNIT_DEAD))
		{
			m_bUnitAtt = false;
			m_dwState = UNIT_MOVE;
			m_pAStar->StartAStarPos(m_tInfo.vPos, m_vPatrolEndPoint);
		}
	}
	if (m_dwSubState == UNIT_STAND)
	{

		if (pBestList->empty() && (m_AttTarget == NULL || m_AttTarget->GetState() == UNIT_DEAD))
		{
			m_bUnitAtt = false;
			m_dwState = UNIT_STAND;
		}
	}

	if ((m_dwState == UNIT_MOVE || m_dwState == UNIT_ATTACK) &&
		pBestList->empty() && (m_AttTarget == NULL || m_AttTarget->GetState() == UNIT_DEAD) && !m_bUnitAtt)
	{
		m_dwState = UNIT_STAND;
		if (m_dwSubState != UNIT_HOLD)
			m_dwSubState = UNIT_STAND;
	}
}
void CReaver::Cloocking(void)
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
void CReaver::Frame(void)
{
	switch (m_dwState)
	{
	case UNIT_ATTACK:
	case UNIT_MOVE:
		if (m_fAngle < -168.75 || m_fAngle > 168.75)
		{
			m_tFrame = FRAME(8.f, 25.f, 144.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -168.75 && m_fAngle < -146.25)
		{
			m_tFrame = FRAME(6.f, 25.f, 142.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -146.25 && m_fAngle < -123.75)
		{
			m_tFrame = FRAME(4.f, 25.f, 140.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -123.75 && m_fAngle < -101.25)
		{
			m_tFrame = FRAME(2.f, 25.f, 138.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -101.25 && m_fAngle < -78.75)
		{
			m_tFrame = FRAME(0.f, 25.f, 136.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -78.75 && m_fAngle < -56.25)
		{
			m_tFrame = FRAME(2.f, 25.f, 138.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -56.25 && m_fAngle < -33.75)
		{
			m_tFrame = FRAME(4.f, 25.f, 140.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -33.75 && m_fAngle < -11.25)
		{
			m_tFrame = FRAME(6.f, 25.f, 142.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -11.25 && m_fAngle < 11.25)
		{
			m_tFrame = FRAME(8.f, 25.f, 144.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 11.25 && m_fAngle < 33.75)
		{
			m_tFrame = FRAME(10.f, 25.f, 146.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 33.75 && m_fAngle < 56.25)
		{
			m_tFrame = FRAME(12.f, 25.f, 148.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 56.25 && m_fAngle < 78.75)
		{
			m_tFrame = FRAME(14.f, 25.f, 150.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 78.75 && m_fAngle < 101.25)
		{
			m_tFrame = FRAME(16.f, 25.f, 152.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 101.25 && m_fAngle < 123.75)
		{
			m_tFrame = FRAME(14.f, 25.f, 150.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= 123.75 && m_fAngle < 146.25)
		{
			m_tFrame = FRAME(12.f, 25.f, 148.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= 146.25 && m_fAngle < 168.75)
		{
			m_tFrame = FRAME(10.f, 25.f, 146.f);
			m_tInfo.vLook.x = -1;
		}
		break;
	case UNIT_STAND:
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
		break;
	}

	if (m_dwState != UNIT_STAND && m_dwState != UNIT_ATTACK)
	{
		m_fFrameTime += GET_TIME * m_tFrame.fCount;
		if (m_fFrameTime >= 1)
		{
			m_fFrameTime = 0;
			++m_iFrameCount;
		}
	}

	if (m_dwState == UNIT_MOVE)
	{
		if (m_tFrame.fFrame + m_iFrameCount * 17 > m_tFrame.fMax)
		{
			m_iFrameCount = 0;
		}
	}

	m_tAttDelay.fFrame += GET_TIME * m_tAttDelay.fCount;


	if (m_dwState == UNIT_ATTACK)
	{
		if (m_tAttDelay.fFrame > m_tAttDelay.fMax)
		{
			if (m_iScarabCount > 0)
			{
				--m_iScarabCount;
				m_tAttDelay.fFrame = 0.f;
				ObjMgr->AddGameObject(CFactory<CScarab>::CreateGameUnitScarab(m_AttTarget, m_tInfo.vPos, m_eTeamType), OBJ_UNIT);
			}
		}
	}

	if (m_dwUnitState == UNIT_STASISFIELD)
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
			if (m_dwUnitState == UNIT_STASISFIELD)
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

void CReaver::AStarMove(void)
{
	if(m_dwUnitState == UNIT_STASISFIELD)
		return;
	list<int>* pBestList = m_pAStar->GetBestList();

	if (pBestList->empty())
	{
		if (m_dwState != UNIT_ATTACK &&
			m_dwState != UNIT_HOLD)
		{
			m_dwState = UNIT_STAND;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_iClickMyInterpace[1] = 1;
		}

		return;
	}

	if (m_dwState != UNIT_ATTACK &&
		m_dwState != UNIT_HOLD)
	{
		for (int i = 0; i < 9; ++i)
			m_iClickMyInterpace[i] = 0;
		m_iClickMyInterpace[0] = 1;
	}

	const vector<TILE*> pVecTile
		= (ObjMgr->FindGameObject<CBackGround>())->GetTile();

	int iNextTileIndex = pBestList->front();

	//각도
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

