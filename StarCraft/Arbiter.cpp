#include "stdafx.h"
#include "Arbiter.h"
#include "TextureMgr.h"
#include "Device.h"
#include "ObjMgr.h"
#include "TimeMgr.h"
#include "KeyMgr.h"
#include "Mouse.h"
#include "BackGround.h"
#include "Factory.h"
#include "Effect.h"
#include "Interpace.h"
#include "AttEffect.h"

CArbiter::CArbiter()
{
}


CArbiter::~CArbiter()
{
	Release();
}

HRESULT CArbiter::Initialize(void)
{
	list<UNIT*> UnitList = ObjMgr->GetUnitList();

	list<UNIT*>::iterator iter = UnitList.begin();
	list<UNIT*>::iterator iter_end = UnitList.end();

	lstrcpy(m_tInfo.szObjKey, L"Arbiter");
	lstrcpy(m_tInfo.szStateKey, L"Normal");
	lstrcpy(m_tUnitInfo.szName, L"Arbiter");

	for (; iter != iter_end; ++iter)
	{
		if (!lstrcmp((*iter)->szName, m_tInfo.szObjKey))
		{
			m_tUnitInfo = (*(*iter));
			break;
		}
	}

	m_tUnitInfo.eType = UNIT_ARR;
	m_dwState = UNIT_STAND;
	m_bClickKey = true;
	m_tInfo.vSize = D3DXVECTOR3(32.f, 32.f, 0.f);
	m_tInfo.vLook = D3DXVECTOR3(1.f, 0.f, 0.f);
	m_tClick = FRAME(0.f, 10.f, 2.f);
	m_tFrame = FRAME(8.f, 5.f, 9.f);
	m_fSpeed = 130.f;
	m_fFrameTime = 0.f;
	m_iFrameCount = 0;
	m_vEndPoint = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_eRenderType = RENDER_AIR_UNIT;
	m_tAttDelay = FRAME(0.f, 7.f, 14.f);
	m_iView = 9;
	return S_OK;
}

int CArbiter::Update(void)
{
	
		

	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
	if (D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_ARBITER_MPMAX) == 1)
	{
		m_tUnitInfo.iMpMax = 250;
	}

	D_CAST(CInterpace, pInterpace)->SetUpgradeCount(TEAM_PLAYER, UPGRADE_ARBITER_RECOLL, 1);
	D_CAST(CInterpace, pInterpace)->SetUpgradeCount(TEAM_PLAYER, UPGRADE_ARBITER_STACISFIELD, 1);
	m_tUnitInfo.iMp = 150;

	if (m_dwState == UNIT_DEAD || m_tUnitInfo.iHp <= 0)
		m_bDeadCheck = true;

	if (m_bDeadCheck)
	{
		SoundMgr->PlaySound(L"pabDth00.wav", CHANNEL_EFFECT_DEAD, 0.5f);
		D_CAST(CInterpace, pInterpace)->EraseUnitList(this);
		ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_SCOUT_DEAD), OBJ_EFFECT);
		return 1;
	}
	SelectKeyCheck();
	Frame();
	Move();
	Function();
	m_tCreateTime.fFrame += GET_TIME;


	if (m_AttTarget != NULL &&
		((m_AttTarget->GetDeadCheck() ||
			m_AttTarget->GetAlphType() == ALPH_NOMAL_CLOOCKING ||
			m_AttTarget->GetAlphType() == ALPH_CREATE_CLOOCKING) &&
			m_AttTarget->GetTeamType() != m_eTeamType))
		m_AttTarget = NULL;

	if (m_AttTarget != NULL && m_AttTarget->GetUnitState() == UNIT_STASISFIELD)
		m_AttTarget = NULL;

	if (m_tCreateTime.fFrame > 2.f)
	{
		m_tCreateTime.fFrame = 0;
		if (m_tUnitInfo.iMp < m_tUnitInfo.iMpMax)
		{
			++m_tUnitInfo.iMp;
		}
	}

	return 0;
}

void CArbiter::Render(void)
{
	D3DXMATRIX matScale, matTrans;

	if (m_bSelect)
	{
		CObjectRenderMgr::SelectUnitUIRender(this, 3, 7.f, 4, 40.f);

		///Mp Frame
		const TEX_INFO* pSelecttexture = TextureMgr->GetTexture(L"Bar"
			, L"MPFrame", 2);

		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y + 50
			, 0.f);

		float fX = pSelecttexture->ImageInfo.Width / 2.f;
		float fY = pSelecttexture->ImageInfo.Height / 2.f;

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pSelecttexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		pSelecttexture = TextureMgr->GetTexture(L"Bar"
			, L"MPBar", 2);

		RECT rc = { NULL
			, NULL
			, S_LONG_CAST(fX * 2 * S_FLOAT_CAST(m_tUnitInfo.iMp) / S_FLOAT_CAST(m_tUnitInfo.iMpMax))
			, S_LONG_CAST(fY * 2) };

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pSelecttexture->pTexture
			, &rc
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	const TEX_INFO* pShuttletexture = TextureMgr->GetTexture(m_tInfo.szObjKey
		, m_tInfo.szStateKey, S_INT_CAST(m_tFrame.fFrame));

	D3DXMatrixScaling(&matScale, m_tInfo.vLook.x, 1.f, 0.f);
	D3DXMatrixTranslation(&matTrans
		, m_tInfo.vPos.x - m_vScroll.x
		, m_tInfo.vPos.y - m_vScroll.y
		, 0.f);

	matScale *= matTrans;

	float fX = pShuttletexture->ImageInfo.Width / 2.f;
	float fY = pShuttletexture->ImageInfo.Height / 2.f;


	if (m_eTeamType == TEAM_PLAYER)
	{
		Device->GetSprite()->SetTransform(&matScale);
		Device->GetSprite()->Draw(pShuttletexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	else if (m_eTeamType == TEAM_ENEMY)
	{
		Device->GetSprite()->SetTransform(&matScale);
		Device->GetSprite()->Draw(pShuttletexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 160, 160));
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

void CArbiter::Release(void)
{
}

void CArbiter::SelectKeyCheck(void)
{
	if (m_dwUnitState == UNIT_STASISFIELD)
		return;
	if (!m_bSelect)
		return;
	DWORD dwKey = GET_KEY;
	CGameObject* ptMouse = ObjMgr->GetList()[OBJ_MOUSE].front();
	D3DXVECTOR3 vMouse = D_CAST(CMouse, ptMouse)->GetMousePos();

	if (dwKey & KEY_RBUTTON && m_bClickKey)
	{
		m_bClickKey = false;

	
		// ¸¶¿ì½º ÀÎÅÍÆäÀÌ½º Å¬¸¯ ¹æÁö
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
		m_vEndPoint = vMouse;
		m_bUnitAtt = false;
		m_AttTarget = NULL;

		for (int i = 0; i < 9; ++i)
			m_iClickMyInterpace[i] = 0;
		m_iClickMyInterpace[0] = 1;
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
		m_vEndPoint = vMouse;
		ptMouse->SetSubState(MOUSE_ATTACK_POINT_NOMAL);
		ptMouse->SetState(MOUSE_CLICK);
		for (int i = 0; i < 9; ++i)
			m_iClickMyInterpace[i] = 0;
		m_iClickMyInterpace[2] = 1;
	}

	if (dwKey & KEY_LBUTTON && (ptMouse->GetState() == MOUSE_ATTACK_POINT_PATOL || ptMouse->GetSubState() == MOUSE_ATTACK_POINT_PATOL))
	{
		m_vEndPoint = vMouse;
		m_vPatrolStartPoint = m_tInfo.vPos;
		m_vPatrolEndPoint = vMouse;
		m_dwSubState = UNIT_PATROL;
		m_dwState = UNIT_MOVE;

		for (int i = 0; i < 9; ++i)
			m_iClickMyInterpace[i] = 0;
		m_iClickMyInterpace[3] = 1;

		ptMouse->SetState(MOUSE_CLICK);
		ptMouse->SetSubState(MOUSE_ATTACK_POINT_PATOL);
		m_bPatrol = false;
	}

	if (dwKey & KEY_H)
	{
		m_dwState = UNIT_STAND;
		m_dwSubState = UNIT_HOLD;
		m_vEndPoint = m_tInfo.vPos;
		for (int i = 0; i < 9; ++i)
			m_iClickMyInterpace[i] = 0;
		m_iClickMyInterpace[4] = 1;
	}

	if (dwKey & KEY_S)
	{
		m_bUnitAtt = false;
		m_AttTarget = NULL;
		m_dwState = UNIT_STAND;
		m_dwSubState = UNIT_STAND;
		m_vEndPoint = m_tInfo.vPos;

		for (int i = 0; i < 9; ++i)
			m_iClickMyInterpace[i] = 0;
		m_iClickMyInterpace[1] = 1;
	}

	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
	if (dwKey & KEY_T && m_bClickKey &&
		1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_ARBITER_STACISFIELD))
	{
		m_bClickKey = false;
		if (m_tUnitInfo.iMp >= 100)
		{
			m_bUnitAtt = false;
			D_CAST(CMouse, ptMouse)->SetState(MOUSE_SKILL_POINT2);
		}
		else
		{

		}
	}

	if (dwKey & KEY_R && m_bClickKey &&
		1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_ARBITER_RECOLL))
	{
		m_bClickKey = false;
		if (m_tUnitInfo.iMp >= 150)
		{
			m_bUnitAtt = false;
			D_CAST(CMouse, ptMouse)->SetState(MOUSE_SKILL_POINT);
		}
		else
		{

		}
	}

	if (dwKey & KEY_LBUTTON && m_bClickKey)
	{
		m_bClickKey = false;

		if (D_CAST(CMouse, ptMouse)->GetState() == MOUSE_SKILL_POINT)
		{
			m_tUnitInfo.iMp -= 150;
			SoundMgr->PlaySound(L"PAbFol00.wav", CHANNEL_EFFECT2, 0.5f, vMouse);
			ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameUnitEffectTarget(this, vMouse, EFFECT_ARBITER_RECALL_START, m_eTeamType), OBJ_EFFECT);
		}

		if (D_CAST(CMouse, ptMouse)->GetState() == MOUSE_SKILL_POINT2)
		{
			m_dwSubState = UNIT_SKILL2;
			m_dwState = UNIT_MOVE;
			m_vEndPoint = vMouse;
			ptMouse->SetState(MOUSE_CLICK);
			m_bUnitAtt = false;
			m_AttTarget = NULL;
		}	
	}




	if (!dwKey)
	{
		m_tClick.fFrame += GET_TIME * m_tClick.fCount;

		if (m_tClick.fFrame > m_tClick.fMax)
			m_bClickKey = true;
	}

}

void CArbiter::CheckOneClick(void)
{
	if (m_dwUnitState == UNIT_STASISFIELD)
		return;
	D3DXMATRIX matTrans;

	// 1¹øÂ° Ä­
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

	// 2¹øÂ° Ä­
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

	// 3¹øÂ° Ä­
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

	// 4¹øÂ° Ä­
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

	// 5¹øÂ° Ä­
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


	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();

	if (1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_ARBITER_RECOLL))
	{
		m_iClickMyInterpace[6] = 0;
	}
	else
	{
		m_iClickMyInterpace[6] = 2;
	}

	if (1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_ARBITER_STACISFIELD))
	{
		m_iClickMyInterpace[7] = 0;
	}
	else
	{
		m_iClickMyInterpace[7] = 1;
	}

	// 7¹øÂ° Ä­
	pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"Recall", m_iClickMyInterpace[6]);

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


	// 8¹øÂ° Ä­
	pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"StasisField", m_iClickMyInterpace[7]);

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

void CArbiter::Function(void)
{
	if (m_dwState == UNIT_DEAD || m_dwUnitState == UNIT_STASISFIELD)
		return;


	if (m_dwSubState == UNIT_SKILL2)
	{
		if (fabs(m_tInfo.vPos.x - m_vEndPoint.x) < 200.f &&
			fabs(m_tInfo.vPos.y - m_vEndPoint.y) < 200.f)
		{
			m_tUnitInfo.iMp -= 100;
			m_dwState = UNIT_STAND;
			m_dwSubState = UNIT_STAND;
			ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_vEndPoint, EFFECT_ARBITER_STASISFIELD), OBJ_EFFECT);
			SoundMgr->PlaySound(L"PabCag00.wav", CHANNEL_EFFECT2, 0.5f, m_vEndPoint);
			list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

			list<CGameObject*>::iterator iter = pUnitList.begin();
			list<CGameObject*>::iterator iter_end = pUnitList.end();

			for (; iter != iter_end; ++iter)
			{
				if ((*iter)->GetPos().x >= m_vEndPoint.x - 70.f &&
					(*iter)->GetPos().x <= m_vEndPoint.x + 70.f &&
					(*iter)->GetPos().y >= m_vEndPoint.y - 70.f &&
					(*iter)->GetPos().y <= m_vEndPoint.y + 70.f)
				{
					if ((*iter) == this)
						continue;

						(*iter)->SetStateTime(25);
						(*iter)->SetUnitState(UNIT_STASISFIELD);
				}
			}
		}
	}

	if (m_dwSubState != UNIT_MOVE && m_dwSubState != UNIT_SKILL2)
	{
		list<CGameObject*> pObjList = ObjMgr->GetList()[OBJ_BUILD];
		CDistanceMgr::GetLowDistanceAttTargetBuild(this, pObjList);
		pObjList = ObjMgr->GetList()[OBJ_UNIT];
		CDistanceMgr::GetLowDistanceAttTargetGround(this, pObjList);
	}


	if ((m_AttTarget == NULL || m_AttTarget->GetState() == UNIT_DEAD) && m_dwState != UNIT_MOVE)
	{
		m_bUnitAtt = false;
		m_dwState = UNIT_STAND;
	}
	if (m_dwSubState != UNIT_HOLD && m_AttTarget != NULL && m_AttTarget->GetState() != UNIT_DEAD && m_bUnitAtt)
	{
		m_vEndPoint = m_AttTarget->GetInfo().vPos;
		m_dwState = UNIT_MOVE;
	}

	if (m_bUnitAtt && m_AttTarget != NULL &&
		fabs(m_AttTarget->GetPos().x - m_tInfo.vPos.x) < fabs(m_AttTarget->GetInfo().vSize.x / 2.f - m_tInfo.vSize.x / 2.f) + 120.f &&
		fabs(m_AttTarget->GetPos().y - m_tInfo.vPos.y) < fabs(m_AttTarget->GetInfo().vSize.y / 2.f - m_tInfo.vSize.y / 2.f) + 120.f)
	{
		m_dwState = UNIT_ATTACK;
		if (m_dwSubState != UNIT_HOLD)
		{
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_iClickMyInterpace[2] = 1;
		}


		float fWidth = m_AttTarget->GetPos().x - m_tInfo.vPos.x;
		float fHeight = m_AttTarget->GetPos().y - m_tInfo.vPos.y;

		float fDistance = sqrtf((fWidth * fWidth) + (fHeight * fHeight));

		float fAngle = acosf(fWidth / fDistance);

		if (m_AttTarget->GetPos().y < m_tInfo.vPos.y)
			fAngle *= -1.f;

		m_fAngle = D3DXToDegree(fAngle);
	}


	if (m_dwSubState == UNIT_PATROL)
	{
		if (m_dwState == UNIT_STAND)
		{
			if (!m_bPatrol)
			{
				m_vEndPoint = m_vPatrolStartPoint;
				m_dwState = UNIT_MOVE;
				m_bPatrol = true;

				for (int i = 0; i < 9; ++i)
					m_iClickMyInterpace[i] = 0;
				m_iClickMyInterpace[3] = 1;
			}
			else
			{
				m_vEndPoint = m_vPatrolEndPoint;
				m_dwState = UNIT_MOVE;
				m_bPatrol = false;

				for (int i = 0; i < 9; ++i)
					m_iClickMyInterpace[i] = 0;
				m_iClickMyInterpace[3] = 1;
			}
		}

	}
	if (m_dwSubState == UNIT_ATTACK)
	{

		if (m_vEndPoint != m_vPatrolEndPoint && (m_AttTarget == NULL || m_AttTarget->GetState() == UNIT_DEAD))
		{
			m_bUnitAtt = false;
			m_dwState = UNIT_MOVE;
			m_vEndPoint = m_vPatrolEndPoint;
		}
	}
	if (m_dwSubState == UNIT_STAND)
	{

		if ((m_AttTarget == NULL || m_AttTarget->GetState() == UNIT_DEAD))
		{
			m_bUnitAtt = false;
			m_dwState = UNIT_STAND;

			if (m_dwSubState != UNIT_HOLD)
				m_dwSubState = UNIT_STAND;
		}
	}
}

void CArbiter::Frame(void)
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

	m_tAttDelay.fFrame += GET_TIME * m_tAttDelay.fCount;

	if (m_dwState == UNIT_ATTACK)
	{
		if (m_tAttDelay.fFrame > m_tAttDelay.fMax)
		{
			m_tAttDelay.fFrame = 0.f;
			if (m_AttTarget != NULL)
			{
				SoundMgr->PlaySound(L"dragbull.wav", CHANNEL_EFFECT_ATT3, 0.5f, m_tInfo.vPos);
				ObjMgr->AddGameObject(CFactory<CAttEffect>::CreateGameUnitEffectTarget(m_tInfo.vPos, EFFECT_ARBITER_ATT, m_AttTarget, m_tUnitInfo), OBJ_EFFECT);
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

			if (m_tStateCount.fFrame + m_iStateCount >= (m_tStateCount.fMax / 2))
			{
				m_iStateCount = 0;
				--m_iStateTime;
			}
		}
		if (m_iStateTime <= 0)
		{
			m_iStasis = 0;
			m_dwUnitState = UNIT_NORMAL;
		}
	}
}

void CArbiter::Move(void)
{
	if (m_dwState != UNIT_MOVE || m_dwUnitState == UNIT_STASISFIELD || m_dwState == UNIT_DEAD)
		return;

	float fWidth = m_vEndPoint.x - m_tInfo.vPos.x;
	float fHeight = m_vEndPoint.y - m_tInfo.vPos.y;

	float fDistance = sqrtf((fWidth * fWidth) + (fHeight * fHeight));

	float fAngle = acosf(fWidth / fDistance);

	if (m_vEndPoint.y < m_tInfo.vPos.y)
		fAngle *= -1.f;

	m_fAngle = D3DXToDegree(fAngle);

	//¹æÇâ
	m_tInfo.vDir = m_vEndPoint - m_tInfo.vPos;

	float fDist = D3DXVec3Length(&m_tInfo.vDir);

	D3DXVec3Normalize(&m_tInfo.vDir, &m_tInfo.vDir);

	m_tInfo.vPos += m_tInfo.vDir * m_fSpeed * GET_TIME;

	if (fabs(m_vEndPoint.x - m_tInfo.vPos.x) < 4 && fabs(m_vEndPoint.y - m_tInfo.vPos.y) < 4)
	{
		if (m_dwSubState != UNIT_PATROL)
		{
			m_dwSubState = UNIT_STAND;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_iClickMyInterpace[1] = 1;
		}

		m_dwState = UNIT_STAND;
	}
}

