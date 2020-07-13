#include "stdafx.h"
#include "DarkTemplar.h"
#include "TextureMgr.h"
#include "Device.h"
#include "ObjMgr.h"
#include "TimeMgr.h"
#include "AStar.h"
#include "KeyMgr.h"
#include "Mouse.h"
#include "BackGround.h"
#include "Interpace.h"
#include "DarkArchon.h"
#include "Factory.h"

CDarkTemplar::CDarkTemplar()
{
}


CDarkTemplar::~CDarkTemplar()
{
	Release();
}

HRESULT CDarkTemplar::Initialize(void)
{
	list<UNIT*> UnitList = ObjMgr->GetUnitList();

	list<UNIT*>::iterator iter = UnitList.begin();
	list<UNIT*>::iterator iter_end = UnitList.end();

	lstrcpy(m_tInfo.szObjKey, L"DarkTemplar");
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
	m_tInfo.vSize = D3DXVECTOR3(24.f, 24.f, 0.f);
	m_tInfo.vLook = D3DXVECTOR3(1.f, 0.f, 0.f);
	m_tClick = FRAME(0.f, 10.f, 2.f);
	m_tFrame = FRAME(8.f, 5.f, 9.f);
	m_fSpeed = 125.f;
	m_fFrameTime = 0.f;
	m_iFrameCount = 0;
	m_tAttDelay = FRAME(0.f, 10.f, 10.f);
	m_eRenderType = RENDER_GROUND;
	m_eAlphType = ALPH_CREATE_CLOOCKING;
	m_iArmor = 1;
	m_iView = 9;
	m_tStateCount = FRAME(0.f, 24.f, 24.f);
	m_iStateCount = 0;
	return S_OK;
}

int CDarkTemplar::Update(void)
{
	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
	switch (D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_GROUND_ATT))
	{
	case 1:
		m_tUnitInfo.iAtt = 43;
		break;
	case 2:
		m_tUnitInfo.iAtt = 46;
		break;
	case 3:
		m_tUnitInfo.iAtt = 49;
		break;
	}

	switch (D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_GROUND_ARMOR))
	{
	case 1:
		m_iArmor = 2;
		break;
	case 2:
		m_iArmor = 3;
		break;
	case 3:
		m_iArmor = 4;
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

	if (m_eAlphType == ALPH_CREATE_CLOOCKING && m_tAlphCount.fFrame < m_tAlphCount.fMax)
	{
		m_tAlphCount.fFrame += m_tAlphCount.fCount * GET_TIME;

		if (m_tAlphCount.fFrame >= m_tAlphCount.fMax)
		{
			m_tAlphCount.fFrame = m_tAlphCount.fMax;
		}
	}
	list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];
	list<CGameObject*> pBuildList = ObjMgr->GetList()[OBJ_BUILD];
	CDistanceMgr::CheckCloking(this, pUnitList, pBuildList);


	if (m_AttTarget != NULL &&
		((m_AttTarget->GetDeadCheck() ||
			m_AttTarget->GetAlphType() == ALPH_NOMAL_CLOOCKING ||
			m_AttTarget->GetAlphType() == ALPH_CREATE_CLOOCKING) &&
			m_AttTarget->GetTeamType() != m_eTeamType))
		m_AttTarget = NULL;

	if (m_AttTarget != NULL && m_AttTarget->GetUnitState() == UNIT_STASISFIELD)
		m_AttTarget = NULL;

	
	if (m_bDeadCheck)
	{
		D_CAST(CInterpace, pInterpace)->EraseUnitList(this);
		return 1;
	}
	SelectKeyCheck();
	Function();
	Frame();
	AStarMove();
	return 0;
}

void CDarkTemplar::Render(void)
{
	D3DXMATRIX matScale, matTrans;

	if (m_bSelect)
	{
		CObjectRenderMgr::SelectUnitUIRender(this, 1, 7.f, 0, 25.f);
	}

	const TEX_INFO* pDarkTemplartexture = NULL;

	if (m_dwState != UNIT_DEAD && m_dwUnitState != UNIT_MAELSTORM)
	{
	pDarkTemplartexture = TextureMgr->GetTexture(L"Dark Templar"
		, m_tInfo.szStateKey, S_INT_CAST(m_tFrame.fFrame) + S_INT_CAST(m_iFrameCount * 17));
	}
	else
	{
		pDarkTemplartexture = TextureMgr->GetTexture(L"Dark Templar"
			, m_tInfo.szStateKey, S_INT_CAST(m_tFrame.fFrame) + m_iFrameCount);
	}
	D3DXMatrixScaling(&matScale, m_tInfo.vLook.x, 1.f, 0.f);
	D3DXMatrixTranslation(&matTrans
		, m_tInfo.vPos.x - m_vScroll.x
		, m_tInfo.vPos.y - m_vScroll.y
		, 0.f);

	matScale *= matTrans;

	float fX = pDarkTemplartexture->ImageInfo.Width / 2.f;
	float fY = pDarkTemplartexture->ImageInfo.Height / 2.f;

	if (m_eTeamType == TEAM_PLAYER)
	{
		Device->GetSprite()->SetTransform(&matScale);
		Device->GetSprite()->Draw(pDarkTemplartexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame), 255, 255, 255));
	}
	else if (m_eTeamType == TEAM_ENEMY)
	{
		if (m_eAlphType == ALPH_CREATE_CLOOCKING && m_dwUnitState != UNIT_MAELSTORM)
		{
			Device->GetSprite()->SetTransform(&matScale);
			Device->GetSprite()->Draw(pDarkTemplartexture->pTexture
				, NULL
				, &D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame - 85), 255, 160, 160));
		}
		else
		{
			Device->GetSprite()->SetTransform(&matScale);
			Device->GetSprite()->Draw(pDarkTemplartexture->pTexture
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
			, D3DCOLOR_ARGB(80, 255, 160, 160));
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
			, D3DCOLOR_ARGB(130, 255, 255, 255));
	}
}

void CDarkTemplar::Release(void)
{
}

void CDarkTemplar::SelectKeyCheck(void)
{
	if (m_dwUnitState == UNIT_MAELSTORM || m_dwUnitState == UNIT_STASISFIELD)
		return;
	if (!m_bSelect)
		return;
	DWORD dwKey = GET_KEY;
	CGameObject* ptMouse = ObjMgr->GetList()[OBJ_MOUSE].front();
	D3DXVECTOR3 vMouse = D_CAST(CMouse, ptMouse)->GetMousePos();

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
		m_dwSubState = UNIT_MOVE;
		m_bUnitAtt = false;
		m_AttTarget = NULL;
		m_pAStar->StartAStarPos(m_tInfo.vPos, vMouse);
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

	if (dwKey & KEY_R && m_bClickKey)
	{
		m_bClickKey = false;

		if (m_iCreateNumber == 0)
		{
			list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

			for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
			{
				if ((*iter)->GetTeamType() != m_eTeamType ||
					(*iter) == this ||
					lstrcmp((*iter)->GetUnitInfo().szName, L"DarkTemplar") ||
					D_CAST(CDarkTemplar, (*iter))->GetCreateTemplar() != NULL)
					continue;

				m_CreateTemplar = (*iter);
				D_CAST(CDarkTemplar, (*iter))->SetCreateTemplar(this);

				m_iCreateNumber = 1;
				if (D_CAST(CDarkTemplar, (*iter))->GetCreateNumber() == 0)
				{
					D_CAST(CDarkTemplar, (*iter))->SetCreateNumber(100);
				}

				m_dwState = UNIT_MOVE;
				m_dwSubState = UNIT_SKILL3;
				return;
			}
		}
	}

	if (!dwKey)
	{
		m_tClick.fFrame += GET_TIME * m_tClick.fCount;

		if (m_tClick.fFrame > m_tClick.fMax)
			m_bClickKey = true;
	}
}

void CDarkTemplar::Function(void)
{
	if (m_dwUnitState == UNIT_MAELSTORM || m_dwUnitState == UNIT_STASISFIELD)
		return;
	if (m_dwState == UNIT_DEAD)
		return;

	if (m_dwSubState == UNIT_SKILL3)
	{

		if (m_CreateTemplar == NULL || m_CreateTemplar->GetState() == UNIT_DEAD)
		{
			m_iCreateNumber = 0;
			m_CreateTemplar = NULL;
			m_dwState = UNIT_STAND;
			m_dwSubState = UNIT_STAND;
		}
		else
		{
			D_CAST(CDarkTemplar, m_CreateTemplar)->GetAStar()->StartAStarPos(m_CreateTemplar->GetPos(), m_tInfo.vPos);
			m_pAStar->StartAStarPos(m_tInfo.vPos, m_CreateTemplar->GetPos());
		}
		if (fabs(m_CreateTemplar->GetPos().x - m_tInfo.vPos.x) < 40.f &&
			fabs(m_CreateTemplar->GetPos().y - m_tInfo.vPos.y) < 40.f)
		{
			if (m_iCreateNumber == 1)
			{
				SoundMgr->PlaySound(L"pteSum00.wav", CHANNEL_EFFECT3, 0.7f);
				ObjMgr->AddGameObject(CFactory<CDarkArchon>::CreateGameObject(m_tInfo.vPos, m_eTeamType), OBJ_UNIT);
				m_bDeadCheck = true;
				m_CreateTemplar->SetDeadCheck(true);
			}
		}
		return;
	}



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

	if (m_bUnitAtt &&
		fabs(m_AttTarget->GetPos().x - m_tInfo.vPos.x)  < fabs(m_AttTarget->GetInfo().vSize.x / 2.f - m_tInfo.vSize.x / 2.f) + 40.f &&
		fabs(m_AttTarget->GetPos().y - m_tInfo.vPos.y)  < fabs(m_AttTarget->GetInfo().vSize.y / 2.f - m_tInfo.vSize.y / 2.f) + 40.f)
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
		if(m_dwSubState != UNIT_HOLD)
		m_dwSubState = UNIT_STAND;
	}

}

void CDarkTemplar::CheckOneClick(void)
{
	if (m_dwUnitState == UNIT_MAELSTORM || m_dwUnitState == UNIT_STASISFIELD)
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

	// 9번째 칸
	pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"Dark Archon", 2);

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

void CDarkTemplar::Frame(void)
{
	switch (m_dwState)
	{
	case UNIT_ATTACK:
		if (m_fAngle < -168.75 || m_fAngle > 168.75)
		{
			m_tFrame = FRAME(8.f, 25.f, 161.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -168.75 && m_fAngle < -146.25)
		{
			m_tFrame = FRAME(6.f, 25.f, 159.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -146.25 && m_fAngle < -123.75)
		{
			m_tFrame = FRAME(4.f, 25.f, 157.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -123.75 && m_fAngle < -101.25)
		{
			m_tFrame = FRAME(2.f, 25.f, 155.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -101.25 && m_fAngle < -78.75)
		{
			m_tFrame = FRAME(0.f, 25.f, 153.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -78.75 && m_fAngle < -56.25)
		{
			m_tFrame = FRAME(2.f, 25.f, 155.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -56.25 && m_fAngle < -33.75)
		{
			m_tFrame = FRAME(4.f, 25.f, 157.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -33.75 && m_fAngle < -11.25)
		{
			m_tFrame = FRAME(6.f, 25.f, 159.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -11.25 && m_fAngle < 11.25)
		{
			m_tFrame = FRAME(8.f, 25.f, 161.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 11.25 && m_fAngle < 33.75)
		{
			m_tFrame = FRAME(10.f, 25.f, 163.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 33.75 && m_fAngle < 56.25)
		{
			m_tFrame = FRAME(12.f, 25.f, 165.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 56.25 && m_fAngle < 78.75)
		{
			m_tFrame = FRAME(14.f, 25.f, 167.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 78.75 && m_fAngle < 101.25)
		{
			m_tFrame = FRAME(16.f, 25.f, 169.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 101.25 && m_fAngle < 123.75)
		{
			m_tFrame = FRAME(14.f, 25.f, 167.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= 123.75 && m_fAngle < 146.25)
		{
			m_tFrame = FRAME(12.f, 25.f, 165.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= 146.25 && m_fAngle < 168.75)
		{
			m_tFrame = FRAME(10.f, 25.f, 163.f);
			m_tInfo.vLook.x = -1;
		}
		break;
	case UNIT_MOVE:
		if (m_fAngle < -168.75 || m_fAngle > 168.75)
		{
			m_tFrame = FRAME(8.f, 25.f, 127.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -168.75 && m_fAngle < -146.25)
		{
			m_tFrame = FRAME(6.f, 25.f, 125.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -146.25 && m_fAngle < -123.75)
		{
			m_tFrame = FRAME(4.f, 25.f, 123.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -123.75 && m_fAngle < -101.25)
		{
			m_tFrame = FRAME(2.f, 25.f, 121.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -101.25 && m_fAngle < -78.75)
		{
			m_tFrame = FRAME(0.f, 25.f, 119.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -78.75 && m_fAngle < -56.25)
		{
			m_tFrame = FRAME(2.f, 25.f, 121.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -56.25 && m_fAngle < -33.75)
		{
			m_tFrame = FRAME(4.f, 25.f, 123.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -33.75 && m_fAngle < -11.25)
		{
			m_tFrame = FRAME(6.f, 25.f, 125.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -11.25 && m_fAngle < 11.25)
		{
			m_tFrame = FRAME(8.f, 25.f, 127.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 11.25 && m_fAngle < 33.75)
		{
			m_tFrame = FRAME(10.f, 25.f, 129.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 33.75 && m_fAngle < 56.25)
		{
			m_tFrame = FRAME(12.f, 25.f, 131.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 56.25 && m_fAngle < 78.75)
		{
			m_tFrame = FRAME(14.f, 25.f, 133.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 78.75 && m_fAngle < 101.25)
		{
			m_tFrame = FRAME(16.f, 25.f, 135.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 101.25 && m_fAngle < 123.75)
		{
			m_tFrame = FRAME(14.f, 25.f, 133.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= 123.75 && m_fAngle < 146.25)
		{
			m_tFrame = FRAME(12.f, 25.f, 131.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= 146.25 && m_fAngle < 168.75)
		{
			m_tFrame = FRAME(10.f, 25.f, 129.f);
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
	case UNIT_DEAD:
		m_tFrame = FRAME(0.f, 20.f, 6.f);
		break;
	}
	if (m_dwState == UNIT_STAND ||
		m_dwState == UNIT_MOVE)
	{
		lstrcpy(m_tInfo.szStateKey, L"Move");
	}
	else if (m_dwState == UNIT_ATTACK)
	{
		lstrcpy(m_tInfo.szStateKey, L"Att");
	}
	else if (m_dwState == UNIT_DEAD)
	{
		lstrcpy(m_tInfo.szStateKey, L"Die");
	}

	if (m_dwPreState != m_dwState)
	{
		m_iFrameCount = 0;
		m_dwPreState = m_dwState;
	}

	if (m_dwState != UNIT_STAND && m_dwState != UNIT_ATTACK && m_dwState != UNIT_DEAD)
	{
		m_fFrameTime += GET_TIME * m_tFrame.fCount;
		if (m_fFrameTime >= 1)
		{
			m_fFrameTime = 0;
			++m_iFrameCount;
		}
	}

	m_tAttDelay.fFrame += GET_TIME * m_tAttDelay.fCount;

	if (m_dwState == UNIT_ATTACK)
	{

		if (m_tAttDelay.fFrame > m_tAttDelay.fMax)
		{
			m_fFrameTime += GET_TIME * m_tFrame.fCount;
			if (m_fFrameTime >= 1)
			{
				m_fFrameTime = 0;
				++m_iFrameCount;
			}

			if (m_tFrame.fFrame + m_iFrameCount * 17 > m_tFrame.fMax)
			{
				m_tAttDelay.fFrame = 0.f;
				m_iFrameCount = 0;
				if (m_AttTarget != NULL)
				{
					SoundMgr->PlaySound(L"uzefir00.wav", CHANNEL_EFFECT_ATT2, 0.5f, m_tInfo.vPos);
					if (m_AttTarget->GetBuildType() == BUILD_END)
						CDamageMgr::DamageCalculation(this, m_AttTarget);
					else
						CDamageMgr::DamageCalculationBuild(this, m_AttTarget);
				}
					
			}
		}
	}
	else if (m_dwState == UNIT_DEAD)
	{
		m_fFrameTime += GET_TIME * m_tFrame.fCount;
		if (m_fFrameTime >= 1.f)
		{
			m_fFrameTime = 0;
			++m_iFrameCount;
		}

		if (m_tFrame.fFrame + m_iFrameCount > m_tFrame.fMax)
		{
			
			m_bDeadCheck = true;
			m_iFrameCount = 0;
		}
	}
	else
	{
		if (m_tFrame.fFrame + m_iFrameCount * 17 > m_tFrame.fMax)
		{
			m_iFrameCount = 0;
		}
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


void CDarkTemplar::AStarMove(void)
{
	if (m_dwUnitState == UNIT_MAELSTORM || m_dwUnitState == UNIT_STASISFIELD)
		return;
	if (m_dwState == UNIT_DEAD)
		return;

	list<int>* pBestList = m_pAStar->GetBestList();

	if (pBestList->empty())
	{
		switch (m_dwSubState)
		{
		case UNIT_HOLD:
			if (!m_bUnitAtt)
			{
				m_dwState = UNIT_STAND;
			}
			else
			{
				m_dwState = UNIT_ATTACK;
			}
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_iClickMyInterpace[4] = 1;
			break;
		case UNIT_STAND:
		case UNIT_MOVE:
			if (m_AttTarget == NULL)
			{
				m_dwSubState = UNIT_STAND;
				m_dwState = UNIT_STAND;
				for (int i = 0; i < 9; ++i)
					m_iClickMyInterpace[i] = 0;
				m_iClickMyInterpace[1] = 1;
			}
			else
			{
				m_dwState = UNIT_ATTACK;
				for (int i = 0; i < 9; ++i)
					m_iClickMyInterpace[i] = 0;
				m_iClickMyInterpace[2] = 1;
			}
			break;
		case UNIT_ATTACK:
			if (m_dwState != UNIT_ATTACK && m_AttTarget == NULL)
			{
				for (int i = 0; i < 9; ++i)
					m_iClickMyInterpace[i] = 0;
				m_iClickMyInterpace[1] = 1;
			}
			else
			{
				for (int i = 0; i < 9; ++i)
					m_iClickMyInterpace[i] = 0;
				m_iClickMyInterpace[2] = 1;
			}
			break;
		case UNIT_PATROL:
			if (m_dwState != UNIT_ATTACK)
			{
				for (int i = 0; i < 9; ++i)
					m_iClickMyInterpace[i] = 0;
				m_iClickMyInterpace[3] = 1;
			}
			else
			{
				m_dwState = UNIT_MOVE;
				for (int i = 0; i < 9; ++i)
					m_iClickMyInterpace[i] = 0;
				m_iClickMyInterpace[2] = 1;
			}
			break;
		}
		return;
	}

	if (m_dwSubState == UNIT_ATTACK)
	{
		for (int i = 0; i < 9; ++i)
			m_iClickMyInterpace[i] = 0;
		m_iClickMyInterpace[2] = 1;
	}
	else if (m_dwSubState == UNIT_PATROL)
	{
		for (int i = 0; i < 9; ++i)
			m_iClickMyInterpace[i] = 0;
		m_iClickMyInterpace[3] = 1;
	}
	else if (m_dwState == UNIT_ATTACK)
	{
		for (int i = 0; i < 9; ++i)
			m_iClickMyInterpace[i] = 0;
		m_iClickMyInterpace[2] = 1;
	}
	else if (m_dwState != UNIT_HOLD)
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