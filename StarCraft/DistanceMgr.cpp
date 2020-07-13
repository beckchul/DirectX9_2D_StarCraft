#include "stdafx.h"
#include "DistanceMgr.h"
#include "GameObject.h"
#include "Probe.h"
#include "MapResource.h"

void CDistanceMgr::GetLowDistanceObject(CGameObject * pObject, list<CGameObject*> pList)
{

}

bool CDistanceMgr::GetLowDistanceNexus(CGameObject * pObject, list<CGameObject*> pList)
{
	bool bCheck = false;
	float fX = 0.f, fY = 0.f, fMinDist = -1.f, fDist = 0.f;
	
	list<CGameObject*>::iterator iter = pList.begin();
	list<CGameObject*>::iterator iter_end = pList.end();

	for (; iter != iter_end; ++iter)
	{
		if ((*iter)->GetBuildType() == BUILD_NEXUS &&
			(*iter)->GetTeamType() == pObject->GetTeamType())
		{
			bCheck = true;
			fX = (*iter)->GetPos().x - pObject->GetPos().x;
			fY = (*iter)->GetPos().y - pObject->GetPos().y;

			fDist = sqrtf(powf(fX, 2) + powf(fY, 2));

			if (fMinDist == -1.f)
			{
				fMinDist = fDist;
				D_CAST(CProbe, pObject)->SetTargetNexus((*iter));
			}
			if (fMinDist > fDist)
			{
				D_CAST(CProbe, pObject)->SetTargetNexus((*iter));
				fMinDist = fDist;
			}

			D_CAST(CProbe, pObject)->SetProbeGetRsource(3);
		}
	}

	if (bCheck)
		return true;
	else
		return false;
}

void CDistanceMgr::GetLowDistanceMineral(CGameObject * pObject, list<CGameObject*> pList)
{
	float fX = 0.f, fY = 0.f, fMinDist = -1.f, fDist = 0.f;

	list<CGameObject*>::iterator iter = pList.begin();
	list<CGameObject*>::iterator iter_end = pList.end();

	for (; iter != iter_end; ++iter)
	{
		if (D_CAST(CMapResource, (*iter))->GetResourceType() == RESOURCE_GAS)
			continue;

		if ((D_CAST(CMapResource, (*iter))->GetDigUp() == 1))
			continue;
		fX = (*iter)->GetPos().x - pObject->GetPos().x;
		fY = (*iter)->GetPos().y - pObject->GetPos().y;

		fDist = sqrtf(powf(fX, 2) + powf(fY, 2));

		if (fDist > 1000)
			continue;

		if (fMinDist == -1.f)
		{
			fMinDist = fDist;
			D_CAST(CProbe, pObject)->SetTarget((*iter));
		}
		if (fMinDist > fDist)
		{
			D_CAST(CProbe, pObject)->SetTarget((*iter));
			fMinDist = fDist;
		}
	}
}

void CDistanceMgr::GetLowDistanceGas(CGameObject * pObject, list<CGameObject*> pList)
{
	float fX = 0.f, fY = 0.f, fMinDist = -1.f, fDist = 0.f;

	list<CGameObject*>::iterator iter = pList.begin();
	list<CGameObject*>::iterator iter_end = pList.end();

	for (; iter != iter_end; ++iter)
	{
		if ((*iter)->GetBuildType() == BUILD_ASSIMILATOR)
		{
			fX = (*iter)->GetPos().x - pObject->GetPos().x;
			fY = (*iter)->GetPos().y - pObject->GetPos().y;

			fDist = sqrtf(powf(fX, 2) + powf(fY, 2));

			if (fDist > 1000)
				continue;

			if(fMinDist == -1.f)
			{
				fMinDist = fDist;
				D_CAST(CProbe, pObject)->SetTarget((*iter));
			}
			if (fMinDist > fDist)
			{
				D_CAST(CProbe, pObject)->SetTarget((*iter));
				fMinDist = fDist;
			}
		}
	}
}

void CDistanceMgr::GetLowDistanceAttTargetGround(CGameObject * pObject, list<CGameObject*> pList)
{
	if (!lstrcmp(pObject->GetUnitInfo().szName, L"HighTemplar")
		|| !lstrcmp(pObject->GetUnitInfo().szName, L"DarkArchon")
		|| !lstrcmp(pObject->GetUnitInfo().szName, L"Probe")
		|| pObject->GetState() == UNIT_DEAD)
		return;

	if (!lstrcmp(pObject->GetUnitInfo().szName, L"Scarab"))
		return;

	float fX = 0.f, fY = 0.f, fMinDist = -1.f, fDist = 0.f;

	list<CGameObject*>::iterator iter = pList.begin();
	list<CGameObject*>::iterator iter_end = pList.end();

	for (; iter != iter_end; ++iter)		// 건물 체크 필요
	{
		if ((*iter)->GetTeamType() == pObject->GetTeamType() ||
			(*iter)->GetState() == UNIT_DEAD ||
			(((*iter)->GetAlphType() == ALPH_NOMAL_CLOOCKING ||
			(*iter)->GetAlphType() == ALPH_CREATE_CLOOCKING) &&
				(*iter)->GetUnitState() != UNIT_MAELSTORM))
			continue;

		if ((*iter)->GetUnitState() == UNIT_STASISFIELD)
			continue;

		if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Scarab"))
			continue;

		if (!lstrcmp(pObject->GetUnitInfo().szName, L"Zealot") 
			|| !lstrcmp(pObject->GetUnitInfo().szName, L"DarkTemplar")
			|| !lstrcmp(pObject->GetUnitInfo().szName, L"Reaver"))
		{
			if ((*iter)->GetUnitInfo().eType == UNIT_ARR)
				continue;
		}

		if (!lstrcmp(pObject->GetUnitInfo().szName, L"Corsair"))
		{
			if ((*iter)->GetUnitInfo().eType == UNIT_GROUND)
				continue;
		}

		fX = (*iter)->GetPos().x - pObject->GetPos().x;
		fY = (*iter)->GetPos().y - pObject->GetPos().y;

		fDist = sqrtf(powf(fX, 2) + powf(fY, 2));

		if (!lstrcmp(pObject->GetUnitInfo().szName, L"Zealot")
			|| !lstrcmp(pObject->GetUnitInfo().szName, L"DarkTemplar")
			|| !lstrcmp(pObject->GetUnitInfo().szName, L"Archon")
			|| !lstrcmp(pObject->GetUnitInfo().szName, L"Scout")
			|| !lstrcmp(pObject->GetUnitInfo().szName, L"Corsair")
			|| !lstrcmp(pObject->GetUnitInfo().szName, L"Arbiter"))
		{
			if (fDist > 210)
				continue;
		}
		
		if (!lstrcmp(pObject->GetUnitInfo().szName, L"Dragoon") ||
			!lstrcmp(pObject->GetBuildInfo().szName, L"PhotonCannon"))
		{
			if (fDist > 250)
				continue;
		}

		if (!lstrcmp(pObject->GetUnitInfo().szName, L"Reaver"))
		{
			if (fDist > 270)
				continue;
		}

		if (!lstrcmp(pObject->GetUnitInfo().szName, L"Carrier"))
		{
			if (fDist > 300)
				continue;
		}

		if (fMinDist == -1.f)
		{
			fMinDist = fDist;
			pObject->SetAttTarget(*iter);
			pObject->SetUnitAtt(true);
		}
		if (fMinDist > fDist)
		{
			pObject->SetAttTarget(*iter);
			fMinDist = fDist;
		}
	}
}

void CDistanceMgr::GetLowDistanceAttTargetBuild(CGameObject * pObject, list<CGameObject*> pList)
{
	if (!lstrcmp(pObject->GetUnitInfo().szName, L"HighTemplar")
		|| !lstrcmp(pObject->GetUnitInfo().szName, L"DarkArchon")
		|| !lstrcmp(pObject->GetUnitInfo().szName, L"Probe")
		|| pObject->GetState() == UNIT_DEAD)
		return;

	pObject->SetUnitAtt(false);

	float fX = 0.f, fY = 0.f, fMinDist = -1.f, fDist = 0.f;

	list<CGameObject*>::iterator iter = pList.begin();
	list<CGameObject*>::iterator iter_end = pList.end();

	for (; iter != iter_end; ++iter)		// 건물 체크 필요
	{
		if ((*iter)->GetTeamType() == pObject->GetTeamType() ||
			(*iter)->GetState() == UNIT_DEAD)
			continue;
		fX = (*iter)->GetPos().x - pObject->GetPos().x;
		fY = (*iter)->GetPos().y - pObject->GetPos().y;

		fDist = sqrtf(powf(fX, 2) + powf(fY, 2));

		if (!lstrcmp(pObject->GetUnitInfo().szName, L"Zealot")
			|| !lstrcmp(pObject->GetUnitInfo().szName, L"DarkTemplar")
			|| !lstrcmp(pObject->GetUnitInfo().szName, L"Archon")
			|| !lstrcmp(pObject->GetUnitInfo().szName, L"Scout")
			|| !lstrcmp(pObject->GetUnitInfo().szName, L"Arbiter"))
		{
			if (fDist > 210)
				continue;
		}

		if (!lstrcmp(pObject->GetUnitInfo().szName, L"Dragoon") ||
			!lstrcmp(pObject->GetBuildInfo().szName, L"PhotonCannon"))
		{
			if (fDist > 250)
				continue;
		}

		if (!lstrcmp(pObject->GetUnitInfo().szName, L"Reaver"))
		{
			if (fDist > 270)
				continue;
		}

		if (!lstrcmp(pObject->GetUnitInfo().szName, L"Carrier"))
		{
			if (fDist > 300)
				continue;
		}

		if (fMinDist == -1.f)
		{
			fMinDist = fDist;
			pObject->SetAttTarget(*iter);
			pObject->SetUnitAtt(true);
		}
		if (fMinDist > fDist)
		{
			pObject->SetAttTarget(*iter);
			fMinDist = fDist;
		}
	}
}

void CDistanceMgr::GetLowDistanceAttTargetAir(CGameObject * pObject, list<CGameObject*> pList)
{
}

void CDistanceMgr::CheckNoamlCloking(CGameObject * pObject, list<CGameObject*> pList)
{
	float fX = 0.f, fY = 0.f, fDist = 0.f;

	list<CGameObject*>::iterator iter = pList.begin();
	list<CGameObject*>::iterator iter_end = pList.end();

	for (; iter != iter_end; ++iter)
	{
		if ((*iter)->GetState() == UNIT_DEAD)
			continue;

		if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Arbiter"))
		{
			if (pObject->GetTeamType() == (*iter)->GetTeamType())
			{
				fX = (*iter)->GetPos().x - pObject->GetPos().x;
				fY = (*iter)->GetPos().y - pObject->GetPos().y;

				fDist = sqrtf(powf(fX, 2) + powf(fY, 2));

				if (fDist < 220)
				{
					if (pObject->GetAlphType() == ALPH_NOMAL)
						pObject->SetAlphType(ALPH_NOMAL_CLOOCKING);
					return;
				}
			}
		}
	}

	if (pObject->GetAlphType() == ALPH_NOMAL_CLOOCKING || pObject->GetAlphType() == LOOK_ALPH_NOMAL_CLOOCKING)
		pObject->SetAlphType(ALPH_NOMAL);
}

void CDistanceMgr::CheckCloking(CGameObject * pObject, list<CGameObject*> pList, list<CGameObject*> pList2)
{
	float fX = 0.f, fY = 0.f, fDist = 0.f;

	list<CGameObject*>::iterator iter = pList.begin();
	list<CGameObject*>::iterator iter_end = pList.end();

	for (; iter != iter_end; ++iter)
	{
		if ((*iter)->GetState() == UNIT_DEAD)
			continue;

		if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Observer"))
		{
			if (pObject->GetTeamType() != (*iter)->GetTeamType())
			{
				fX = (*iter)->GetPos().x - pObject->GetPos().x;
				fY = (*iter)->GetPos().y - pObject->GetPos().y;

				fDist = sqrtf(powf(fX, 2) + powf(fY, 2));

				if (fDist < 220)
				{
					if (pObject->GetAlphType() == ALPH_CREATE_CLOOCKING)
						pObject->SetAlphType(LOOK_ALPH_CREATE_CLOOCKING);

					if (pObject->GetAlphType() == ALPH_NOMAL_CLOOCKING)
						pObject->SetAlphType(LOOK_ALPH_NOMAL_CLOOCKING);

					return;
				}
			}
		}
	}


	iter = pList2.begin();
	iter_end = pList2.end();

	for (; iter != iter_end; ++iter)
	{
		if ((*iter)->GetState() == UNIT_DEAD)
			continue;

		if (!lstrcmp((*iter)->GetBuildInfo().szName, L"PhotonCannon"))
		{
			if (pObject->GetTeamType() != (*iter)->GetTeamType())
			{
				fX = (*iter)->GetPos().x - pObject->GetPos().x;
				fY = (*iter)->GetPos().y - pObject->GetPos().y;

				fDist = sqrtf(powf(fX, 2) + powf(fY, 2));

				if (fDist < 270)
				{
					if (pObject->GetAlphType() == ALPH_CREATE_CLOOCKING)
						pObject->SetAlphType(LOOK_ALPH_CREATE_CLOOCKING);

					if (pObject->GetAlphType() == ALPH_NOMAL_CLOOCKING)
						pObject->SetAlphType(LOOK_ALPH_NOMAL_CLOOCKING);

					return;
				}
			}
		}
	}

	if (pObject->GetAlphType() == LOOK_ALPH_CREATE_CLOOCKING)
		pObject->SetAlphType(ALPH_CREATE_CLOOCKING);

	if (pObject->GetAlphType() == LOOK_ALPH_NOMAL_CLOOCKING)
		pObject->SetAlphType(ALPH_NOMAL_CLOOCKING);
}



CDistanceMgr::CDistanceMgr()
{
}


CDistanceMgr::~CDistanceMgr()
{
}
