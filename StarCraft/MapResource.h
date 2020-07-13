#pragma once
#include "GameObject.h"
class CMapResource :
	public CGameObject
{
private:
	float		 m_fFrameTime;
	int			 m_iFrameCount;
	RESOURCE m_tResource;
	int m_iDigUp;
public:
	int GetDigUp(void)
	{
		return m_iDigUp;
	}
	void SetDigUp(int iA)
	{
		m_iDigUp = iA;
	}
	void SetResourceType(eRESOURCE_TYPE _eType)
	{
		m_tResource.eType = _eType;
	}
	eRESOURCE_TYPE GetResourceType(void)
	{
		return m_tResource.eType;
	}
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void    CheckOneClick(void) {}
	virtual void	CreateUnit(bool Check) {}
public:
	CMapResource();
	virtual ~CMapResource();
};

