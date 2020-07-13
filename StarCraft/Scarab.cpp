#include "stdafx.h"
#include "Scarab.h"
#include "AStar.h"
#include "Device.h"
#include "TextureMgr.h"
#include "TimeMgr.h"
#include "ObjMgr.h"
#include "BackGround.h"
#include "Factory.h"
#include "Effect.h"
#include "Interpace.h"

CScarab::CScarab()
{
}

CScarab::~CScarab()
{
	Release();
}

HRESULT CScarab::Initialize(void)
{
	m_pAStar = new CAStar;
	lstrcpy(m_tInfo.szObjKey, L"Effect");
	lstrcpy(m_tInfo.szStateKey, L"Scarab");

	lstrcpy(m_tUnitInfo.szName, L"Scarab");
	m_iArmor = 100;
	m_tInfo.vSize = D3DXVECTOR3(8.f, 8.f, 0.f);
	m_tUnitInfo.iAtt = 100;
	m_tCreateTime = FRAME(1.f, 1.f, 1.f);
	m_tUnitInfo.eType = UNIT_GROUND;
	m_dwState = UNIT_DEAD;
	m_fSpeed = 260.f;
	m_tStateCount = FRAME(1.f, 8.f, 1.f);

	if (m_AttTarget != NULL && m_AttTarget->GetState() != UNIT_DEAD)
		m_pAStar->StartAStarPos(m_tInfo.vPos, m_AttTarget->GetPos());
	return S_OK;
}

int CScarab::Update(void)
{

	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();

	if (D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_REAVER_DAMAGE) == 1)
		m_iArmor = 125;

	m_tCreateTime.fFrame += GET_TIME * m_tCreateTime.fCount;

	if (m_tCreateTime.fFrame >= m_tCreateTime.fMax)
	{
		m_tCreateTime.fFrame = 0.f;
		
	}

	m_tStateCount.fFrame += GET_TIME * m_tStateCount.fCount;
	if (m_tStateCount.fFrame >= m_tStateCount.fMax)
	{
		m_tStateCount.fFrame = 0.f;
		ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_SCARAB_MOVE), OBJ_EFFECT);
	}

	AStarMove();
	Frame();

	if (m_bDeadCheck)
	{

		if (m_AttTarget->GetBuildType() == BUILD_END)
		{
			list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];
			CDamageMgr::DamageCalculationSplashEnemy(m_tInfo.vPos, m_iArmor, D3DXVECTOR3(60.f, 60.f, 0.f), pUnitList, m_eTeamType);
		}
		else
			CDamageMgr::DamageCalculationBuild(m_iArmor, m_AttTarget);

		
		SoundMgr->PlaySound(L"psahit00.wav", CHANNEL_EFFECT_ATT, 0.5f, m_tInfo.vPos);
		ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_SCARAB_DEAD), OBJ_EFFECT);
		return 1;
	}
	return 0;
}

void CScarab::Render(void)
{
	D3DXMATRIX matScale, matTrans;

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

	Device->GetSprite()->SetTransform(&matScale);
	Device->GetSprite()->Draw(pShuttletexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CScarab::Release(void)
{
}

void CScarab::Frame(void)
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
}

void CScarab::AStarMove(void)
{
	list<int>* pBestList = m_pAStar->GetBestList();

	if (pBestList->empty())
	{
		m_bDeadCheck = true;
		return;
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
