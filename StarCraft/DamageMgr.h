#pragma once

class CGameObject;
#include "Enum.h"

class CDamageMgr
{
public:
	static void DamageCalculation(CGameObject* pObject1, CGameObject* pObject2);
	static void DamageCalculation(int _iAtt, CGameObject* pObject2);
	static void DamageCalculationBuild(CGameObject* pObject1, CGameObject* pObject2);
	static void DamageCalculationBuild(int _iAtt, CGameObject* pObject2);
	static void DamageCalculationSplash(CGameObject* pObject, CGameObject* pTarget, list<CGameObject*> _pObjectlist);
	static void DamageCalculationSplash(CGameObject* pObject, list<CGameObject*> _pObjectlist);
	static void DamageCalculationSplashAll(D3DXVECTOR3 _vPos, D3DXVECTOR3 _vSize, list<CGameObject*> _pObjectlist);
	static void DamageCalculationSplashEnemy(D3DXVECTOR3 _vPos, int iAtt, D3DXVECTOR3 _vSize, list<CGameObject*> _pObjectlist, eTEAM_TYPE _eType);
private:
	static void Sound(CGameObject* pObject);
public:
	CDamageMgr();
	~CDamageMgr();
};

