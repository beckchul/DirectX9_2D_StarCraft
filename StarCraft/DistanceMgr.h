#pragma once

class CGameObject;

class CDistanceMgr
{
public:
	static void GetLowDistanceObject(CGameObject* pObject, list<CGameObject*> pList);
	static bool GetLowDistanceNexus(CGameObject* pObject, list<CGameObject*> pList);
	static void GetLowDistanceMineral(CGameObject* pObject, list<CGameObject*> pList);
	static void GetLowDistanceGas(CGameObject* pObject, list<CGameObject*> pList);
	static void GetLowDistanceAttTargetGround(CGameObject* pObject, list<CGameObject*> pList);
	static void GetLowDistanceAttTargetBuild(CGameObject* pObject, list<CGameObject*> pList);
	static void GetLowDistanceAttTargetAir(CGameObject* pObject, list<CGameObject*> pList);
	static void CheckCloking(CGameObject * pObject, list<CGameObject*> pList, list<CGameObject*> pList2);
	static void CheckNoamlCloking(CGameObject * pObject, list<CGameObject*> pList);
public:
	CDistanceMgr();
	~CDistanceMgr();
};

