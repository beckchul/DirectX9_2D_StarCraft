#include "stdafx.h"
#include "CreateBuild.h"
#include "ObjMgr.h"
#include "Device.h"
#include "TextureMgr.h"
#include "TimeMgr.h"
#include "KeyMgr.h"
#include "Interpace.h"
#include "Mouse.h"
#include "Factory.h"
#include "BackGround.h"
#include "Nexus.h"
#include "Pylon.h"
#include "Assimilator.h"
#include "GateWay.h"
#include "Forge.h"
#include "PhotonCannon.h"
#include "CyberneticsCore.h"
#include "ShieldBattery.h"
#include "RoboticsFacility.h"
#include "StarGate.h"
#include "CitadelofAdun.h"
#include "RoboticsSupportBay.h"
#include "FleetBeacon.h"
#include "TemplarArchives.h"
#include "Observatory.h"
#include "ArbiterTribunal.h"

CCreateBuild::CCreateBuild()
{
}


CCreateBuild::~CCreateBuild()
{
	Release();
}



HRESULT CCreateBuild::Initialize(void)
{
	m_tCreateTime.fMax = 5;//S_FLOAT_CAST(m_tBuildInfo.iMakeTime);
	m_tCreateTime.fFrame = 0.f;
	m_tFrame = FRAME(0.f, 20.f, 11.f);
	m_bCreateEnd = false;
	m_tBuildInfo.iHp = 0;
	m_tBuildInfo.iShield = 0;
	m_eBuildCreateType = BUILD_CREATE_CREATE;
	m_iFrameFirst = 5;
	m_iSelectCount = 0;
	SettingBuildInfo();
	SettingTileOption();
	m_eRenderType = RENDER_GROUND;
	m_iView = 3;
	return S_OK;
}

int CCreateBuild::Update(void)
{

	m_tCreateTime.fCount += GET_TIME;

	if (!m_bDeadCheck && m_tCreateTime.fCount >= 1)
	{
		m_tCreateTime.fCount = 0;
		++m_tCreateTime.fFrame;
	}

	if (m_bDeadCheck)
	{
		m_tCreateTime.fFrame += GET_TIME * m_tCreateTime.fCount;
	}

	if (!m_bDeadCheck && m_tCreateTime.fMax <= m_tCreateTime.fFrame)
	{
		m_bDeadCheck = true;
		SoundMgr->PlaySound(L"pshbld03.wav", CHANNEL_EFFECT, 0.5f, m_tInfo.vPos);
	}

	if (m_bDeadCheck && !m_bCreateEnd)
	{
		m_bCreateEnd = true;
		m_tFrame = FRAME(0.f, 20.f, 20.f);
	}

	m_tBuildInfo.iHp = S_INT_CAST(S_FLOAT_CAST(m_tBuildInfo.iHpMax) * (m_tCreateTime.fFrame / m_tCreateTime.fMax));
	m_tBuildInfo.iShield = S_INT_CAST(S_FLOAT_CAST(m_tBuildInfo.iShieldMax) * (m_tCreateTime.fFrame / m_tCreateTime.fMax));

	if (m_tBuildInfo.iHp > m_tBuildInfo.iHpMax)
		m_tBuildInfo.iHp = m_tBuildInfo.iHpMax;

	if (m_tBuildInfo.iShield > m_tBuildInfo.iShieldMax)
		m_tBuildInfo.iShield = m_tBuildInfo.iShieldMax;

	m_tFrame.fFrame += GET_TIME * m_tFrame.fCount;

	if (m_tFrame.fFrame > m_tFrame.fMax)
	{
		if (m_bDeadCheck == true)
		{
			m_tFrame.fFrame = m_tFrame.fMax;
			SettingTileOption();
			if (!lstrcmp(L"Nexus", m_tBuildInfo.szName))
			{
				if (m_bSelect)
				{
					ObjMgr->AddGameObject(CFactory<CNexus>::CreateGameObjectBuild(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				else
				{
					ObjMgr->AddGameObject(CFactory<CNexus>::CreateGameObject(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				return 1;
			}
			else if (!lstrcmp(L"Pylon", m_tBuildInfo.szName))
			{
				if (m_bSelect)
				{
					ObjMgr->AddGameObject(CFactory<CPylon>::CreateGameObjectBuild(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				else
				{
					ObjMgr->AddGameObject(CFactory<CPylon>::CreateGameObject(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				return 1;
			}
			else if (!lstrcmp(L"Assimilator", m_tBuildInfo.szName))
			{
				if (m_bSelect)
				{
					ObjMgr->AddGameObject(CFactory<CAssimilator>::CreateGameObjectBuild(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				else
				{
					ObjMgr->AddGameObject(CFactory<CAssimilator>::CreateGameObject(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				return 1;
			}
			else if (!lstrcmp(L"GateWay", m_tBuildInfo.szName))
			{
				if (m_bSelect)
				{
					ObjMgr->AddGameObject(CFactory<CGateWay>::CreateGameObjectBuild(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				else
				{
					ObjMgr->AddGameObject(CFactory<CGateWay>::CreateGameObject(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				return 1;
			}
			else if (!lstrcmp(L"Forge", m_tBuildInfo.szName))
			{
				if (m_bSelect)
				{
					ObjMgr->AddGameObject(CFactory<CForge>::CreateGameObjectBuild(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				else
				{
					ObjMgr->AddGameObject(CFactory<CForge>::CreateGameObject(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				return 1;
			}
			else if (!lstrcmp(L"PhotonCannon", m_tBuildInfo.szName))
			{
				if (m_bSelect)
				{
					ObjMgr->AddGameObject(CFactory<CPhotonCannon>::CreateGameObjectBuild(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				else
				{
					ObjMgr->AddGameObject(CFactory<CPhotonCannon>::CreateGameObject(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				return 1;
			}
			else if (!lstrcmp(L"CyberneticsCore", m_tBuildInfo.szName))
			{
				if (m_bSelect)
				{
					ObjMgr->AddGameObject(CFactory<CCyberneticsCore>::CreateGameObjectBuild(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				else
				{
					ObjMgr->AddGameObject(CFactory<CCyberneticsCore>::CreateGameObject(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				return 1;
			}
			else if (!lstrcmp(L"ShieldBattery", m_tBuildInfo.szName))
			{
				if (m_bSelect)
				{
					ObjMgr->AddGameObject(CFactory<CShieldBattery>::CreateGameObjectBuild(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				else
				{
					ObjMgr->AddGameObject(CFactory<CShieldBattery>::CreateGameObject(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				return 1;
			}
			else if (!lstrcmp(L"RoboticsFacility", m_tBuildInfo.szName))
			{
				if (m_bSelect)
				{
					ObjMgr->AddGameObject(CFactory<CRoboticsFacility>::CreateGameObjectBuild(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				else
				{
					ObjMgr->AddGameObject(CFactory<CRoboticsFacility>::CreateGameObject(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				return 1;
			}
			else if (!lstrcmp(L"StarGate", m_tBuildInfo.szName))
			{
				if (m_bSelect)
				{
					ObjMgr->AddGameObject(CFactory<CStarGate>::CreateGameObjectBuild(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				else
				{
					ObjMgr->AddGameObject(CFactory<CStarGate>::CreateGameObject(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				return 1;
			}
			else if (!lstrcmp(L"CitadelofAdun", m_tBuildInfo.szName))
			{
				if (m_bSelect)
				{
					ObjMgr->AddGameObject(CFactory<CCitadelofAdun>::CreateGameObjectBuild(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				else
				{
					ObjMgr->AddGameObject(CFactory<CCitadelofAdun>::CreateGameObject(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				return 1;
			}
			else if (!lstrcmp(L"RoboticsSupportBay", m_tBuildInfo.szName))
			{
				if (m_bSelect)
				{
					ObjMgr->AddGameObject(CFactory<CRoboticsSupportBay>::CreateGameObjectBuild(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				else
				{
					ObjMgr->AddGameObject(CFactory<CRoboticsSupportBay>::CreateGameObject(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				return 1;
			}
			else if (!lstrcmp(L"FleetBeacon", m_tBuildInfo.szName))
			{
				if (m_bSelect)
				{
					ObjMgr->AddGameObject(CFactory<CFleetBeacon>::CreateGameObjectBuild(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				else
				{
					ObjMgr->AddGameObject(CFactory<CFleetBeacon>::CreateGameObject(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				return 1;
			}
			else if (!lstrcmp(L"TemplarArchives", m_tBuildInfo.szName))
			{
				if (m_bSelect)
				{
					ObjMgr->AddGameObject(CFactory<CTemplarArchives>::CreateGameObjectBuild(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				else
				{
					ObjMgr->AddGameObject(CFactory<CTemplarArchives>::CreateGameObject(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				return 1;
			}
			else if (!lstrcmp(L"Observatory", m_tBuildInfo.szName))
			{
				if (m_bSelect)
				{
					ObjMgr->AddGameObject(CFactory<CObservatory>::CreateGameObjectBuild(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				else
				{
					ObjMgr->AddGameObject(CFactory<CObservatory>::CreateGameObject(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				return 1;
			}
			else if (!lstrcmp(L"ArbiterTribunal", m_tBuildInfo.szName))
			{
				if (m_bSelect)
				{
					ObjMgr->AddGameObject(CFactory<CArbiterTribunal>::CreateGameObjectBuild(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				else
				{
					ObjMgr->AddGameObject(CFactory<CArbiterTribunal>::CreateGameObject(m_tInfo.vPos, m_eTeamType), OBJ_BUILD);
				}
				return 1;
			}
			
		}
		else
			m_tFrame.fFrame = S_FLOAT_CAST(m_iFrameFirst);
	}
		
	if (m_bSelect)
	{
		DWORD dwKey = GET_KEY;

		if (dwKey & KEY_X)
		{
			CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
			D_CAST(CInterpace, pInterpace)->NotPortaraits();
			D_CAST(CInterpace, pInterpace)->ClearInterpaceUnitList();
			SettingTileOption();
			return 1;
		}
		if (dwKey & KEY_LBUTTON)			// 인터페이스 클릭용
		{
			CGameObject* ptMouse = ObjMgr->GetList()[OBJ_MOUSE].front();
			D3DXVECTOR3 vMouse = D_CAST(CMouse, ptMouse)->GetRenderMousePos();
			CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();

			if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 0))
			{
				CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
				D_CAST(CInterpace, pInterpace)->NotPortaraits();
				D_CAST(CInterpace, pInterpace)->ClearInterpaceUnitList();
				SettingTileOption();
				return 1;
			}
		}
	}
	
	return 0;
}

void CCreateBuild::Render(void)
{
	D3DXMATRIX matScale, matTrans;

	
		const TEX_INFO* pSelecttexture = NULL;

		

	if (m_bSelect)
	{
		switch (m_eTeamType)
		{
		case TEAM_PLAYER:
			pSelecttexture = TextureMgr->GetTexture(L"Select"
				, L"Ally", m_iSelectCount);
			break;
		case TEAM_ENEMY:
			pSelecttexture = TextureMgr->GetTexture(L"Select"
				, L"Enemy", m_iSelectCount);
			break;
		}

		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y
			, 0.f);

		float fX = pSelecttexture->ImageInfo.Width / 2.f;
		float fY = pSelecttexture->ImageInfo.Height / 2.f;

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pSelecttexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		/////////////

		if (m_iSelectCount == 5)
			m_iSelectCount = 7;

		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y + 60
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		/// Hp Frame
		pSelecttexture = TextureMgr->GetTexture(L"Bar"
			, L"HPFrame", m_iSelectCount);

		fX = pSelecttexture->ImageInfo.Width / 2.f;
		fY = pSelecttexture->ImageInfo.Height / 2.f;

		Device->GetSprite()->Draw(pSelecttexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		float fPercent = S_FLOAT_CAST(m_tBuildInfo.iHp) / S_FLOAT_CAST(m_tBuildInfo.iHpMax) * 100.f;
		int iR = 0, iG = 0, iB = 0;
		if (S_INT_CAST(fPercent) <= 33)
		{
			iR = 224, iG = 44, iB = 44;
		}
		else if (S_INT_CAST(fPercent) <= 66)
		{
			iR = 252, iG = 252, iB = 56;
		}
		else
		{
			iR = 36, iG = 152, iB = 36;
		}

		/// Hp Bar	
		pSelecttexture = TextureMgr->GetTexture(L"Bar"
			, L"HPBar", m_iSelectCount);

		fX = pSelecttexture->ImageInfo.Width / 2.f;
		fY = pSelecttexture->ImageInfo.Height / 2.f;

		RECT rc = { NULL
			, NULL
			, S_LONG_CAST(fX * 2 * S_FLOAT_CAST(m_tBuildInfo.iHp) / S_FLOAT_CAST(m_tBuildInfo.iHpMax))
			, S_LONG_CAST(fY * 2) };

		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y + 60
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);


		Device->GetSprite()->Draw(pSelecttexture->pTexture
			, &rc
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, iR, iG, iB));

		/// Shild Bar
		pSelecttexture = TextureMgr->GetTexture(L"Bar"
			, L"ShieldBar", m_iSelectCount);

		rc = { NULL
			, NULL
			, S_LONG_CAST(fX * 2 * S_FLOAT_CAST(m_tBuildInfo.iShield) / S_FLOAT_CAST(m_tBuildInfo.iShieldMax))
			, S_LONG_CAST(fY * 2) };

		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y + 60
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);


		Device->GetSprite()->Draw(pSelecttexture->pTexture
			, &rc
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	if (m_bDeadCheck)
	{
		const TEX_INFO* pBuildtexture = NULL;
		switch (m_eBuildType)
		{
		case BUILD_NEXUS:
			pBuildtexture = TextureMgr->GetTexture(L"BuildTime"
				, L"Nexus", S_INT_CAST(m_tFrame.fFrame));
			break;
		case BUILD_PYLON:
			pBuildtexture = TextureMgr->GetTexture(L"BuildTime"
				, L"Pylon", S_INT_CAST(m_tFrame.fFrame));
			break;
		case BUILD_ASSIMILATOR:
			pBuildtexture = TextureMgr->GetTexture(L"BuildTime"
				, L"Assimilator", S_INT_CAST(m_tFrame.fFrame));
			break;
		case BUILD_GATEWAY:
			pBuildtexture = TextureMgr->GetTexture(L"BuildTime"
				, L"Gateway", S_INT_CAST(m_tFrame.fFrame));
			break;
		case BUILD_FORGE:pBuildtexture = TextureMgr->GetTexture(L"BuildTime"
				, L"Forge", S_INT_CAST(m_tFrame.fFrame));
			break;
		case BUILD_PHOTON_CANNON:
			pBuildtexture = TextureMgr->GetTexture(L"BuildTime"
				, L"Photon Cannon", S_INT_CAST(m_tFrame.fFrame));
			break;
		case BUILD_CYBERNETICS_CORE:
			pBuildtexture = TextureMgr->GetTexture(L"BuildTime"
				, L"Cybernetics Core", S_INT_CAST(m_tFrame.fFrame));
			break;
		case BUILD_SHIELD_BATTERY:
			pBuildtexture = TextureMgr->GetTexture(L"BuildTime"
				, L"Shield Battery", S_INT_CAST(m_tFrame.fFrame));
			break;
		case BUILD_ROBOTICS_FACILITY:
			pBuildtexture = TextureMgr->GetTexture(L"BuildTime"
				, L"Robotics Facility", S_INT_CAST(m_tFrame.fFrame));
			break;
		case BUILD_STARGATE:
			pBuildtexture = TextureMgr->GetTexture(L"BuildTime"
				, L"Stargate", S_INT_CAST(m_tFrame.fFrame));
			break;
		case BUILD_CITADEL_OF_ADUN:
			pBuildtexture = TextureMgr->GetTexture(L"BuildTime"
				, L"Citadel of Adun", S_INT_CAST(m_tFrame.fFrame));
			break;
		case BUILD_ROBOTICS_SUPPORT_BAY:
			pBuildtexture = TextureMgr->GetTexture(L"BuildTime"
				, L"Robotics Support Bay", S_INT_CAST(m_tFrame.fFrame));
			break;
		case BUILD_FLEET_BEACON:
			pBuildtexture = TextureMgr->GetTexture(L"BuildTime"
				, L"Fleet Beacon", S_INT_CAST(m_tFrame.fFrame));
			break;
		case BUILD_TEMPAR_ARCHIVES:
			pBuildtexture = TextureMgr->GetTexture(L"BuildTime"
				, L"Tempar Archives", S_INT_CAST(m_tFrame.fFrame));
			break;
		case BUILD_OBSERVATORY:
			pBuildtexture = TextureMgr->GetTexture(L"BuildTime"
				, L"Observatory", S_INT_CAST(m_tFrame.fFrame));
			break;
		case BUILD_ARBITER_TRIBUNAL:
			pBuildtexture = TextureMgr->GetTexture(L"BuildTime"
				, L"Arbiter Tribunal", S_INT_CAST(m_tFrame.fFrame));
			break;
		}


		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y
			, 0.f);

		float fX = pBuildtexture->ImageInfo.Width / 2.f;
		float fY = pBuildtexture->ImageInfo.Height / 2.f;

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pBuildtexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	else
	{
		const TEX_INFO* pBuildtexture = TextureMgr->GetTexture(L"Effect"
			, L"CreateBuild", S_INT_CAST(m_tFrame.fFrame));

		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y
			, 0.f);

		float fX = pBuildtexture->ImageInfo.Width / 2.f;
		float fY = pBuildtexture->ImageInfo.Height / 2.f;

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pBuildtexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

	}
}

void CCreateBuild::Release(void)
{
}

void CCreateBuild::CheckOneClick(void)
{
	D3DXMATRIX matTrans;
	const TEX_INFO* pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"Cancel", m_iClickMyInterpace[8]);

	float fX = pInterpacetexture->ImageInfo.Width / 2.f;
	float fY = pInterpacetexture->ImageInfo.Height / 2.f;

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

void CCreateBuild::SettingBuildInfo(void)
{
	if (!lstrcmp(L"Nexus", m_tBuildInfo.szName))
	{
		m_eBuildType = BUILD_NEXUS;
		m_iSelectCount = 8;
	}
	else if (!lstrcmp(L"GateWay", m_tBuildInfo.szName))
	{
		m_eBuildType = BUILD_GATEWAY;
		m_iSelectCount = 8;
	}
	else if(!lstrcmp(L"StarGate", m_tBuildInfo.szName))
	{
		m_eBuildType = BUILD_STARGATE;
		m_iSelectCount = 7;
	}
	else if (!lstrcmp(L"Assimilator", m_tBuildInfo.szName))
	{
		m_eBuildType = BUILD_ASSIMILATOR;
		m_iSelectCount = 5;
	}
	else if (!lstrcmp(L"Forge", m_tBuildInfo.szName))
	{
		m_eBuildType = BUILD_FORGE;
		m_iSelectCount = 6;
	}
	else if (!lstrcmp(L"CyberneticsCore", m_tBuildInfo.szName))
	{
		m_eBuildType = BUILD_CYBERNETICS_CORE;
		m_iSelectCount = 5;
	}
	else if (!lstrcmp(L"ShieldBattery", m_tBuildInfo.szName))
	{
		m_eBuildType = BUILD_SHIELD_BATTERY;
		m_iSelectCount = 5;
	}
	else if (!lstrcmp(L"RoboticsFacility", m_tBuildInfo.szName))
	{
		m_eBuildType = BUILD_ROBOTICS_FACILITY;
		m_iSelectCount = 5;
	}
	else if (!lstrcmp(L"CitadelofAdun", m_tBuildInfo.szName))
	{
		m_eBuildType = BUILD_CITADEL_OF_ADUN;
		m_iSelectCount = 5;
	}
	else if (!lstrcmp(L"RoboticsSupportBay", m_tBuildInfo.szName))
	{
		m_eBuildType = BUILD_ROBOTICS_SUPPORT_BAY;
		m_iSelectCount = 5;
	}
	else if (!lstrcmp(L"FleetBeacon", m_tBuildInfo.szName))
	{
		m_eBuildType = BUILD_FLEET_BEACON;
		m_iSelectCount = 5;
	}
	else if (!lstrcmp(L"TemplarArchives", m_tBuildInfo.szName))
	{
		m_eBuildType = BUILD_TEMPAR_ARCHIVES;
		m_iSelectCount = 5;
	}
	else if (!lstrcmp(L"Observatory", m_tBuildInfo.szName))
	{
		m_eBuildType = BUILD_OBSERVATORY;
		m_iSelectCount = 5;
	}
	else if (!lstrcmp(L"ArbiterTribunal", m_tBuildInfo.szName))
	{
		m_eBuildType = BUILD_ARBITER_TRIBUNAL;
		m_iSelectCount = 5;
	}
	else if (!lstrcmp(L"Pylon", m_tBuildInfo.szName))
	{
		m_eBuildType = BUILD_PYLON;
		m_iSelectCount = 4;
	}
	else if (!lstrcmp(L"PhotonCannon", m_tBuildInfo.szName))
	{
		m_eBuildType = BUILD_PHOTON_CANNON;
		m_iSelectCount = 4;
	}
}

void CCreateBuild::SettingTileOption(void)
{
	m_tInfo.vSize.x = S_FLOAT_CAST(m_tBuildInfo.iSizeX * TILECX);
	m_tInfo.vSize.y = S_FLOAT_CAST(m_tBuildInfo.iSizeY * TILECY);

	int iX = S_INT_CAST(m_tInfo.vPos.x / TILECX);
	int iY = S_INT_CAST(m_tInfo.vPos.y / TILECY);

	int iIndex = iX + (TILEEX * iY);

	if (iIndex < 0 || iIndex >= TILEEX * TILEEY)
		return;

	CGameObject* pObj = ObjMgr->GetList()[OBJ_BACKGROUND].front();

	if (m_tBuildInfo.iSizeX == 4 && m_tBuildInfo.iSizeY == 3)
	{
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - 1);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex + 1);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - 2);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX - 1);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX + 1);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX - 2);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex + TILEEX - 1);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex + TILEEX);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex + TILEEX + 1);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex + TILEEX - 2);
	}
	else if (m_tBuildInfo.iSizeX == 3 && m_tBuildInfo.iSizeY == 2)
	{
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - 1);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex + 1);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX - 1);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX + 1);
	}
	else if (m_tBuildInfo.iSizeX == 2 && m_tBuildInfo.iSizeY == 2)
	{
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - 1);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX);
		D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX - 1);
	}
}
