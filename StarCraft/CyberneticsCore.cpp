#include "stdafx.h"
#include "CyberneticsCore.h"
#include "ObjMgr.h"
#include "Device.h"
#include "TextureMgr.h"
#include "BackGround.h"
#include "TimeMgr.h"
#include "Interpace.h"
#include "KeyMgr.h"
#include "Mouse.h"
#include "Effect.h"
#include "Factory.h"

CCyberneticsCore::CCyberneticsCore()
{
}

CCyberneticsCore::~CCyberneticsCore()
{
	Release();
}

HRESULT CCyberneticsCore::Initialize(void)
{
	list<BUILD*> BuildList = ObjMgr->GetBuildList();

	list<BUILD*>::iterator iter = BuildList.begin();
	list<BUILD*>::iterator iter_end = BuildList.end();

	lstrcpy(m_tInfo.szObjKey, L"Building");
	lstrcpy(m_tInfo.szStateKey, L"CyberneticsCore");

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
	m_eBuildType = BUILD_CYBERNETICS_CORE;
	m_vStartingPoint = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_iView = 10;
	m_eRenderType = RENDER_GROUND;
	SettingTile();
	return S_OK;
}

int CCyberneticsCore::Update(void)
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

void CCyberneticsCore::Render(void)
{
	D3DXMATRIX matScale, matTrans;

	if (m_bSelect)
	{
		CObjectRenderMgr::SelectBuildUIRender(this, 6, 0.f, 6, 40.f);
	}

	const TEX_INFO* pBuildtexture = TextureMgr->GetTexture(L"Shad"
		, L"Cybernetics Core", 0);

	float fX = pBuildtexture->ImageInfo.Width / 2.f;
	float fY = pBuildtexture->ImageInfo.Height / 2.f;

	D3DXMatrixTranslation(&matTrans
		, m_tInfo.vPos.x - m_vScroll.x
		, m_tInfo.vPos.y - m_vScroll.y
		, 0.f);
	if (m_bCreateUpgrade)
	{
		pBuildtexture = TextureMgr->GetTexture(L"Building"
			, L"Cybernetics Core", S_INT_CAST(m_tFrame.fFrame));

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
	else
	{
		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pBuildtexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(230, 255, 255, 255));

		pBuildtexture = TextureMgr->GetTexture(L"Building"
			, L"Cybernetics Core", 0);

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
}

void CCyberneticsCore::Release(void)
{
}

void CCyberneticsCore::CheckOneClick(void)
{
	D3DXMATRIX matTrans;
	float fX = 0.f, fY = 0.f;
	const TEX_INFO* pInterpacetexture = NULL;

	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
	list<CGameObject*> pBuildList = ObjMgr->GetList()[OBJ_BUILD];

	bool bCheck1 = false;
	for (list<CGameObject*>::iterator iter = pBuildList.begin(); iter != pBuildList.end(); ++iter)
	{
		if ((*iter)->GetBuildType() == BUILD_FLEET_BEACON && (*iter)->GetBuildCreateType() != BUILD_CREATE_CREATE)
		{
			bCheck1 = true;
		}
	}

	if (0 != D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT) && !bCheck1)
	{
		m_iClickMyInterpace[0] = 1;
	}
	else
	{
		m_iClickMyInterpace[0] = 0;
	}
	if (0 != D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR) && !bCheck1)
	{
		m_iClickMyInterpace[1] = 1;
	}
	else
	{
		m_iClickMyInterpace[1] = 0;
	}


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
		if (3 > D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT))
		{
			// 1번째 칸
			pInterpacetexture = TextureMgr->GetTexture(L"Button"
				, L"ArrAttUpgrade", m_iClickMyInterpace[0]);

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

		if (3 > D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR))
		{
			// 2번째 칸
			pInterpacetexture = TextureMgr->GetTexture(L"Button"
				, L"ArrArmorUpgrade", m_iClickMyInterpace[1]);

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
		}
		else
		{
			m_iClickMyInterpace[1] = 3;
		}
		if (0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_DRAGOON_RANGE))
		{
			// 3번째 칸
			pInterpacetexture = TextureMgr->GetTexture(L"Button"
				, L"RangeUP", m_iClickMyInterpace[2]);

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
		else
		{
			m_iClickMyInterpace[2] = 3;
		}
		
	}
}

void CCyberneticsCore::CreateUnit(bool Check)
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

				if (m_eTeamType == TEAM_PLAYER)
				{
					ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_PING), OBJ_EFFECT);
					SoundMgr->PlaySound(L"PAdUpd06.wav", CHANNEL_MAIN, 0.9f);
				}
				

				if (!lstrcmp(m_szKey, L"W"))
				{
					if (0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT))
					{
						D_CAST(CInterpace, pInterpace)->SetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT, 1);
					}
					else if (1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT))
					{
						D_CAST(CInterpace, pInterpace)->SetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT, 2);
					}
					else if (2 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT))
					{
						D_CAST(CInterpace, pInterpace)->SetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT, 3);
					}
				}
				else if (!lstrcmp(m_szKey, L"A"))
				{
					if (0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR))
					{
						D_CAST(CInterpace, pInterpace)->SetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR, 1);
					}
					else if (1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR))
					{
						D_CAST(CInterpace, pInterpace)->SetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR, 2);
					}
					else if (2 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR))
					{
						D_CAST(CInterpace, pInterpace)->SetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR, 3);
					}
				}
				else if (!lstrcmp(m_szKey, L"S"))
				{
					if (0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_DRAGOON_RANGE))
					{
						D_CAST(CInterpace, pInterpace)->SetUpgradeCount(m_eTeamType, UPGRADE_DRAGOON_RANGE, 1);
					}
				}

				m_eBuildCreateType = BUILD_CREATE_NOMAL;
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

		if (!lstrcmp(m_szKey, L"W"))
		{
			pBigWireTexture = TextureMgr->GetTexture(L"Produce"
				, L"Icon", 15);
		}
		else if (!lstrcmp(m_szKey, L"A"))
		{
			pBigWireTexture = TextureMgr->GetTexture(L"Produce"
				, L"Icon", 16);
		}
		else if (!lstrcmp(m_szKey, L"S"))
		{
			pBigWireTexture = TextureMgr->GetTexture(L"Produce"
				, L"Icon", 17);
		}


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

void CCyberneticsCore::Function(void)
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
			// 자원반환 넣기
			if (!lstrcmp(m_szKey, L"W"))
			{
				if (0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT))
				{
					D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 100);
					D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 100);
				}
				else if (1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT))
				{
					D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 175);
					D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 175);
				}
				else if (2 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT))
				{
					D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 250);
					D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 250);
				}
			}
			else if (!lstrcmp(m_szKey, L"A"))
			{
				if (0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR))
				{
					D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 150);
					D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 150);
				}
				else if (1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR))
				{
					D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 225);
					D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 225);
				}
				else if (2 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR))
				{
					D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 300);
					D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 300);
				}
			}
			else if (!lstrcmp(m_szKey, L"S"))
			{
				if (0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_DRAGOON_RANGE))
				{
					D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 150);
					D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 150);
				}
			}

			m_bKeyInput = true;
			m_eBuildCreateType = BUILD_CREATE_NOMAL;
			m_bCreateUpgrade = false;
			return;
		}
		if (dwKey & KEY_X && !m_bKeyInput)
		{
			if (!lstrcmp(m_szKey, L"W"))
			{
				if (0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT))
				{
					D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 100);
					D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 100);
				}
				else if (1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT))
				{
					D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 175);
					D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 175);
				}
				else if (2 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT))
				{
					D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 250);
					D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 250);
				}
			}
			else if (!lstrcmp(m_szKey, L"A"))
			{
				if (0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR))
				{
					D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 150);
					D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 150);
				}
				else if (1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR))
				{
					D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 225);
					D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 225);
				}
				else if (2 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR))
				{
					D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 300);
					D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 300);
				}
			}
			else if (!lstrcmp(m_szKey, L"S"))
			{
				if (0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_DRAGOON_RANGE))
				{
					D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 150);
					D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, 150);
				}
			}

			m_bKeyInput = true;
			m_eBuildCreateType = BUILD_CREATE_NOMAL;
			m_bCreateUpgrade = false;
		}
	}
	else
	{
		if (dwKey & KEY_W && !m_bKeyInput &&
			m_iClickMyInterpace[0] == 0)
		{
			if (0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT))
			{
				if (iMineral >= 100)
				{
					if (iGas >= 100)
					{
						D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -100);
						D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -100);
						m_eBuildCreateType = BUILD_CREATE_UNIT;
						lstrcpy(m_szKey, L"W");
					}
				}
			}
			else if (1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT))
			{
				if (iMineral >= 175)
				{
					if (iGas >= 175)
					{
						D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -175);
						D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -175);
						m_eBuildCreateType = BUILD_CREATE_UNIT;
						lstrcpy(m_szKey, L"W");
					}
				}
			}
			else if (2 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT))
			{
				if (iMineral >= 250)
				{
					if (iGas >= 250)
					{
						D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -250);
						D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -250);
						m_eBuildCreateType = BUILD_CREATE_UNIT;
						lstrcpy(m_szKey, L"W");
					}
				}
			}
		}

		if (dwKey & KEY_A && !m_bKeyInput &&
			m_iClickMyInterpace[1] == 0)
		{
			if (0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR))
			{
				if (iMineral >= 150)
				{
					if (iGas >= 150)
					{
						D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -150);
						D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -150);
						m_eBuildCreateType = BUILD_CREATE_UNIT;
						lstrcpy(m_szKey, L"A");
					}
				}
			}
			else if (1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR))
			{
				if (iMineral >= 225)
				{
					if (iGas >= 225)
					{
						D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -225);
						D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -225);
						m_eBuildCreateType = BUILD_CREATE_UNIT;
						lstrcpy(m_szKey, L"A");
					}
				}
			}
			else if (2 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR))
			{
				if (iMineral >= 300)
				{
					if (iGas >= 300)
					{
						D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -300);
						D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -300);
						m_eBuildCreateType = BUILD_CREATE_UNIT;
						lstrcpy(m_szKey, L"A");
					}
				}
			}
		}

		if (dwKey & KEY_S && !m_bKeyInput &&
			m_iClickMyInterpace[2] == 0)
		{
			if (0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_DRAGOON_RANGE))
			{
				if (iMineral >= 150)
				{
					if (iGas >= 150)
					{
						D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -150);
						D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -150);
						m_eBuildCreateType = BUILD_CREATE_UNIT;
						lstrcpy(m_szKey, L"S");
					}
				}
			}
		}

		if (dwKey & KEY_LBUTTON && !m_bKeyInput)
		{
			m_bKeyInput = true;
			// 첫번째 칸
			if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 0) &&
				m_iClickMyInterpace[0] == 0)
			{
				if (0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT))
				{
					if (iMineral >= 100)
					{
						if (iGas >= 100)
						{
							D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -100);
							D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -100);
							m_eBuildCreateType = BUILD_CREATE_UNIT;
							lstrcpy(m_szKey, L"W");
						}
					}
				}
				else if (1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT))
				{
					if (iMineral >= 175)
					{
						if (iGas >= 175)
						{
							D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -175);
							D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -175);
							m_eBuildCreateType = BUILD_CREATE_UNIT;
							lstrcpy(m_szKey, L"W");
						}
					}
				}
				else if (2 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ATT))
				{
					if (iMineral >= 250)
					{
						if (iGas >= 250)
						{
							D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -250);
							D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -250);
							m_eBuildCreateType = BUILD_CREATE_UNIT;
							lstrcpy(m_szKey, L"W");
						}
					}
				}
				return;
			}
			// 두번째 칸
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 1) &&
				m_iClickMyInterpace[1] == 0)
			{
				if (0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR))
				{
					if (iMineral >= 150)
					{
						if (iGas >= 150)
						{
							D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -150);
							D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -150);
							m_eBuildCreateType = BUILD_CREATE_UNIT;
							lstrcpy(m_szKey, L"A");
						}
					}
				}
				else if (1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR))
				{
					if (iMineral >= 225)
					{
						if (iGas >= 225)
						{
							D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -225);
							D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -225);
							m_eBuildCreateType = BUILD_CREATE_UNIT;
							lstrcpy(m_szKey, L"A");
						}
					}
				}
				else if (2 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_AIR_ARMOR))
				{
					if (iMineral >= 300)
					{
						if (iGas >= 300)
						{
							D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -300);
							D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -300);
							m_eBuildCreateType = BUILD_CREATE_UNIT;
							lstrcpy(m_szKey, L"A");
						}
					}
				}
				return;
			}
			// 세번째 칸
			else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 2) &&
				m_iClickMyInterpace[2] == 0)
			{
				if (0 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_DRAGOON_RANGE))
				{
					if (iMineral >= 150)
					{
						if (iGas >= 150)
						{
							D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -150);
							D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_eTeamType, -150);
							m_eBuildCreateType = BUILD_CREATE_UNIT;
							lstrcpy(m_szKey, L"S");
						}
					}
				}
				return;
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

void CCyberneticsCore::SettingTile(void)
{
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


