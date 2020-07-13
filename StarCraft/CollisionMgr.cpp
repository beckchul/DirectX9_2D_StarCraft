#include "stdafx.h"
#include "CollisionMgr.h"
#include "GameObject.h"
#include "AStar.h"
#include "Probe.h"
#include "Zealot.h"
#include "Dragoon.h"
#include "HighTemplar.h"
#include "DarkTemplar.h"
#include "Archon.h"
#include "DarkArchon.h"
#include "Reaver.h"



CCollisionMgr::CCollisionMgr()
{
}


CCollisionMgr::~CCollisionMgr()
{
}

void CCollisionMgr::CollisionObjectRectEX(CGameObject* pDstObj, list<CGameObject*> pSrcList)
{
	if (pDstObj->GetUnitInfo().eType != UNIT_GROUND ||
		pDstObj->GetDeadCheck() ||
		!pDstObj->GetCollision() ||
		pDstObj->GetState() == UNIT_STAND ||
		!lstrcmp(pDstObj->GetUnitInfo().szName, L"Scarab"))
		return;

	list<CGameObject*>::iterator srcIter_begin = pSrcList.begin();
	list<CGameObject*>::iterator srcIter_end = pSrcList.end();

	float fMoveX = 0;
	float fMoveY = 0;

	for (; srcIter_begin != srcIter_end; ++srcIter_begin)
	{
		if ((*srcIter_begin)->GetUnitInfo().eType != UNIT_GROUND ||
			true == (*srcIter_begin)->GetDeadCheck() ||
			!(*srcIter_begin)->GetCollision() ||
			(*srcIter_begin) == pDstObj ||
			!lstrcmp((*srcIter_begin)->GetUnitInfo().szName, L"Scarab"))
			continue;
		if (CheckCollRect(pDstObj, (*srcIter_begin), &fMoveX, &fMoveY))
		{
			// 충돌이 되었다면, 충돌된 영역만큼 
			// 충돌한 대상을  fMoveX와 fMoveY만큼 밀어낼 것.
			
				if (fMoveX > fMoveY)
				{
					if (pDstObj->GetPos().y < (*srcIter_begin)->GetPos().y)
						fMoveY *= -1.f;

					pDstObj->SetPos(D3DXVECTOR3(pDstObj->GetPos().x,
						pDstObj->GetPos().y + fMoveY * 0.3f, 0.f));

					(*srcIter_begin)->SetPos(D3DXVECTOR3((*srcIter_begin)->GetPos().x,
						(*srcIter_begin)->GetPos().y - fMoveY * 0.3f, 0.f));
				}
				else
				{
					if (pDstObj->GetPos().x < (*srcIter_begin)->GetPos().x)
						fMoveX *= -1.f;

					pDstObj->SetPos(D3DXVECTOR3(pDstObj->GetPos().x + fMoveX * 0.3f,
						pDstObj->GetPos().y, 0.f));

					(*srcIter_begin)->SetPos(D3DXVECTOR3((*srcIter_begin)->GetPos().x - fMoveX * 0.3f,
						(*srcIter_begin)->GetPos().y, 0.f));
				}

				CAStar* pAStar = NULL;
				if (!lstrcmp(pDstObj->GetUnitInfo().szName, L"Probe"))
				{
					if (D_CAST(CProbe, pDstObj)->GetCreateBuild() == true)
						return;

					pAStar = D_CAST(CProbe, pDstObj)->GetAStar();
				}
				else if (!lstrcmp(pDstObj->GetUnitInfo().szName, L"Zealot"))
				{
					pAStar = D_CAST(CZealot, pDstObj)->GetAStar();
				}
				else if (!lstrcmp(pDstObj->GetUnitInfo().szName, L"Dragoon"))
				{
					pAStar = D_CAST(CDragoon, pDstObj)->GetAStar();
				}
				else if (!lstrcmp(pDstObj->GetUnitInfo().szName, L"HighTemplar"))
				{
					pAStar = D_CAST(CHighTemplar, pDstObj)->GetAStar();
				}
				else if (!lstrcmp(pDstObj->GetUnitInfo().szName, L"DarkTemplar"))
				{
					pAStar = D_CAST(CDarkTemplar, pDstObj)->GetAStar();
				}
				else if (!lstrcmp(pDstObj->GetUnitInfo().szName, L"Archon"))
				{
					pAStar = D_CAST(CArchon, pDstObj)->GetAStar();
				}
				else if (!lstrcmp(pDstObj->GetUnitInfo().szName, L"DarkArchon"))
				{
					pAStar = D_CAST(CDarkArchon, pDstObj)->GetAStar();
				}
				else if (!lstrcmp(pDstObj->GetUnitInfo().szName, L"Reaver"))
				{
					pAStar = D_CAST(CReaver, pDstObj)->GetAStar();
				}
				else
				{
					return;
				}
				pAStar->StopCheck(pDstObj, (*srcIter_begin));
			
		}
	}
}

void CCollisionMgr::CollisionObjectRectAirEX(list<CGameObject*> pDstList, list<CGameObject*> pSrcList)
{
	list<CGameObject*>::iterator dstIter_begin = pDstList.begin();
	list<CGameObject*>::iterator dstIter_end = pDstList.end();

	float fMoveX = 0;
	float fMoveY = 0;

	dstIter_begin = pDstList.begin();
	dstIter_end = pDstList.end();

	for (; dstIter_begin != dstIter_end; ++dstIter_begin)
	{
		if ((*dstIter_begin)->GetUnitInfo().eType != UNIT_ARR ||
			true == (*dstIter_begin)->GetDeadCheck() ||
			!lstrcmp((*dstIter_begin)->GetUnitInfo().szName, L"Interceptor"))
			continue;

		list<CGameObject*>::iterator srcIter_begin = pSrcList.begin();
		list<CGameObject*>::iterator srcIter_end = pSrcList.end();

		for (; srcIter_begin != srcIter_end; ++srcIter_begin)
		{
			if ((*srcIter_begin)->GetUnitInfo().eType != UNIT_ARR ||
				true == (*srcIter_begin)->GetDeadCheck() ||
				(*srcIter_begin) == (*dstIter_begin))
				continue;


			if (CheckCollRect2((*dstIter_begin), (*srcIter_begin), &fMoveX, &fMoveY))
			{
				// 충돌이 되었다면, 충돌된 영역만큼 
				// 충돌한 대상을  fMoveX와 fMoveY만큼 밀어낼 것.
				if (fMoveX > fMoveY)
				{
					if ((*dstIter_begin)->GetPos().y < (*srcIter_begin)->GetPos().y)
						fMoveY *= -1.f;

					(*dstIter_begin)->SetPos(D3DXVECTOR3((*dstIter_begin)->GetPos().x,
						(*dstIter_begin)->GetPos().y + fMoveY * 0.1f, 0.f));

					(*srcIter_begin)->SetPos(D3DXVECTOR3((*srcIter_begin)->GetPos().x,
						(*srcIter_begin)->GetPos().y + fMoveY * 0.1f, 0.f));
				}
				else
				{
					if ((*dstIter_begin)->GetPos().x < (*srcIter_begin)->GetPos().x)
						fMoveX *= -1.f;

					(*dstIter_begin)->SetPos(D3DXVECTOR3((*dstIter_begin)->GetPos().x + fMoveX * 0.1f,
						(*dstIter_begin)->GetPos().y, 0.f));

					(*srcIter_begin)->SetPos(D3DXVECTOR3((*srcIter_begin)->GetPos().x + fMoveX * 0.1f,
						(*srcIter_begin)->GetPos().y, 0.f));
				}
			}
		}
	}
}

void CCollisionMgr::CollisionCreateBuildRectEX(CGameObject* pDstObj, CGameObject* pSrcObj)
{

	float fMoveX = 0;
	float fMoveY = 0;



		if (pDstObj->GetCollision() == false)
			return;

		if (CheckCollRect(pDstObj, pSrcObj, &fMoveX, &fMoveY))
		{
			if (fMoveX > fMoveY)
			{
				if (pDstObj->GetPos().y < pSrcObj->GetPos().y)
					fMoveY *= -1.f;

				pDstObj->SetPos(D3DXVECTOR3(pDstObj->GetPos().x,
					pDstObj->GetPos().y + fMoveY * 0.01f, 0.f));
			}
			else
			{
				if (pDstObj->GetPos().x < pSrcObj->GetPos().x)
					fMoveX *= -1.f;

				pDstObj->SetPos(D3DXVECTOR3(pDstObj->GetPos().x + fMoveX * 0.01f,
					pDstObj->GetPos().y, 0.f));
			}
		}
}

bool CCollisionMgr::CheckCollRect(CGameObject * pDest, CGameObject * pSour, float * pMoveX, float * pMoveY)
{
	float fWidth = (pDest->GetInfo().vSize.x / 2) + (pSour->GetInfo().vSize.x / 2);
	float fHeight = (pDest->GetInfo().vSize.y / 2) + (pSour->GetInfo().vSize.y / 2);

	// 두 사각형의 중점 간의 x, y 거리를 구한다.
	float fDistanceX = fabs(pDest->GetPos().x - pSour->GetPos().x);
	float fDistanceY = fabs(pDest->GetPos().y - pSour->GetPos().y);

	// 두 사각형의 중점 간의 x, y거리가 두 사각형의 가로 세로 절반 합보다 작으면 충돌.
	if (fDistanceX < fWidth && fDistanceY < fHeight)
	{
		*pMoveX = fWidth - fDistanceX;
		*pMoveY = fHeight - fDistanceY;
		return true;
	}

	return false;
}


bool CCollisionMgr::CheckCollRect2(CGameObject * pDest, CGameObject * pSour, float * pMoveX, float * pMoveY)
{
	float fWidth = (pDest->GetInfo().vSize.x / 5) + (pSour->GetInfo().vSize.x / 5);
	float fHeight = (pDest->GetInfo().vSize.y / 5) + (pSour->GetInfo().vSize.y / 5);

	// 두 사각형의 중점 간의 x, y 거리를 구한다.
	float fDistanceX = fabs(pDest->GetPos().x - pSour->GetPos().x);
	float fDistanceY = fabs(pDest->GetPos().y - pSour->GetPos().y);

	// 두 사각형의 중점 간의 x, y거리가 두 사각형의 가로 세로 절반 합보다 작으면 충돌.
	if (fDistanceX < fWidth && fDistanceY < fHeight)
	{
		*pMoveX = fWidth - fDistanceX;
		*pMoveY = fHeight - fDistanceY;
		return true;
	}

	return false;
}
