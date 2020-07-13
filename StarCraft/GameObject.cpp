#include "stdafx.h"
#include "GameObject.h"

D3DXVECTOR3 CGameObject::m_vScroll = D3DXVECTOR3(0.f, 0.f, 0.f);


CGameObject::CGameObject()
{
	for(int i = 0; i < 9; ++i)
		m_iClickMyInterpace[i] = 0;
	m_bDeadCheck = false;
	m_fAngle = 0.f;
	ZeroMemory(&m_tInfo, sizeof(INFO));
	ZeroMemory(&m_tUnitInfo, sizeof(UNIT));
	ZeroMemory(&m_tBuildInfo, sizeof(BUILD));
	m_bSelect = false;
	D3DXMatrixIdentity(&m_tInfo.matWorld);
	m_eBuildType = BUILD_END;
	m_eBuildCreateType = BUILD_CREATE_NOMAL;
	m_bCollision = true;
	m_eAlphType = ALPH_NOMAL;
	m_tAlphCount = FRAME(0.f, 250.f, 150.f);
	m_dwState = UNIT_NORMAL;
	UNIT_NORMAL;
	m_AttTarget = NULL;
	m_bUnitAtt = false;
	m_iArmor = 0;
	m_iShield = 0;
	m_iStasis = 0;
	m_tStateCount = FRAME(0.f, 24.f, 24.f);
	m_iStateCount = 0;
	m_iView = 0;

	m_bAI2 = false;
	m_bAI = false;
}

CGameObject::~CGameObject()
{
}
