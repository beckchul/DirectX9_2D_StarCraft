#pragma once
#include "GameObject.h"

class CAStar;

class CProbe :
	public CGameObject
{
private:
	CGameObject* m_pTarget;
	bool		 m_bAngle;
	bool		 m_bGas;
	bool		 m_bGas2;
	int			 m_iGetResource;
	float		 m_fGetResourceCount;
	bool		 m_bGetResource;
	bool		 m_bMouseLClick;
	bool		 m_bCreateBuild;
	FRAME		 m_tMouseLClick;
	CGameObject* m_pTargetNexus;
	CAStar*		 m_pAStar;
	TCHAR		m_szBuildKey[MIN_STR];
	D3DXVECTOR3	m_vBuildPos;
public:
	bool GetCreateBuild(void)
	{
		return m_bCreateBuild;
	}
	int GetProbeGetResource(void)
	{
		return m_iGetResource;
	}
	CAStar* GetAStar(void)
	{
		return m_pAStar;
	}
public:
	void SetTarget(CGameObject* _pTarget)
	{
		m_pTarget = _pTarget;
	}
	void SetTargetNexus(CGameObject* _pNexus)
	{
		m_pTargetNexus = _pNexus;
	}
	void SetProbeGetRsource(int _iResource)
	{
		m_iGetResource = _iResource;
	}
public:
	void DigUpResource(void);
public:
	void AI(void);
	void Frame(void);
	void AStarMove(void);
	void SelectKeyCheck(void);
	void TechnologyTree(void);
	bool CheckBuild(int iCount);
	void CreateBuild(void);
	void Cloocking(void);

public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void CheckOneClick(void);
	virtual void	CreateUnit(bool Check) {}
public:
	CProbe();
	virtual ~CProbe();
};

