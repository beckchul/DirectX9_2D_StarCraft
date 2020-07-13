#pragma once

class CGameObject;


class CCollisionMgr
{
public:
	static void CollisionObjectRectEX(CGameObject* pDstObj, list<CGameObject*> pSrcList);
	static void CollisionObjectRectAirEX(list<CGameObject*> pDstList, list<CGameObject*> pSrcList);
	static void CollisionCreateBuildRectEX(CGameObject* pDstObj, CGameObject* pSrcObj);
private:
	static bool CheckCollRect(CGameObject* pDest, CGameObject* pSour, float* pMoveX, float* pMoveY);
	static bool CheckCollRect2(CGameObject * pDest, CGameObject * pSour, float * pMoveX, float * pMoveY);
public:
	CCollisionMgr();
	~CCollisionMgr();
};

