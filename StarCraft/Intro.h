#pragma once
#include "Scene.h"
class CIntro :
	public CScene
{
private:
	HWND m_hVideo;
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
public:
	CIntro();
	virtual ~CIntro();
};

