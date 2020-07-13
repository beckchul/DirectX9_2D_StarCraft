#include "stdafx.h"
#include "Mouse.h"
#include "TimeMgr.h"
#include "Device.h"
#include "TextureMgr.h"
#include "ObjMgr.h"
#include "BackGround.h"
#include "KeyMgr.h"
#include "Interpace.h"
#include "Interceptor.h"

CMouse::CMouse()
{
}

CMouse::~CMouse()
{
	Release();
}

HRESULT CMouse::Initialize(void)
{
	ShowCursor(FALSE);
	lstrcpy(m_tInfo.szObjKey, L"Mouse");
	lstrcpy(m_tInfo.szStateKey, L"Normal");
	m_tFrame = FRAME(0.f, 5.f, 4.f);
	m_iFrameFirst = S_INT_CAST(m_tFrame.fFrame);
	m_dwState = MOUSE_NOMAL;
	m_dwSubState = m_dwState;
	m_bOnDragClick = false;
	m_bBuildClick = false;
	m_bBuildSave = false;
	m_eRenderType = RENDER_MOUSE;
	m_dwSubState = MOUSE_NOMAL;
	return S_OK;
}

int CMouse::Update(void)
{
	RallyPoint();
	Drag();
	GetMousePos();
	TileCheck();
	ObjectCheck();
	CreateBuildCheck();
	Frame();

	return 0;
}

void CMouse::Render(void)
{
	D3DXMATRIX matTrans;
	const TEX_INFO* pMousetexture;
	float fX = 0.f, fY = 0.f;
	if (m_bOnDragClick && !m_bBuildClick)
	{
		D3DXMATRIX matScale;

		fX = m_tInfo.vPos.x - m_vDragPos.x;
		fY = m_tInfo.vPos.y - m_vDragPos.y;



		float fScaleX;
		float fScaleY;

		if (fabs(fX) < 50 && fabs(fY) < 50)
		{
			pMousetexture = TextureMgr->GetTexture(L"Mouse", L"Drag", 3);
			fScaleX = fX / 50.f;
			fScaleY = fY / 50.f;
		}
		else if (fabs(fX) < 100 && fabs(fY) < 100)
		{
			pMousetexture = TextureMgr->GetTexture(L"Mouse", L"Drag", 2);
			fScaleX = fX / 100.f;
			fScaleY = fY / 100.f;
		}
		else
		{
			pMousetexture = TextureMgr->GetTexture(L"Mouse", L"Drag", 1);
			fScaleX = fX / 200.f;
			fScaleY = fY / 200.f;
		}

		D3DXMatrixScaling(&matScale, fScaleX, fScaleY, 0.f);

		D3DXMatrixTranslation(&matTrans
			, m_vDragPos.x - m_vScroll.x
			, m_vDragPos.y - m_vScroll.y
			, 0.f);

		matScale *= matTrans;

		Device->GetSprite()->SetTransform(&matScale);
		Device->GetSprite()->Draw(pMousetexture->pTexture
			, NULL//&rc
			, NULL//&D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));
	}


	pMousetexture = TextureMgr->GetTexture(m_tInfo.szObjKey
		, m_tInfo.szStateKey, S_INT_CAST(m_tFrame.fFrame));

	D3DXMatrixTranslation(&matTrans
		, m_ptMousePos.x
		, m_ptMousePos.y
		, 0.f);

	fX = pMousetexture->ImageInfo.Width / 2.f;
	fY = pMousetexture->ImageInfo.Height / 2.f;


	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pMousetexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));

	if (m_bBuildClick)
	{
		CreateBuild();
	}
}

void CMouse::Release(void)
{
}

void CMouse::CreateBuild(void)
{
	int iX = S_INT_CAST(m_tInfo.vPos.x / TILECX);
	int iY = S_INT_CAST(m_tInfo.vPos.y / TILECY);

	int iIndex = iX + (TILEEX * iY);

	if (iIndex < 0 || iIndex >= TILEEX * TILEEY)
		return;

	vector<PTILE> vecTile = D_CAST(CBackGround, (ObjMgr->GetList()[OBJ_BACKGROUND].front()))->GetTile();

	D3DXVECTOR3 vBuildRenderPos = vecTile[iIndex]->vPos - D3DXVECTOR3(vecTile[iIndex]->vSize.x / 2.f, vecTile[iIndex]->vSize.y / 2.f, 0.f);


	D3DXMATRIX matTrans;

	// 칸수 가져오기
	list<BUILD*> BuildList = ObjMgr->GetBuildList();

	list<BUILD*>::iterator iter = BuildList.begin();
	list<BUILD*>::iterator iter_end = BuildList.end();

	switch (m_eBuildType)
	{
	case BUILD_NEXUS:
		lstrcpy(m_szBuildKey, L"Nexus");
		break;
	case BUILD_PYLON:
		lstrcpy(m_szBuildKey, L"Pylon");
		break;
	case BUILD_ASSIMILATOR:
		lstrcpy(m_szBuildKey, L"Assimilator");
		break;
	case BUILD_GATEWAY:
		lstrcpy(m_szBuildKey, L"GateWay");
		break;
	case BUILD_FORGE:
		lstrcpy(m_szBuildKey, L"Forge");
		break;
	case BUILD_PHOTON_CANNON:
		lstrcpy(m_szBuildKey, L"PhotonCannon");
		break;
	case BUILD_CYBERNETICS_CORE:
		lstrcpy(m_szBuildKey, L"CyberneticsCore");
		break;
	case BUILD_SHIELD_BATTERY:
		lstrcpy(m_szBuildKey, L"ShieldBattery");
		break;
	case BUILD_ROBOTICS_FACILITY:
		lstrcpy(m_szBuildKey, L"RoboticsFacility");
		break;
	case BUILD_STARGATE:
		lstrcpy(m_szBuildKey, L"StarGate");
		break;
	case BUILD_CITADEL_OF_ADUN:
		lstrcpy(m_szBuildKey, L"CitadelofAdun");
		break;
	case BUILD_ROBOTICS_SUPPORT_BAY:
		lstrcpy(m_szBuildKey, L"RoboticsSupportBay");
		break;
	case BUILD_FLEET_BEACON:
		lstrcpy(m_szBuildKey, L"FleetBeacon");
		break;
	case BUILD_TEMPAR_ARCHIVES:
		lstrcpy(m_szBuildKey, L"TemplarArchives");
		break;
	case BUILD_OBSERVATORY:
		lstrcpy(m_szBuildKey, L"Observatory");
		break;
	case BUILD_ARBITER_TRIBUNAL:
		lstrcpy(m_szBuildKey, L"ArbiterTribunal");
		break;
	}

	for (; iter != iter_end; ++iter)
	{
		if (!lstrcmp((*iter)->szName, m_szBuildKey))
		{
			m_tBuildInfo = (*(*iter));
			break;
		}
	}

	iX = S_INT_CAST((vBuildRenderPos.x - 1) / TILECX);
	iY = S_INT_CAST((vBuildRenderPos.y - 1) / TILECY);

	int iIndex2 = iX + (TILEEX * iY);



	const TEX_INFO* BuildTexture = TextureMgr->GetTexture(L"Preview"
		, L"Building", S_INT_CAST(m_eBuildType));

	if (m_tBuildInfo.iSizeX == 2 && m_tBuildInfo.iSizeY == 2)
	{
		D3DXMatrixTranslation(&matTrans
			, vBuildRenderPos.x - m_vScroll.x
			, vBuildRenderPos.y - m_vScroll.y
			, 0.f);
	}
	if (m_tBuildInfo.iSizeX == 3 && m_tBuildInfo.iSizeY == 2)
	{
		D3DXMatrixTranslation(&matTrans
			, vBuildRenderPos.x - m_vScroll.x + TILECX / 2.f
			, vBuildRenderPos.y - m_vScroll.y
			, 0.f);
	}
	if (m_tBuildInfo.iSizeX == 4 && m_tBuildInfo.iSizeY == 3)
	{
		iIndex2 -= 1;
		D3DXMatrixTranslation(&matTrans
			, vBuildRenderPos.x - m_vScroll.x
			, vBuildRenderPos.y - m_vScroll.y + TILECY / 2.f
			, 0.f);
	}

	if (iIndex2 < 0 || iIndex2 >= TILEEX * TILEEY)
		return;


	float fX = BuildTexture->ImageInfo.Width / 2.f;
	float fY = BuildTexture->ImageInfo.Height / 2.f;


	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(BuildTexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(150, 255, 255, 255));



	m_bBuildSave = true;

	for (int i = 0; i < m_tBuildInfo.iSizeX; ++i)
	{
		for (int j = 0; j < m_tBuildInfo.iSizeY; ++j)
		{
			bool bTrue = false;

			if (iIndex2 + i + j * TILEEX < 0 || iIndex2 + i + j * TILEEX >= TILEEX * TILEEY)
				return;

			m_vBuildPos = vecTile[iIndex2]->vPos;

			D3DXVECTOR3 vBuildPos = vecTile[iIndex2 + i + j * TILEEX]->vPos;

			const TEX_INFO* CheckTexture;

			if (!lstrcmp(m_szBuildKey, L"Assimilator"))
			{
				if (vecTile[iIndex2 + i + j * TILEEX]->byOption == 3)
				{
					CheckTexture = TextureMgr->GetTexture(L"Preview"
						, L"Check", 0);
				}
				else
				{
					CheckTexture = TextureMgr->GetTexture(L"Preview"
						, L"Check", 1);
					m_bBuildSave = false;
				}
			}
			else if (!lstrcmp(m_szBuildKey, L"Nexus"))
			{
				if ((vecTile[iIndex2 + i + j * TILEEX]->byDrawID == 1 ||
					vecTile[iIndex2 + i + j * TILEEX]->byDrawID == 3))
				{
					CheckTexture = TextureMgr->GetTexture(L"Preview"
						, L"Check", 1);

					m_bBuildSave = false;
				}
				else
				{
					if (vecTile[iIndex2 + i + j * TILEEX]->byOption == 0 ||
						vecTile[iIndex2 + i + j * TILEEX]->byOption == 4)
					{
						CheckTexture = TextureMgr->GetTexture(L"Preview"
							, L"Check", 0);
					}
					else
					{
						CheckTexture = TextureMgr->GetTexture(L"Preview"
							, L"Check", 1);

						m_bBuildSave = false;
					}
				}
			}
			else if (vecTile[iIndex2 + i + j * TILEEX]->byOption == 0 ||
				vecTile[iIndex2 + i + j * TILEEX]->byOption == 4)
			{

				if (!lstrcmp(m_szBuildKey, L"Pylon"))
				{
					CheckTexture = TextureMgr->GetTexture(L"Preview"
						, L"Check", 0);
				}
				else
				{
					if (vecTile[iIndex2 + i + j * TILEEX]->byDrawID == 2 ||
						vecTile[iIndex2 + i + j * TILEEX]->byDrawID == 3)
					{
						CheckTexture = TextureMgr->GetTexture(L"Preview"
							, L"Check", 0);
					}
					else
					{
						for (int i = 0; i < m_tBuildInfo.iSizeX; ++i)
						{
							for (int j = 0; j < m_tBuildInfo.iSizeY; ++j)
							{

								if (iIndex2 + i + j * TILEEX < 0 || iIndex2 + i + j * TILEEX >= TILEEX * TILEEY)
									return;

								m_vBuildPos = vecTile[iIndex2]->vPos;

								D3DXVECTOR3 vBuildPos = vecTile[iIndex2 + i + j * TILEEX]->vPos;

								CheckTexture = TextureMgr->GetTexture(L"Preview"
									, L"Check", 1);


								fX = CheckTexture->ImageInfo.Width / 2.f;
								fY = CheckTexture->ImageInfo.Height / 2.f;

								D3DXMatrixTranslation(&matTrans
									, vBuildPos.x - m_vScroll.x
									, vBuildPos.y - m_vScroll.y
									, 0.f);

								Device->GetSprite()->SetTransform(&matTrans);
								Device->GetSprite()->Draw(CheckTexture->pTexture
									, NULL
									, &D3DXVECTOR3(fX, fY, 0.f)
									, NULL
									, D3DCOLOR_ARGB(150, 255, 255, 255));

								m_bBuildSave = false;
							}
						}

						return;
					}
				}

			}
			else
			{
				CheckTexture = TextureMgr->GetTexture(L"Preview"
					, L"Check", 1);

				m_bBuildSave = false;
			}




			fX = CheckTexture->ImageInfo.Width / 2.f;
			fY = CheckTexture->ImageInfo.Height / 2.f;

			D3DXMatrixTranslation(&matTrans
				, vBuildPos.x - m_vScroll.x
				, vBuildPos.y - m_vScroll.y
				, 0.f);

			Device->GetSprite()->SetTransform(&matTrans);
			Device->GetSprite()->Draw(CheckTexture->pTexture
				, NULL
				, &D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(150, 255, 255, 255));

			list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

			list<CGameObject*>::iterator iter = pUnitList.begin();
			list<CGameObject*>::iterator iter_end = pUnitList.end();

			for (; iter != iter_end; ++iter)
			{
				if (!(*iter)->GetSelect())
				{
					if ((*iter)->GetUnitInfo().eType == UNIT_ARR)
						continue;
					int iX = S_INT_CAST((*iter)->GetPos().x / TILECX);
					int iY = S_INT_CAST((*iter)->GetPos().y / TILECY);
					int iIndex3 = iX + (TILEEX * iY);
					if (iIndex3 < 0 || iIndex3 >= TILEEX * TILEEY)
						return;


					if ((iIndex2 + i + j * TILEEX) == iIndex3)
					{
						m_bBuildSave = false;
						m_vBuildPos = vecTile[iIndex2]->vPos;
						D3DXVECTOR3 vBuildPos = vecTile[iIndex2 + i + j * TILEEX]->vPos;
						const TEX_INFO* CheckTexture = TextureMgr->GetTexture(L"Preview"
							, L"Check", 1);
						fX = CheckTexture->ImageInfo.Width / 2.f;
						fY = CheckTexture->ImageInfo.Height / 2.f;

						D3DXMatrixTranslation(&matTrans
							, vBuildPos.x - m_vScroll.x
							, vBuildPos.y - m_vScroll.y
							, 0.f);

						Device->GetSprite()->SetTransform(&matTrans);
						Device->GetSprite()->Draw(CheckTexture->pTexture
							, NULL
							, &D3DXVECTOR3(fX, fY, 0.f)
							, NULL
							, D3DCOLOR_ARGB(150, 255, 255, 255));
					}
				}
			}
		}
	}
}

void CMouse::RallyPoint(void)
{
	if (m_dwState != MOUSE_TARGET_RALLY)
		return;

	DWORD dwKey = GET_KEY;

	if (dwKey & KEY_LBUTTON)
	{
		if (m_ptMousePos.x >= 490 && m_ptMousePos.y >= 340 &&
			m_ptMousePos.x <= 640 && m_ptMousePos.y <= 480)
			return;

		if (m_ptMousePos.x >= 150 && m_ptMousePos.y >= 380 &&
			m_ptMousePos.x <= 490 && m_ptMousePos.y <= 480)
			return;

		if (m_ptMousePos.x >= 0 && m_ptMousePos.y >= 320 &&
			m_ptMousePos.x <= 150 && m_ptMousePos.y <= 480)
			return;

		m_pRallyPointObject->SetStartingPoint(m_tInfo.vPos);
		m_dwState = MOUSE_CLICK;
		return;
	}

}

void CMouse::CreateBuildCheck(void)
{
	list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

	list<CGameObject*>::iterator iter = pUnitList.begin();
	list<CGameObject*>::iterator iter_end = pUnitList.end();

	for (; iter != iter_end; ++iter)
	{
		if ((*iter)->GetBuildType() != BUILD_END &&
			(*iter)->GetBuildType() != BUILD_GROUND &&
			(*iter)->GetBuildType() != BUILD_ARR &&
			!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe"))
		{
			m_bBuildClick = true;

			m_eBuildType = (*iter)->GetBuildType();
			return;
		}
	}

	m_bBuildClick = false;
}

void CMouse::TileCheck(void)
{
	if (m_bBuildClick ||
		m_dwState == MOUSE_TARGET_RALLY ||
		m_dwState == MOUSE_ATTACK_POINT_NOMAL ||
		m_dwState == MOUSE_ATTACK_POINT_PLAYER ||
		m_dwState == MOUSE_ATTACK_POINT_ENEMY ||
		m_dwState == MOUSE_ATTACK_POINT_PATOL ||
		m_dwState == MOUSE_SKILL_POINT ||
		m_dwState == MOUSE_SKILL_POINT2 || 
		m_dwState == MOUSE_SKILL_POINT3)
		return;

	if (m_ptMousePos.x >= 490 && m_ptMousePos.y >= 340 &&
		m_ptMousePos.x <= 640 && m_ptMousePos.y <= 480)
		return;

	if (m_ptMousePos.x >= 150 && m_ptMousePos.y >= 380 &&
		m_ptMousePos.x <= 490 && m_ptMousePos.y <= 480)
		return;

	if (m_ptMousePos.x >= 0 && m_ptMousePos.y >= 320 &&
		m_ptMousePos.x <= 150 && m_ptMousePos.y <= 480)
		return;

	int iX = S_INT_CAST(m_tInfo.vPos.x / TILECX);
	int iY = S_INT_CAST(m_tInfo.vPos.y / TILECY);

	int iIndex = iX + (TILEEX * iY);

	if (iIndex < 0 || iIndex >= TILEEX * TILEEY)
		return;

	vector<PTILE> vecTile = D_CAST(CBackGround, (ObjMgr->GetList()[OBJ_BACKGROUND].front()))->GetTile();


	DWORD dwKey = GET_KEY;

	if (vecTile[iIndex]->byOption == 2 || vecTile[iIndex]->byOption == 3)
	{
		if (m_dwState == MOUSE_NOMAL)
			m_dwState = MOUSE_OBJECT;
	}
}

void CMouse::Drag(void)
{
	if (m_dwState == MOUSE_TARGET_RALLY ||
		m_dwState == MOUSE_ATTACK_POINT_NOMAL ||
		m_dwState == MOUSE_ATTACK_POINT_PLAYER ||
		m_dwState == MOUSE_ATTACK_POINT_ENEMY ||
		m_dwState == MOUSE_ATTACK_POINT_PATOL ||
		m_dwState == MOUSE_SKILL_POINT ||
		m_dwState == MOUSE_SKILL_POINT2 ||
		m_dwState == MOUSE_SKILL_POINT3)
		return;
	if (m_bBuildClick)
	{
		m_dwState = MOUSE_NOMAL;
		m_bOnDragClick = false;
		return;
	}

	DWORD dwKey = GET_KEY;

	
	if (m_ptMousePos.x >= 6 && m_ptMousePos.x < 134 && m_ptMousePos.y >= 348 && m_ptMousePos.y < 476)
		return;

	if (dwKey & KEY_LBUTTON)
	{
		if (!m_bOnDragClick)
		{
			m_vDragPos = m_tInfo.vPos;
			m_bOnDragClick = true;
			return;
		}

		if (m_vDragPos != m_tInfo.vPos)
		{
			m_dwState = MOUSE_DRAG;
		}

	}
	else if (m_bOnDragClick)
	{
		if (fabs(m_vDragPos.x - m_tInfo.vPos.x) < 15.f && fabs(m_vDragPos.y - m_tInfo.vPos.y) < 15.f)
		{		// 아주 조금 실수로 드래그 하는것 방지
		}
		else if (m_bOnDragClick == true && m_vDragPos != m_tInfo.vPos)	// 드래그 클릭 했을시
		{
			int iCount = 0;
			float fX = 0.f, fY = 0.f;

			list<CGameObject*> pObjList = ObjMgr->GetList()[OBJ_BUILD];

			list<CGameObject*>::iterator iter = pObjList.begin();
			list<CGameObject*>::iterator iter_end = pObjList.end();

			for (; iter != iter_end; ++iter)
			{
				(*iter)->SetSelect(false);
			}

			pObjList = ObjMgr->GetList()[OBJ_UNIT];

			iter = pObjList.begin();
			iter_end = pObjList.end();

			for (; iter != iter_end; ++iter)
			{
				(*iter)->SetSelect(false);
			}
			iter = pObjList.begin();
			iter_end = pObjList.end();


			if (m_vDragPos.x < m_tInfo.vPos.x && m_vDragPos.y < m_tInfo.vPos.y)	 // 드래그 클릭 시 유닛 검사
			{
				for (; iter != iter_end; ++iter)
				{
					if ((*iter)->GetTeamType() == TEAM_PLAYER &&
						(*iter)->GetPos().x > m_vDragPos.x &&
						(*iter)->GetPos().x < m_tInfo.vPos.x &&
						(*iter)->GetPos().y > m_vDragPos.y &&
						(*iter)->GetPos().y < m_tInfo.vPos.y)
					{
						if (!lstrcmp((*iter)->GetInfo().szObjKey, L"Interceptor") && !D_CAST(CInterceptor, (*iter))->InterceptorRender())
							continue;
						(*iter)->SetSelect(true);

						++iCount;
						CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
						if (iCount == 1)
						{
							D_CAST(CInterpace, pInterpace)->NotPortaraits();
							D_CAST(CInterpace, pInterpace)->SetUnitInfo((*iter)->GetUnitInfo());
							D_CAST(CInterpace, pInterpace)->OneObjectClick(true);
							D_CAST(CInterpace, pInterpace)->ClearInterpaceUnitList();
							D_CAST(CInterpace, pInterpace)->SetInterpaceUnitList(*iter);
						}
						else
						{
							D_CAST(CInterpace, pInterpace)->OneObjectClick(false);
							D_CAST(CInterpace, pInterpace)->SetInterpaceUnitList(*iter);
						}
						if (iCount >= 12)
							break;
					}
				}
			}
			else if (m_vDragPos.x > m_tInfo.vPos.x && m_vDragPos.y < m_tInfo.vPos.y) // 드래그 클릭 시 유닛 검사
			{
				for (; iter != iter_end; ++iter)
				{
					if ((*iter)->GetTeamType() == TEAM_PLAYER &&
						(*iter)->GetPos().x < m_vDragPos.x &&
						(*iter)->GetPos().x > m_tInfo.vPos.x &&
						(*iter)->GetPos().y > m_vDragPos.y &&
						(*iter)->GetPos().y < m_tInfo.vPos.y)
					{
						if (!lstrcmp((*iter)->GetInfo().szObjKey, L"Interceptor") && !D_CAST(CInterceptor, (*iter))->InterceptorRender())
							continue;
						(*iter)->SetSelect(true);

						++iCount;
						CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
						if (iCount == 1)
						{
							D_CAST(CInterpace, pInterpace)->NotPortaraits();
							D_CAST(CInterpace, pInterpace)->SetUnitInfo((*iter)->GetUnitInfo());
							D_CAST(CInterpace, pInterpace)->OneObjectClick(true);
							D_CAST(CInterpace, pInterpace)->ClearInterpaceUnitList();
							D_CAST(CInterpace, pInterpace)->SetInterpaceUnitList(*iter);
						}
						else
						{
							D_CAST(CInterpace, pInterpace)->OneObjectClick(false);
							D_CAST(CInterpace, pInterpace)->SetInterpaceUnitList(*iter);
						}
						if (iCount >= 12)
							break;
					}
				}
			}
			else if (m_vDragPos.x < m_tInfo.vPos.x && m_vDragPos.y > m_tInfo.vPos.y)  // 드래그 클릭 시 유닛 검사
			{
				for (; iter != iter_end; ++iter)
				{
					if ((*iter)->GetTeamType() == TEAM_PLAYER &&
						(*iter)->GetPos().x > m_vDragPos.x &&
						(*iter)->GetPos().x < m_tInfo.vPos.x &&
						(*iter)->GetPos().y < m_vDragPos.y &&
						(*iter)->GetPos().y > m_tInfo.vPos.y)
					{
						if (!lstrcmp((*iter)->GetInfo().szObjKey, L"Interceptor") && !D_CAST(CInterceptor, (*iter))->InterceptorRender())
							continue;
						(*iter)->SetSelect(true);

						++iCount;
						CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
						if (iCount == 1)
						{
							D_CAST(CInterpace, pInterpace)->NotPortaraits();
							D_CAST(CInterpace, pInterpace)->SetUnitInfo((*iter)->GetUnitInfo());
							D_CAST(CInterpace, pInterpace)->OneObjectClick(true);
							D_CAST(CInterpace, pInterpace)->ClearInterpaceUnitList();
							D_CAST(CInterpace, pInterpace)->SetInterpaceUnitList(*iter);

						}
						else
						{
							D_CAST(CInterpace, pInterpace)->OneObjectClick(false);
							D_CAST(CInterpace, pInterpace)->SetInterpaceUnitList(*iter);

						}
						if (iCount >= 12)
							break;
					}
				}
			}
			else if (m_vDragPos.x > m_tInfo.vPos.x && m_vDragPos.y > m_tInfo.vPos.y)  // 드래그 클릭 시 유닛 검사
			{
				for (; iter != iter_end; ++iter)
				{
					if ((*iter)->GetTeamType() == TEAM_PLAYER &&
						(*iter)->GetPos().x < m_vDragPos.x &&
						(*iter)->GetPos().x > m_tInfo.vPos.x &&
						(*iter)->GetPos().y < m_vDragPos.y &&
						(*iter)->GetPos().y > m_tInfo.vPos.y)
					{
						if (!lstrcmp((*iter)->GetInfo().szObjKey, L"Interceptor") && !D_CAST(CInterceptor, (*iter))->InterceptorRender())
							continue;
						(*iter)->SetSelect(true);

						++iCount;
						CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
						if (iCount == 1)
						{
							D_CAST(CInterpace, pInterpace)->NotPortaraits();
							D_CAST(CInterpace, pInterpace)->SetUnitInfo((*iter)->GetUnitInfo());
							D_CAST(CInterpace, pInterpace)->OneObjectClick(true);
							D_CAST(CInterpace, pInterpace)->ClearInterpaceUnitList();
							D_CAST(CInterpace, pInterpace)->SetInterpaceUnitList(*iter);
						}
						else
						{
							D_CAST(CInterpace, pInterpace)->OneObjectClick(false);
							D_CAST(CInterpace, pInterpace)->SetInterpaceUnitList(*iter);
						}
						if (iCount >= 12)
							break;
					}
				}
			}
			if (iCount == 1)
			{

			}
			else if( iCount == 0)
			{
				pObjList = ObjMgr->GetList()[OBJ_BUILD];
				iter = pObjList.begin();
				iter_end = pObjList.end();

				for (; iter != iter_end; ++iter)
				{
					if ((*iter)->GetTeamType() == TEAM_PLAYER &&
						(*iter)->GetPos().x > m_vDragPos.x &&
						(*iter)->GetPos().x < m_tInfo.vPos.x &&
						(*iter)->GetPos().y > m_vDragPos.y &&
						(*iter)->GetPos().y < m_tInfo.vPos.y)
					{
						(*iter)->SetSelect(true);
						CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
						D_CAST(CInterpace, pInterpace)->NotPortaraits();
						D_CAST(CInterpace, pInterpace)->SetBuildInfo((*iter)->GetBuildInfo());
						D_CAST(CInterpace, pInterpace)->OneObjectClick(true);
						D_CAST(CInterpace, pInterpace)->ClearInterpaceUnitList();
						D_CAST(CInterpace, pInterpace)->SetInterpaceUnitList(*iter);
					}
				}
			}
			else if (iCount != 1)		// 유닛을 정렬
			{
				CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
				list<CGameObject*> pList = D_CAST(CInterpace, pInterpace)->GetInterpaceUnitList();


				pList.sort(AttCompare);
				list<CGameObject*>::iterator iter = pList.begin();
				D_CAST(CInterpace, pInterpace)->SetUnitInfo((*iter)->GetUnitInfo());

			}

			CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();		
		}

		bool bClickCheck = false;

		list<CGameObject*> pObjList = ObjMgr->GetList()[OBJ_BUILD];

		list<CGameObject*>::iterator iter = pObjList.begin();
		list<CGameObject*>::iterator iter_end = pObjList.end();

		for (; iter != iter_end; ++iter)
		{
			if ((*iter)->GetSelect())
				bClickCheck = true;
		}

		pObjList = ObjMgr->GetList()[OBJ_UNIT];

		iter = pObjList.begin();
		iter_end = pObjList.end();

		for (; iter != iter_end; ++iter)
		{
			if ((*iter)->GetSelect())
			{
				bClickCheck = true;
			}
		}

		// 아무것도 드래그가 안되었을 때
		if (!bClickCheck)
		{
			CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
			D_CAST(CInterpace, pInterpace)->NotPortaraits();
			D_CAST(CInterpace, pInterpace)->ClearInterpaceUnitList();
		}


		m_dwState = MOUSE_NOMAL;
		m_bOnDragClick = false;
	}
}

void CMouse::ObjectCheck(void)
{
	if (m_bBuildClick ||
		m_dwState == MOUSE_TARGET_RALLY ||
		m_dwState == MOUSE_ATTACK_POINT_NOMAL ||
		m_dwState == MOUSE_ATTACK_POINT_PLAYER || 
		m_dwState == MOUSE_ATTACK_POINT_ENEMY ||
		m_dwState == MOUSE_ATTACK_POINT_PATOL ||
		m_dwState == MOUSE_SKILL_POINT ||
		m_dwState == MOUSE_SKILL_POINT2 ||
		m_dwState == MOUSE_SKILL_POINT3 ||
		m_dwState == MOUSE_CLICK)
		return;

	if (m_dwState == MOUSE_DRAG)
		return;

	DWORD dwKey = GET_KEY;

	list<CGameObject*> pObjList = ObjMgr->GetList()[OBJ_UNIT];

	list<CGameObject*>::iterator iter = pObjList.begin();
	list<CGameObject*>::iterator iter_end = pObjList.end();

	float fL = 0.f, fU = 0.f, fR = 0.f, fD = 0.f;

	for (; iter != iter_end; ++iter)
	{
		if (m_ptMousePos.x >= 490 && m_ptMousePos.y >= 340 &&
			m_ptMousePos.x <= 640 && m_ptMousePos.y <= 480)
			return;

		if (m_ptMousePos.x >= 150 && m_ptMousePos.y >= 380 &&
			m_ptMousePos.x <= 490 && m_ptMousePos.y <= 480)
			return;

		if (m_ptMousePos.x >= 0 && m_ptMousePos.y >= 320 &&
			m_ptMousePos.x <= 150 && m_ptMousePos.y <= 480)
			return;

		float fL = (*iter)->GetInfo().vPos.x - ((*iter)->GetInfo().vSize.x / 2.f);
		float fU = (*iter)->GetInfo().vPos.y - ((*iter)->GetInfo().vSize.y / 2.f);
		float fR = (*iter)->GetInfo().vPos.x + ((*iter)->GetInfo().vSize.x / 2.f);
		float fD = (*iter)->GetInfo().vPos.y + ((*iter)->GetInfo().vSize.y / 2.f);

		if (m_tInfo.vPos.x < fL ||
			m_tInfo.vPos.x > fR ||
			m_tInfo.vPos.y < fU ||
			m_tInfo.vPos.y > fD)
			continue;

		if ((*iter)->GetTeamType() == TEAM_PLAYER)
			m_dwState = MOUSE_PLAYER;
		else if ((*iter)->GetTeamType() == TEAM_ENEMY)
			m_dwState = MOUSE_ENEMY;

		if (dwKey & KEY_LBUTTON)
		{
			list<CGameObject*>::iterator iter2 = pObjList.begin();
			list<CGameObject*>::iterator iter_end2 = pObjList.end();

			for (; iter2 != iter_end2; ++iter2)
			{
				(*iter2)->SetSelect(false);
			}

			list<CGameObject*> pObjList2 = ObjMgr->GetList()[OBJ_BUILD];

			iter2 = pObjList2.begin();
			iter_end2 = pObjList2.end();

			for (; iter2 != iter_end2; ++iter2)
			{
				(*iter2)->SetSelect(false);
			}

			if (!lstrcmp((*iter)->GetInfo().szObjKey, L"Interceptor") && !D_CAST(CInterceptor, (*iter))->InterceptorRender())
				continue;

			(*iter)->SetSelect(true);

			CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
			D_CAST(CInterpace, pInterpace)->NotPortaraits();
			D_CAST(CInterpace, pInterpace)->SetUnitInfo((*iter)->GetUnitInfo());
			D_CAST(CInterpace, pInterpace)->OneObjectClick(true);
			D_CAST(CInterpace, pInterpace)->ClearInterpaceUnitList();
			D_CAST(CInterpace, pInterpace)->SetInterpaceUnitList(*iter);



			if ((*iter)->GetTeamType() == TEAM_PLAYER)
			if (!lstrcmp(pInterpace->GetInfo().szObjKey, L"Arbiter"))
			{
				int iNum = rand() % 4;
				if (iNum == 0)
				{
					SoundMgr->PlaySound(L"pabWht00.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 1)
				{
					SoundMgr->PlaySound(L"pabWht01.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 2)
				{
					SoundMgr->PlaySound(L"pabWht02.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 3)
				{
					SoundMgr->PlaySound(L"pabWht03.wav", CHANNEL_MAIN, 0.9f);
				}
			}
			else if (!lstrcmp(pInterpace->GetInfo().szObjKey, L"Archon"))
			{
				int iNum = rand() % 4;
				if (iNum == 0)
				{
					SoundMgr->PlaySound(L"parWht00.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 1)
				{
					SoundMgr->PlaySound(L"parWht01.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 2)
				{
					SoundMgr->PlaySound(L"parWht02.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 3)
				{
					SoundMgr->PlaySound(L"parWht03.wav", CHANNEL_MAIN, 0.9f);
				}
			}
			else if (!lstrcmp(pInterpace->GetInfo().szObjKey, L"Carrier"))
			{
				int iNum = rand() % 4;
				if (iNum == 0)
				{
					SoundMgr->PlaySound(L"PCaWht00.WAV", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 1)
				{
					SoundMgr->PlaySound(L"PCaWht01.WAV", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 2)
				{
					SoundMgr->PlaySound(L"PCaWht02.WAV", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 3)
				{
					SoundMgr->PlaySound(L"PCaWht03.WAV", CHANNEL_MAIN, 0.9f);
				}
			}
			else if (!lstrcmp(pInterpace->GetInfo().szObjKey, L"Corsair"))
			{
				int iNum = rand() % 4;
				if (iNum == 0)
				{
					SoundMgr->PlaySound(L"pcowht00.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 1)
				{
					SoundMgr->PlaySound(L"pcowht01.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 2)
				{
					SoundMgr->PlaySound(L"pcowht02.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 3)
				{
					SoundMgr->PlaySound(L"pcowht03.wav", CHANNEL_MAIN, 0.9f);
				}
			}
			else if (!lstrcmp(pInterpace->GetInfo().szObjKey, L"DarkArchon"))
			{
				int iNum = rand() % 4;
				if (iNum == 0)
				{
					SoundMgr->PlaySound(L"pdawht00.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 1)
				{
					SoundMgr->PlaySound(L"pdawht01.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 2)
				{
					SoundMgr->PlaySound(L"pdawht02.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 3)
				{
					SoundMgr->PlaySound(L"pdawht03.wav", CHANNEL_MAIN, 0.9f);
				}
			}
			else if (!lstrcmp(pInterpace->GetInfo().szObjKey, L"DarkTemplar"))
			{
				int iNum = rand() % 4;
				if (iNum == 0)
				{
					SoundMgr->PlaySound(L"PDTWht00.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 1)
				{
					SoundMgr->PlaySound(L"PDTWht01.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 2)
				{
					SoundMgr->PlaySound(L"PDTWht02.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 3)
				{
					SoundMgr->PlaySound(L"PDTWht03.wav", CHANNEL_MAIN, 0.9f);
				}
			}
			else if (!lstrcmp(pInterpace->GetInfo().szObjKey, L"Dragoon"))
			{
				int iNum = rand() % 7;
				if (iNum == 0)
				{
					SoundMgr->PlaySound(L"PDrWht00.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 1)
				{
					SoundMgr->PlaySound(L"PDrWht01.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 2)
				{
					SoundMgr->PlaySound(L"PDrWht02.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 3)
				{
					SoundMgr->PlaySound(L"PDrWht03.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 4)
				{
					SoundMgr->PlaySound(L"PDrWht04.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 5)
				{
					SoundMgr->PlaySound(L"PDrWht05.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 6)
				{
					SoundMgr->PlaySound(L"PDrWht06.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 7)
				{
					SoundMgr->PlaySound(L"PDrWht07.wav", CHANNEL_MAIN, 0.9f);
				}
			}
			else if (!lstrcmp(pInterpace->GetInfo().szObjKey, L"HighTemplar"))
			{
				int iNum = rand() % 4;
				if (iNum == 0)
				{
					SoundMgr->PlaySound(L"pteWht00.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 1)
				{
					SoundMgr->PlaySound(L"pteWht01.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 2)
				{
					SoundMgr->PlaySound(L"pteWht02.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 3)
				{
					SoundMgr->PlaySound(L"pteWht03.wav", CHANNEL_MAIN, 0.9f);
				}
			}
			else if (!lstrcmp(pInterpace->GetInfo().szObjKey, L"Observer"))
			{
			/*	int iNum = rand() % 2;
				if (iNum == 0)
				{*/
					SoundMgr->PlaySound(L"pwiwht00.wav", CHANNEL_MAIN, 0.9f);
			/*	}
				else if (iNum == 1)
				{
					SoundMgr->PlaySound(L"pwiwht01.wav", CHANNEL_MAIN, 0.9f);
				}*/
			}
			else if (!lstrcmp(pInterpace->GetInfo().szObjKey, L"Probe"))
			{
				int iNum = rand() % 4;
				if (iNum == 0)
				{
					SoundMgr->PlaySound(L"pprwht00.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 1)
				{
					SoundMgr->PlaySound(L"pprwht01.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 2)
				{
					SoundMgr->PlaySound(L"pprwht02.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 3)
				{
					SoundMgr->PlaySound(L"pprwht03.wav", CHANNEL_MAIN, 0.9f);
				}
			}
			else if (!lstrcmp(pInterpace->GetInfo().szObjKey, L"Reaver"))
			{
				int iNum = rand() % 4;
				if (iNum == 0)
				{
					SoundMgr->PlaySound(L"ptrwht00.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 1)
				{
					SoundMgr->PlaySound(L"ptrwht01.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 2)
				{
					SoundMgr->PlaySound(L"ptrwht02.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 3)
				{
					SoundMgr->PlaySound(L"ptrwht03.wav", CHANNEL_MAIN, 0.9f);
				}
			}
			else if (!lstrcmp(pInterpace->GetInfo().szObjKey, L"Scout"))
			{
				int iNum = rand() % 4;
				if (iNum == 0)
				{
					SoundMgr->PlaySound(L"PScWht00.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 1)
				{
					SoundMgr->PlaySound(L"PScWht01.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 2)
				{
					SoundMgr->PlaySound(L"PScWht02.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 3)
				{
					SoundMgr->PlaySound(L"PScWht03.wav", CHANNEL_MAIN, 0.9f);
				}
			}
			else if (!lstrcmp(pInterpace->GetInfo().szObjKey, L"Shuttle"))
			{
				int iNum = rand() % 4;
				if (iNum == 0)
				{
					SoundMgr->PlaySound(L"pshwht00.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 1)
				{
					SoundMgr->PlaySound(L"pshwht01.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 2)
				{
					SoundMgr->PlaySound(L"pshwht02.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 3)
				{
					SoundMgr->PlaySound(L"pshwht03.wav", CHANNEL_MAIN, 0.9f);
				}
			}
			else if (!lstrcmp(pInterpace->GetInfo().szObjKey, L"Zealot"))
			{
				int iNum = rand() % 4;
				if (iNum == 0)
				{
					SoundMgr->PlaySound(L"PZeWht00.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 1)
				{
					SoundMgr->PlaySound(L"PZeWht01.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 2)
				{
					SoundMgr->PlaySound(L"PZeWht02.wav", CHANNEL_MAIN, 0.9f);
				}
				else if (iNum == 3)
				{
					SoundMgr->PlaySound(L"PZeWht03.wav", CHANNEL_MAIN, 0.9f);
				}
			}
			return;
		}
	}


	pObjList = ObjMgr->GetList()[OBJ_BUILD];
	iter = pObjList.begin();
	iter_end = pObjList.end();

	for (; iter != iter_end; ++iter)
	{
		if (m_ptMousePos.x >= 490 && m_ptMousePos.y >= 340 &&
			m_ptMousePos.x <= 640 && m_ptMousePos.y <= 480)
			return;

		if (m_ptMousePos.x >= 150 && m_ptMousePos.y >= 380 &&
			m_ptMousePos.x <= 490 && m_ptMousePos.y <= 480)
			return;

		if (m_ptMousePos.x >= 0 && m_ptMousePos.y >= 320 &&
			m_ptMousePos.x <= 150 && m_ptMousePos.y <= 480)
			return;

		float fL = (*iter)->GetInfo().vPos.x - ((*iter)->GetInfo().vSize.x / 2.f);
		float fU = (*iter)->GetInfo().vPos.y - ((*iter)->GetInfo().vSize.y / 2.f);
		float fR = (*iter)->GetInfo().vPos.x + ((*iter)->GetInfo().vSize.x / 2.f);
		float fD = (*iter)->GetInfo().vPos.y + ((*iter)->GetInfo().vSize.y / 2.f);

		if (m_tInfo.vPos.x < fL ||
			m_tInfo.vPos.x > fR ||
			m_tInfo.vPos.y < fU ||
			m_tInfo.vPos.y > fD)
			continue;

		if ((*iter)->GetTeamType() == TEAM_PLAYER)
			m_dwState = MOUSE_PLAYER;
		else if ((*iter)->GetTeamType() == TEAM_ENEMY)
			m_dwState = MOUSE_ENEMY;

		if (dwKey & KEY_LBUTTON)
		{
			list<CGameObject*>::iterator iter2 = pObjList.begin();
			list<CGameObject*>::iterator iter_end2 = pObjList.end();

			for (; iter2 != iter_end2; ++iter2)
			{
				(*iter2)->SetSelect(false);
			}

			list<CGameObject*> pObjList2 = ObjMgr->GetList()[OBJ_UNIT];

			iter2 = pObjList2.begin();
			iter_end2 = pObjList2.end();

			for (; iter2 != iter_end2; ++iter2)
			{
				(*iter2)->SetSelect(false);
				(*iter2)->SetBuildType(BUILD_END);
			}

			(*iter)->SetSelect(true);

			CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();

			D_CAST(CInterpace, pInterpace)->NotPortaraits();
			D_CAST(CInterpace, pInterpace)->SetBuildInfo((*iter)->GetBuildInfo());
			D_CAST(CInterpace, pInterpace)->OneObjectClick(true);
			D_CAST(CInterpace, pInterpace)->ClearInterpaceUnitList();
			D_CAST(CInterpace, pInterpace)->SetInterpaceUnitList(*iter);

			if((*iter)->GetTeamType() == TEAM_PLAYER)
			if (!lstrcmp((*iter)->GetBuildInfo().szName, L"Nexus"))
			{
				SoundMgr->PlaySound(L"pnewht00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (!lstrcmp((*iter)->GetBuildInfo().szName, L"Pylon"))
			{
				SoundMgr->PlaySound(L"ppywht00.wav", CHANNEL_MAIN, 0.9f);
			}
			else  if (!lstrcmp((*iter)->GetBuildInfo().szName, L"Assimilator"))
			{
				SoundMgr->PlaySound(L"paswht00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (!lstrcmp((*iter)->GetBuildInfo().szName, L"GateWay"))
			{
				SoundMgr->PlaySound(L"pgawht00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (!lstrcmp((*iter)->GetBuildInfo().szName, L"Forge"))
			{
				SoundMgr->PlaySound(L"pfowht00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (!lstrcmp((*iter)->GetBuildInfo().szName, L"PhotonCannon"))
			{
				SoundMgr->PlaySound(L"ppbwht00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (!lstrcmp((*iter)->GetBuildInfo().szName, L"CyberneticsCore"))
			{
				SoundMgr->PlaySound(L"pgcwht00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (!lstrcmp((*iter)->GetBuildInfo().szName, L"ShieldBattery"))
			{
				SoundMgr->PlaySound(L"pbawht00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (!lstrcmp((*iter)->GetBuildInfo().szName, L"RoboticsFacility"))
			{
				SoundMgr->PlaySound(L"prowht00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (!lstrcmp((*iter)->GetBuildInfo().szName, L"CitadelofAdun"))
			{
				SoundMgr->PlaySound(L"pciwht00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (!lstrcmp((*iter)->GetBuildInfo().szName, L"RoboticsSupportBay"))
			{
				SoundMgr->PlaySound(L"pnawht00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (!lstrcmp((*iter)->GetBuildInfo().szName, L"FleetBeacon"))
			{
				SoundMgr->PlaySound(L"pwawht00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (!lstrcmp((*iter)->GetBuildInfo().szName, L"TemplarArchives"))
			{
				SoundMgr->PlaySound(L"pacwht00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (!lstrcmp((*iter)->GetBuildInfo().szName, L"Observatory"))
			{
				SoundMgr->PlaySound(L"pbewht00.wav", CHANNEL_MAIN, 0.9f);
			}
			else if (!lstrcmp((*iter)->GetBuildInfo().szName, L"ArbiterTribunal"))
			{
				SoundMgr->PlaySound(L"ptrwht00.wav", CHANNEL_MAIN, 0.9f);
			}
			return;
		}

	}
}


D3DXVECTOR3 CMouse::GetMousePos(void)
{
	POINT ptMouse;

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	m_ptMousePos = D3DXVECTOR3(S_FLOAT_CAST(ptMouse.x)
		, S_FLOAT_CAST(ptMouse.y)
		, 0.f);

	m_tInfo.vPos = D3DXVECTOR3(m_ptMousePos.x + m_vScroll.x
		, m_ptMousePos.y + m_vScroll.y
		, 0.f);

	return m_tInfo.vPos;
}

void CMouse::Frame(void)
{
	if (m_dwState != m_dwSubState)
	{
		switch (m_dwState)
		{
		case MOUSE_NOMAL:
			lstrcpy(m_tInfo.szStateKey, L"Normal");
			m_tFrame = FRAME(0.f, 5.f, 4.f);
			break;
		case MOUSE_CLICK:
			lstrcpy(m_tInfo.szStateKey, L"Click");
			m_tFrame = FRAME(0.f, 30.f, 4.f);
			break;
		case MOUSE_R:
			lstrcpy(m_tInfo.szStateKey, L"Direction");
			m_tFrame = FRAME(0.f, 1.f, 1.f);
			break;
		case MOUSE_RU:
			lstrcpy(m_tInfo.szStateKey, L"Direction");
			m_tFrame = FRAME(2.f, 1.f, 3.f);
			break;
		case MOUSE_U:
			lstrcpy(m_tInfo.szStateKey, L"Direction");
			m_tFrame = FRAME(4.f, 1.f, 5.f);
			break;
		case MOUSE_LU:
			lstrcpy(m_tInfo.szStateKey, L"Direction");
			m_tFrame = FRAME(6.f, 1.f, 7.f);
			break;
		case MOUSE_L:
			lstrcpy(m_tInfo.szStateKey, L"Direction");
			m_tFrame = FRAME(8.f, 1.f, 9.f);
			break;
		case MOUSE_LD:
			lstrcpy(m_tInfo.szStateKey, L"Direction");
			m_tFrame = FRAME(10.f, 1.f, 11.f);
			break;
		case MOUSE_D:
			lstrcpy(m_tInfo.szStateKey, L"Direction");
			m_tFrame = FRAME(12.f, 1.f, 13.f);
			break;
		case MOUSE_RD:
			lstrcpy(m_tInfo.szStateKey, L"Direction");
			m_tFrame = FRAME(14.f, 1.f, 15.f);
			break;
		case MOUSE_DRAG:
			lstrcpy(m_tInfo.szStateKey, L"Drag");
			m_tFrame = FRAME(0.f, 5.f, 0.f);
			break;
		case MOUSE_ENEMY:
			lstrcpy(m_tInfo.szStateKey, L"Enemy");
			m_tFrame = FRAME(0.f, 7.f, 13.f);
			break;
		case MOUSE_OBJECT:
			lstrcpy(m_tInfo.szStateKey, L"Object");
			m_tFrame = FRAME(0.f, 7.f, 13.f);
			break;
		case MOUSE_PLAYER:
			lstrcpy(m_tInfo.szStateKey, L"Player");
			m_tFrame = FRAME(0.f, 5.f, 13.f);
			break;
		case MOUSE_TARGET_ALLY:
			lstrcpy(m_tInfo.szStateKey, L"TargetAlly");
			m_tFrame = FRAME(0.f, 5.f, 1.f);
			break;
		case MOUSE_TARGET_ENEMY:
			lstrcpy(m_tInfo.szStateKey, L"TargetEnemy");
			m_tFrame = FRAME(0.f, 5.f, 1.f);
			break;
		case MOUSE_TARGET_NEUTRAL:
		case MOUSE_ATTACK_POINT_PATOL:
		case MOUSE_TARGET_RALLY:
		case MOUSE_ATTACK_POINT_NOMAL:
		case MOUSE_SKILL_POINT:
		case MOUSE_SKILL_POINT2:
		case MOUSE_SKILL_POINT3:
			lstrcpy(m_tInfo.szStateKey, L"TargetNeutral");
			m_tFrame = FRAME(0.f, 5.f, 1.f);
			break;
		}
		m_iFrameFirst = S_INT_CAST(m_tFrame.fFrame);
		m_dwSubState = m_dwState;
	}
	m_tFrame.fFrame += GET_TIME * m_tFrame.fCount;

	if (m_dwState == MOUSE_TARGET_NEUTRAL ||
		!lstrcmp(m_tInfo.szStateKey, L"TargetEnemy") ||
		!lstrcmp(m_tInfo.szStateKey, L"TargetAlly") ||
		!lstrcmp(m_tInfo.szStateKey, L"Direction") ||
		!lstrcmp(m_tInfo.szStateKey, L"Enemy") ||
		!lstrcmp(m_tInfo.szStateKey, L"Object") ||
		!lstrcmp(m_tInfo.szStateKey, L"Player"))
	{
		m_dwState = MOUSE_NOMAL;
	}

	if (m_tFrame.fFrame > m_tFrame.fMax)
	{
		if (!lstrcmp(m_tInfo.szStateKey, L"Click"))
			m_dwState = MOUSE_NOMAL;

		m_tFrame.fFrame = S_FLOAT_CAST(m_iFrameFirst);
	}
}





