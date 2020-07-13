#pragma once
#include "GameObject.h"

class CAStar;

class CDarkTemplar :
	public CGameObject
{
private:
	bool		 m_bClickKey;
	FRAME		 m_tClick;
	FRAME		 m_tAttDelay;
	float		 m_fFrameTime;
	int			 m_iFrameCount;
	CAStar*		 m_pAStar;

	CGameObject* m_CreateTemplar;
	int			 m_iCreateNumber;
public:
	CGameObject* GetCreateTemplar(void)
	{
		return m_CreateTemplar;
	}
	void SetCreateTemplar(CGameObject* _pObjet)
	{
		m_CreateTemplar = _pObjet;
	}
	void SetCreateNumber(int _iNumber)
	{
		m_iCreateNumber = _iNumber;
	}
	int GetCreateNumber(void)
	{
		return m_iCreateNumber;
	}

public:
	CAStar* GetAStar(void)
	{
		return m_pAStar;
	}
	void Frame(void);
	void Function(void);
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
	CDarkTemplar();
	virtual ~CDarkTemplar();
};

