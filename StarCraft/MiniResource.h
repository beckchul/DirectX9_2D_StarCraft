#pragma once
#include "GameObject.h"
class CMiniResource :
	public CGameObject
{
private:
	eRESOURCE_TYPE eType;
	CGameObject* m_pTarget;
public:
	void Frame(void);
	void SetObject(CGameObject* pObj)
	{
		m_pTarget = pObj;
	}
	void SetType(eRESOURCE_TYPE _eType)
	{
		eType = _eType;
	}
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void    CheckOneClick(void) {}
	virtual void	CreateUnit(bool Check) {}
public:
	CMiniResource();
	virtual ~CMiniResource();
};

