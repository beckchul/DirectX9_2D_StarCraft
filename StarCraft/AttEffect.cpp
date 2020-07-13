#include "stdafx.h"
#include "AttEffect.h"
#include "Device.h"
#include "TextureMgr.h"
#include "TimeMgr.h"
#include "BackGround.h"
#include "ObjMgr.h"
#include "Factory.h"
#include "Effect.h"

CAttEffect::CAttEffect()
{
}


CAttEffect::~CAttEffect()
{
	Release();
}

HRESULT CAttEffect::Initialize(void)
{
	switch (m_eEffectType)
	{
	case EFFECT_DRAGOON_ATT:
	case EFFECT_PHOTON_ATT:
	case EFFECT_ARBITER_ATT:
		m_tFrame = FRAME(0.f, 30.f, 4.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Ball");
		m_eRenderType = RENDER_GROUND_EFFECT;
		m_iFrameCount = 0;
		m_tInfo.vSize = D3DXVECTOR3(16.f, 16.f, 0.f);
		m_fSpeed = 500.f;
		break;
	case EFFECT_ARCHON_ATT:
		m_tFrame = FRAME(0.f, 20.f, 5.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Shockball");
		m_eRenderType = RENDER_AIR_EFFECT;
		m_tInfo.vSize = D3DXVECTOR3(50.f, 50.f, 0.f);
		m_iFrameCount = 0;
		break;
	case EFFECT_SCOUT_ATT:
		m_tFrame = FRAME(0.f, 30.f, 4.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Scout_Air");
		m_eRenderType = RENDER_GROUND_EFFECT;
		m_tInfo.vSize = D3DXVECTOR3(16.f, 16.f, 0.f);
		m_fSpeed = 100.f;
		m_tCreateTime = FRAME(0.f, 20.f, 5.f);
		break;
	case EFFECT_SCOUT_ATT2:
		m_tFrame = FRAME(0.f, 30.f, 4.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Scout_Air");
		m_eRenderType = RENDER_GROUND_EFFECT;
		m_tInfo.vSize = D3DXVECTOR3(16.f, 16.f, 0.f);
		m_tCreateTime = FRAME(0.f, 20.f, 5.f);
		m_fSpeed = 100.f;
		break;
	case EFFECT_STORM:
		m_tFrame = FRAME(0.f, 30.f, 13.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Storm");
		m_eRenderType = RENDER_GROUND_EFFECT;
		m_tInfo.vSize = D3DXVECTOR3(80.f, 70.f, 0.f);
		m_tCreateTime = FRAME(0.f, 5.f, 8.f);
		m_fSpeed = 0.f;
		m_iArmor = 0;
		break;
	case ERRECT_CORSAIR_ATT:
		m_tFrame = FRAME(0.f, 30.f, 6.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Corsair_Splash");
		m_eRenderType = RENDER_AIR_EFFECT;
		m_tInfo.vSize = D3DXVECTOR3(40.f, 40.f, 0.f);
		m_iFrameCount = 0;
		break;
	case EFFECT_INTERCEPTOR_ATT:
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Intercep_Bullet");
		m_eRenderType = RENDER_AIR_EFFECT;
		m_iFrameCount = 0;
		m_fSpeed = 500.f;
		break;
	}


	int iX = S_INT_CAST(m_tInfo.vPos.x / TILECX);
	int iY = S_INT_CAST(m_tInfo.vPos.y / TILECY);

	int iIndex = iX + (TILEEX * iY);

	if (iIndex < 0 || iIndex >= TILEEX * TILEEY)
		return S_FALSE;

	CGameObject* pObj = ObjMgr->GetList()[OBJ_BACKGROUND].front();



	if (D_CAST(CBackGround, pObj)->GetTileOption(iIndex) == 0)
	{
		m_bGround = true;
	}
	else
	{
		m_bGround = false;
	}

	return S_OK;
}

int CAttEffect::Update(void)
{
	Angle();
	Frame();
	Function();
	if (m_bDeadCheck)
	{
		switch (m_eEffectType)
		{
		case EFFECT_DRAGOON_ATT:
			SoundMgr->PlaySound(L"explosm.wav", CHANNEL_EFFECT3, 0.3f, m_tInfo.vPos);
			ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_DRAGOON_ATT_EX), OBJ_EFFECT);
			break;
		case EFFECT_SCOUT_ATT:
		case EFFECT_SCOUT_ATT2:
		case EFFECT_PHOTON_ATT:
		case EFFECT_ARBITER_ATT:
			ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_SCOUT_ATT_EX), OBJ_EFFECT);
			SoundMgr->PlaySound(L"explosm.wav", CHANNEL_EFFECT3, 0.3f, m_tInfo.vPos);
			break;
		}
		return 1;
	}

	return 0;
}

void CAttEffect::Render(void)
{
	D3DXMATRIX matTrans;

	const TEX_INFO* pEffectture = TextureMgr->GetTexture(m_tInfo.szObjKey
		, m_tInfo.szStateKey, S_INT_CAST(m_tFrame.fFrame) + m_iFrameCount);

	D3DXMatrixTranslation(&matTrans
		, m_tInfo.vPos.x - m_vScroll.x
		, m_tInfo.vPos.y - m_vScroll.y
		, 0.f);

	float fX = pEffectture->ImageInfo.Width / 2.f;
	float fY = pEffectture->ImageInfo.Height / 2.f;


	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pEffectture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CAttEffect::Release(void)
{
}


void CAttEffect::Frame(void)
{
	if (m_eEffectType == EFFECT_SCOUT_ATT || m_eEffectType == EFFECT_SCOUT_ATT2 
		|| m_eEffectType == EFFECT_INTERCEPTOR_ATT)
		return;

	m_fFrameTime += GET_TIME * m_tFrame.fCount;

	if (m_fFrameTime >= 1.f)
	{
		m_fFrameTime = 0.f;
		++m_iFrameCount;
	}

	if (m_tFrame.fFrame + m_iFrameCount > m_tFrame.fMax)
	{
		switch (m_eEffectType)
		{
		case EFFECT_DRAGOON_ATT:
		case EFFECT_PHOTON_ATT:
		case EFFECT_STORM:
		case EFFECT_ARBITER_ATT:
			m_iFrameCount = 0;
			break;
		case EFFECT_ARCHON_ATT:
		{
			m_bDeadCheck = true;

			list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

			if (m_pTarget->GetBuildType() == BUILD_END)
				CDamageMgr::DamageCalculationSplash(this, m_pTarget, pUnitList);
			else
				CDamageMgr::DamageCalculationBuild(m_tUnitInfo.iAtt, m_pTarget);
		}

		break;
		case ERRECT_CORSAIR_ATT:
		{
			m_bDeadCheck = true;

			list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];
			CDamageMgr::DamageCalculationSplash(this, pUnitList);
		}
		break;
		}
	}

	if (m_eEffectType == EFFECT_STORM)
	{
		m_fSpeed += GET_TIME * m_tCreateTime.fCount;

		if (m_fSpeed >= 1.f)
		{
			m_fSpeed = 0;
			m_iArmor++;
			list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];
			CDamageMgr::DamageCalculationSplashAll(m_tInfo.vPos, m_tInfo.vSize, pUnitList);
		}

		if (m_tCreateTime.fFrame + m_iArmor >= m_tCreateTime.fMax)
			m_bDeadCheck = true;
	}
}

void CAttEffect::Function(void)
{
	if (m_eEffectType == EFFECT_STORM || m_eEffectType == EFFECT_ARCHON_ATT || m_eEffectType == ERRECT_CORSAIR_ATT)
		return;

	if (m_pTarget == NULL || m_pTarget->GetDeadCheck())
	{
		m_bDeadCheck = true;
		return;
	}

	if (m_eEffectType == EFFECT_SCOUT_ATT)
	{

		if (m_tCreateTime.fFrame < m_tCreateTime.fMax)
		{
			m_tCreateTime.fFrame += GET_TIME * m_tCreateTime.fCount;

			m_tInfo.vDir = m_pTarget->GetInfo().vPos + D3DXVECTOR3(100.f, 0.f, 0.f) - m_tInfo.vPos;

			float fDist = D3DXVec3Length(&m_tInfo.vDir);

			D3DXVec3Normalize(&m_tInfo.vDir, &m_tInfo.vDir);

			m_tInfo.vPos += m_tInfo.vDir * m_fSpeed * GET_TIME;
		}
		else
		{
			m_fSpeed = 400.f;
			m_tInfo.vDir = m_pTarget->GetInfo().vPos - m_tInfo.vPos;

			float fDist = D3DXVec3Length(&m_tInfo.vDir);

			D3DXVec3Normalize(&m_tInfo.vDir, &m_tInfo.vDir);

			m_tInfo.vPos += m_tInfo.vDir * m_fSpeed * GET_TIME;
		}
	}
	else if (m_eEffectType == EFFECT_SCOUT_ATT2)
	{
		if (m_tCreateTime.fFrame < m_tCreateTime.fMax)
		{
			m_tCreateTime.fFrame += GET_TIME * m_tCreateTime.fCount;

			m_tInfo.vDir = m_pTarget->GetInfo().vPos - D3DXVECTOR3(100.f, 0.f, 0.f) - m_tInfo.vPos;

			float fDist = D3DXVec3Length(&m_tInfo.vDir);

			D3DXVec3Normalize(&m_tInfo.vDir, &m_tInfo.vDir);

			m_tInfo.vPos += m_tInfo.vDir * m_fSpeed * GET_TIME;
		}
		else
		{
			m_fSpeed = 400.f;
			m_tInfo.vDir = m_pTarget->GetInfo().vPos - m_tInfo.vPos;

			float fDist = D3DXVec3Length(&m_tInfo.vDir);

			D3DXVec3Normalize(&m_tInfo.vDir, &m_tInfo.vDir);

			m_tInfo.vPos += m_tInfo.vDir * m_fSpeed * GET_TIME;
		}
	}
	else
	{
		m_tInfo.vDir = m_pTarget->GetInfo().vPos - m_tInfo.vPos;

		float fDist = D3DXVec3Length(&m_tInfo.vDir);

		D3DXVec3Normalize(&m_tInfo.vDir, &m_tInfo.vDir);

		m_tInfo.vPos += m_tInfo.vDir * m_fSpeed * GET_TIME;
	}
		



	if (fabs(m_tInfo.vPos.x - m_pTarget->GetInfo().vPos.x) < fabs(m_tInfo.vSize.x / 2.f + m_pTarget->GetInfo().vSize.x / 2.f) &&
		fabs(m_tInfo.vPos.y - m_pTarget->GetInfo().vPos.y) < fabs(m_tInfo.vSize.y / 2.f + m_pTarget->GetInfo().vSize.y / 2.f))
	{
		if (m_eEffectType == EFFECT_DRAGOON_ATT || m_eEffectType == EFFECT_INTERCEPTOR_ATT)
		{
			if(m_pTarget->GetBuildType() == BUILD_END)
				CDamageMgr::DamageCalculation(m_tUnitInfo.iAtt, m_pTarget);
			else
				CDamageMgr::DamageCalculationBuild(m_tUnitInfo.iAtt, m_pTarget);
		}
		else if (m_eEffectType == EFFECT_ARBITER_ATT)
		{
			if (m_pTarget->GetBuildType() == BUILD_END)
				CDamageMgr::DamageCalculation(m_tUnitInfo.iAtt, m_pTarget);
			else
				CDamageMgr::DamageCalculationBuild(m_tUnitInfo.iAtt, m_pTarget);
		}
		else if (m_eEffectType == EFFECT_PHOTON_ATT)
		{
			if (m_pTarget->GetBuildType() == BUILD_END)
				CDamageMgr::DamageCalculation(20, m_pTarget);
			else
				CDamageMgr::DamageCalculationBuild(20, m_pTarget);
		}
			
		else if (m_eEffectType == EFFECT_SCOUT_ATT || m_eEffectType == EFFECT_SCOUT_ATT2)
			CDamageMgr::DamageCalculation(S_INT_CAST(m_tUnitInfo.iAtt * 1.5f), m_pTarget);

		m_bDeadCheck = true;
	}
}

void CAttEffect::Angle(void)
{
	if (EFFECT_SCOUT_ATT != m_eEffectType && m_eEffectType != EFFECT_SCOUT_ATT2 
		&& m_eEffectType != EFFECT_INTERCEPTOR_ATT)
		return;

	float fWidth = m_pTarget->GetInfo().vPos.x - m_tInfo.vPos.x;
	float fHeight = m_pTarget->GetInfo().vPos.y - m_tInfo.vPos.y;

	float fDistance = sqrtf((fWidth * fWidth) + (fHeight * fHeight));

	float fAngle = acosf(fWidth / fDistance);

	if (m_pTarget->GetInfo().vPos.y < m_tInfo.vPos.y)
		fAngle *= -1.f;

	m_fAngle = D3DXToDegree(fAngle);

	switch (m_eEffectType)
	{
	case EFFECT_SCOUT_ATT:
	case EFFECT_SCOUT_ATT2:
	case EFFECT_INTERCEPTOR_ATT:
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
}