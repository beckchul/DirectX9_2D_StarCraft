#pragma once
#include "GameObject.h"
class CEffect :
	public CGameObject
{
private:
	eEFFECT_TYPE m_eEffectType;
	float		 m_fFrameTime;
	int			 m_iFrameCount;
	int			 m_iAlphCount;
public:
	void SetEffectType(eEFFECT_TYPE _eType)
	{
		m_eEffectType = _eType;
	}
	void SetTarget(CGameObject* _pObject)
	{
		m_AttTarget = _pObject;
	}
public:
	void Shad(void);
	void Angle(void);
	void Frame(void);
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void    CheckOneClick(void) {}
	virtual void	CreateUnit(bool Check) {}
public:
	CEffect();
	virtual ~CEffect();
};

