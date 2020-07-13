#pragma once
#include "GameObject.h"
class CInterpace :
	public CGameObject
{
private:
	int		m_iWorldMineral[TEAM_END];
	int		m_iWorldGas[TEAM_END];

	int		m_iHumunCount[TEAM_END];
	int		m_iHumunMaxCount[TEAM_END];
private:
	list<CGameObject*> m_InterpaceUnitList;
	TCHAR	szPortraitsStateKey[MIN_STR];
	FRAME	m_tNoise;
	RECT	m_tInterpaceRect[9];
	int		m_iBigWireNumber;
	int		m_iSmallWireNumber;
	int		m_iStatsNumber;
	bool	m_bUnitOneClick;
	bool	m_bSelectUnit;
	bool	m_bSelectBuild;
	bool	m_bRenderPortraits;
	float	m_fPortraitsFrameFirst;


	bool	m_bMapLook;

	bool		 m_bClickKey;
	FRAME		 m_tClick;
	FRAME m_tAIFrame;
	int m_iAITimeCount;
private:
	int		m_iUpgradeCount[TEAM_END][UPGRADE_END];

public:
	int GetUpgradeCount(eTEAM_TYPE _eTeamType, eUPGRADE_TYPE _eType)
	{
		return m_iUpgradeCount[_eTeamType][_eType];
	}
	void SetUpgradeCount(eTEAM_TYPE _eTeamType, eUPGRADE_TYPE _eType, int _iCount)
	{
		m_iUpgradeCount[_eTeamType][_eType] = _iCount;
	}
public:
	void ClearInterpaceUnitList(void)
	{
		m_InterpaceUnitList.clear();
	}
	void SetInterpaceUnitList(CGameObject* pObject)
	{
		m_InterpaceUnitList.push_back(pObject);
	}
	void EraseUnitList(CGameObject* pObject)
	{
		for (list<CGameObject*>::iterator iter = m_InterpaceUnitList.begin(); iter != m_InterpaceUnitList.end(); ++iter)
		{
			if ((*iter) == pObject)
			{
				m_InterpaceUnitList.erase(iter);
				return;
			}
		}
	}
	list<CGameObject*> GetInterpaceUnitList(void)
	{
		return m_InterpaceUnitList;
	}
public:
	bool GetMapLook(void)
	{
		return m_bMapLook;
	}
	int GetAICount(void)
	{
		return m_iAITimeCount;
	}
	FRAME GetAIFrame(void)
	{
		return m_tAIFrame;
	}
	bool GetOneClick(void)
	{
		return m_bUnitOneClick;
	}
	int GetWorldMineral(eTEAM_TYPE eType)
	{
		return m_iWorldMineral[eType];
	}
	int GetWorldGas(eTEAM_TYPE eType)
	{
		return m_iWorldGas[eType];
	}
	int GetHumanCount(eTEAM_TYPE _eType)
	{
		return m_iHumunCount[_eType];
	}
	int GetHumanMaxCount(eTEAM_TYPE _eType)
	{
		return m_iHumunMaxCount[_eType];
	}
public:
	void PlusWorldMineral(eTEAM_TYPE eType, int _iCount)
	{
		m_iWorldMineral[eType] += _iCount;
	}
	void PlusWorldGas(eTEAM_TYPE eType, int _iCount)
	{
		m_iWorldGas[eType] += _iCount;
	}
	void SetUnitInfo(UNIT _tUnitInfo)
	{
		m_tUnitInfo = _tUnitInfo;
		m_bSelectUnit = true;
		m_bSelectBuild = false;
		SettingInfo();
		SettingPortraits();
	}
	void SetBuildInfo(BUILD _tBuildInfo)
	{
		m_tBuildInfo = _tBuildInfo;
		m_bSelectUnit = false;
		m_bSelectBuild = true;
		SettingInfo();
		SettingPortraits();
	}
	void NotPortaraits(void)
	{
		m_bRenderPortraits = false;
	}
	void OneObjectClick(bool _bCheck)
	{
		if (!m_bRenderPortraits)
			return;

		m_bUnitOneClick = _bCheck;

		if (m_bUnitOneClick)
		{
			SettingCenterInterpase();
		}
	}
	
	bool CheckingPointInRect(D3DXVECTOR3 vPoint, int _iRectNumber)
	{
		if (m_tInterpaceRect[_iRectNumber].left < vPoint.x &&
			m_tInterpaceRect[_iRectNumber].right > vPoint.x &&
			m_tInterpaceRect[_iRectNumber].top < vPoint.y &&
			m_tInterpaceRect[_iRectNumber].bottom > vPoint.y)
			return true;

		return false;
	}
public:
	void MiniClick(void);
	void MiniView(void);
	void CenterUnitList(bool _bCheck);
	void RenderNormalInterpace(void);
	void RenderUnNormalnInterpace(int iCount);
	void FrameMove(void);
	void SettingPortraits(void);
	void SettingInfo(void);
	void SettingCenterInterpase(void);
	void SettingInterpaceRect(void);
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void    CheckOneClick(void) {}
	virtual void	CreateUnit(bool Check) {}
public:
	CInterpace();
	virtual ~CInterpace();
};

