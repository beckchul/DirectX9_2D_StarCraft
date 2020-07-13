#pragma once
#include "GameObject.h"
class CShuttle :
	public CGameObject
{
private:
	bool		 m_bClickKey;
	FRAME		 m_tClick;
	float		 m_fFrameTime;
	int			 m_iFrameCount;
public:
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
	CShuttle();
	virtual ~CShuttle();
};
