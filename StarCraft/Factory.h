#pragma once

class CGameObject;
#include "AStar.h"
#include "Carrier.h"

template<typename T>
class CFactory abstract
{
public:
	static CGameObject* CreateGameObject(void)
	{
		CGameObject* pObject = new T;
		pObject->Initialize();

		return pObject;
	}

	static CGameObject* CreateGameObject(eTEAM_TYPE eType)
	{
		CGameObject* pObject = new T;
		pObject->SetTeamType(eType);
		pObject->Initialize();

		return pObject;
	}

	static CGameObject* CreateGameObject(D3DXVECTOR3 vPos)
	{
		CGameObject* pObject = new T;
		pObject->SetPos(vPos);
		pObject->Initialize();

		return pObject;
	}

	static CGameObject* CreateGameEffect(D3DXVECTOR3 vPos, eEFFECT_TYPE _eEffect)
	{
		CGameObject* pObject = new T;
		D_CAST(T, pObject)->SetEffectType(_eEffect);
		pObject->SetPos(vPos);
		pObject->Initialize();

		return pObject;
	}

	static CGameObject* CreateGameEffect(D3DXVECTOR3 vPos,float _fAngle , eEFFECT_TYPE _eEffect)
	{
		CGameObject* pObject = new T;
		D_CAST(T, pObject)->SetEffectType(_eEffect);
		pObject->SetAngle(_fAngle);
		pObject->SetPos(vPos);
		pObject->Initialize();

		return pObject;
	}

	static CGameObject* CreateGameUnitEffectTarget(CGameObject* pTarget, eEFFECT_TYPE _eEffect, eTEAM_TYPE eType)
	{
		CGameObject* pObject = new T;
		D_CAST(T, pObject)->SetEffectType(_eEffect);
		D_CAST(T, pObject)->SetTarget(pTarget);
		pObject->SetTeamType(eType);
		pObject->Initialize();

		return pObject;
	}

	static CGameObject* CreateGameUnitEffectTarget(CGameObject* pTarget, D3DXVECTOR3 _vPos, eEFFECT_TYPE _eEffect, eTEAM_TYPE eType)
	{
		CGameObject* pObject = new T;
		D_CAST(T, pObject)->SetEffectType(_eEffect);
		D_CAST(T, pObject)->SetTarget(pTarget);
		pObject->SetTeamType(eType);
		pObject->SetPos(_vPos);
		pObject->Initialize();

		return pObject;
	}

	static CGameObject* CreateGameUnitScarab(CGameObject* pTarget, D3DXVECTOR3 _vPos, eTEAM_TYPE eType)
	{
		CGameObject* pObject = new T;
		pObject->SetAttTarget(pTarget);
		pObject->SetTeamType(eType);
		pObject->SetPos(_vPos);
		pObject->Initialize();

		return pObject;
	}

	static CGameObject* CreateGameUnitEffectTarget(D3DXVECTOR3 vPos, eEFFECT_TYPE _eEffect, CGameObject* pTarget, UNIT _tUnitInfo)
	{
		CGameObject* pObject = new T;
		D_CAST(T, pObject)->SetEffectType(_eEffect);
		D_CAST(T, pObject)->SetTarget(pTarget);
		pObject->SetEffectUnitInfo(_tUnitInfo);
		pObject->SetPos(vPos);
		pObject->Initialize();

		return pObject;
	}

	static CGameObject* CreateGameUnitSkillEffect(D3DXVECTOR3 vPos, eEFFECT_TYPE _eEffect)
	{
		CGameObject* pObject = new T;
		D_CAST(T, pObject)->SetEffectType(_eEffect);
		pObject->SetPos(vPos);
		pObject->Initialize();

		return pObject;
	}

	static CGameObject* CreateGameUnitInterceptor(CGameObject* _pCarrier, eTEAM_TYPE _eTeamType)
	{
		CGameObject* pObject = new T;
		pObject->SetTeamType(_eTeamType);
		D_CAST(T, pObject)->SetCarrier(_pCarrier);
		pObject->Initialize();
		D_CAST(CCarrier, _pCarrier)->InterceptorPush(pObject);
		return pObject;
	}

	static CGameObject* CreateGameUnitEffectTarget(D3DXVECTOR3 vPos, eEFFECT_TYPE _eEffect, CGameObject* pTarget, UNIT _tUnitInfo, eTEAM_TYPE _eType)
	{
		CGameObject* pObject = new T;
		D_CAST(T, pObject)->SetEffectType(_eEffect);
		D_CAST(T, pObject)->SetTarget(pTarget);
		pObject->SetEffectUnitInfo(_tUnitInfo);
		pObject->SetPos(vPos);
		pObject->SetTeamType(_eType);
		pObject->Initialize();

		return pObject;
	}

	static CGameObject* CreateGameObject(D3DXVECTOR3 vPos, eTEAM_TYPE eType)
	{
		CGameObject* pObject = new T;
		pObject->SetTeamType(eType);
		pObject->SetPos(vPos);
		pObject->Initialize();
		
		return pObject;
	}

	static CGameObject* CreateGameObjectRallyGround(D3DXVECTOR3 vPos, D3DXVECTOR3 vPos2, eTEAM_TYPE eType)
	{
		CGameObject* pObject = new T;
		pObject->SetTeamType(eType);
		pObject->SetPos(vPos);
		pObject->Initialize();
		D_CAST(T, pObject)->GetAStar()->StartAStarPos(vPos, vPos2);
		return pObject;
	}

	static CGameObject* CreateGameObjectRallyAir(D3DXVECTOR3 vPos, D3DXVECTOR3 vPos2, DWORD _dwState, eTEAM_TYPE eType)
	{
		CGameObject* pObject = new T;
		pObject->SetTeamType(eType);
		pObject->SetPos(vPos);
		pObject->Initialize();
		pObject->SetEndPoint(vPos2);
		pObject->SetState(_dwState);
		return pObject;
	}

	static CGameObject* CreateGameObjectRallyGround(D3DXVECTOR3 vPos, D3DXVECTOR3 vPos2, DWORD _dwState, eTEAM_TYPE eType)
	{
		CGameObject* pObject = new T;
		pObject->SetTeamType(eType);
		pObject->SetPos(vPos);
		pObject->Initialize();
		pObject->SetState(_dwState);
		D_CAST(T, pObject)->GetAStar()->StartAStarPos(vPos, vPos2);
		return pObject;
	}

	static CGameObject* CreateGameObjectBuild(D3DXVECTOR3 vPos, eTEAM_TYPE eType)
	{
		CGameObject* pObject = new T;
		pObject->SetTeamType(eType);
		pObject->SetPos(vPos);
		pObject->Initialize();

		pObject->SetSelect(true);
		CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
		D_CAST(CInterpace, pInterpace)->NotPortaraits();
		D_CAST(CInterpace, pInterpace)->OneObjectClick(true);
		D_CAST(CInterpace, pInterpace)->ClearInterpaceUnitList();
		D_CAST(CInterpace, pInterpace)->SetInterpaceUnitList(pObject);
		return pObject;
	}

	static CGameObject* CreateGameBuildObject(D3DXVECTOR3 vPos, BUILD _tBuildInfo, eTEAM_TYPE eType)
	{
		CGameObject* pObject = new T;
		pObject->SetBuildInfo(_tBuildInfo);
		pObject->SetTeamType(eType);
		pObject->SetPos(vPos);
		pObject->Initialize();

		return pObject;
	}

	static CGameObject* CreateGameResource(D3DXVECTOR3 vPos, D3DXVECTOR3 vSize, eRESOURCE_TYPE _eType)
	{
		CGameObject* pObject = new T;
		pObject->SetPos(vPos);
		pObject->SetSize(vSize);
		D_CAST(T, pObject)->SetResourceType(_eType);
		pObject->Initialize();
		return pObject;
	}

	static CGameObject* CreateDigUpResource(CGameObject* pObj, eRESOURCE_TYPE _eType)
	{
		CGameObject* pObject = new T;
		D_CAST(T, pObject)->SetObject(pObj);
		D_CAST(T, pObject)->SetType(_eType);
		return pObject;
	}
};
