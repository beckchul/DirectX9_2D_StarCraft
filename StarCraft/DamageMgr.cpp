#include "stdafx.h"
#include "DamageMgr.h"
#include "GameObject.h"

CDamageMgr::CDamageMgr()
{
}


CDamageMgr::~CDamageMgr()
{
}

void CDamageMgr::DamageCalculation(CGameObject * pObject1, CGameObject * pObject2)
{
	if (pObject2 == NULL || pObject2->GetDeadCheck() == true)
		return;

	int iOverDamage = pObject2->GetUnitInfo().iShield - pObject1->GetUnitInfo().iAtt;

	pObject2->SetUnitShild(pObject1->GetUnitInfo().iAtt);

	if (iOverDamage < 0)
	{
		pObject2->SetUnitShild(iOverDamage);
		pObject2->SetUnitHp(-iOverDamage);
	}
	else
	{

	}

	if (pObject2->GetUnitInfo().iHp <= 0)
	{		
		pObject2->SetState(UNIT_DEAD);
		Sound(pObject2);
	}
}

void CDamageMgr::DamageCalculation(int _iAtt, CGameObject * pObject2)
{
	if (pObject2 == NULL || pObject2->GetDeadCheck() == true)
		return;

	int iOverDamage = pObject2->GetUnitInfo().iShield - _iAtt;

	pObject2->SetUnitShild(_iAtt);

	if (iOverDamage < 0)
	{
		pObject2->SetUnitShild(iOverDamage);
		pObject2->SetUnitHp(-iOverDamage);
	}
	else
	{

	}	
	if (pObject2->GetUnitInfo().iHp <= 0)
	{
		pObject2->SetState(UNIT_DEAD);
		Sound(pObject2);
	}
}

void CDamageMgr::DamageCalculationBuild(CGameObject * pObject1, CGameObject * pObject2)
{
	if (pObject2 == NULL || pObject2->GetDeadCheck() == true)
		return;

	int iOverDamage = pObject2->GetBuildInfo().iShield - pObject1->GetUnitInfo().iAtt;

	pObject2->SetUnitShild2(pObject1->GetUnitInfo().iAtt);

	if (iOverDamage < 0)
	{
		pObject2->SetUnitShild2(iOverDamage);
		pObject2->SetUnitHp2(-iOverDamage);
	}
	else
	{

	}

	if (pObject2->GetBuildInfo().iHp <= 0)
	{
		pObject2->SetState(UNIT_DEAD);
		Sound(pObject2);
	}
}

void CDamageMgr::DamageCalculationBuild(int _iAtt, CGameObject * pObject2)
{
	if (pObject2 == NULL || pObject2->GetDeadCheck() == true)
		return;

	int iOverDamage = pObject2->GetBuildInfo().iShield - _iAtt;

	pObject2->SetUnitShild2(_iAtt);

	if (iOverDamage < 0)
	{
		pObject2->SetUnitShild2(iOverDamage);
		pObject2->SetUnitHp2(-iOverDamage);
	}
	else
	{

	}

	if (pObject2->GetBuildInfo().iHp <= 0)
	{
		pObject2->SetState(UNIT_DEAD);
		Sound(pObject2);
	}
}

void CDamageMgr::DamageCalculationSplash(CGameObject* pObject, CGameObject * pTarget
									, list<CGameObject*> _pObjectlist)
{

	if (pTarget == NULL || pTarget->GetDeadCheck() == true || pTarget->GetUnitState() == UNIT_STASISFIELD)
		return;

	int iOverDamage = pTarget->GetUnitInfo().iShield - pObject->GetUnitInfo().iAtt;

	pTarget->SetUnitShild(pObject->GetUnitInfo().iAtt);

	if (iOverDamage < 0)
	{
		pTarget->SetUnitShild(iOverDamage);
		pTarget->SetUnitHp(-iOverDamage);
	}
	else
	{

	}

	if (pTarget->GetUnitInfo().iHp <= 0)
	{
		pTarget->SetState(UNIT_DEAD);
		Sound(pTarget);
	}

	list<CGameObject*>::iterator iter = _pObjectlist.begin();
	list<CGameObject*>::iterator iter_end = _pObjectlist.end();

	for (; iter != iter_end; ++iter)
	{
		if (pObject->GetTeamType() == (*iter)->GetTeamType() ||
			(*iter) == NULL || (*iter)->GetDeadCheck() == true ||
			(*iter) == pTarget ||
			(*iter)->GetSubState() == UNIT_STASISFIELD)
			continue;

		if(fabs(pObject->GetPos().x - (*iter)->GetPos().x) < fabs(pObject->GetInfo().vSize.x + (*iter)->GetInfo().vSize.x / 2.f) &&
			fabs(pObject->GetPos().y - (*iter)->GetPos().y) < fabs(pObject->GetInfo().vSize.y + (*iter)->GetInfo().vSize.y / 2.f))
		{
			iOverDamage = (*iter)->GetUnitInfo().iShield - pObject->GetUnitInfo().iAtt / 2;

			(*iter)->SetUnitShild(pObject->GetUnitInfo().iAtt / 2);

			if (iOverDamage < 0)
			{
				(*iter)->SetUnitShild(iOverDamage);
				(*iter)->SetUnitHp(-iOverDamage);
			}
			else
			{

			}

			if ((*iter)->GetUnitInfo().iHp <= 0)
			{
				(*iter)->SetState(UNIT_DEAD);
			}
		}
	}
}

void CDamageMgr::DamageCalculationSplash(CGameObject * pObject, list<CGameObject*> _pObjectlist)
{
	list<CGameObject*>::iterator iter = _pObjectlist.begin();
	list<CGameObject*>::iterator iter_end = _pObjectlist.end();

	for (; iter != iter_end; ++iter)
	{
		if (pObject->GetTeamType() == (*iter)->GetTeamType() ||
			(*iter) == NULL || (*iter)->GetDeadCheck() == true ||
			(*iter)->GetUnitState() == UNIT_STASISFIELD)
			continue;

		if (fabs(pObject->GetPos().x - (*iter)->GetPos().x) < fabs(pObject->GetInfo().vSize.x + (*iter)->GetInfo().vSize.x / 2.f) &&
			fabs(pObject->GetPos().y - (*iter)->GetPos().y) < fabs(pObject->GetInfo().vSize.y + (*iter)->GetInfo().vSize.y / 2.f))
		{
			int iOverDamage = (*iter)->GetUnitInfo().iShield - pObject->GetUnitInfo().iAtt;

			(*iter)->SetUnitShild(pObject->GetUnitInfo().iAtt);

			if (iOverDamage < 0)
			{
				(*iter)->SetUnitShild(iOverDamage);
				(*iter)->SetUnitHp(-iOverDamage);
			}
			else
			{

			}

			if ((*iter)->GetUnitInfo().iHp <= 0)
			{
				(*iter)->SetState(UNIT_DEAD);
				Sound(*iter);
			}
		}
	}
}

void CDamageMgr::DamageCalculationSplashAll(D3DXVECTOR3 _vPos, D3DXVECTOR3 _vSize, list<CGameObject*> _pObjectlist)
{
	list<CGameObject*>::iterator iter = _pObjectlist.begin();
	list<CGameObject*>::iterator iter_end = _pObjectlist.end();

	for (; iter != iter_end; ++iter)
	{
		if ((*iter) == NULL || (*iter)->GetDeadCheck() == true ||
			(*iter)->GetUnitState() == UNIT_STASISFIELD)
			continue;

		if (fabs(_vPos.x - (*iter)->GetPos().x) < fabs(_vSize.x + (*iter)->GetInfo().vSize.x / 2.f) &&
			fabs(_vPos.y - (*iter)->GetPos().y) < fabs(_vSize.y + (*iter)->GetInfo().vSize.y / 2.f))
		{
			int iOverDamage = (*iter)->GetUnitInfo().iShield - 12;

			(*iter)->SetUnitShild(12);

			if (iOverDamage < 0)
			{
				(*iter)->SetUnitShild(iOverDamage);
				(*iter)->SetUnitHp(-iOverDamage);
			}
			else
			{

			}

			if ((*iter)->GetUnitInfo().iHp <= 0)
			{
				(*iter)->SetState(UNIT_DEAD);
				Sound(*iter);
			}
		}
	}
}

void CDamageMgr::DamageCalculationSplashEnemy(D3DXVECTOR3 _vPos, int iAtt, D3DXVECTOR3 _vSize, list<CGameObject*> _pObjectlist, eTEAM_TYPE _eType)
{
	list<CGameObject*>::iterator iter = _pObjectlist.begin();
	list<CGameObject*>::iterator iter_end = _pObjectlist.end();

	for (; iter != iter_end; ++iter)
	{
		if ((*iter) == NULL || (*iter)->GetDeadCheck() == true ||
			(*iter)->GetUnitState() == UNIT_STASISFIELD ||
			(*iter)->GetTeamType() == _eType)
			continue;

		if (fabs(_vPos.x - (*iter)->GetPos().x) < fabs(_vSize.x + (*iter)->GetInfo().vSize.x / 2.f) &&
			fabs(_vPos.y - (*iter)->GetPos().y) < fabs(_vSize.y + (*iter)->GetInfo().vSize.y / 2.f))
		{
			int iOverDamage = (*iter)->GetUnitInfo().iShield - iAtt;

			(*iter)->SetUnitShild(iAtt);

			if (iOverDamage < 0)
			{
				(*iter)->SetUnitShild(iOverDamage);
				(*iter)->SetUnitHp(-iOverDamage);
			}
			else
			{

			}

			if ((*iter)->GetUnitInfo().iHp <= 0)
			{
				(*iter)->SetState(UNIT_DEAD);
				Sound(*iter);
			}
		}
	}
}

void CDamageMgr::Sound(CGameObject * pObject)
{
	if(!lstrcmp(pObject->GetUnitInfo().szName, L"Zealot"))
		SoundMgr->PlaySound(L"pzeDth00.wav", CHANNEL_EFFECT_DEAD, 0.5f, pObject->GetPos());
	else if(!lstrcmp(pObject->GetUnitInfo().szName, L"Dragoon"))
		SoundMgr->PlaySound(L"pdrDth00.wav", CHANNEL_EFFECT_DEAD, 0.5f, pObject->GetPos());
	else if (!lstrcmp(pObject->GetUnitInfo().szName, L"HighTemplar"))
		SoundMgr->PlaySound(L"pteDth00.wav", CHANNEL_EFFECT_DEAD, 0.5f, pObject->GetPos());
	else if (!lstrcmp(pObject->GetUnitInfo().szName, L"DarkTemplar"))
		SoundMgr->PlaySound(L"PDTDth00.wav", CHANNEL_EFFECT_DEAD, 0.5f, pObject->GetPos());
	else if (!lstrcmp(pObject->GetUnitInfo().szName, L"Shuttle"))
		SoundMgr->PlaySound(L"PDTDth00.wav", CHANNEL_EFFECT_DEAD, 0.5f, pObject->GetPos());
	else if (!lstrcmp(pObject->GetUnitInfo().szName, L"Corsair") ||
		!lstrcmp(pObject->GetUnitInfo().szName, L"Scout") || 
		!lstrcmp(pObject->GetUnitInfo().szName, L"Archon") || 
		!lstrcmp(pObject->GetUnitInfo().szName, L"DarkArchon"))
		SoundMgr->PlaySound(L"explosm.wav", CHANNEL_EFFECT_DEAD, 0.5f, pObject->GetPos());
	else if (!lstrcmp(pObject->GetUnitInfo().szName, L"Carrier"))
	{
		if (rand() % 2 == 0)
			SoundMgr->PlaySound(L"PCaDth00.wav", CHANNEL_EFFECT_DEAD, 0.5f, pObject->GetPos());
		else
			SoundMgr->PlaySound(L"PCaDth01.wav", CHANNEL_EFFECT_DEAD, 0.5f, pObject->GetPos());
	}
	
}

