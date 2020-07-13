#pragma once
#include "GameObject.h"

class CAStar;

class CScarab :
	public CGameObject
{
private:
	CAStar* m_pAStar;
	void Frame(void);
	void AStarMove(void);
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void CheckOneClick(void) {}
	virtual void	CreateUnit(bool Check) {}
public:
	CScarab();
	virtual ~CScarab();
};

