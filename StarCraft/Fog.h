#pragma once
#include "GameObject.h"
class CFog :
	public CGameObject
{
private:
	vector<PTILE> m_vecTile;
	int m_iAlphCount;
public:
	vector<PTILE> GetTile(void)
	{
		return m_vecTile;
	}
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void    CheckOneClick(void) {}
	virtual void	CreateUnit(bool Check) {}
public:
	CFog();
	virtual ~CFog();
};

