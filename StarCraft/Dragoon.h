#pragma once
#include "GameObject.h"

class CAStar;

class CDragoon :
	public CGameObject
{
private:
	bool		 m_bClickKey;
	FRAME		 m_tClick;
	FRAME		 m_tAttDelay;
	float		 m_fRange;
	float		 m_fFrameTime;
	int			 m_iFrameCount;
	CAStar*		 m_pAStar;
public:
	CAStar* GetAStar(void)
	{
		return m_pAStar;
	}
	void Clooking(void);
	void Funtion(void);
	void Frame(void);
	void AStarMove(void);
	void SelectKeyCheck(void);
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void CheckOneClick(void);
	virtual void	CreateUnit(bool Check) {}
public:
	CDragoon();
	virtual ~CDragoon();
};

