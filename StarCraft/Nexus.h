#pragma once
#include "GameObject.h"
class CNexus :
	public CGameObject
{
private:

	bool		 m_bKeyInput;
	bool		 m_bCreateUnit;
	FRAME		 m_tClick;
public:
	void Function(void);
	void SettingTile(void);
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void    CheckOneClick(void);
	virtual void	CreateUnit(bool Check);
public:
	CNexus();
	virtual ~CNexus();
};

