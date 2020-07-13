#pragma once
#include "GameObject.h"
class CInterceptor :
	public CGameObject
{
private:
	FRAME		 m_tAttDelay;
	float		 m_fFrameTime;
	int			 m_iFrameCount;
	CGameObject* m_Carrier;
	bool		 m_bCommand;
	bool		 m_bRender;
	bool		 m_bPattonClear;
	bool		 m_bPattonAttack;
public:
	bool InterceptorRender(void)
	{
		return m_bRender;
	}
	bool GetCommand(void)
	{
		return m_bCommand;
	}
	void SetCommand(bool _bCommand)
	{
		m_bCommand = _bCommand;
	}
	void SetCarrier(CGameObject* _Carrier)
	{
		m_Carrier = _Carrier;
	}
	void Function(void);
	void Cloocking(void);
	void Frame(void);
	void Move(void);
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void CheckOneClick(void) {}
	virtual void	CreateUnit(bool Check) {}
public:
	CInterceptor();
	virtual ~CInterceptor();
};

