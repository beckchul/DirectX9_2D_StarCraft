#include "stdafx.h"
#include "Stage.h"
#include "TextureMgr.h"
#include "ObjMgr.h"
#include "Factory.h"
#include "BackGround.h"
#include "Mouse.h"
#include "Interpace.h"
#include "Probe.h"
#include "MapResource.h"
#include "Nexus.h"
#include "AStar.h"
#include "CollisionMgr.h"
#include "Zealot.h"
#include "Dragoon.h"
#include "HighTemplar.h"
#include "DarkTemplar.h"
#include "Archon.h"
#include "DarkArchon.h"
#include "Shuttle.h"
#include "Reaver.h"
#include "Observer.h"
#include "Scout.h"
#include "Corsair.h"
#include "Carrier.h"
#include "Arbiter.h"
#include "Fog.h"
#include "SoundMgr.h"

CStage::CStage()
{
}

CStage::~CStage()
{
	Release();
}

HRESULT CStage::Initialize(void)
{
	ObjMgr->AddGameObject(CFactory<CFog>::CreateGameObject(), OBJ_FOG);
	ObjMgr->AddGameObject(CFactory<CBackGround>::CreateGameObject(), OBJ_BACKGROUND);
	ObjMgr->AddGameObject(CFactory<CMouse>::CreateGameObject(TEAM_PLAYER), OBJ_MOUSE);
	ObjMgr->AddGameObject(CFactory<CInterpace>::CreateGameObject(TEAM_PLAYER), OBJ_INTERPACE);
	ObjMgr->AddGameObject(CFactory<CProbe>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 300.f, MAXSCROLLY + 150.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CProbe>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 330.f, MAXSCROLLY + 150.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CProbe>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 360.f, MAXSCROLLY + 150.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CProbe>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 390.f, MAXSCROLLY + 150.f, 0.f), TEAM_PLAYER), OBJ_UNIT);

	/*ObjMgr->AddGameObject(CFactory<CZealot>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 200.f, MAXSCROLLY + 100.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CZealot>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 200.f, MAXSCROLLY + 140.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CZealot>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 200.f, MAXSCROLLY + 160.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CZealot>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 200.f, MAXSCROLLY + 180.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CDragoon>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 100.f, MAXSCROLLY + 50.f, 0.f), TEAM_PLAYER), OBJ_UNIT);*/
	//ObjMgr->AddGameObject(CFactory<CHighTemplar>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 100.f, MAXSCROLLY + 100.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	//ObjMgr->AddGameObject(CFactory<CHighTemplar>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 100.f, MAXSCROLLY + 130.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	//ObjMgr->AddGameObject(CFactory<CDarkTemplar>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 100.f, MAXSCROLLY + 160.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	//ObjMgr->AddGameObject(CFactory<CDarkTemplar>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 100.f, MAXSCROLLY + 190.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	//
	//ObjMgr->AddGameObject(CFactory<CDarkArchon>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 100.f, MAXSCROLLY + 250.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	//ObjMgr->AddGameObject(CFactory<CShuttle>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 150.f, MAXSCROLLY + 0.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	//ObjMgr->AddGameObject(CFactory<CReaver>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 150.f, MAXSCROLLY + 50.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	//ObjMgr->AddGameObject(CFactory<CObserver>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 150.f, MAXSCROLLY + 100.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	//ObjMgr->AddGameObject(CFactory<CScout>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 150.f, MAXSCROLLY + 150.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	/*ObjMgr->AddGameObject(CFactory<CCorsair>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 190.f, MAXSCROLLY + 150.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CCarrier>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 170.f, MAXSCROLLY + 0.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CCarrier>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 200.f, MAXSCROLLY + 0.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CCarrier>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 230.f, MAXSCROLLY + 0.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CCarrier>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 260.f, MAXSCROLLY + 0.f, 0.f), TEAM_PLAYER), OBJ_UNIT);*/
	//ObjMgr->AddGameObject(CFactory<CArbiter>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX + 300.f, MAXSCROLLY + 0.f, 0.f), TEAM_PLAYER), OBJ_UNIT);
	
	
	/*ObjMgr->AddGameObject(CFactory<CZealot>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 500.f, MAXSCROLLY - 100.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CZealot>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 500.f, MAXSCROLLY - 140.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CZealot>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 500.f, MAXSCROLLY - 180.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CZealot>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 500.f, MAXSCROLLY - 220.f, 0.f), TEAM_ENEMY), OBJ_UNIT);

	ObjMgr->AddGameObject(CFactory<CZealot>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 550.f, MAXSCROLLY - 100.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CZealot>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 550.f, MAXSCROLLY - 140.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CZealot>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 550.f, MAXSCROLLY - 180.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CZealot>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 550.f, MAXSCROLLY - 220.f, 0.f), TEAM_ENEMY), OBJ_UNIT);


	ObjMgr->AddGameObject(CFactory<CDragoon>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 500.f, MAXSCROLLY - 260.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CDragoon>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 500.f, MAXSCROLLY - 300.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CDragoon>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 500.f, MAXSCROLLY - 340.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CDragoon>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 460.f, MAXSCROLLY - 100.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CHighTemplar>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 460.f, MAXSCROLLY - 140.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CDarkTemplar>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 460.f, MAXSCROLLY - 180.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CArchon>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 460.f, MAXSCROLLY - 220.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CDarkArchon>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 460.f, MAXSCROLLY - 260.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CShuttle>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 460.f, MAXSCROLLY - 300.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CReaver>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 460.f, MAXSCROLLY - 340.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CObserver>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 1000.f, MAXSCROLLY - 300.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CScout>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 600.f, MAXSCROLLY -300.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CCorsair>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 700.f, MAXSCROLLY - 300.f, 0.f), TEAM_ENEMY), OBJ_UNIT);

	ObjMgr->AddGameObject(CFactory<CCarrier>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 800.f, MAXSCROLLY - 300.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CArbiter>::CreateGameObject(D3DXVECTOR3(MAXSCROLLX - 900.f, MAXSCROLLY - 300.f, 0.f), TEAM_ENEMY), OBJ_UNIT);*/


	SoundMgr->PlayBGM(L"protoss.wav", CHANNEL_BGM, 0.35f);

	ObjMgr->AddGameObject(CFactory<CNexus>::CreateGameObject(D3DXVECTOR3(3809.f, 3793.f, 0.f), TEAM_PLAYER), OBJ_BUILD);
	ObjMgr->AddGameObject(CFactory<CNexus>::CreateGameObject(D3DXVECTOR3(3808.f, 240.f, 0.f), TEAM_ENEMY), OBJ_BUILD);

	ObjMgr->AddGameObject(CFactory<CProbe>::CreateGameObject(D3DXVECTOR3(3898.f, 200.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CProbe>::CreateGameObject(D3DXVECTOR3(3898.f, 240.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CProbe>::CreateGameObject(D3DXVECTOR3(3898.f, 280.f, 0.f), TEAM_ENEMY), OBJ_UNIT);
	ObjMgr->AddGameObject(CFactory<CProbe>::CreateGameObject(D3DXVECTOR3(3898.f, 350.f, 0.f), TEAM_ENEMY), OBJ_UNIT);


	list<RESOURCE*> ResourceList = ObjMgr->GetReourceList();

	list<RESOURCE*>::iterator iter = ResourceList.begin();
	list<RESOURCE*>::iterator iter_end = ResourceList.end();

	for (; iter != iter_end; ++iter)
	{
		ObjMgr->AddGameObject(CFactory<CMapResource>::CreateGameResource(
			(*iter)->vPos, (*iter)->vSize, (*iter)->eType), OBJ_RESOURCE);
	}

	return S_OK;
}

int CStage::Update(void)
{
	list<CGameObject*> pGroundUnitList = ObjMgr->GetList()[OBJ_UNIT];
	list<CGameObject*> pGroundUnitList2 = ObjMgr->GetList()[OBJ_UNIT];
	
	for (list<CGameObject*>::iterator iter = pGroundUnitList.begin(); iter != pGroundUnitList.end(); ++iter)
	{
		CCollisionMgr::CollisionObjectRectEX((*iter), pGroundUnitList);
	}
	

	//list<CGameObject*> pArrUnitList = ObjMgr->GetList()[OBJ_UNIT];
	//CCollisionMgr::CollisionObjectRectAirEX(pArrUnitList, pArrUnitList);

	
	


	ObjMgr->Update();

	CGameObject* pMouse = ObjMgr->GetList()[OBJ_MOUSE].front();
	if (pMouse->GetSubState() == MOUSE_ATTACK_POINT_NOMAL || pMouse->GetSubState() == MOUSE_ATTACK_POINT_PATOL
		|| pMouse->GetSubState() == MOUSE_SKILL_POINT)
		pMouse->SetSubState(MOUSE_NOMAL);

	return 0;
}

void CStage::Render(void)
{
	ObjMgr->Render();
}

void CStage::Release(void)
{
	ObjMgr->DestroyInstance();
	TextureMgr->DestroyInstance();
}