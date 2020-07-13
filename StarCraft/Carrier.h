#pragma once
#include "GameObject.h"
class CCarrier :
	public CGameObject
{
private:
	bool		 m_bClickKey;
	FRAME		 m_tClick;
	float		 m_fFrameTime;
	int			 m_iFrameCount;
	bool		 m_bCreateUnit;
	list<CGameObject*> m_InterceptorList;
	size_t		 m_iIntercepterSize;
public:
	void InterceptorPush(CGameObject* _pInterceptor)
	{
		m_InterceptorList.push_back(_pInterceptor);
	}
	void InterceptorPop(CGameObject* _pInterceptor)
	{
		for (list<CGameObject*>::iterator iter = m_InterceptorList.begin();
			iter != m_InterceptorList.end(); ++iter)
		{
			if ((*iter) == _pInterceptor)
			{
				m_InterceptorList.erase(iter);
				return;
			}
		}
	}
public:
	void Function(void);
	void Cloocking(void);
	void Frame(void);
	void Move(void);
	void SelectKeyCheck(void);
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void CheckOneClick(void);
	virtual void	CreateUnit(bool Check);
public:
	CCarrier();
	virtual ~CCarrier();
};

