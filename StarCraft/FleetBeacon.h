#pragma once
#include "GameObject.h"
class CFleetBeacon :
	public CGameObject
{
private:
	bool		 m_bKeyInput;
	bool		 m_bCreateUpgrade;
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
	CFleetBeacon();
	virtual ~CFleetBeacon();
};

