#pragma once
#include "GameObject.h"
class CPhotonCannon :
	public CGameObject
{
private:
	bool		 m_bKeyInput;
	FRAME		 m_tClick;
	FRAME		 m_tAttDelay;
	float		 m_fFrameC;
	int			 m_iFramecount;
public:
	void SelectKeyCheck(void);
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
	CPhotonCannon();
	virtual ~CPhotonCannon();
};

