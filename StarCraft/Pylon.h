#pragma once
#include "GameObject.h"
class CPylon :
	public CGameObject
{
public:
	void SettingTile(void);
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void    CheckOneClick(void);
	virtual void	CreateUnit(bool Check);
public:
	CPylon();
	virtual ~CPylon();
};

