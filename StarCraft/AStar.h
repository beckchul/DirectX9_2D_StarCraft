#pragma once

#include "Include.h"

class CGameObject;

class CAStar
{

private:
	list<int>		  m_BestList;		 //내가 가야할 경로.

private:
	int		m_iStartIndex;
	int		m_iGoalIndex;

public:
	list<int>* GetBestList(void)
	{
		return &m_BestList;
	}
	void AStarClear(void)
	{
		m_BestList.clear();
	}
	void StopCheck(CGameObject *pObject, CGameObject* pObject2);
public:
	void StartAStar(const int& iStartIndex, const int& iGoalIndex);
	void StartAStarPos(const D3DXVECTOR3& vStartPos, const D3DXVECTOR3& vGoalPos);
	void  MakeRoute(void);
public:
	int GetTileIndex(const D3DXVECTOR3& vPos);

public:
	void Release(void);

public:
	CAStar(void);
	~CAStar(void);
};
