#pragma once
#include "GameObject.h"
class CCreateBuild :
	public CGameObject
{
private:
	bool	m_bCreateEnd;
	int     m_iSelectCount;
	void SettingBuildInfo(void);
	void SettingTileOption(void);
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void    CheckOneClick(void);
	virtual void	CreateUnit(bool Check) {}
public:
	CCreateBuild();
	virtual ~CCreateBuild();
};

