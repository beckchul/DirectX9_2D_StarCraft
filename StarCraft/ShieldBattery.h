#pragma once
#include "GameObject.h"
class CShieldBattery :
	public CGameObject
{
private:
private:
	bool		 m_bKeyInput;
	FRAME		 m_tClick;
public:
	void SettingTile(void);
	void Function(void);
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void    CheckOneClick(void);
	virtual void	CreateUnit(bool Check);
public:
	CShieldBattery();
	virtual ~CShieldBattery();
};

