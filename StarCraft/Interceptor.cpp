#include "stdafx.h"
#include "Interceptor.h"
#include "AttEffect.h"
#include "Factory.h"
#include "ObjMgr.h"
#include "Device.h"
#include "TextureMgr.h"
#include "TimeMgr.h"
#include "Carrier.h"
#include "Interpace.h"
#include "Effect.h"

CInterceptor::CInterceptor()
{
}


CInterceptor::~CInterceptor()
{
	Release();
}

HRESULT CInterceptor::Initialize(void)
{
	list<UNIT*> UnitList = ObjMgr->GetUnitList();

	list<UNIT*>::iterator iter = UnitList.begin();
	list<UNIT*>::iterator iter_end = UnitList.end();

	lstrcpy(m_tInfo.szObjKey, L"Interceptor");
	lstrcpy(m_tInfo.szStateKey, L"Normal");
	lstrcpy(m_tUnitInfo.szName, L"Interceptor");

	for (; iter != iter_end; ++iter)
	{
		if (!lstrcmp((*iter)->szName, m_tInfo.szObjKey))
		{
			m_tUnitInfo = (*(*iter));
			break;
		}
	}

	m_bCommand = false;
	m_tUnitInfo.eType = UNIT_ARR;
	m_dwState = UNIT_STAND;
	m_tInfo.vSize = D3DXVECTOR3(16.f, 16.f, 0.f);
	m_fSpeed = 130.f;
	m_fFrameTime = 0.f;
	m_iFrameCount = 0;
	m_bRender = false;
	m_bPattonClear = true;
	m_vEndPoint = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_eRenderType = RENDER_AIR_UNIT;
	m_bPattonAttack = false;
	m_iView = 7;
	m_tInfo.vPos = m_Carrier->GetPos();
	return S_OK;
}

int CInterceptor::Update(void)
{
	
	if (m_Carrier == NULL || m_Carrier->GetState() == UNIT_DEAD)
		m_bDeadCheck = true;

	if (m_dwState == UNIT_DEAD || m_tUnitInfo.iHp <= 0)
		m_bDeadCheck = true;
		

	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
	if (m_bDeadCheck)
	{
		D_CAST(CInterpace, pInterpace)->EraseUnitList(this);
		if(m_Carrier != NULL && m_Carrier->GetState() != UNIT_DEAD && m_Carrier->GetUnitInfo().iHp >= 1)
		D_CAST(CCarrier, m_Carrier)->InterceptorPop(this);
		ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_INTERCEPTOR_DEAD), OBJ_EFFECT);
		return 1;
	}

	

	if (m_bCommand)
		m_bRender = true;

	if (m_bRender)
	{
		
		SetAttTarget(m_Carrier->GetAttTArget());
		if (m_AttTarget == NULL)
		{
			m_bCommand = false;
		}

		Function();
		Cloocking();
		Frame();
		Move();
	}
	else
	{
		m_tInfo.vPos = m_Carrier->GetPos();
	}

	return 0;
}

void CInterceptor::Render(void)
{
	if (!m_bRender)
		return;

	D3DXMATRIX matScale, matTrans;

	if (m_bSelect)
	{
		CObjectRenderMgr::SelectUnitUIRender(this, 0, 4.f, 0, 20.f);
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

void CInterceptor::Release(void)
{
}

void CInterceptor::Function(void)
{
	if (m_dwUnitState == UNIT_STASISFIELD)
		return;

	if (!m_bCommand)
	{
		float fWidth = m_Carrier->GetPos().x - m_tInfo.vPos.x;
		float fHeight = m_Carrier->GetPos().y - m_tInfo.vPos.y;

		float fDistance = sqrtf((fWidth * fWidth) + (fHeight * fHeight));

		float fAngle = acosf(fWidth / fDistance);

		if (m_Carrier->GetPos().y < m_tInfo.vPos.y)
			fAngle *= -1.f;

		m_fAngle = D3DXToDegree(fAngle);

		m_tInfo.vDir = m_Carrier->GetPos() - m_tInfo.vPos;

		float fDist = D3DXVec3Length(&m_tInfo.vDir);

		D3DXVec3Normalize(&m_tInfo.vDir, &m_tInfo.vDir);

		m_tInfo.vPos += m_tInfo.vDir * 800.f * GET_TIME;

		if (m_Carrier->GetState() == UNIT_STAND &&
			m_Carrier->GetPos().x - m_Carrier->GetInfo().vSize.x / 2.f < m_tInfo.vPos.x &&
			m_Carrier->GetPos().x + m_Carrier->GetInfo().vSize.x / 2.f > m_tInfo.vPos.x &&
			m_Carrier->GetPos().y - m_Carrier->GetInfo().vSize.y / 2.f < m_tInfo.vPos.y &&
			m_Carrier->GetPos().y + m_Carrier->GetInfo().vSize.y / 2.f > m_tInfo.vPos.y)
		{
			m_bRender = false;
			m_tUnitInfo.iShield = m_tUnitInfo.iShieldMax;
			m_tUnitInfo.iHp = m_tUnitInfo.iHpMax;
		}
	}
	else
	{
			

		if (m_bPattonClear)
		{
			
			m_vEndPoint.x = (m_AttTarget->GetPos().x + rand() % 500 - 250);
			m_vEndPoint.y = (m_AttTarget->GetPos().y + rand() % 500 - 250);
	

			m_dwState = UNIT_MOVE;

			m_fSpeed = S_FLOAT_CAST(230.f + rand() % 220);
			m_bPattonClear = false;
			m_bPattonAttack = true;
			lstrcpy(m_tInfo.szStateKey, L"Normal");
		}
		else
		{
			if (m_bPattonAttack &&
				fabs(m_tInfo.vPos.x - m_AttTarget->GetPos().x) < 50.f &&
				fabs(m_tInfo.vPos.y - m_AttTarget->GetPos().y) < 50.f)
			{
				SoundMgr->PlaySound(L"phohit00.wav", CHANNEL_EFFECT_ATT4, 0.5f, m_tInfo.vPos);
				m_bPattonAttack = false;
				ObjMgr->AddGameObject(CFactory<CAttEffect>::CreateGameUnitEffectTarget(m_tInfo.vPos, EFFECT_INTERCEPTOR_ATT, m_AttTarget, m_tUnitInfo), OBJ_EFFECT);
				lstrcpy(m_tInfo.szStateKey, L"Att");
			}
		}
	}
}

void CInterceptor::Cloocking(void)
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

void CInterceptor::Frame(void)
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

void CInterceptor::Move(void)
{
	if (m_dwState != UNIT_MOVE || m_dwUnitState == UNIT_STASISFIELD)
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
		m_bPattonClear = true;
	}
}