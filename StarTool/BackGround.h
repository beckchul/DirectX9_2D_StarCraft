#pragma once

#include "Include.h"
class CStarToolView;

class CBackGround
{
private:
	vector<PTILE>	m_vecTile;
	CStarToolView*	m_pView;
public:
	void TilePicking(D3DXVECTOR3 _vPos, int _iOption);
	void MiniView(void);
	void TileRelease(void)
	{
		Release();
	}
public:
	void SetView(CStarToolView* _pView)
	{
		m_pView = _pView;
	}
	void SetTile(vector<PTILE> _vecTile)
	{
		m_vecTile = _vecTile;
	}
	vector<PTILE> GetTile(void)
	{
		return m_vecTile;
	}

public:
	HRESULT Initialize(void);
	int		Update(void);
	void	Render(void);
	void	Release(void);
public:
	CBackGround(void);
	~CBackGround(void);
};
