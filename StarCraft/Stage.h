#pragma once
#include "Scene.h"
class CStage :
	public CScene
{
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);

public:
	CStage();
	virtual ~CStage();
};

