#pragma once

#include "Include.h"
#include "ObjectRenderMgr.h"
#include "DistanceMgr.h"
#include "DamageMgr.h"
#include "SoundMgr.h"

class CGameObject
{
protected:
	eALPH_TYPE m_eAlphType;					// 투명유닛 타입
	D3DXVECTOR3 m_vStartingPoint;			// 건물 Rally Point
	D3DXVECTOR3  m_vEndPoint;				// 공중유닛 도착지점
	list<TCHAR*> m_UnitCreateList;			// 건물 생산 라인
	INFO  m_tInfo;							// Object 위치 크기 방향 정보
	UNIT  m_tUnitInfo;						// 유닛의 정보
	BUILD m_tBuildInfo;						// 건물의 정보
	int	  m_iFrameFirst;					// 프레임을 움직이기 위한 변수
	int	  m_iArmor;							// 방어력 변수
	int   m_iShield;						// 쉴드 변수
	bool  m_bCollision;						// 충돌 여부 체크
	bool  m_bSelect;						// 선택 여부 체크
	bool  m_bDeadCheck;						// 사망 여부 체크
	int   m_iClickMyInterpace[9];			// 유닛 & 건물의 오른쪽 하단 UI 불 체크
	float m_fSpeed;							// 유닛의 스피드
	float m_fAngle;							// 유닛의 Frame을 위한 각도
	DWORD m_dwState;						// 유닛의 Frame상태 or 건물의 유닛 생산 상태 or 마우스 상태
	DWORD m_dwPreState;						// 유닛의 Frame이전 상태
	DWORD m_dwSubState;						// 유닛의 상태
	DWORD m_dwUnitState;					// 유닛의 상태이상
	eRENDER_TYPE m_eRenderType;				// Render(소팅)을 위한 변수
	eUNIT_TYPE eUnitType;					// 지상 공중 유닛 구분 위한 변수
	eTEAM_TYPE m_eTeamType;					// 팀 구분을 위한 변수
	eBUILD_TYPE m_eBuildType;				// 건물 구분 위한 변수
	eBUILD_CREATE_TYPE m_eBuildCreateType;	// 지어지는 건물인지 구분 위한 변수
	FRAME m_tCreateTime;					// 건물에서 유닛을 만드는데 드는 시간 변수 or 특수유닛의 각종 활용 변수
	FRAME m_tFrame;							// 유닛의 Frame 변수
	FRAME m_tAlphCount;						// 투명도 계산 변수
	FRAME m_tStateCount;					// 상태이상 계산 변수
	int   m_iStateCount;					// 상태이상 초
	float m_fStateCount;					// 상태이상 초단위 계산
	int   m_iStateTime;						// 상태이상 시간
	int   m_iStasis;

	int m_iView;
	bool		  m_bUnitAtt;				// 공격 시작 유무
	CGameObject*  m_AttTarget;				// 공격 대상 변수
	D3DXVECTOR3	  m_vPatrolStartPoint;		// 유닛의 Patrol 지점 변수
	D3DXVECTOR3	  m_vPatrolEndPoint;
	bool		  m_bPatrol;				// Patrol왔다갔다


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

