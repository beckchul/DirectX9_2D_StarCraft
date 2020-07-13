#pragma once
#include "GameObject.h"

class CAStar;

class CHighTemplar :
	public CGameObject
{
private:
	bool		 m_bClickKey;
	FRAME		 m_tClick;
	float		 m_fFrameTime;
	int			 m_iFrameCount;
	CAStar*		 m_pAStar;
	float		 m_fMoveCount;


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
	CAStar* GetAStar(void)
	{
		return m_pAStar;
	}
	void Function(void);
	void Frame(void);
	void Clooking(void);
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
	CHighTemplar();
	virtual ~CHighTemplar();
};

