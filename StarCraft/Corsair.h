#pragma once
#include "GameObject.h"
class CCorsair :
	public CGameObject
{
private:
	bool		 m_bClickKey;
	FRAME		 m_tClick;
	float		 m_fFrameTime;
	int			 m_iFrameCount;
	FRAME		 m_tAttDelay;
public:
	void Function(void);
	void Cloocking(void);
	void Frame(void);
	void Move(void);
	void SelectKeyCheck(void);
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void CheckOneClick(void);
	virtual void	CreateUnit(bool Check) {}
public:
	CCorsair();
	virtual ~CCorsair();
};

