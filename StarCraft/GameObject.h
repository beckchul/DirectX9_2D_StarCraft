#pragma once

#include "Include.h"
#include "ObjectRenderMgr.h"
#include "DistanceMgr.h"
#include "DamageMgr.h"
#include "SoundMgr.h"

class CGameObject
{
protected:
	eALPH_TYPE m_eAlphType;					// �������� Ÿ��
	D3DXVECTOR3 m_vStartingPoint;			// �ǹ� Rally Point
	D3DXVECTOR3  m_vEndPoint;				// �������� ��������
	list<TCHAR*> m_UnitCreateList;			// �ǹ� ���� ����
	INFO  m_tInfo;							// Object ��ġ ũ�� ���� ����
	UNIT  m_tUnitInfo;						// ������ ����
	BUILD m_tBuildInfo;						// �ǹ��� ����
	int	  m_iFrameFirst;					// �������� �����̱� ���� ����
	int	  m_iArmor;							// ���� ����
	int   m_iShield;						// ���� ����
	bool  m_bCollision;						// �浹 ���� üũ
	bool  m_bSelect;						// ���� ���� üũ
	bool  m_bDeadCheck;						// ��� ���� üũ
	int   m_iClickMyInterpace[9];			// ���� & �ǹ��� ������ �ϴ� UI �� üũ
	float m_fSpeed;							// ������ ���ǵ�
	float m_fAngle;							// ������ Frame�� ���� ����
	DWORD m_dwState;						// ������ Frame���� or �ǹ��� ���� ���� ���� or ���콺 ����
	DWORD m_dwPreState;						// ������ Frame���� ����
	DWORD m_dwSubState;						// ������ ����
	DWORD m_dwUnitState;					// ������ �����̻�
	eRENDER_TYPE m_eRenderType;				// Render(����)�� ���� ����
	eUNIT_TYPE eUnitType;					// ���� ���� ���� ���� ���� ����
	eTEAM_TYPE m_eTeamType;					// �� ������ ���� ����
	eBUILD_TYPE m_eBuildType;				// �ǹ� ���� ���� ����
	eBUILD_CREATE_TYPE m_eBuildCreateType;	// �������� �ǹ����� ���� ���� ����
	FRAME m_tCreateTime;					// �ǹ����� ������ ����µ� ��� �ð� ���� or Ư�������� ���� Ȱ�� ����
	FRAME m_tFrame;							// ������ Frame ����
	FRAME m_tAlphCount;						// ���� ��� ����
	FRAME m_tStateCount;					// �����̻� ��� ����
	int   m_iStateCount;					// �����̻� ��
	float m_fStateCount;					// �����̻� �ʴ��� ���
	int   m_iStateTime;						// �����̻� �ð�
	int   m_iStasis;

	int m_iView;
	bool		  m_bUnitAtt;				// ���� ���� ����
	CGameObject*  m_AttTarget;				// ���� ��� ����
	D3DXVECTOR3	  m_vPatrolStartPoint;		// ������ Patrol ���� ����
	D3DXVECTOR3	  m_vPatrolEndPoint;
	bool		  m_bPatrol;				// Patrol�Դٰ���


	bool m_bAI;
	bool m_bAI2;
protected:
	static D3DXVECTOR3 m_vScroll;
public:
	bool GetAI(void)
	{
		return m_bAI;
	}
	int GetView(void)
	{
		return m_iView;
	}
	static D3DXVECTOR3 GetScrollPos(void)
	{
		return m_vScroll;
	}

	DWORD GetUnitState(void)
	{
		return m_dwUnitState;
	}

	eRENDER_TYPE GetRenderType(void)
	{
		return m_eRenderType;
	}
	list<TCHAR*> GetCreateUnitList(void)
	{
		return m_UnitCreateList;
	}
	FRAME GetCreateTime(void)
	{
		return m_tCreateTime;
	}
	eBUILD_CREATE_TYPE GetBuildCreateType(void)
	{
		return m_eBuildCreateType;
	}
	D3DXVECTOR3 GetPos(void)
	{
		return m_tInfo.vPos;
	}
	INFO GetInfo(void)
	{
		return m_tInfo;
	}
	float GetAngle(void)
	{
		return m_fAngle;
	}
	eTEAM_TYPE GetTeamType(void)
	{
		return m_eTeamType;
	}
	eBUILD_TYPE GetBuildType(void)
	{
		return m_eBuildType;
	}
	bool GetCollision(void)
	{
		return m_bCollision;
	}
	DWORD GetState(void)
	{
		return m_dwState;
	}
	bool GetSelect(void)
	{
		return m_bSelect;
	}
	UNIT GetUnitInfo(void)
	{
		return m_tUnitInfo;
	}
	BUILD GetBuildInfo(void)
	{
		return m_tBuildInfo;
	}
	bool GetDeadCheck(void)
	{
		return m_bDeadCheck;
	}
	CGameObject* GetAttTArget(void)
	{
		return m_AttTarget;
	}
	eALPH_TYPE GetAlphType(void)
	{
		return m_eAlphType;
	}
	DWORD GetSubState(void)
	{
		return m_dwSubState;
	}
public:
	void SetUnitAtt(bool _bAtt)
	{
		m_bUnitAtt = _bAtt;
	}
	void SetAttTarget(CGameObject* _pObject)
	{
		m_AttTarget = _pObject;
	}
	void SetMyClickInterpace(int iIndex, int iNumber)
	{
		m_iClickMyInterpace[iIndex] = iNumber;
	}
	void SetPos(D3DXVECTOR3& vPos)
	{
		m_tInfo.vPos = vPos;
	}
	void SetSize(D3DXVECTOR3& vSize)
	{
		m_tInfo.vSize = vSize;
	}
	void SetState(DWORD _State)
	{
		m_dwState = _State;
	}
	void SetSelect(bool _bSelect)
	{
		m_bSelect = _bSelect;
	}
	void SetTeamType(eTEAM_TYPE _eType)
	{
		m_eTeamType = _eType;
	}
	void SetBuildType(eBUILD_TYPE _eType)
	{
		m_eBuildType = _eType;
	}
	void SetBuildInfo(BUILD _tInfo)
	{
		m_tBuildInfo = _tInfo;
	}
	void SetStartingPoint(D3DXVECTOR3 _vStartingPoint)
	{
		m_vStartingPoint = _vStartingPoint;
	}
	void SetEndPoint(D3DXVECTOR3 _vPoint)
	{
		m_vEndPoint = _vPoint;
	}
	void SetUnitShild(int _iAtt)
	{
		m_tUnitInfo.iShield -= _iAtt;
	}
	void SetUnitHp(int _iAtt)
	{
		m_tUnitInfo.iHp -= _iAtt;
	}
	void SetUnitMp(int _iMp)
	{
		m_tUnitInfo.iMp -= _iMp;
	}
	void SetUnitShild2(int _iAtt)
	{
		m_tBuildInfo.iShield -= _iAtt;
	}
	void SetUnitHp2(int _iAtt)
	{
		m_tBuildInfo.iHp -= _iAtt;
	}
	void SetUnitMp2(int _iMp)
	{
		m_tBuildInfo.iMp -= _iMp;
	}
	void SetEffectUnitInfo(UNIT _tUnitInfo)
	{
		m_tUnitInfo = _tUnitInfo;
	}
	void SetDeadCheck(bool _bCheck)
	{
		m_bDeadCheck = _bCheck;
	}
	void SetAlphType(eALPH_TYPE _eType)
	{
		m_eAlphType = _eType;
	}
	void CleckMineral(void)
	{
		m_dwPreState = OBJECT_RENDER_TRUE;
	}
	void SetAngle(float _fAngle)
	{
		m_fAngle = _fAngle;
	}
	void SetSubState(DWORD _State)
	{
		m_dwSubState = _State;
	}
	void SetUnitState(DWORD _State)
	{
		m_dwUnitState = _State;
	}
	void SetStateTime(int _iCount)
	{
		m_iStateTime = _iCount;
	}
public:
	virtual HRESULT Initialize(void)PURE;
	virtual int		Update(void)PURE;
	virtual void	Render(void)PURE;
	virtual void	Release(void)PURE;
	virtual void    CheckOneClick(void)PURE;
	virtual void	CreateUnit(bool Check)PURE;
public:
	CGameObject();
	virtual ~CGameObject();
};

