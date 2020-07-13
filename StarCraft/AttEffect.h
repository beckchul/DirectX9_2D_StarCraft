#pragma once
#include "GameObject.h"

class CAttEffect :
	public CGameObject
{
private:
	CGameObject* m_pTarget;
	bool		 m_bGround;
	eEFFECT_TYPE m_eEffectType;
	float		 m_fFrameTime;
	int			 m_iFrameCount;
public:
	void SetEffectType(eEFFECT_TYPE _eType)
	{
		m_eEffectType = _eType;
	}
	void SetTarget(CGameObject* _pTarget)
	{
		m_pTarget = _pTarget;
	}
public:
	void Angle(void);
	void Frame(void);
	void Function(void);
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void    CheckOneClick(void) {}
	virtual void	CreateUnit(bool Check) {}
public:
	CAttEffect();
	virtual ~CAttEffect();
};

