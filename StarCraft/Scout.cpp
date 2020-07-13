#include "stdafx.h"
#include "Scout.h"
#include "TextureMgr.h"
#include "Device.h"
#include "ObjMgr.h"
#include "TimeMgr.h"
#include "KeyMgr.h"
#include "Mouse.h"
#include "BackGround.h"
#include "Factory.h"
#include "Effect.h"
#include "AttEffect.h"
#include "Interpace.h"

CScout::CScout()
{
}


CScout::~CScout()
{
	Release();
}

HRESULT CScout::Initialize(void)
{
	list<UNIT*> UnitList = ObjMgr->GetUnitList();

	list<UNIT*>::iterator iter = UnitList.begin();
	list<UNIT*>::iterator iter_end = UnitList.end();

	lstrcpy(m_tInfo.szObjKey, L"Scout");
	lstrcpy(m_tInfo.szStateKey, L"Normal");
	lstrcpy(m_tUnitInfo.szName, L"Scout");

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
	m_fSpeed = 160.f;
	m_fFrameTime = 0.f;
	m_iFrameCount = 0;
	m_vEndPoint = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_tAttDelay = FRAME(0.f, 7.f, 10.f);
	m_eRenderType = RENDER_AIR_UNIT;
	m_iView = 9;
	return S_OK;
}

int CScout::Update(void)
{
	if (m_AttTarget != NULL &&
		((m_AttTarget->GetDeadCheck() ||
			m_AttTarget->GetAlphType() == ALPH_NOMAL_CLOOCKING ||
			m_AttTarget->GetAlphType() == ALPH_CREATE_CLOOCKING) &&
			m_AttTarget->GetTeamType() != m_eTeamType))
		m_AttTarget = NULL;

	if (m_AttTarget != NULL && m_AttTarget->GetUnitState() == UNIT_STASISFIELD)
		m_AttTarget = NULL;

	if (m_dwState == UNIT_DEAD || m_tUnitInfo.iHp <= 0)
		m_bDeadCheck = true;

	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
	if (m_bDeadCheck)
	{
		SoundMgr->PlaySound(L"PScDth00.wav", CHANNEL_EFFECT_DEAD, 0.5f, m_tInfo.vPos);
		D_CAST(CInterpace, pInterpace)->EraseUnitList(this);
		ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_SCOUT_DEAD), OBJ_EFFECT);
		return 1;
	}
	Cloocking();
	SelectKeyCheck();
	Function();
	Frame();
	Move();

	

	return 0;
}

void CScout::Render(void)
{
	D3DXMATRIX matScale, matTrans;

	if (m_bSelect)
	{
		CObjectRenderMgr::SelectUnitUIRender(this, 2, 7.f, 3, 20.f);
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
			, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame), 255, 255, 255));
	}
	else if (m_eTeamType == TEAM_ENEMY)
	{
		if (m_eAlphType == ALPH_NOMAL_CLOOCKING)
		{
			Device->GetSprite()->SetTransform(&matScale);
			Device->GetSprite()->Draw(pShuttletexture->pTexture
				, NULL
				, &D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame - 85), 255, 160, 160));
		}
		else
		{
			Device->GetSprite()->SetTransform(&matScale);
			Device->GetSprite()->Draw(pShuttletexture->pTexture
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

void CScout::Release(void)
{
}

void CScout::SelectKeyCheck(void)
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

	if (!dwKey)
	{
		m_tClick.fFrame += GET_TIME * m_tClick.fCount;

		if (m_tClick.fFrame > m_tClick.fMax)
			m_bClickKey = true;
	}
}

void CScout::CheckOneClick(void)
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
}
void CScout::Function(void)
{
	if (m_dwState == UNIT_DEAD || m_dwUnitState == UNIT_STASISFIELD)
		return;

	if (m_dwSubState != UNIT_MOVE)
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
		fabs(m_AttTarget->GetPos().x - m_tInfo.vPos.x) < fabs(m_AttTarget->GetInfo().vSize.x / 2.f - m_tInfo.vSize.x / 2.f) + 130.f &&
		fabs(m_AttTarget->GetPos().y - m_tInfo.vPos.y) < fabs(m_AttTarget->GetInfo().vSize.y / 2.f - m_tInfo.vSize.y / 2.f) + 130.f)
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
void CScout::Cloocking(void)
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
void CScout::Frame(void)
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
				if (m_AttTarget->GetBuildType() == BUILD_END)
				{
					if (m_AttTarget->GetUnitInfo().eType == UNIT_GROUND)
					{
						SoundMgr->PlaySound(L"blastgn2.wav", CHANNEL_EFFECT_ATT2, 0.5f, m_tInfo.vPos);
						
						ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_AttTarget->GetPos(), EFFECT_SCOUT_ATT), OBJ_EFFECT);
						CDamageMgr::DamageCalculation(m_tUnitInfo.iAtt, m_AttTarget);
					}
					else
					{
						
						SoundMgr->PlaySound(L"hkmissle.wav", CHANNEL_EFFECT_ATT2, 0.5f, m_tInfo.vPos);
						ObjMgr->AddGameObject(CFactory<CAttEffect>::CreateGameUnitEffectTarget(m_tInfo.vPos, EFFECT_SCOUT_ATT, m_AttTarget, m_tUnitInfo), OBJ_EFFECT);
						ObjMgr->AddGameObject(CFactory<CAttEffect>::CreateGameUnitEffectTarget(m_tInfo.vPos, EFFECT_SCOUT_ATT2, m_AttTarget, m_tUnitInfo), OBJ_EFFECT);
					}
				}
				else
				{
					SoundMgr->PlaySound(L"blastgn2.wav", CHANNEL_EFFECT_ATT2, 0.5f, m_tInfo.vPos);
					ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_AttTarget->GetPos(), EFFECT_SCOUT_ATT), OBJ_EFFECT);
					CDamageMgr::DamageCalculationBuild(m_tUnitInfo.iAtt, m_AttTarget);
				}
			}
		}
		if (S_INT_CAST(m_tAttDelay.fFrame) % S_INT_CAST(m_tAttDelay.fMax) == 0)
			lstrcpy(m_tInfo.szStateKey, L"Att");
		else
			lstrcpy(m_tInfo.szStateKey, L"Normal");
	}
	else
	{
		lstrcpy(m_tInfo.szStateKey, L"Normal");
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

void CScout::Move(void)
{
	if (m_dwState == UNIT_DEAD || m_dwUnitState == UNIT_STASISFIELD)
		return;
	if (m_dwState != UNIT_MOVE)
		return;

	float fWidth = m_vEndPoint.x - m_tInfo.vPos.x;
	float fHeight = m_vEndPoint.y - m_tInfo.vPos.y;

	float fDistance = sqrtf((fWidth * fWidth) + (fHeight * fHeight));

	float fAngle = acosf(fWidth / fDistance);

	if (m_vEndPoint.y < m_tInfo.vPos.y)
		fAngle *= -1.f;

	m_fAngle = D3DXToDegree(fAngle);

	//방향
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