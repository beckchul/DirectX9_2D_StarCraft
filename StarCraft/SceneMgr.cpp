#include "StdAfx.h"
#include "SceneMgr.h"
#include "Scene.h"
#include "Logo.h"
#include "Intro.h"
#include "Stage.h"

IMPLEMENT_SINGLETON(CSceneMgr);

CSceneMgr::CSceneMgr(void)
	:m_pScene(NULL)
{
}

CSceneMgr::~CSceneMgr(void)
{
	Release();
}

void CSceneMgr::Initialize(void)
{
	m_pScene		= NULL;
	m_bChangeScene	= false;
	m_eType			= SCENE_END;
}

int CSceneMgr::Update(void)
{
	if(m_bChangeScene)
	{
		m_bChangeScene = false;
		SetScene(m_eType);
	}

	if(m_pScene)
		m_pScene->Update();

	return 0;
}

void CSceneMgr::Render()
{
	if(m_pScene)
		m_pScene->Render();
}

void CSceneMgr::Release(void)
{
	Safe_Delete(m_pScene);
}

void CSceneMgr::SetScene(eSCENE_TYPE eType)
{
	if(m_pScene)
	{
		SAFE_DELETE(m_pScene);
	}

	switch(eType)
	{
	case SCENE_INTRO:
		m_pScene = new CIntro;
		break;
	case SCENE_LOGO:
		m_pScene = new CLogo;
		break;

	case SCENE_STAGE:
		m_pScene = new CStage;
		break;
	}

	m_pScene->Initialize();
}

void CSceneMgr::SetChangeScene(eSCENE_TYPE eType)
{
	m_eType = eType;
	m_bChangeScene = true;
}
