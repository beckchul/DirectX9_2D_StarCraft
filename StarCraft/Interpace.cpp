#include "stdafx.h"
#include "Interpace.h"
#include "Device.h"
#include "TextureMgr.h"
#include "TimeMgr.h"
#include "ObjMgr.h"
#include "Mouse.h"
#include "KeyMgr.h"
#include "Fog.h"

CInterpace::CInterpace()
{
}


CInterpace::~CInterpace()
{
}

HRESULT CInterpace::Initialize(void)
{
	m_bSelectUnit = false;
	m_bSelectBuild = false;
	m_bRenderPortraits = false;
	m_bUnitOneClick = false;
	m_fPortraitsFrameFirst = 0.f;
	m_tNoise = FRAME(0.f, 30.f, 4.f);
	m_iBigWireNumber = 0;
	m_iStatsNumber = 0;
	m_iWorldMineral[TEAM_PLAYER] = 7000;
	m_iWorldGas[TEAM_PLAYER] = 6000;
	m_iWorldMineral[TEAM_ENEMY] = 30000;
	m_iWorldGas[TEAM_ENEMY] = 30000;
	m_iHumunCount[TEAM_PLAYER] = 0;
	m_iHumunCount[TEAM_ENEMY] = 0;
	m_iHumunMaxCount[TEAM_PLAYER] = 0;
	m_iHumunMaxCount[TEAM_ENEMY] = 0;
	SettingInterpaceRect();
	m_eRenderType = RENDER_INTERPACE;
	m_tClick = FRAME(0.f, 2.f, 2.f);
	m_bClickKey = true;
	m_bMapLook = false;
	m_iAITimeCount = 0;
	m_tAIFrame = FRAME(0.f, 1.f, 1.f);

	for (int j = 0; j < TEAM_END; ++j)
	{
		for (int i = 0; i < UPGRADE_END; ++i)
			m_iUpgradeCount[j][i] = 0;
	}
	
	return S_OK;
}

int CInterpace::Update(void)
{
	MiniClick();
	FrameMove();
	CenterUnitList(true);


	m_tAIFrame.fFrame += GET_TIME * m_tAIFrame.fCount;

	if (m_tAIFrame.fFrame >= m_tAIFrame.fMax)
	{
		m_tAIFrame.fFrame = 0.f;
		++m_iAITimeCount;
	}



	return 0;
}

void CInterpace::Render(void)
{
	// 기본 Base //
	D3DXMATRIX matTrans, matScale;

	const TEX_INFO* pMapTexture = TextureMgr->GetTexture(L"Map", L"Main", 1);

	D3DXMatrixTranslation(&matTrans
		, pMapTexture->ImageInfo.Width / 2.f
		, pMapTexture->ImageInfo.Height / 2.f
		, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pMapTexture->pTexture
		, NULL
		, &D3DXVECTOR3(WINCX / 2.f, WINCY / 2.f, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));


	MiniView();

	/////자원 미네랄
	TCHAR szResource[MIN_STR] = L"";

	_itow_s(m_iWorldMineral[TEAM_PLAYER], szResource, 10);

	D3DXMatrixTranslation(&matTrans, 455.f, 5.f, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);

	Device->GetFont()->DrawTextW(Device->GetSprite(),
		szResource, lstrlen(szResource)
		, NULL, NULL, D3DCOLOR_ARGB(255, 34, 177, 76));

	/////자원 가스
	_itow_s(m_iWorldGas[TEAM_PLAYER], szResource, 10);

	D3DXMatrixTranslation(&matTrans, 525.f, 5.f, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);

	Device->GetFont()->DrawTextW(Device->GetSprite(),
		szResource, lstrlen(szResource)
		, NULL, NULL, D3DCOLOR_ARGB(255, 34, 177, 76));

	//////인구
	list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

	int iHumunCount[TEAM_END] = { 0, 0 };

	for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
	{
		if((*iter)->GetTeamType() == TEAM_PLAYER)
			iHumunCount[TEAM_PLAYER] += (*iter)->GetUnitInfo().iPeople;
		else if((*iter)->GetTeamType() == TEAM_ENEMY)
			iHumunCount[TEAM_ENEMY] += (*iter)->GetUnitInfo().iPeople;
	}
		


	int iHumunMaxCount[TEAM_END] = { 0, 0 };
	list<CGameObject*> pBuildList = ObjMgr->GetList()[OBJ_BUILD];
	for (list<CGameObject*>::iterator iter = pBuildList.begin(); iter != pBuildList.end(); ++iter)
	{
		if ((*iter)->GetBuildType() == BUILD_PYLON &&
			(*iter)->GetBuildCreateType() != BUILD_CREATE_CREATE)
		{
			if ((*iter)->GetTeamType() == TEAM_PLAYER)
				iHumunMaxCount[TEAM_PLAYER] += 8;
			else if ((*iter)->GetTeamType() == TEAM_ENEMY)
				iHumunMaxCount[TEAM_ENEMY] += 8;
		}
			
		if ((*iter)->GetBuildType() == BUILD_NEXUS &&
			(*iter)->GetBuildCreateType() != BUILD_CREATE_CREATE)
		{
			if ((*iter)->GetTeamType() == TEAM_PLAYER)
				iHumunMaxCount[TEAM_PLAYER] += 9;
			else if ((*iter)->GetTeamType() == TEAM_ENEMY)
				iHumunMaxCount[TEAM_ENEMY] += 9;
		}
	}

	m_iHumunCount[TEAM_PLAYER] = iHumunCount[TEAM_PLAYER];
	m_iHumunMaxCount[TEAM_PLAYER] = iHumunMaxCount[TEAM_PLAYER];

	m_iHumunCount[TEAM_ENEMY] = iHumunCount[TEAM_ENEMY];
	m_iHumunMaxCount[TEAM_ENEMY] = iHumunMaxCount[TEAM_ENEMY];

	_itow_s(iHumunCount[TEAM_PLAYER], szResource, 10);

	TCHAR szBuf[MIN_STR] = L"";
	_itow_s(iHumunMaxCount[TEAM_PLAYER], szBuf, 10);

	lstrcat(szResource, L"/");
	lstrcat(szResource, szBuf);

	D3DXMatrixTranslation(&matTrans, 590.f, 5.f, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);

	Device->GetFont()->DrawTextW(Device->GetSprite(),
		szResource, lstrlen(szResource)
		, NULL, NULL, D3DCOLOR_ARGB(255, 34, 177, 76));

	//초상화
	if (m_bRenderPortraits)		// 메인
	{

		const TEX_INFO* pPortraitsTexture = TextureMgr->GetTexture(L"Portraits"
			, szPortraitsStateKey, S_INT_CAST(m_tFrame.fFrame));

		float fX = pPortraitsTexture->ImageInfo.Width / 2.f;
		float fY = pPortraitsTexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, fX + 414.f
			, fY + 414.f
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pPortraitsTexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		if (m_tNoise.fFrame <= m_tNoise.fMax)
		{
			pPortraitsTexture = TextureMgr->GetTexture(L"Portraits"
				, L"Noise", S_INT_CAST(m_tNoise.fFrame));

			float fX = pPortraitsTexture->ImageInfo.Width / 2.f;
			float fY = pPortraitsTexture->ImageInfo.Height / 2.f;

			D3DXMatrixTranslation(&matTrans
				, fX + 414.f
				, fY + 414.f
				, 0.f);

			Device->GetSprite()->SetTransform(&matTrans);
			Device->GetSprite()->Draw(pPortraitsTexture->pTexture
				, NULL
				, &D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(255, 255, 255, 255));
		}
	}

	
	if (m_bUnitOneClick && m_bRenderPortraits)	// 한마리만 클릭
	{
		//폰트
		TCHAR szInfo[MIN_STR] = L"", szBuf[MIN_STR] = L"";

		// BIgWire
		if (m_bSelectUnit)	// 유닛 
		{
			list<CGameObject*>::iterator iter = m_InterpaceUnitList.begin();

			if (iter == m_InterpaceUnitList.end())
				return;
			m_tUnitInfo = (*iter)->GetUnitInfo();

			if (!(*iter)->GetDeadCheck())
			{
				float fPercent = S_FLOAT_CAST(m_tUnitInfo.iHp) / S_FLOAT_CAST(m_tUnitInfo.iHpMax) * 100.f;

				int iCount = 0;
				if (S_INT_CAST(fPercent) <= 10)
				{
					iCount = 6;
				}
				else if (S_INT_CAST(fPercent) <= 25)
				{
					iCount = 5;
				}
				else if (S_INT_CAST(fPercent) <= 40)
				{
					iCount = 4;
				}
				else if (S_INT_CAST(fPercent) <= 55)
				{
					iCount = 3;
				}
				else if (S_INT_CAST(fPercent) <= 70)
				{
					iCount = 2;
				}
				else if (S_INT_CAST(fPercent) <= 85)
				{
					iCount = 0;
				}

				int iR = 0, iG = 0, iB = 0;
				if (S_INT_CAST(fPercent) <= 25)
				{
					iR = 255, iG = 11, iB = 11;
				}
				else if (S_INT_CAST(fPercent) <= 50)
				{
					iR = 212, iG = 86, iB = 0;
				}
				else if (S_INT_CAST(fPercent) <= 75)
				{
					iR = 255, iG = 232, iB = 11;
				}
				else
				{
					iR = 55, iG = 255, iB = 82;
				}


				const TEX_INFO* pBigWireTexture = TextureMgr->GetTexture(L"Wirefram"
					, L"BigWirefram", m_iBigWireNumber + iCount);

				float fX = pBigWireTexture->ImageInfo.Width / 2.f;
				float fY = pBigWireTexture->ImageInfo.Height / 2.f;

				D3DXMatrixTranslation(&matTrans
					, fX + 155.f
					, fY + 390.f
					, 0.f);

				Device->GetSprite()->SetTransform(&matTrans);
				Device->GetSprite()->Draw(pBigWireTexture->pTexture
					, NULL
					, &D3DXVECTOR3(fX, fY, 0.f)
					, NULL
					, D3DCOLOR_ARGB(255, 255, 255, 255));


				if ((*iter)->GetBuildCreateType() == BUILD_CREATE_NOMAL)
				{
					if ((*iter)->GetState() != UNIT_CREATE)
					{
						//Stats
						pBigWireTexture = TextureMgr->GetTexture(L"Status"
							, L"Normal", m_iStatsNumber);

						fX = pBigWireTexture->ImageInfo.Width / 2.f;
						fY = pBigWireTexture->ImageInfo.Height / 2.f;

						D3DXMatrixTranslation(&matTrans
							, fX + 235.f
							, fY + 390.f
							, 0.f);

						Device->GetSprite()->SetTransform(&matTrans);
						Device->GetSprite()->Draw(pBigWireTexture->pTexture
							, NULL
							, &D3DXVECTOR3(fX, fY, 0.f)
							, NULL
							, D3DCOLOR_ARGB(255, 255, 255, 255));
					}
					else
					{
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




						float fPercent = (*iter)->GetCreateTime().fFrame / (*iter)->GetCreateTime().fMax;

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
					}
				}
				else
				{
					(*iter)->CreateUnit(false);
				}

				if ((*iter)->GetUnitState() != UNIT_STASISFIELD)
				{
					//쉴드	유닛
					_itow_s(m_tUnitInfo.iShield, szBuf, 10);
					lstrcpy(szInfo, szBuf);
					lstrcat(szInfo, L"/");
					_itow_s(m_tUnitInfo.iShieldMax, szBuf, 10);
					lstrcat(szInfo, szBuf);

					D3DXMatrixTranslation(&matTrans, 155.f, 455.f, 0.f);
					D3DXMatrixScaling(&matScale, 0.7f, 0.7f, 0.f);
					matScale *= matTrans;
					Device->GetSprite()->SetTransform(&matScale);

					Device->GetFont()->DrawTextW(Device->GetSprite(),
						szInfo, lstrlen(szInfo)
						, NULL, NULL, D3DCOLOR_ARGB(255, 184, 184, 232));


					// 체력
					D3DXMatrixTranslation(&matTrans, 195.f, 455.f, 0.f);
					D3DXMatrixScaling(&matScale, 0.7f, 0.7f, 0.f);
					matScale *= matTrans;
					Device->GetSprite()->SetTransform(&matScale);

					_itow_s(m_tUnitInfo.iHp, szBuf, 10);
					lstrcpy(szInfo, szBuf);
					lstrcat(szInfo, L"/");
					_itow_s(m_tUnitInfo.iHpMax, szBuf, 10);
					lstrcat(szInfo, szBuf);

					Device->GetFont()->DrawTextW(Device->GetSprite(),
						szInfo, lstrlen(szInfo)
						, NULL, NULL, D3DCOLOR_ARGB(255, iR, iG, iB));

					//마나	유닛
					if (m_tUnitInfo.iMpMax != 0)
					{
						D3DXMatrixTranslation(&matTrans, 170.f, 465.f, 0.f);
						D3DXMatrixScaling(&matScale, 0.7f, 0.7f, 0.f);
						matScale *= matTrans;
						Device->GetSprite()->SetTransform(&matScale);

						_itow_s(m_tUnitInfo.iMp, szBuf, 10);
						lstrcpy(szInfo, szBuf);
						lstrcat(szInfo, L"/");
						_itow_s(m_tUnitInfo.iMpMax, szBuf, 10);
						lstrcat(szInfo, szBuf);

						Device->GetFont()->DrawTextW(Device->GetSprite(),
							szInfo, lstrlen(szInfo)
							, NULL, NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
					}
				}
			}
		}
		else if (m_bSelectBuild)		// 건물
		{
			list<CGameObject*>::iterator iter = m_InterpaceUnitList.begin();
			m_tBuildInfo = (*iter)->GetBuildInfo();

			if (!(*iter)->GetDeadCheck())
			{
				float fPercent = S_FLOAT_CAST(m_tBuildInfo.iHp) / S_FLOAT_CAST(m_tBuildInfo.iHpMax) * 100.f;
				int iCount = 0;
				if (S_INT_CAST(fPercent) <= 10)
				{
					iCount = 6;
				}
				else if (S_INT_CAST(fPercent) <= 25)
				{
					iCount = 5;
				}
				else if (S_INT_CAST(fPercent) <= 40)
				{
					iCount = 4;
				}
				else if (S_INT_CAST(fPercent) <= 55)
				{
					iCount = 3;
				}
				else if (S_INT_CAST(fPercent) <= 70)
				{
					iCount = 2;
				}
				else if (S_INT_CAST(fPercent) <= 85)
				{
					iCount = 0;
				}

				int iR = 0, iG = 0, iB = 0;
				if (S_INT_CAST(fPercent) <= 25)
				{
					iR = 255, iG = 11, iB = 11;
				}
				else if (S_INT_CAST(fPercent) <= 50)
				{
					iR = 212, iG = 86, iB = 0;
				}
				else if (S_INT_CAST(fPercent) <= 75)
				{
					iR = 255, iG = 232, iB = 11;
				}
				else
				{
					iR = 55, iG = 255, iB = 82;
				}

				const TEX_INFO* pBigWireTexture = TextureMgr->GetTexture(L"Wirefram"
					, L"BigWirefram", m_iBigWireNumber + iCount);

				float fX = pBigWireTexture->ImageInfo.Width / 2.f;
				float fY = pBigWireTexture->ImageInfo.Height / 2.f;

				D3DXMatrixTranslation(&matTrans
					, fX + 155.f
					, fY + 390.f
					, 0.f);

				Device->GetSprite()->SetTransform(&matTrans);
				Device->GetSprite()->Draw(pBigWireTexture->pTexture
					, NULL
					, &D3DXVECTOR3(fX, fY, 0.f)
					, NULL
					, D3DCOLOR_ARGB(255, 255, 255, 255));

				if ((*iter)->GetBuildCreateType() == BUILD_CREATE_NOMAL || (*iter)->GetTeamType() != TEAM_PLAYER)
				{
					//Stats
					pBigWireTexture = TextureMgr->GetTexture(L"Status"
						, L"Normal", m_iStatsNumber);

					fX = pBigWireTexture->ImageInfo.Width / 2.f;
					fY = pBigWireTexture->ImageInfo.Height / 2.f;

					D3DXMatrixTranslation(&matTrans
						, fX + 235.f
						, fY + 390.f
						, 0.f);

					Device->GetSprite()->SetTransform(&matTrans);
					Device->GetSprite()->Draw(pBigWireTexture->pTexture
						, NULL
						, &D3DXVECTOR3(fX, fY, 0.f)
						, NULL
						, D3DCOLOR_ARGB(255, 255, 255, 255));
				}
				else if ((*iter)->GetBuildCreateType() == BUILD_CREATE_CREATE)
				{
					pBigWireTexture = TextureMgr->GetTexture(L"Status"
						, L"Create", 0);

					fX = pBigWireTexture->ImageInfo.Width / 2.f;
					fY = pBigWireTexture->ImageInfo.Height / 2.f;

					D3DXMatrixTranslation(&matTrans
						, fX + 235.f
						, fY + 390.f
						, 0.f);

					Device->GetSprite()->SetTransform(&matTrans);
					Device->GetSprite()->Draw(pBigWireTexture->pTexture
						, NULL
						, &D3DXVECTOR3(fX, fY, 0.f)
						, NULL
						, D3DCOLOR_ARGB(255, 255, 255, 255));

					pBigWireTexture = TextureMgr->GetTexture(L"Status"
						, L"Create", 1);

					fX = pBigWireTexture->ImageInfo.Width / 2.f;
					fY = pBigWireTexture->ImageInfo.Height / 2.f;

					D3DXMatrixTranslation(&matTrans
						, 257.f
						, 418.f
						, 0.f);

					m_tCreateTime = (*iter)->GetCreateTime();
					float fPercent = m_tCreateTime.fFrame / m_tCreateTime.fMax;

					if (fPercent > 1.f)
						fPercent = 1.f;

					RECT rc = { NULL, NULL
						, S_LONG_CAST(fPercent * fX * 2), S_LONG_CAST(fY * 2) };

					Device->GetSprite()->SetTransform(&matTrans);
					Device->GetSprite()->Draw(pBigWireTexture->pTexture
						, &rc
						, NULL //&D3DXVECTOR3(fX, fY, 0.f)
						, NULL
						, D3DCOLOR_ARGB(255, 255, 255, 255));
				}
				else if ((*iter)->GetBuildCreateType() == BUILD_CREATE_UNIT && (*iter)->GetTeamType() == TEAM_PLAYER)
				{
					(*iter)->CreateUnit(false);
				}

				//쉴드	건물
				_itow_s(m_tBuildInfo.iShield, szBuf, 10);
				lstrcpy(szInfo, szBuf);
				lstrcat(szInfo, L"/");
				_itow_s(m_tBuildInfo.iShieldMax, szBuf, 10);
				lstrcat(szInfo, szBuf);

				D3DXMatrixTranslation(&matTrans, 150.f, 455.f, 0.f);
				D3DXMatrixScaling(&matScale, 0.7f, 0.7f, 0.f);
				matScale *= matTrans;
				Device->GetSprite()->SetTransform(&matScale);

				Device->GetFont()->DrawTextW(Device->GetSprite(),
					szInfo, lstrlen(szInfo)
					, NULL, NULL, D3DCOLOR_ARGB(255, 184, 184, 232));


				// 체력
				D3DXMatrixTranslation(&matTrans, 190.f, 455.f, 0.f);
				D3DXMatrixScaling(&matScale, 0.7f, 0.7f, 0.f);
				matScale *= matTrans;
				Device->GetSprite()->SetTransform(&matScale);

				_itow_s(m_tBuildInfo.iHp, szBuf, 10);
				lstrcpy(szInfo, szBuf);
				lstrcat(szInfo, L"/");
				_itow_s(m_tBuildInfo.iHpMax, szBuf, 10);
				lstrcat(szInfo, szBuf);

				Device->GetFont()->DrawTextW(Device->GetSprite(),
					szInfo, lstrlen(szInfo)
					, NULL, NULL, D3DCOLOR_ARGB(255, iR, iG, iB));
			}
		}

		if (m_bSelectUnit)
		{
			list<CGameObject*> pObjectList = ObjMgr->GetList()[OBJ_UNIT];

			list<CGameObject*>::iterator iter = pObjectList.begin();
			list<CGameObject*>::iterator iter_end = pObjectList.end();

			for (; iter != iter_end; ++iter)
			{
				if ((*iter)->GetSelect() && (*iter)->GetTeamType() == TEAM_PLAYER)
				{
					//인터페이스 아이콘
					(*iter)->CheckOneClick();
				}
			}
		}
		else if (m_bSelectBuild)
		{
			list<CGameObject*> pObjectList = ObjMgr->GetList()[OBJ_BUILD];

			list<CGameObject*>::iterator iter = pObjectList.begin();
			list<CGameObject*>::iterator iter_end = pObjectList.end();

			for (; iter != iter_end; ++iter)
			{
				if ((*iter)->GetSelect() && (*iter)->GetTeamType() == TEAM_PLAYER)
				{
					//인터페이스 아이콘
					(*iter)->CheckOneClick();
				}
			}
		}

	}
	else if (m_bRenderPortraits)	// 여러마리 클릭
	{
		CenterUnitList(false);

		list<CGameObject*>::iterator iter = m_InterpaceUnitList.begin();
		list<CGameObject*>::iterator iter_end = m_InterpaceUnitList.end();

		CGameObject* pTempUnit = NULL;
		bool bCheck = true;
		int iCount = 0;
		for (; iter != iter_end; ++iter)			// 여러마리 있는 애들중에 죽었을때 처리 필요
		{

				if ((*iter)->GetSelect() && bCheck)
				{
					if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe"))
					{
						iCount = 1;
					}
					else if (!lstrcmp((*iter)->GetUnitInfo().szName, L"HighTemplar"))
					{
						iCount = 2;
					}
					else if (!lstrcmp((*iter)->GetUnitInfo().szName, L"DarkTemplar"))
					{
						iCount = 3;
					}
					else if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Carrier"))
					{
						iCount = 4;
					}
					else if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Arbiter"))
					{
						iCount = 5;
					}
					pTempUnit = (*iter);
					bCheck = false;
				}
				if (lstrcmp(pTempUnit->GetUnitInfo().szName, (*iter)->GetUnitInfo().szName))
				{
					RenderNormalInterpace();
					return;
				}
		}

		RenderUnNormalnInterpace(iCount);
		return;
	}
}

void CInterpace::Release(void)
{
}


void CInterpace::MiniClick(void)
{
	DWORD dwKey = GET_KEY;
	CGameObject* pMouse = ObjMgr->GetList()[OBJ_MOUSE].front();
	D3DXVECTOR3 vMouse = D_CAST(CMouse, pMouse)->GetMousePos();

	if (dwKey & KEY_LBUTTON)
	{
		D3DXVECTOR3 vPoint = vMouse - m_vScroll - D3DXVECTOR3(10.f, 6.5f, 0.f);

		if (vPoint.x >= 6 && vPoint.x < 134 && vPoint.y >= 348 && vPoint.y < 476)
		{
			m_vScroll = (vPoint - D3DXVECTOR3(6.f, 348.f, 0.f)) * 32.f;

			if (m_vScroll.x < 0)
				m_vScroll.x = 0;
			else if (m_vScroll.x > MAXSCROLLX)
				m_vScroll.x = MAXSCROLLX;
			if (m_vScroll.y < 0)
				m_vScroll.y = 0;
			else if (m_vScroll.y > MAXSCROLLY)
				m_vScroll.y = MAXSCROLLY;
		}
	}

	
	if (dwKey & KEY_U)
		m_bMapLook = true;

	if (dwKey & KEY_J)
		m_bMapLook = false;

	if (m_InterpaceUnitList.empty())
		return;


	if ((dwKey & KEY_RBUTTON || (dwKey & KEY_LBUTTON && pMouse->GetSubState() == MOUSE_ATTACK_POINT_NOMAL)) &&
		m_bClickKey)
	{
		m_bClickKey = false;

		if (!lstrcmp(szPortraitsStateKey, L"Arbiter"))
		{
			int iNum = rand() % 4;
			if (iNum == 0)
			{
				SoundMgr->PlaySound(L"pabYes00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 1)
			{
				SoundMgr->PlaySound(L"pabYes01.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 2)
			{
				SoundMgr->PlaySound(L"pabYes02.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 3)
			{
				SoundMgr->PlaySound(L"pabYes03.wav", CHANNEL_MAIN, 0.9f);
			}
		}
		else if (!lstrcmp(szPortraitsStateKey, L"Archon"))
		{
			int iNum = rand() % 4;
			if (iNum == 0)
			{
				SoundMgr->PlaySound(L"parYes00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 1)
			{
				SoundMgr->PlaySound(L"parYes01.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 2)
			{
				SoundMgr->PlaySound(L"parYes02.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 3)
			{
				SoundMgr->PlaySound(L"parYes03.wav", CHANNEL_MAIN, 0.9f);
			}
		}
		else if (!lstrcmp(szPortraitsStateKey, L"Carrier"))
		{
			int iNum = rand() % 4;
			if (iNum == 0)
			{
				SoundMgr->PlaySound(L"PCaYes00.WAV", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 1)
			{
				SoundMgr->PlaySound(L"PCaYes01.WAV", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 2)
			{
				SoundMgr->PlaySound(L"PCaYes02.WAV", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 3)
			{
				SoundMgr->PlaySound(L"PCaYes03.WAV", CHANNEL_MAIN, 0.9f);
			}
		}
		else if (!lstrcmp(szPortraitsStateKey, L"Corsair"))
		{
			int iNum = rand() % 4;
			if (iNum == 0)
			{
				SoundMgr->PlaySound(L"pcoyes00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 1)
			{
				SoundMgr->PlaySound(L"pcoyes01.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 2)
			{
				SoundMgr->PlaySound(L"pcoyes02.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 3)
			{
				SoundMgr->PlaySound(L"pcoyes03.wav", CHANNEL_MAIN, 0.9f);
			}
		}
		else if (!lstrcmp(szPortraitsStateKey, L"Dark Archon"))
		{
			int iNum = rand() % 4;
			if (iNum == 0)
			{
				SoundMgr->PlaySound(L"pdayes00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 1)
			{
				SoundMgr->PlaySound(L"pdayes01.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 2)
			{
				SoundMgr->PlaySound(L"pdayes02.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 3)
			{
				SoundMgr->PlaySound(L"pdayes03.wav", CHANNEL_MAIN, 0.9f);
			}
		}
		else if (!lstrcmp(szPortraitsStateKey, L"Dark Templar"))
		{
			int iNum = rand() % 4;
			if (iNum == 0)
			{
				SoundMgr->PlaySound(L"PDTYes00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 1)
			{
				SoundMgr->PlaySound(L"PDTYes01.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 2)
			{
				SoundMgr->PlaySound(L"PDTYes02.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 3)
			{
				SoundMgr->PlaySound(L"PDTYes03.wav", CHANNEL_MAIN, 0.9f);
			}
		}
		else if (!lstrcmp(szPortraitsStateKey, L"Dragoon"))
		{
			int iNum = rand() % 6;
			if (iNum == 0)
			{
				SoundMgr->PlaySound(L"PDrYes00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 1)
			{
				SoundMgr->PlaySound(L"PDrYes01.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 2)
			{
				SoundMgr->PlaySound(L"PDrYes02.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 3)
			{
				SoundMgr->PlaySound(L"PDrYes03.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 4)
			{
				SoundMgr->PlaySound(L"PDrYes04.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 5)
			{
				SoundMgr->PlaySound(L"PDrYes05.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 6)
			{
				SoundMgr->PlaySound(L"PDryes06.wav", CHANNEL_MAIN, 0.9f);
			}
		}
		else if (!lstrcmp(szPortraitsStateKey, L"High Templar"))
		{
			int iNum = rand() % 4;
			if (iNum == 0)
			{
				SoundMgr->PlaySound(L"pteYes00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 1)
			{
				SoundMgr->PlaySound(L"pteYes01.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 2)
			{
				SoundMgr->PlaySound(L"pteYes02.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 3)
			{
				SoundMgr->PlaySound(L"pteYes03.wav", CHANNEL_MAIN, 0.9f);
			}
		}
		else if (!lstrcmp(szPortraitsStateKey, L"Observer"))
		{
			//int iNum = rand() % 2;
			//if (iNum == 0)
			//{
			//	SoundMgr->PlaySound(L"pwiyes00.wav", CHANNEL_MAIN, 0.9f);
			//}
			//else if (iNum == 1)
			//{
				SoundMgr->PlaySound(L"pwiyes01.wav", CHANNEL_MAIN, 0.9f);
			/*}*/
		}
		else if (!lstrcmp(szPortraitsStateKey, L"Probe"))
		{
			int iNum = rand() % 4;
			if (iNum == 0)
			{
				SoundMgr->PlaySound(L"ppryes00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 1)
			{
				SoundMgr->PlaySound(L"ppryes01.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 2)
			{
				SoundMgr->PlaySound(L"ppryes02.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 3)
			{
				SoundMgr->PlaySound(L"ppryes03.wav", CHANNEL_MAIN, 0.9f);
			}
		}
		else if (!lstrcmp(szPortraitsStateKey, L"Reaver"))
		{
			int iNum = rand() % 4;
			if (iNum == 0)
			{
				SoundMgr->PlaySound(L"ptryes00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 1)
			{
				SoundMgr->PlaySound(L"ptryes01.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 2)
			{
				SoundMgr->PlaySound(L"ptryes02.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 3)
			{
				SoundMgr->PlaySound(L"ptryes03.wav", CHANNEL_MAIN, 0.9f);
			}
		}
		else if (!lstrcmp(szPortraitsStateKey, L"Scout"))
		{
			int iNum = rand() % 4;
			if (iNum == 0)
			{
				SoundMgr->PlaySound(L"PScYes00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 1)
			{
				SoundMgr->PlaySound(L"PScYes01.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 2)
			{
				SoundMgr->PlaySound(L"PScYes02.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 3)
			{
				SoundMgr->PlaySound(L"PScYes03.wav", CHANNEL_MAIN, 0.9f);
			}
		}
		else if (!lstrcmp(szPortraitsStateKey, L"Shuttle"))
		{
			int iNum = rand() % 4;
			if (iNum == 0)
			{
				SoundMgr->PlaySound(L"pshyes00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 1)
			{
				SoundMgr->PlaySound(L"pshyes01.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 2)
			{
				SoundMgr->PlaySound(L"pshyes02.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 3)
			{
				SoundMgr->PlaySound(L"pshyes03.wav", CHANNEL_MAIN, 0.9f);
			}
		}
		else if (!lstrcmp(szPortraitsStateKey, L"Zealot"))
		{
			int iNum = rand() % 4;
			if (iNum == 0)
			{
				SoundMgr->PlaySound(L"PZeYes00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 1)
			{
				SoundMgr->PlaySound(L"PZeYes01.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 2)
			{
				SoundMgr->PlaySound(L"PZeYes02.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (iNum == 3)
			{
				SoundMgr->PlaySound(L"PZeYes03.wav", CHANNEL_MAIN, 0.9f);
			}
		}
		
	}

	if (!dwKey)
	{
		m_tClick.fFrame += GET_TIME * m_tClick.fCount;

		if (m_tClick.fFrame > m_tClick.fMax)
			m_bClickKey = true;
	}
}

void CInterpace::MiniView(void)
{
	const TEX_INFO* pBigWireTexture = NULL;
	D3DXMATRIX matTrans, matScale;
	float fX = 0.f, fY = 0.f; 
	float fMiniX = 0.f, fMiniY = 0.f;


	list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];
	list<CGameObject*> pBuildList = ObjMgr->GetList()[OBJ_BUILD];

	for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
	{
		if ((*iter)->GetTeamType() == TEAM_PLAYER)
		{
			pBigWireTexture = TextureMgr->GetTexture(L"Map", L"Mini", 1);
		}
		else if ((*iter)->GetTeamType() == TEAM_ENEMY)
		{
			pBigWireTexture = TextureMgr->GetTexture(L"Map", L"Mini", 2);
		}
			fX = pBigWireTexture->ImageInfo.Width / 2.f;
			fY = pBigWireTexture->ImageInfo.Height / 2.f;

			fMiniX = (*iter)->GetPos().x / 32.f;
			fMiniY = (*iter)->GetPos().y / 32.f;

			D3DXMatrixScaling(&matScale, 2.f, 2.f, 0.f);
			D3DXMatrixTranslation(&matTrans, fMiniX + 6.f, fMiniY + 348, 0.f);

			matScale = matScale * matTrans;

			Device->GetSprite()->SetTransform(&matTrans);
			Device->GetSprite()->Draw(pBigWireTexture->pTexture
				, NULL
				, &D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	for (list<CGameObject*>::iterator iter = pBuildList.begin(); iter != pBuildList.end(); ++iter)
	{
		if ((*iter)->GetTeamType() == TEAM_PLAYER)
		{
			pBigWireTexture = TextureMgr->GetTexture(L"Map", L"Mini", 1);
		}
		else if ((*iter)->GetTeamType() == TEAM_ENEMY)
		{
			pBigWireTexture = TextureMgr->GetTexture(L"Map", L"Mini", 2);
		}
			fX = pBigWireTexture->ImageInfo.Width / 2.f;
			fY = pBigWireTexture->ImageInfo.Height / 2.f;

			fMiniX = (*iter)->GetPos().x / 32.f;
			fMiniY = (*iter)->GetPos().y / 32.f;

			D3DXMatrixScaling(&matScale, 3.f, 3.f, 0.f);
			D3DXMatrixTranslation(&matTrans, fMiniX + 6.f, fMiniY + 348, 0.f);

			matScale = matScale * matTrans;

			Device->GetSprite()->SetTransform(&matScale);
			Device->GetSprite()->Draw(pBigWireTexture->pTexture
				, NULL
				, &D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	if (!m_bMapLook)
	{
		CGameObject* pFog = ObjMgr->GetList()[OBJ_FOG].front();

		vector<PTILE> pTIie = D_CAST(CFog, pFog)->GetTile();
		pBigWireTexture = TextureMgr->GetTexture(L"Map", L"Mini", 3);
		fX = pBigWireTexture->ImageInfo.Width / 2.f;
		fY = pBigWireTexture->ImageInfo.Height / 2.f;

		for (int i = 0; i < 16; ++i)
		{
			for (int j = 0; j < 16; ++j)
			{

				fMiniX = (pTIie[i * 8 + j * 8 * TILEEX + 3 + 3 * TILEEX]->vPos.x + 16.f) / 32.f;
				fMiniY = (pTIie[i * 8 + j * 8 * TILEEX + 3 + 3 * TILEEX]->vPos.y + 16.f) / 32.f;

				D3DXMatrixTranslation(&matTrans, fMiniX + 6.f, fMiniY + 348, 0.f);

				Device->GetSprite()->SetTransform(&matTrans);

				int ijj = i * 8 + j * 8 * TILEEX + 3 + 3 * TILEEX;

				if (pTIie[i * 8 + j * 8 * TILEEX + 3 + 3 * TILEEX]->byOption == 0)
				{
					Device->GetSprite()->Draw(pBigWireTexture->pTexture
						, NULL
						, &D3DXVECTOR3(fX, fY, 0.f)
						, NULL
						, D3DCOLOR_ARGB(255, 255, 255, 255));
				}
				else if (pTIie[i * 8 + j * 8 * TILEEX + 3 + 3 * TILEEX]->byOption == 1)
				{
					Device->GetSprite()->Draw(pBigWireTexture->pTexture
						, NULL
						, &D3DXVECTOR3(fX, fY, 0.f)
						, NULL
						, D3DCOLOR_ARGB(150, 255, 255, 255));
				}
			}
		}

		pBigWireTexture = TextureMgr->GetTexture(L"Map", L"Mini", 0);
		fX = pBigWireTexture->ImageInfo.Width / 2.f;
		fY = pBigWireTexture->ImageInfo.Height / 2.f;

		fMiniX = m_vScroll.x / 32.f;
		fMiniY = m_vScroll.y / 32.f;
		D3DXMatrixTranslation(&matTrans, fMiniX + 16.f, fMiniY + 354.5f, 0.f);
		Device->GetSprite()->SetTransform(&matTrans);

		Device->GetSprite()->Draw(pBigWireTexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
}

void CInterpace::CenterUnitList(bool _bCheck)
{
	D3DXMATRIX matTrans;

	if (_bCheck)
	{
		CGameObject* ptMouse = ObjMgr->GetList()[OBJ_MOUSE].front();
		D3DXVECTOR3 vMouse = D_CAST(CMouse, ptMouse)->GetRenderMousePos();

		DWORD dwKey = GET_KEY;
		if (dwKey & KEY_LBUTTON)
		{
			list<CGameObject*>::iterator iter = m_InterpaceUnitList.begin();
			list<CGameObject*>::iterator iter_end = m_InterpaceUnitList.end();

			int iCount = 0;

			for (; iter != iter_end; ++iter)
			{
				RECT rc = { S_LONG_CAST(155.f + (iCount / 2) * 33.f),
					S_LONG_CAST(390.f + (iCount % 2) * 34.f),
					S_LONG_CAST(155.f + (iCount / 2) * 33.f + 33),
					S_LONG_CAST(390.f + (iCount % 2) * 34.f + 34) };

				if (rc.left < vMouse.x &&
					rc.right > vMouse.x &&
					rc.top < vMouse.y &&
					rc.bottom > vMouse.y)
				{
					list<CGameObject*>::iterator iter2 = m_InterpaceUnitList.begin();
					list<CGameObject*>::iterator iter_end2 = m_InterpaceUnitList.end();

					for (; iter2 != iter_end2; ++iter2)
					{
						(*iter2)->SetSelect(false);
					}
					iter2 = m_InterpaceUnitList.begin();
					iter_end2 = m_InterpaceUnitList.end();
					for (; iter2 != iter_end2;)
					{
						if ((*iter) != (*iter2))
							iter2 = m_InterpaceUnitList.erase(iter2);
						else
							++iter2;
					}
					NotPortaraits();
					(*iter)->SetSelect(true);
					SetUnitInfo((*iter)->GetUnitInfo());
					OneObjectClick(true);		
					return;
				}
				++iCount;
			}
		}
	}
	else
	{
		list<CGameObject*>::iterator iter = m_InterpaceUnitList.begin();
		list<CGameObject*>::iterator iter_end = m_InterpaceUnitList.end();

		int iCount = 0;

		for (; iter != iter_end; ++iter)
		{
			m_tUnitInfo = (*iter)->GetUnitInfo();

			if (!lstrcmp(m_tUnitInfo.szName, L"Arbiter"))
			{
				m_iSmallWireNumber = 98;
			}
			else if (!lstrcmp(m_tUnitInfo.szName, L"Archon"))
			{
				m_iSmallWireNumber = 28;
			}
			else if (!lstrcmp(m_tUnitInfo.szName, L"Carrier"))
			{
				m_iSmallWireNumber = 77;
			}
			else if (!lstrcmp(m_tUnitInfo.szName, L"Corsair"))
			{
				m_iSmallWireNumber = 91;
			}
			else if (!lstrcmp(m_tUnitInfo.szName, L"DarkArchon"))
			{
				m_iSmallWireNumber = 42;
			}
			else if (!lstrcmp(m_tUnitInfo.szName, L"DarkTemplar"))
			{
				m_iSmallWireNumber = 35;
			}
			else if (!lstrcmp(m_tUnitInfo.szName, L"Dragoon"))
			{
				m_iSmallWireNumber = 14;
			}
			else if (!lstrcmp(m_tUnitInfo.szName, L"HighTemplar"))
			{
				m_iSmallWireNumber = 21;
			}
			else if (!lstrcmp(m_tUnitInfo.szName, L"Interceptor"))
			{
				m_iSmallWireNumber = 84;
			}
			else if (!lstrcmp(m_tUnitInfo.szName, L"Observer"))
			{
				m_iSmallWireNumber = 63;
			}
			else if (!lstrcmp(m_tUnitInfo.szName, L"Probe"))
			{
				m_iSmallWireNumber = 0;
			}
			else if (!lstrcmp(m_tUnitInfo.szName, L"Reaver"))
			{
				m_iSmallWireNumber = 49;
			}
			else if (!lstrcmp(m_tUnitInfo.szName, L"Scout"))
			{
				m_iSmallWireNumber = 70;
			}
			else if (!lstrcmp(m_tUnitInfo.szName, L"Shuttle"))
			{
				m_iSmallWireNumber = 56;
			}
			else if (!lstrcmp(m_tUnitInfo.szName, L"Zealot"))
			{
				m_iSmallWireNumber = 7;
			}

			float fPercent = S_FLOAT_CAST(m_tUnitInfo.iHp) / S_FLOAT_CAST(m_tUnitInfo.iHpMax) * 100.f;

			
			
			int iPercentCount = 0;
			if (S_INT_CAST(fPercent) <= 10)
			{
				iPercentCount = 6;
			}
			else if (S_INT_CAST(fPercent) <= 25)
			{
				iPercentCount = 5;
			}
			else if (S_INT_CAST(fPercent) <= 40)
			{
				iPercentCount = 4;
			}
			else if (S_INT_CAST(fPercent) <= 55)
			{
				iPercentCount = 3;
			}
			else if (S_INT_CAST(fPercent) <= 70)
			{
				iPercentCount = 2;
			}
			else if (S_INT_CAST(fPercent) <= 85)
			{
				iPercentCount = 1;
			}

			const TEX_INFO* pBigWireTexture = TextureMgr->GetTexture(L"Wirefram"
				, L"SmallWirefram", m_iSmallWireNumber + iPercentCount);

			float fX = pBigWireTexture->ImageInfo.Width / 2.f;
			float fY = pBigWireTexture->ImageInfo.Height / 2.f;



			D3DXMatrixTranslation(&matTrans
				, fX + 155.f + (iCount / 2) * fX * 2
				, fY + 390.f + (iCount % 2) * fY * 2
				, 0.f);

			Device->GetSprite()->SetTransform(&matTrans);
			Device->GetSprite()->Draw(pBigWireTexture->pTexture
				, NULL
				, &D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(255, 255, 255, 255));

			++iCount;
		}
	}
}

void CInterpace::RenderNormalInterpace(void)
{
	list<CGameObject*>::iterator iter = m_InterpaceUnitList.begin();
	list<CGameObject*>::iterator iter_end = m_InterpaceUnitList.end();

	for (; iter != iter_end; ++iter)
	{
		if ((*iter)->GetState() == UNIT_ATTACK)
		{
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_iClickMyInterpace[2] = 1;
			break;
		}
		else if ((*iter)->GetState() == UNIT_MOVE)
		{
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_iClickMyInterpace[0] = 1;
			break;
		}
		else if ((*iter)->GetState() == UNIT_HOLD)
		{
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_iClickMyInterpace[4] = 1;
			break;
		}
		else if ((*iter)->GetState() == UNIT_PATROL)
		{
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_iClickMyInterpace[3] = 1;
			break;
		}
	}

	bool bCheck = false;
	for (int i = 0; i < 9; ++i)
	{
		if (m_iClickMyInterpace[i] == 1)
		{
			bCheck = true;
			break;
		}
	}

	if (!bCheck)
	{
		for (int i = 0; i < 9; ++i)
			m_iClickMyInterpace[i] = 0;
		m_iClickMyInterpace[1] = 1;
	}

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

	// 3번째 칸
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

void CInterpace::RenderUnNormalnInterpace(int iCount)
{
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

	// 3번째 칸
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

	if (iCount == 1)
	{
		// 5번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Mining", m_iClickMyInterpace[4]);

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
	else if (iCount == 2)
	{

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


		if (1 == GetUpgradeCount(m_eTeamType, UPGRADE_HIGHTEMPLAR_STORM))
		{
			m_iClickMyInterpace[6] = 0;
		}
		else
		{
			m_iClickMyInterpace[6] = 2;
		}

		if (1 == GetUpgradeCount(m_eTeamType, UPGRADE_HIGHTEMPLAR_ILLUSION))
		{
			m_iClickMyInterpace[7] = 0;
		}
		else
		{
			m_iClickMyInterpace[7] = 1;
		}
		// 7번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Storm", m_iClickMyInterpace[6]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 505.f + fX
			, 440.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));


		// 8번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"illusion", m_iClickMyInterpace[7]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 550.f + fX
			, 440.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		// 9번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Archon", m_iClickMyInterpace[8]);

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
	else if (iCount == 3)
	{
		// 9번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Dark Archon", 0);

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
	else if (iCount == 5)
	{
		if (1 == GetUpgradeCount(m_eTeamType, UPGRADE_ARBITER_RECOLL))
		{
			m_iClickMyInterpace[6] = 0;
		}
		else
		{
			m_iClickMyInterpace[6] = 2;
		}

		if (1 == GetUpgradeCount(m_eTeamType, UPGRADE_ARBITER_STACISFIELD))
		{
			m_iClickMyInterpace[7] = 0;
		}
		else
		{
			m_iClickMyInterpace[7] = 1;
		}

		// 7번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Recall", m_iClickMyInterpace[6]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 505.f + fX
			, 440.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));


		// 8번째 칸
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"StasisField", m_iClickMyInterpace[7]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 550.f + fX
			, 440.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
}

void CInterpace::FrameMove(void)
{
	m_tFrame.fFrame += GET_TIME * m_tFrame.fCount;
	m_tNoise.fFrame += GET_TIME * m_tNoise.fCount;

	if (m_tFrame.fFrame > m_tFrame.fMax)
	{
		m_tFrame.fFrame = m_fPortraitsFrameFirst;
	}
}

void CInterpace::SettingPortraits(void)
{
	if (!m_bRenderPortraits)
		return;

	if (m_bSelectBuild)
	{
		lstrcpy(szPortraitsStateKey, L"Aldaris");
		m_tFrame = FRAME(0.f, 6.5f, 74.f);
	}
	else if (!lstrcmp(m_tInfo.szObjKey, L"Arbiter"))
	{
		lstrcpy(szPortraitsStateKey, L"Arbiter");
		m_tFrame = FRAME(0.f, 6.5f, 75.f);
	}
	else if (!lstrcmp(m_tInfo.szObjKey, L"Archon"))
	{
		lstrcpy(szPortraitsStateKey, L"Archon");
		m_tFrame = FRAME(0.f, 6.5f, 69.f);
	}
	else if (!lstrcmp(m_tInfo.szObjKey, L"Carrier"))
	{
		lstrcpy(szPortraitsStateKey, L"Carrier");
		m_tFrame = FRAME(0.f, 6.5f, 74.f);
	}
	else if (!lstrcmp(m_tInfo.szObjKey, L"Corsair"))
	{
		lstrcpy(szPortraitsStateKey, L"Corsair");
		m_tFrame = FRAME(0.f, 6.5f, 74.f);
	}
	else if (!lstrcmp(m_tInfo.szObjKey, L"DarkArchon"))
	{
		lstrcpy(szPortraitsStateKey, L"Dark Archon");
		m_tFrame = FRAME(0.f, 6.5f, 69.f);
	}
	else if (!lstrcmp(m_tInfo.szObjKey, L"DarkTemplar"))
	{
		lstrcpy(szPortraitsStateKey, L"Dark Templar");
		m_tFrame = FRAME(0.f, 6.5f, 59.f);
	}
	else if (!lstrcmp(m_tInfo.szObjKey, L"Dragoon"))
	{
		lstrcpy(szPortraitsStateKey, L"Dragoon");
		m_tFrame = FRAME(0.f, 6.5f, 74.f);
	}
	else if (!lstrcmp(m_tInfo.szObjKey, L"HighTemplar"))
	{
		lstrcpy(szPortraitsStateKey, L"High Templar");
		m_tFrame = FRAME(0.f, 6.5f, 76.f);
	}
	else if (!lstrcmp(m_tInfo.szObjKey, L"Interceptor"))
	{
		lstrcpy(szPortraitsStateKey, L"Interceptor");
		m_tFrame = FRAME(0.f, 6.5f, 74.f);
	}
	else if (!lstrcmp(m_tInfo.szObjKey, L"Observer"))
	{
		lstrcpy(szPortraitsStateKey, L"Observer");
		m_tFrame = FRAME(0.f, 6.5f, 74.f);
	}
	else if (!lstrcmp(m_tInfo.szObjKey, L"Probe"))
	{
		lstrcpy(szPortraitsStateKey, L"Probe");
		m_tFrame = FRAME(0.f, 6.5f, 74.f);
	}
	else if (!lstrcmp(m_tInfo.szObjKey, L"Reaver"))
	{
		lstrcpy(szPortraitsStateKey, L"Reaver");
		m_tFrame = FRAME(0.f, 6.5f, 74.f);
	}
	else if (!lstrcmp(m_tInfo.szObjKey, L"Scout"))
	{
		lstrcpy(szPortraitsStateKey, L"Scout");
		m_tFrame = FRAME(0.f, 6.5f, 64.f);
	}
	else if (!lstrcmp(m_tInfo.szObjKey, L"Shuttle"))
	{
		lstrcpy(szPortraitsStateKey, L"Shuttle");
		m_tFrame = FRAME(0.f, 6.5f, 74.f);
	}
	else if (!lstrcmp(m_tInfo.szObjKey, L"Zealot"))
	{
		lstrcpy(szPortraitsStateKey, L"Zealot");
		m_tFrame = FRAME(0.f, 6.5f, 49.f);
	}
}

void CInterpace::SettingInfo(void)
{
	if (m_bSelectUnit)
	{
		lstrcpy(m_tInfo.szObjKey, m_tUnitInfo.szName);
	}
	else if (m_bSelectBuild)
	{
		lstrcpy(m_tInfo.szObjKey, m_tBuildInfo.szName);
	}
	m_bRenderPortraits = true;
	m_tNoise.fFrame = 0.f;
}

void CInterpace::SettingCenterInterpase(void)
{
	if (m_bSelectUnit)
	{
		if (!lstrcmp(m_tInfo.szObjKey, L"Arbiter"))
		{
			m_iBigWireNumber = 98;
			m_iStatsNumber = 12;
		}
		else if (!lstrcmp(m_tInfo.szObjKey, L"Archon"))
		{
			m_iBigWireNumber = 28;
			m_iStatsNumber = 10;
		}
		else if (!lstrcmp(m_tInfo.szObjKey, L"Carrier"))
		{
			m_iBigWireNumber = 77;
			m_iStatsNumber = 13;
		}
		else if (!lstrcmp(m_tInfo.szObjKey, L"Corsair"))
		{
			m_iBigWireNumber = 91;
			m_iStatsNumber = 19;
		}
		else if (!lstrcmp(m_tInfo.szObjKey, L"DarkArchon"))
		{
			m_iBigWireNumber = 42;
			m_iStatsNumber = 11;
		}
		else if (!lstrcmp(m_tInfo.szObjKey, L"DarkTemplar"))
		{
			m_iBigWireNumber = 35;
			m_iStatsNumber = 14;
		}
		else if (!lstrcmp(m_tInfo.szObjKey, L"Dragoon"))
		{
			m_iBigWireNumber = 14;
			m_iStatsNumber = 4;
		}
		else if (!lstrcmp(m_tInfo.szObjKey, L"HighTemplar"))
		{
			m_iBigWireNumber = 21;
			m_iStatsNumber = 6;
		}
		else if (!lstrcmp(m_tInfo.szObjKey, L"Interceptor"))
		{
			m_iBigWireNumber = 84;
			m_iStatsNumber = 1;
		}
		else if (!lstrcmp(m_tInfo.szObjKey, L"Observer"))
		{
			m_iBigWireNumber = 63;
			m_iStatsNumber = 2;
		}
		else if (!lstrcmp(m_tInfo.szObjKey, L"Probe"))
		{
			m_iBigWireNumber = 0;
			m_iStatsNumber = 0;
		}
		else if (!lstrcmp(m_tInfo.szObjKey, L"Reaver"))
		{
			m_iBigWireNumber = 49;
			m_iStatsNumber = 5;
		}
		else if (!lstrcmp(m_tInfo.szObjKey, L"Scout"))
		{
			m_iBigWireNumber = 70;
			m_iStatsNumber = 8;
		}
		else if (!lstrcmp(m_tInfo.szObjKey, L"Shuttle"))
		{
			m_iBigWireNumber = 56;
			m_iStatsNumber = 7;
		}
		else if (!lstrcmp(m_tInfo.szObjKey, L"Zealot"))
		{
			m_iBigWireNumber = 7;
			m_iStatsNumber = 3;
		}
	}
	else if (m_bSelectBuild)
	{
		if (!lstrcmp(m_tInfo.szObjKey, L"Nexus"))
		{
			m_iBigWireNumber = 105;
			m_iStatsNumber = 15;
		}
		if (!lstrcmp(m_tInfo.szObjKey, L"Pylon"))
		{
			m_iBigWireNumber = 119;
			m_iStatsNumber = 16;
		}
		if (!lstrcmp(m_tInfo.szObjKey, L"Assimilator"))
		{
			m_iBigWireNumber = 126;
			m_iStatsNumber = 17;
		}
		if (!lstrcmp(m_tInfo.szObjKey, L"GateWay"))
		{
			m_iBigWireNumber = 140;
			m_iStatsNumber = 18;
		}
		if (!lstrcmp(m_tInfo.szObjKey, L"Forge"))
		{
			m_iBigWireNumber = 175;
			m_iStatsNumber = 19;
		}
		if (!lstrcmp(m_tInfo.szObjKey, L"PhotonCannon"))
		{
			m_iBigWireNumber = 147;
			m_iStatsNumber = 20;
		}
		if (!lstrcmp(m_tInfo.szObjKey, L"CyberneticsCore"))
		{
			m_iBigWireNumber = 161;
			m_iStatsNumber = 21;
		}
		if (!lstrcmp(m_tInfo.szObjKey, L"ShieldBattery"))
		{
			m_iBigWireNumber = 210;
			m_iStatsNumber = 22;
		}
		if (!lstrcmp(m_tInfo.szObjKey, L"RoboticsFacility"))
		{
			m_iBigWireNumber = 112;
			m_iStatsNumber = 23;
		}
		if (!lstrcmp(m_tInfo.szObjKey, L"StarGate"))
		{
			m_iBigWireNumber = 182;
			m_iStatsNumber = 24;
		}
		if (!lstrcmp(m_tInfo.szObjKey, L"CitadelofAdun"))
		{
			m_iBigWireNumber = 154;
			m_iStatsNumber = 25;
		}
		if (!lstrcmp(m_tInfo.szObjKey, L"RoboticsSupportBay"))
		{
			m_iBigWireNumber = 203;
			m_iStatsNumber = 26;
		}
		if (!lstrcmp(m_tInfo.szObjKey, L"FleetBeacon"))
		{
			m_iBigWireNumber = 189;
			m_iStatsNumber = 27;
		}
		if (!lstrcmp(m_tInfo.szObjKey, L"TemplarArchives"))
		{
			m_iBigWireNumber = 168;
			m_iStatsNumber = 28;
		}
		if (!lstrcmp(m_tInfo.szObjKey, L"Observatory"))
		{
			m_iBigWireNumber = 133;
			m_iStatsNumber = 29;
		}
		if (!lstrcmp(m_tInfo.szObjKey, L"ArbiterTribunal"))
		{
			m_iBigWireNumber = 196;
			m_iStatsNumber = 30;
		}
	}
}

void CInterpace::SettingInterpaceRect(void)
{
	m_tInterpaceRect[0] = { 505, 357, 550, 392 };
	m_tInterpaceRect[1] = { 550, 357, 585, 392 };
	m_tInterpaceRect[2] = { 595, 357, 630, 392 };
	m_tInterpaceRect[3] = { 505, 397, 550, 432 };
	m_tInterpaceRect[4] = { 550, 397, 585, 432 };
	m_tInterpaceRect[5] = { 595, 397, 630, 432 };
	m_tInterpaceRect[6] = { 505, 440, 550, 475 };
	m_tInterpaceRect[7] = { 550, 440, 585, 475 };
	m_tInterpaceRect[8] = { 595, 440, 630, 475 };
}
