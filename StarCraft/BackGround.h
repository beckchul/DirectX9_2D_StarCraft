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
		Option = 0 Áö»ó ¶¥
				 1 ¸ø°¡´Â ¶¥
				 2 ¹Ì³×¶ö
				 3 °¡½º
				 4 ¾ð´ö ¶¥
				 5 Áö»ó °Ç¹°¶¥
				 6 ¾ð´ö °Ç¹°¶¥
				 7 °Ç¹° Áö¾îÁø °¡½º
				 8 Áö»ó À¯´ÖÀÌ ¸·Àº ¶¥
				 9 ¾ð´ö À¯´ÖÀÌ ¸·Àº ¶¥

		DrawID = 0 º¸Åë
				 1 ³Ø¼­½º ºÒ°¡´É
				 2 ÆÄÀÏ·± µ¿·Â ¹üÀ§
				 3 ÆÄÀÏ·± µ¿·Â ¹üÀ§ + ³Ø¼­½º ºÒ°¡´É
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

