#pragma once
#include "GameObject.h"
class CBackGround :
	public CGameObject
{
private:
	vector<PTILE>	m_vecTile;
private:
	void LoadTile(void);
	void MouseScrollCheck(void);
	void KeyScroll(void);
	void SettingScroll(void);

	void SettingTileOption(void);
	
public:
	const vector<PTILE> GetTile(void)
	{
		return m_vecTile;
	}
	int GetTileOption(int _iIndex)
	{
		return m_vecTile[_iIndex]->byOption;
	}
	/*
		Option = 0 ���� ��
				 1 ������ ��
				 2 �̳׶�
				 3 ����
				 4 ��� ��
				 5 ���� �ǹ���
				 6 ��� �ǹ���
				 7 �ǹ� ������ ����
				 8 ���� ������ ���� ��
				 9 ��� ������ ���� ��

		DrawID = 0 ����
				 1 �ؼ��� �Ұ���
				 2 ���Ϸ� ���� ����
				 3 ���Ϸ� ���� ���� + �ؼ��� �Ұ���
	*/
	void SetTileUnitOption(int _iIndex)
	{
		if (m_vecTile[_iIndex]->byOption == 0)
			m_vecTile[_iIndex]->byOption = 8;
		else if (m_vecTile[_iIndex]->byOption == 8)
			m_vecTile[_iIndex]->byOption = 0;
		if (m_vecTile[_iIndex]->byOption == 4)
			m_vecTile[_iIndex]->byOption = 9;
		else if (m_vecTile[_iIndex]->byOption == 9)
			m_vecTile[_iIndex]->byOption = 4;
	}
	void SetTileBuildOption(int _iIndex)
	{
		if(m_vecTile[_iIndex]->byOption == 0)
			m_vecTile[_iIndex]->byOption = 5;
		else if (m_vecTile[_iIndex]->byOption == 4)
			m_vecTile[_iIndex]->byOption = 6;
		else if (m_vecTile[_iIndex]->byOption == 5)
			m_vecTile[_iIndex]->byOption = 0;
		else if (m_vecTile[_iIndex]->byOption == 6)
			m_vecTile[_iIndex]->byOption = 4;
		else if (m_vecTile[_iIndex]->byOption == 3)
			m_vecTile[_iIndex]->byOption = 7;
		else if (m_vecTile[_iIndex]->byOption == 7)
			m_vecTile[_iIndex]->byOption = 3;
	}
	void SetTileDrawIDCreate(int _iIndex)
	{
		if(m_vecTile[_iIndex]->byDrawID == 0)
			m_vecTile[_iIndex]->byDrawID = 2;
		else if (m_vecTile[_iIndex]->byDrawID == 1)
			m_vecTile[_iIndex]->byDrawID = 3;
	}
	void SetTIleDrawIDDelete(int _iIndex)
	{
		if (m_vecTile[_iIndex]->byDrawID == 2)
			m_vecTile[_iIndex]->byDrawID = 0;
		else if (m_vecTile[_iIndex]->byDrawID == 3)
			m_vecTile[_iIndex]->byDrawID = 1;
	}
	//void SetTileBuildOptionNexus(int iIndex)
	//{
	//	if (m_vecTile[iIndex]->byOption == 0)
	//		m_vecTile[iIndex]->byOption = 7;
	//	else if (m_vecTile[iIndex]->byOption == 4)
	//		m_vecTile[iIndex]->byOption = 8;
	//	else if (m_vecTile[iIndex]->byOption == 7)
	//		m_vecTile[iIndex]->byOption = 0;
	//	else if (m_vecTile[iIndex]->byOption == 8)
	//		m_vecTile[iIndex]->byOption = 4;
	//}
public:
	virtual HRESULT Initialize(void);
	virtual int		Update(void);
	virtual void	Render(void);
	virtual void	Release(void);
	virtual void    CheckOneClick(void) {}
	virtual void	CreateUnit(bool Check) {}
public:
	CBackGround();
	virtual ~CBackGround();
};

