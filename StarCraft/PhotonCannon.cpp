#include "stdafx.h"
#include "PhotonCannon.h"
#include "BackGround.h"
#include "TextureMgr.h"
#include "Device.h"
#include "ObjMgr.h"
#include "KeyMgr.h"
#include "Mouse.h"
#include "TimeMgr.h"
#include "AttEffect.h"
#include "Factory.h"
#include "Interpace.h"
#include "Effect.h"

CPhotonCannon::CPhotonCannon()
{
}

CPhotonCannon::~CPhotonCannon()
{
	Release();
}

HRESULT CPhotonCannon::Initialize(void)
{
	list<BUILD*> BuildList = ObjMgr->GetBuildList();

	list<BUILD*>::iterator iter = BuildList.begin();
	list<BUILD*>::iterator iter_end = BuildList.end();

	lstrcpy(m_tInfo.szObjKey, L"Building");
	lstrcpy(m_tInfo.szStateKey, L"PhotonCannon");

	for (; iter != iter_end; ++iter)
	{
		if (!lstrcmp((*iter)->szName, m_tInfo.szStateKey))
		{
			m_tBuildInfo = (*(*iter));
			break;
		}
	}
	m_tInfo.vSize.x = S_FLOAT_CAST(m_tBuildInfo.iSizeX * TILECX) + 40;
	m_tInfo.vSize.y = S_FLOAT_CAST(m_tBuildInfo.iSizeY * TILECY);
	m_tClick = FRAME(0.f, 10.f, 2.f);
	m_bKeyInput = false;
	m_eBuildCreateType = BUILD_CREATE_NOMAL;
	m_eBuildType = BUILD_PHOTON_CANNON;
	m_vStartingPoint = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_tAttDelay = FRAME(0.f, 10.f, 10.f);
	m_dwState = UNIT_NORMAL;
	m_eRenderType = RENDER_GROUND;
	SettingTile();
	m_tFrame = FRAME(0.f, 12.f, 3.f);
	m_iFramecount = 0;
	m_fFrameC = 0.f;
	m_iView = 10;
	return S_OK;
}

int CPhotonCannon::Update(void)
{
	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
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
		SoundMgr->PlaySound(L"explo4.wav", CHANNEL_EFFECT_CREATE, 0.9f, m_tInfo.vPos);
		ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_BUILD_DEAD), OBJ_EFFECT);
		ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_BUILD_DEAD2), OBJ_EFFECT);
		SettingTile();
		return 1;
	}

	Function();
	SelectKeyCheck();
	return 0;
}

void CPhotonCannon::Render(void)
{
	D3DXMATRIX matScale, matTrans;

	if (m_bSelect)
	{
		CObjectRenderMgr::SelectBuildUIRender(this, 4, 0.f, 4, 30.f);
	}

	const TEX_INFO* pBuildtexture = TextureMgr->GetTexture(L"Shad"
		, L"Photon Cannon", 0);

	float fX = pBuildtexture->ImageInfo.Width / 2.f;
	float fY = pBuildtexture->ImageInfo.Height / 2.f;

	D3DXMatrixTranslation(&matTrans
		, m_tInfo.vPos.x - m_vScroll.x
		, m_tInfo.vPos.y - m_vScroll.y
		, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pBuildtexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(230, 255, 255, 255));

	pBuildtexture = TextureMgr->GetTexture(L"Building"
		, L"Photon Cannon", m_iFramecount + S_INT_CAST(m_tFrame.fFrame));

	D3DXMatrixTranslation(&matTrans
		, m_tInfo.vPos.x - m_vScroll.x
		, m_tInfo.vPos.y - m_vScroll.y
		, 0.f);

	fX = pBuildtexture->ImageInfo.Width / 2.f;
	fY = pBuildtexture->ImageInfo.Height / 2.f;

	Device->GetSprite()->SetTransform(&matTrans);
	if (m_eTeamType == TEAM_PLAYER)
	{
		Device->GetSprite()->Draw(pBuildtexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	else
	{
		Device->GetSprite()->Draw(pBuildtexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 160, 160));
	}
}

void CPhotonCannon::Release(void)
{
}

void CPhotonCannon::SelectKeyCheck(void)
{
	if (!m_bSelect)
		return;
	DWORD dwKey = GET_KEY;
	CGameObject* ptMouse = ObjMgr->GetList()[OBJ_MOUSE].front();
	D3DXVECTOR3 vMouse = D_CAST(CMouse, ptMouse)->GetMousePos();
	if (dwKey & KEY_A)
	{
		ptMouse->SetState(MOUSE_ATTACK_POINT_NOMAL);
	}

	if (dwKey & KEY_S)
	{
		m_dwState = UNIT_STAND;
		for (int i = 0; i < 9; ++i)
			m_iClickMyInterpace[i] = 0;
		m_iClickMyInterpace[1] = 1;
	}
}
void CPhotonCannon::Function(void)
{
	list<CGameObject*> pObjList = ObjMgr->GetList()[OBJ_BUILD];
	CDistanceMgr::GetLowDistanceAttTargetBuild(this, pObjList);
	pObjList = ObjMgr->GetList()[OBJ_UNIT];
	CDistanceMgr::GetLowDistanceAttTargetGround(this, pObjList);

	if (m_bUnitAtt && (m_AttTarget != NULL || m_AttTarget->GetState() != UNIT_DEAD))
	{
		m_dwState = UNIT_ATTACK;
	}
	else
	{
		m_dwState = UNIT_STAND;
	}

	m_tAttDelay.fFrame += GET_TIME * m_tAttDelay.fCount;

	if (m_dwState == UNIT_ATTACK)
	{
		m_fFrameC += GET_TIME * m_tFrame.fCount;

		if (m_fFrameC > 1)
		{
			++m_iFramecount;
			m_fFrameC = 0;
		}

		if (m_iFramecount + m_tFrame.fFrame > m_tFrame.fMax)
		{
			--m_iFramecount;
			if (m_tAttDelay.fFrame > m_tAttDelay.fMax)
			{
				m_tAttDelay.fFrame = 0.f;
				if(m_AttTarget != NULL)
				ObjMgr->AddGameObject(CFactory<CAttEffect>::CreateGameUnitEffectTarget(m_tInfo.vPos + D3DXVECTOR3(0.f, - 15.f, 0.f), EFFECT_PHOTON_ATT, m_AttTarget, m_tUnitInfo), OBJ_EFFECT);
			}
		}
	}

	if (m_dwState == UNIT_STAND)
	{
		m_fFrameC += GET_TIME * m_tFrame.fCount;

		if (m_fFrameC > 1)
		{
			--m_iFramecount;
			m_fFrameC = 0;
		}

		if (m_iFramecount + m_tFrame.fFrame < 0)
		{
			m_iFramecount = 0;
		}
	}
}

void CPhotonCannon::CheckOneClick(void)
{
	D3DXMATRIX matTrans;

	if (m_dwState != UNIT_ATTACK)
	{
		m_iClickMyInterpace[1] = 1;
		m_iClickMyInterpace[2] = 0;
	}
	else
	{
		m_iClickMyInterpace[1] = 0;
		m_iClickMyInterpace[2] = 1;
	}

	// 2¹øÂ° Ä­
	const TEX_INFO* pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"Stop", m_iClickMyInterpace[1]);

	float fX = pInterpacetexture->ImageInfo.Width / 2.f;
	float fY = pInterpacetexture->ImageInfo.Height / 2.f;

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
}

void CPhotonCannon::CreateUnit(bool Check)
{
}


void CPhotonCannon::SettingTile(void)
{
	int iX = S_INT_CAST(m_tInfo.vPos.x / TILECX);
	int iY = S_INT_CAST(m_tInfo.vPos.y / TILECY);

	int iIndex = iX + (TILEEX * iY);

	if (iIndex < 0 || iIndex >= TILEEX * TILEEY)
		return;

	CGameObject* pObj = ObjMgr->GetList()[OBJ_BACKGROUND].front();

	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - 1);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX - 1);
}

