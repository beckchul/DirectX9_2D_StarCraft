#pragma once
#include "GameObject.h"

class CAStar;

class CDarkArchon :
	public CGameObject
{
private:
	bool		 m_bClickKey;
	FRAME		 m_tClick;
	FRAME		 m_tBodyFrame;
	FRAME		 m_tCreateBodyFrame;
	FRAME		 m_tAuraFrame;
	FRAME		 m_tMpFrame;
	float		 m_fFrameTime;
	int			 m_iFrameCount;
	CAStar*		 m_pAStar;
	bool		 m_bCreate;
public:
	CAStar* GetAStar(void)
	{
		return m_pAStar;
	}
	void Function(void);
	void Clooking(void);
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
	CDarkArchon();
	virtual ~CDarkArchon();
};

