#pragma once
#include "GameObject.h"
class CMouse :
	public CGameObject
{
private:
	TCHAR		m_szBuildKey[MIN_STR];
	D3DXVECTOR3 m_ptMousePos;
	D3DXVECTOR3	m_vDragPos;
	bool		m_bOnDragClick;
	bool		m_bBuildClick;
	bool		m_bBuildSave;
	D3DXVECTOR3 m_vBuildPos;
	CGameObject* m_pRallyPointObject;
public:
	void Drag(void);
	void TileCheck(void);
	void ObjectCheck(void);
	void Frame(void);
	void CreateBuildCheck(void);
	void CreateBuild(void);
	void RallyPoint(void);

public:
	static bool AttCompare(CGameObject* _pObj1,CGameObject* _pObj2)
	{
		return _pObj1->GetUnitInfo().iAtt > _pObj2->GetUnitInfo().iAtt;
	}
public:
	D3DXVECTOR3 GetMousePos(void);
	D3DXVECTOR3 GetRenderMousePos(void)
	{
		return m_ptMousePos;
	}
	D3DXVECTOR3 GetBuildPos(void)
	{
		return m_vBuildPos;
	}
	bool GetBuildSave(void)
	{
		return m_bBuildSave;
	}
	bool GetBuildClick(void)
	{
		return m_bBuildClick;
	}
public:
	void SetBuildClick(bool _bClick)
	{
		m_bBuildClick = _bClick;
	}
	void SettingMouseRallyPoint(CGameObject* pObject)//렐리포인트 대상 변수
	{
		m_pRallyPointObject = pObject;
		m_dwState = MOUSE_TARGET_RALLY;
	}
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void    CheckOneClick(void){}
	virtual void	CreateUnit(bool Check) {}
public:
	CMouse();
	virtual ~CMouse();
};

