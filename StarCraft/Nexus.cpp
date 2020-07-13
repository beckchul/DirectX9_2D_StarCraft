#include "stdafx.h"
#include "Nexus.h"
#include "ObjMgr.h"
#include "TextureMgr.h"
#include "Device.h"
#include "BackGround.h"
#include "Mouse.h"
#include "KeyMgr.h"
#include "Interpace.h"
#include "TimeMgr.h"
#include "Factory.h"
#include "Probe.h"
#include "Effect.h"

CNexus::CNexus()
{
}

CNexus::~CNexus()
{
	Release();
}

HRESULT CNexus::Initialize(void)
{
	list<BUILD*> BuildList = ObjMgr->GetBuildList();

	list<BUILD*>::iterator iter = BuildList.begin();
	list<BUILD*>::iterator iter_end = BuildList.end();

	lstrcpy(m_tInfo.szObjKey, L"Building");
	lstrcpy(m_tInfo.szStateKey, L"Nexus");

	for (; iter != iter_end; ++iter)
	{
		if (!lstrcmp((*iter)->szName, m_tInfo.szStateKey))
		{
			m_tBuildInfo = (*(*iter));
			break;
		}
	}
	m_eRenderType = RENDER_GROUND;
	m_tInfo.vSize.x = S_FLOAT_CAST(m_tBuildInfo.iSizeX * TILECX) + 40;
	m_tInfo.vSize.y = S_FLOAT_CAST(m_tBuildInfo.iSizeY * TILECY);
	m_tClick = FRAME(0.f, 10.f, 2.f);
	SettingTile();
	m_bKeyInput = false;
	m_bCreateUnit = false;
	m_eBuildCreateType = BUILD_CREATE_NOMAL;
	m_eBuildType = BUILD_NEXUS;
	m_vStartingPoint = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_tFrame = FRAME(1.f, 20.f, 5.f);
	m_iFrameFirst = S_INT_CAST(m_tFrame.fFrame);
	m_iView = 10;
	return S_OK;
}

int CNexus::Update(void)
{
	Function();
	CreateUnit(true);

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

	if (m_eTeamType != TEAM_PLAYER)
	{
		CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
		int iAICount = D_CAST(CInterpace, pInterpace)->GetAICount();

		FRAME tAIFrame = D_CAST(CInterpace, pInterpace)->GetAIFrame();
		m_vStartingPoint = D3DXVECTOR3(m_tInfo.vPos.x + 45.f, m_tInfo.vPos.y + (rand() % 200) - 100.f, 0.f);

		if ((iAICount % 30) == 0 && tAIFrame.fFrame == 0.f && iAICount < 200)
		{		
			CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
			int iMineral = D_CAST(CInterpace, pInterpace)->GetWorldMineral(m_eTeamType);
			int iHumun = D_CAST(CInterpace, pInterpace)->GetHumanCount(m_eTeamType);
			int iHumunMax = D_CAST(CInterpace, pInterpace)->GetHumanMaxCount(m_eTeamType);

			if (iMineral >= 50)		// 미네랄 검사
			{
				if (iHumunMax - iHumun >= 1)		// 인구수 검사
				{
					if (m_UnitCreateList.size() < 5)
					{
						m_UnitCreateList.push_back(L"P");
						D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -50);
					}
					m_eBuildCreateType = BUILD_CREATE_UNIT;
				}
			}
		}
	}

	return 0;
}

void CNexus::Render(void)
{
	D3DXMATRIX matScale, matTrans;

	if (m_bSelect)
	{
		CObjectRenderMgr::SelectBuildUIRender(this, 8, 0.f, 8, 60.f);
	}
	const TEX_INFO* pBuildtexture = TextureMgr->GetTexture(L"Shad"
		, L"Nexus", 0);

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
			, L"Nexus", 0);

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
	
	

	if (m_bCreateUnit)
	{
		if (m_tFrame.fFrame > 6.f)
			m_tFrame.fFrame = 1.f;
		pBuildtexture = TextureMgr->GetTexture(L"Building"
			, L"Nexus", S_INT_CAST(m_tFrame.fFrame));

		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y
			, 0.f);

		fX = pBuildtexture->ImageInfo.Width / 2.f;
		fY = pBuildtexture->ImageInfo.Height / 2.f;

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pBuildtexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(100, 255, 255, 255));
	}
}

void CNexus::Release(void)
{
}

void CNexus::CheckOneClick(void)
{
	D3DXMATRIX matTrans;
	float fX = 0.f, fY = 0.f;
	const TEX_INFO* pInterpacetexture = NULL;
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

	// 1번째 칸
	pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"Probe", m_iClickMyInterpace[0]);

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

	// 6번째 칸
	pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"Rally", m_iClickMyInterpace[5]);

	fX = pInterpacetexture->ImageInfo.Width / 2.f;
	fY = pInterpacetexture->ImageInfo.Height / 2.f;

	D3DXMatrixTranslation(&matTrans
		, 595.f + fX
		, 397.f + fY
		, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pInterpacetexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CNexus::CreateUnit(bool Check)
{
	if (Check)
	{
		if (!m_bCreateUnit)
		{
			list<TCHAR*>::iterator iter = m_UnitCreateList.begin();

			if (iter == m_UnitCreateList.end())
			{
				m_eBuildCreateType = BUILD_CREATE_NOMAL;
				return;
			}
			
			TCHAR szBuf[5] = L"";
			lstrcpy(szBuf, (*iter));
			if (!lstrcmp(szBuf, L"P"))
			{
				m_tCreateTime = FRAME(0.f, 1.f, 12.5f);
			}
			m_iFrameFirst = S_INT_CAST(m_tFrame.fFrame);
			m_bCreateUnit = true;
		}
		else
		{
			m_tCreateTime.fFrame += GET_TIME * m_tCreateTime.fCount;

			m_tFrame.fFrame += GET_TIME * m_tFrame.fCount;

			if (m_tFrame.fFrame > m_tFrame.fMax)
			{
				m_tFrame.fFrame = 1.f;
			}

			if (m_tCreateTime.fFrame > m_tCreateTime.fMax)
			{
				m_bCreateUnit = false;
				m_tCreateTime.fFrame = m_tCreateTime.fMax;



				list<TCHAR*>::iterator iter = m_UnitCreateList.begin();

				if (iter == m_UnitCreateList.end())
					return;

				TCHAR szBuf[5] = L"";
				lstrcpy(szBuf, (*iter));
				if (!lstrcmp(szBuf, L"P"))
				{
					if (m_eTeamType == TEAM_PLAYER)
					{
						SoundMgr->PlaySound(L"pprrdy00.wav", CHANNEL_MAIN, 0.9f);
						ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_PING), OBJ_EFFECT);
					}
					

					if (m_vStartingPoint.x != NULL && m_vStartingPoint.y != NULL)
					{
						ObjMgr->AddGameObject(CFactory<CProbe>::CreateGameObjectRallyGround
						(D3DXVECTOR3(m_tInfo.vPos.x - m_tInfo.vSize.x / 2.f, m_tInfo.vPos.y + m_tInfo.vSize.y / 2.f, 0.f)
							, m_vStartingPoint, m_eTeamType), OBJ_UNIT);
					}
					else
					{
						ObjMgr->AddGameObject(CFactory<CProbe>::CreateGameObject
						(D3DXVECTOR3(m_tInfo.vPos.x - m_tInfo.vSize.x / 2.f, m_tInfo.vPos.y + m_tInfo.vSize.y / 2.f + 30.f , 0.f), m_eTeamType), OBJ_UNIT);
					}
				}

				m_UnitCreateList.pop_front();
			}
		}
	}
	else
	{
		D3DXMATRIX matScale, matTrans;
		float fX = 0.f, fY = 0.f;
		const TEX_INFO* pBigWireTexture = TextureMgr->GetTexture(L"Produce"
			, L"Frame", 0);
		
		fX = pBigWireTexture->ImageInfo.Width / 2.f;
		fY = pBigWireTexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 230.f + fX
			, 395.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pBigWireTexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		pBigWireTexture = TextureMgr->GetTexture(L"Produce"
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

		

	
		float fPercent = m_tCreateTime.fFrame / m_tCreateTime.fMax;

		if (fPercent > 1.f)
			fPercent = 1.f;

		RECT rc = { NULL, NULL
			, S_LONG_CAST(fPercent * fX * 2), S_LONG_CAST(fY * 2) };
		D3DXMatrixTranslation(&matTrans
			, 270.f
			, 418.f
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pBigWireTexture->pTexture
			, &rc 
			, NULL
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));


		list<TCHAR*>::iterator iter = m_UnitCreateList.begin();
		list<TCHAR*>::iterator iter_end = m_UnitCreateList.end();

		int iCount = 0;
		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter), L"P"))
			{
				pBigWireTexture = TextureMgr->GetTexture(L"Produce"
					, L"Icon", 0);
			}

			fX = pBigWireTexture->ImageInfo.Width / 2.f;
			fY = pBigWireTexture->ImageInfo.Height / 2.f;

			switch (iCount)
			{
			case 0:
				D3DXMatrixTranslation(&matTrans
					, 232.f
					, 397.f
					, 0.f);
				break;
			case 1:
				D3DXMatrixTranslation(&matTrans
					, 232.f
					, 436.f
					, 0.f);
				break;
			case 2:
				D3DXMatrixTranslation(&matTrans
					, 271.f
					, 436.f
					, 0.f);
				break;
			case 3:
				D3DXMatrixTranslation(&matTrans
					, 310.f
					, 436.f
					, 0.f);
				break;
			case 4:
				D3DXMatrixTranslation(&matTrans
					, 349.f
					, 436.f
					, 0.f);
				break;
			}

			Device->GetSprite()->SetTransform(&matTrans);
			Device->GetSprite()->Draw(pBigWireTexture->pTexture
				, NULL
				, NULL //&D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(255, 255, 255, 255));

			++iCount;
		}
	}
}

void CNexus::Function(void)
{
	if (m_eTeamType != TEAM_PLAYER)
		return;

	if (!m_bSelect)
		return;

	DWORD dwKey = GET_KEY;

	CGameObject* ptMouse = ObjMgr->GetList()[OBJ_MOUSE].front();
	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
	D3DXVECTOR3 vMouse = D_CAST(CMouse, ptMouse)->GetRenderMousePos();
	int iMineral = D_CAST(CInterpace, pInterpace)->GetWorldMineral(m_eTeamType);
	int iHumun = D_CAST(CInterpace, pInterpace)->GetHumanCount(m_eTeamType);
	int iHumunMax = D_CAST(CInterpace, pInterpace)->GetHumanMaxCount(m_eTeamType);

	if (m_eBuildCreateType != BUILD_CREATE_NOMAL)
	{
		if (dwKey & KEY_LBUTTON && D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 8) && !m_bKeyInput)
		{
			m_bKeyInput = true;

			if (m_UnitCreateList.size() > 1)
				m_UnitCreateList.pop_back();
			else
			{
				m_UnitCreateList.pop_back();
				m_eBuildCreateType = BUILD_CREATE_NOMAL;
				m_bCreateUnit = false;
			}

			D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 50);
			return;
		}
		if (dwKey & KEY_X && !m_bKeyInput)
		{
			m_bKeyInput = true;

			if (m_UnitCreateList.size() > 1)
				m_UnitCreateList.pop_back();
			else
			{
				m_UnitCreateList.pop_back();
				m_eBuildCreateType = BUILD_CREATE_NOMAL;
				m_bCreateUnit = false;
			}
			D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, 50);
			return;
		}
	}
	if (dwKey & KEY_LBUTTON && !m_bKeyInput)
	{
		m_bKeyInput = true;
		// 첫번째 칸
		if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 0))
		{
			if (iMineral >= 50)
			{
				if (iHumunMax - iHumun >= 1)
				{
					if (m_UnitCreateList.size() < 5)
					{
						m_UnitCreateList.push_back(L"P");
						D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -50);
					}
						
					m_eBuildCreateType = BUILD_CREATE_UNIT;
				}
				else
				{
					//인구 수 부족
				}
			}
			else
			{
				//미네랄 부족
			}
			return;
		}//여섯번째 칸
		else if (D_CAST(CInterpace, pInterpace)->CheckingPointInRect(vMouse, 5))
		{
			D_CAST(CMouse, ptMouse)->SettingMouseRallyPoint(this);
			return;
		}
	}
	if (dwKey & KEY_RBUTTON && !m_bKeyInput)
	{
		m_vStartingPoint = D_CAST(CMouse, ptMouse)->GetMousePos();
		ptMouse->SetState(MOUSE_CLICK);
	}
	if (dwKey & KEY_P && !m_bKeyInput)
	{
		m_bKeyInput = true;

		if (iMineral >= 50)		// 미네랄 검사
		{
			if (iHumunMax - iHumun >= 1)		// 인구수 검사
			{
				if (m_UnitCreateList.size() < 5)
				{
					m_UnitCreateList.push_back(L"P");
					D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_eTeamType, -50);
				}	
				m_eBuildCreateType = BUILD_CREATE_UNIT;
			}
			else
			{
				//
			}
		}
		else
		{
			//
		}
	}
	if (dwKey & KEY_R && !m_bKeyInput)
	{
		m_bKeyInput = true;
		D_CAST(CMouse, ptMouse)->SettingMouseRallyPoint(this);
		return;
	}
	

	if (!dwKey)
	{
		m_tClick.fFrame += GET_TIME * m_tClick.fCount;

		if (m_tClick.fFrame > m_tClick.fMax)
			m_bKeyInput = false;
	}
}

void CNexus::SettingTile(void)
{
	int iX = S_INT_CAST(m_tInfo.vPos.x / TILECX);
	int iY = S_INT_CAST(m_tInfo.vPos.y / TILECY);

	int iIndex = iX + (TILEEX * iY);

	if (iIndex < 0 || iIndex >= TILEEX * TILEEY)
		return;

	CGameObject* pObj = ObjMgr->GetList()[OBJ_BACKGROUND].front();

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
