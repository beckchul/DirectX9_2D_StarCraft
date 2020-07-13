#pragma once
#include "GameObject.h"
class CAssimilator :
	public CGameObject
{
private:
	RESOURCE m_tResource;
public:
	eRESOURCE_TYPE GetResourceType(void)
	{
		return m_tResource.eType;
	}

	void SettingTile(void);
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void    CheckOneClick(void);
	virtual void	CreateUnit(bool Check);
public:
	CAssimilator();
	virtual ~CAssimilator();
};

