#include "stdafx.h"
#include "CitadelofAdun.h"
#include "ObjMgr.h"
#include "Device.h"
#include "TextureMgr.h"
#include "BackGround.h"
#include "TimeMgr.h"
#include "KeyMgr.h"
#include "Mouse.h"
#include "Interpace.h"
#include "Factory.h"
#include "Effect.h"

CCitadelofAdun::CCitadelofAdun()
{
}


CCitadelofAdun::~CCitadelofAdun()
{
	Release();
}

HRESULT CCitadelofAdun::Initialize(void)
{
	list<BUILD*> BuildList = ObjMgr->GetBuildList();

	list<BUILD*>::iterator iter = BuildList.begin();
	list<BUILD*>::iterator iter_end = BuildList.end();

	lstrcpy(m_tInfo.szObjKey, L"Building");
	lstrcpy(m_tInfo.szStateKey, L"CitadelofAdun");

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
	m_bCreateUpgrade = false;
	m_eBuildCreateType = BUILD_CREATE_NOMAL;
	m_eBuildType = BUILD_CITADEL_OF_ADUN;
	m_vStartingPoint = D3DXVECTOR3(0.f, 0.f, 0.f);

	m_eRenderType = RENDER_GROUND;
	SettingTile();
	m_iView = 10;
	return S_OK;
}

int CCitadelofAdun::Update(void)
{
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
	CreateUnit(true);

	return 0;
}

void CCitadelofAdun::Render(void)
{
	D3DXMATRIX matScale, matTrans;

	if (m_bSelect)
	{
		CObjectRenderMgr::SelectBuildUIRender(this, 6, 0.f, 6, 40.f);
	}

	const TEX_INFO* pBuildtexture = TextureMgr->GetTexture(L"Shad"
		, L"Citadel of Adun", 0);

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
		, L"Citadel of Adun", 0);

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

void CCitadelofAdun::Release(void)
{
}

void CCitadelofAdun::Function(void)
{
	if (!m_bSelect)
		return;

	DWORD dwKey = GET_KEY;

	CGameObject* ptMouse = ObjMgr->GetList()[OBJ_MOUSE].front();
	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
	D3DXVECTOR3 vMouse = D_CAST(CMouse, ptMouse)->GetRenderMousePos();
	int iMineral = D_CAST(CInterpace, pInterpace)->GetWorldMineral(m_eTeamType);
	int iGas = D_CAST(CInterpace, pInterpace)->GetWorldGas(m_eTeamType);
	int iHumun = D_CAST(CInterpace, pInterpace)->GetHumanCount(m_eTeamType);
	int iHumunMax = D_CAST(CInterpace, pInterpace)->GetHumanMaxCount(m_eTeamType);

	if (m_eBuildCreateType != BUILD_CREATE_NOMAL)
	{
		if (dwKey & KEY_LBUTTON && D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 8) && !m_bKeyInput)
		{
			D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 150);
			D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 150);

			m_bKeyInput = true;
			m_eBuildCreateType = BUILD_CREATE_NOMAL;
			m_bCreateUpgrade = false;
			return;
		}

		if (dwKey & KEY_X && !m_bKeyInput)
		{
			D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 150);
			D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 150);

			m_bKeyInput = true;
			m_eBuildCreateType = BUILD_CREATE_NOMAL;
			m_bCreateUpgrade = false;
			return;
		}
	}
	else
	{
		if (dwKey & KEY_L && !m_bKeyInput &&
			m_iClickMyInterpace[0] == 0)
		{
			if (0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_ZEALOT_SPEED) &&
				m_iClickMyInterpace[0] == 0)
			{
				m_bKeyInput = true;

				if (iMineral >= 100)
				{
					if (iGas >= 100)
					{
						D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -150);
						D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -150);
						m_eBuildCreateType = BUILD_CREATE_UNIT;
						lstrcpy(m_szKey, L"L");
						return;
					}
				}
			}
		}

		if (dwKey & KEY_LBUTTON && !m_bKeyInput)
		{
			m_bKeyInput = true;
			if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 0) && 
				0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_ZEALOT_SPEED) &&
				m_iClickMyInterpace[0] == 0)
			{
				m_bKeyInput = true;

				if (iMineral >= 150)
				{
					if (iGas >= 150)
					{
						D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -150);
						D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -150);
						m_eBuildCreateType = BUILD_CREATE_UNIT;
						lstrcpy(m_szKey, L"L");
						return;
					}
				}
			}
		}
	}

	if (!dwKey)
	{
		m_tClick.fFrame += GET_TIME * m_tClick.fCount;

		if (m_tClick.fFrame > m_tClick.fMax)
			m_bKeyInput = false;
	}
}

void CCitadelofAdun::CheckOneClick(void)
{
	D3DXMATRIX matTrans;
	const TEX_INFO* pInterpacetexture = NULL;
	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
	float fX = 0.f, fY = 0.f;

	if (m_eBuildCreateType != BUILD_CREATE_NOMAL)
	{
		// 9번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Cancel", m_iClickMyInterpace[8]);

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
	else
	{
		if (0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_ZEALOT_SPEED))
		{
			// 1번째 칸
			pInterpacetexture = TextureMgr->GetTexture(L"Button"
				, L"ZealotSpeedUP", m_iClickMyInterpace[0]);

			fX = pInterpacetexture->ImageInfo.Width / 2.f;
			fY = pInterpacetexture->ImageInfo.Height / 2.f;

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
		}
		else
		{
			m_iClickMyInterpace[0] = 3;
		}
	}
}

void CCitadelofAdun::CreateUnit(bool Check)
{
	if (Check) // 유닛 생산 시간 기능
	{
		if (!m_bCreateUpgrade)
		{
			if (m_eBuildCreateType == BUILD_CREATE_NOMAL)
				return;
			// 업그레이드 기능 만들어야함 // 넥서스 조금만 참조

			m_tCreateTime = FRAME(0.f, 1.f, 10.f);
			m_iFrameFirst = S_INT_CAST(m_tFrame.fFrame);
			m_bCreateUpgrade = true;
		}
		else
		{
			m_tCreateTime.fFrame += GET_TIME * m_tCreateTime.fCount;
			m_tFrame.fFrame += GET_TIME * m_tFrame.fCount;

			if (m_tFrame.fFrame > m_tFrame.fCount)
			{
				m_tFrame.fFrame = S_FLOAT_CAST(m_iFrameFirst);
			}

			if (m_tCreateTime.fFrame > m_tCreateTime.fMax)
			{
				m_bCreateUpgrade = false;
				m_tCreateTime.fFrame = m_tCreateTime.fMax;

				CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();

				D_CAST(CInterpace, pInterpace)->SetUpgradeCount(m_eTeamType, UPGRADE_ZEALOT_SPEED, 1);
				m_eBuildCreateType = BUILD_CREATE_NOMAL;
				return;
			}
		}
	}
	else	// 인터페이스 보여주는거	// 업그레이드 하는놈 띄울것
	{
		D3DXMATRIX matScale, matTrans;
		float fX = 0.f, fY = 0.f;

		const TEX_INFO* pBigWireTexture = TextureMgr->GetTexture(L"Produce"
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

		D3DXMatrixTranslation(&matTrans
			, 270.f
			, 418.f
			, 0.f);


		float fPercent = m_tCreateTime.fFrame / m_tCreateTime.fMax;

		RECT rc = { NULL, NULL
			, S_LONG_CAST(fPercent * fX * 2), S_LONG_CAST(fY * 2) };

		if (fPercent > 1.f)
			fPercent = 1.f;

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pBigWireTexture->pTexture
			, &rc
			, NULL //&D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		pBigWireTexture = TextureMgr->GetTexture(L"Produce"
			, L"Icon", 37);


	fX = pBigWireTexture->ImageInfo.Width / 2.f;
	fY = pBigWireTexture->ImageInfo.Height / 2.f;

	D3DXMatrixTranslation(&matTrans
		, 250.f
		, 420.f
		, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pBigWireTexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
}

void CCitadelofAdun::SettingTile(void)
{
	m_tInfo.vSize.x = S_FLOAT_CAST(m_tBuildInfo.iSizeX * TILECX);
	m_tInfo.vSize.y = S_FLOAT_CAST(m_tBuildInfo.iSizeY * TILECY);

	int iX = S_INT_CAST(m_tInfo.vPos.x / TILECX);
	int iY = S_INT_CAST(m_tInfo.vPos.y / TILECY);

	int iIndex = iX + (TILEEX * iY);

	if (iIndex < 0 || iIndex >= TILEEX * TILEEY)
		return;

	CGameObject* pObj = ObjMgr->GetList()[OBJ_BACKGROUND].front();

	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - 1);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex + 1);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX - 1);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX + 1);
}

