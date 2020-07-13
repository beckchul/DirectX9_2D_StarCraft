#pragma once

#include "Include.h"

class CScene;
class CSceneMgr
{
	DECLARE_SINGLETON(CSceneMgr);
private:
	CScene* m_pScene;

	eSCENE_TYPE	m_eType;
	bool		m_bChangeScene;

public:
	void Initialize(void);
	int Update(void);
	void Render();
	void Release(void);

public:
	eSCENE_TYPE GetSceneType(void)
	{
		return m_eType;
	}
	void SetScene(eSCENE_TYPE eType);
	void SetChangeScene(eSCENE_TYPE eType);

private:
	CSceneMgr(void);
public:
	~CSceneMgr(void);
};
