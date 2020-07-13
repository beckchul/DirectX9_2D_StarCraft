#include "stdafx.h"
#include "Observer.h"
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

CObserver::CObserver()
{
}

CObserver::~CObserver()
{
	Release();
}

HRESULT CObserver::Initialize(void)
{
	list<UNIT*> UnitList = ObjMgr->GetUnitList();

	list<UNIT*>::iterator iter = UnitList.begin();
	list<UNIT*>::iterator iter_end = UnitList.end();

	lstrcpy(m_tInfo.szObjKey, L"Observer");
	lstrcpy(m_tInfo.szStateKey, L"Normal");

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
	m_tInfo.vSize = D3DXVECTOR3(16.f, 16.f, 0.f);
	m_tInfo.vLook = D3DXVECTOR3(1.f, 0.f, 0.f);
	m_tClick = FRAME(0.f, 10.f, 2.f);
	m_tFrame = FRAME(8.f, 5.f, 9.f);
	m_fSpeed = 160.f;
	m_fFrameTime = 0.f;
	m_iFrameCount = 0;
	m_vEndPoint = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_eRenderType = RENDER_AIR_UNIT;
	m_eAlphType = ALPH_CREATE_CLOOCKING;
	m_iView = 9;
	return S_OK;
}

int CObserver::Update(void)
{
	list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];
	list<CGameObject*> pBuildList = ObjMgr->GetList()[OBJ_BUILD];
	CDistanceMgr::CheckCloking(this, pUnitList, pBuildList);

	if (m_dwState == UNIT_DEAD || m_tUnitInfo.iHp <= 0)
		m_bDeadCheck = true;


	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
	if (m_bDeadCheck)
	{
		if (rand() % 2 == 0)
			SoundMgr->PlaySound(L"pwidth00.wav", CHANNEL_EFFECT_DEAD, 0.5f, m_tInfo.vPos);
		else
			SoundMgr->PlaySound(L"pwidth01.wav", CHANNEL_EFFECT_DEAD, 0.5f, m_tInfo.vPos);

		D_CAST(CInterpace, pInterpace)->EraseUnitList(this);
		ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_OBSERVER_DEAD), OBJ_EFFECT);
		return 1;
	}
	SelectKeyCheck();
	Frame();
	Move();

	if (m_eAlphType == ALPH_CREATE_CLOOCKING && m_tAlphCount.fFrame < m_tAlphCount.fMax)
	{
		m_tAlphCount.fFrame += m_tAlphCount.fCount * GET_TIME;

		if (m_tAlphCount.fFrame >= m_tAlphCount.fMax)
		{
			m_tAlphCount.fFrame = m_tAlphCount.fMax;
		}
	}

	
	return 0;
}

void CObserver::Render(void)
{
	D3DXMATRIX matScale, matTrans;

	if (m_bSelect)
	{
		CObjectRenderMgr::SelectUnitUIRender(this, 1, 7.f, 0, 20.f);
	}

	const TEX_INFO* pDarkTemplartexture = TextureMgr->GetTexture(m_tInfo.szObjKey
		, m_tInfo.szStateKey, S_INT_CAST(m_tFrame.fFrame) + S_INT_CAST(m_iFrameCount * 17));

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
		if (m_eAlphType == ALPH_CREATE_CLOOCKING)
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

void CObserver::Release(void)
{
}

void CObserver::SelectKeyCheck(void)
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
		m_vEndPoint = vMouse;

		for (int i = 0; i < 9; ++i)
			m_iClickMyInterpace[i] = 0;
		m_iClickMyInterpace[0] = 1;
	}
	if (dwKey & KEY_P && m_bClickKey)
	{
		ptMouse->SetState(MOUSE_ATTACK_POINT_PATOL);
	}

	if (dwKey & KEY_LBUTTON && ptMouse->GetState() == MOUSE_ATTACK_POINT_NOMAL || ptMouse->GetSubState() == MOUSE_ATTACK_POINT_NOMAL)
	{
		m_vPatrolEndPoint = vMouse;
		m_dwState = UNIT_MOVE;
		m_dwSubState = UNIT_MOVE;
		m_bUnitAtt = false;
		m_AttTarget = NULL;
		ptMouse->SetState(MOUSE_CLICK);
	}

	if (dwKey & KEY_LBUTTON && (ptMouse->GetState() == MOUSE_ATTACK_POINT_PATOL || ptMouse->GetSubState() == MOUSE_ATTACK_POINT_PATOL))
	{
		m_vEndPoint = vMouse;
		m_vPatrolStartPoint = m_tInfo.vPos;
		m_vPatrolEndPoint = vMouse;
		m_dwSubState = UNIT_MOVE;
		m_dwState = UNIT_MOVE;

		for (int i = 0; i < 9; ++i)
			m_iClickMyInterpace[i] = 0;
		m_iClickMyInterpace[3] = 1;

		ptMouse->SetState(MOUSE_CLICK);
		ptMouse->SetSubState(MOUSE_ATTACK_POINT_PATOL);
		m_bPatrol = false;
	}

	if (!dwKey)
	{
		m_tClick.fFrame += GET_TIME * m_tClick.fCount;

		if (m_tClick.fFrame > m_tClick.fMax)
			m_bClickKey = true;
	}
}

void CObserver::CheckOneClick(void)
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

void CObserver::Frame(void)
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

	if ( m_dwUnitState == UNIT_STASISFIELD)
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

void CObserver::Move(void)
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

