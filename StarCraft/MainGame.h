#pragma once

#include "Include.h"
class CDevice;


class CMainGame
{
private:
	CDevice*	m_pDevice;
private:
	float		m_fTime;
	int			m_iCount;

public:
	HRESULT Initialize(void);
	int		Update(void);
	void	Render(void);
	void	Release(void);
public:
	CMainGame();
	~CMainGame();
};

