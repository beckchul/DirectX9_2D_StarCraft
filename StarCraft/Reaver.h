#pragma once
#include "GameObject.h"

class CAStar;

class CReaver :
	public CGameObject
{
private:
	bool		 m_bClickKey;
	FRAME		 m_tClick;
	float		 m_fFrameTime;
	int			 m_iFrameCount;
	CAStar*		 m_pAStar;
	int			 m_iScarabCount;
	int			 m_iScarabCountMax;
	bool		 m_bCreateUnit;
	FRAME		 m_tAttDelay;
public:
	CAStar* GetAStar(void)
	{
		return m_pAStar;
	}
	void Function(void);
	void Cloocking(void);
	void Frame(void);
	void AStarMove(void);
	void SelectKeyCheck(void);
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void CheckOneClick(void);
	virtual void	CreateUnit(bool Check);
public:
	CReaver();
	virtual ~CReaver();
};

