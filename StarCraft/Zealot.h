#pragma once
#include "GameObject.h"

class CAStar;

class CZealot :
	public CGameObject
{
private:
	bool		 m_bClickKey;
	FRAME		 m_tClick;
	FRAME		 m_tAttDelay;
	float		 m_fFrameTime;
	int			 m_iFrameCount;
	CAStar*		 m_pAStar;
public:
	CAStar* GetAStar(void)
	{
		return m_pAStar;
	}
	void Function(void);
	void Frame(void);
	void AStarMove(void);
	void SelectKeyCheck(void);
	void Cloocking(void);
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void CheckOneClick(void);
	virtual void	CreateUnit(bool Check) {}
public:
	CZealot();
	virtual ~CZealot();
};

