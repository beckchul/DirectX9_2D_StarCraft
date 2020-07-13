#pragma once

#include "Include.h"
class CGameObject;

class CObjMgr
{
	DECLARE_SINGLETON(CObjMgr);
private:
	list<CGameObject*>		m_ObjList[OBJ_END];
	vector<CGameObject*>	m_vecRender[RENDER_END];
	list<IMAGE_PATH*>		m_ImagePath;
	list<UNIT*>				m_UnitInfoList;
	list<BUILD*>			m_BuildInfoList;
	list<RESOURCE*>			m_ResourceInfoList;
public:
	void AddGameObject(CGameObject* pObj, eOBJ_TYPE eType);
	static bool CompareY(CGameObject* pSour, CGameObject* pDest);
	void LoadData(void);
	void LoadUnitInfo(void);
	void LoadBuildInfo(void);
	void LoadResourceInfo(void);
	list<CGameObject*>* GetList(void)
	{
		return m_ObjList;
	}
	list<UNIT*> GetUnitList(void)
	{
		return m_UnitInfoList;
	}
	list<BUILD*> GetBuildList(void)
	{
		return m_BuildInfoList;
	}
	list<RESOURCE*> GetReourceList(void)
	{
		return m_ResourceInfoList;
	}
	void DataRelease(void);
public:
	template<typename T>
	T* FindGameObject(int iCount = 0)
	{
		for (int i = 0; i < OBJ_END; ++i)
		{
			OBJITER iter = m_ObjList[i].begin();
			OBJITER iter_end = m_ObjList[i].end();

			for (iter; iter != iter_end; ++iter)
			{
				if (typeid(T) == typeid(*(*iter)))
				{
					if (iCount == 0)
						return  dynamic_cast<T*>((*iter));
					else
						--iCount;
				}
			}
		}
		return NULL;
	}
public:
	HRESULT Initialize(void);
	int		Update(void);
	void	Render(void);
	void	Release(void);
private:
	CObjMgr();
public:
	~CObjMgr();
};

