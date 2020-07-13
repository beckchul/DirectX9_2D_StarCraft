#pragma once
#include "Scene.h"
class CLogo :
	public CScene
{
private:
	HANDLE				m_hThread;
	CRITICAL_SECTION	m_Crt;

private:
	static unsigned int __stdcall ImageLoadThreadFunction(void* pArg);
	CRITICAL_SECTION GetCrt(void)
	{
		return m_Crt;
	}
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
public:
	CLogo();
	virtual ~CLogo();
};

