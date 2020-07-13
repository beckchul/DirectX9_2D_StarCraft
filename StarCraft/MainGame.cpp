#include "stdafx.h"
#include "MainGame.h"
#include "Device.h"
#include "SceneMgr.h"
#include "TextureMgr.h"
#include "TimeMgr.h"
#include "SoundMgr.h"
#include "ObjMgr.h"
#include "KeyMgr.h"
#include "AStar.h"
#include "SoundMgr.h"

CMainGame::CMainGame()
	:m_pDevice(CDevice::GetInstance())
	,m_fTime(0)
	,m_iCount(0)
{
}


CMainGame::~CMainGame()
{
	Release();
}

HRESULT CMainGame::Initialize(void)
{
	if (FAILED(m_pDevice->InitD3D()))
	{
		ERRBOX(L"Device Init Failed!");
		return E_FAIL;
	}

	SoundMgr->Initialize();
	SoundMgr->LoadSoundFile();
	SceneMgr->Initialize();
	SceneMgr->SetChangeScene(SCENE_LOGO);
	CTimeMgr::GetInstance()->InitTime();
	CKeyMgr::GetInstance();

	srand(unsigned int(time(NULL)));

	return S_OK;
}

int CMainGame::Update(void)
{
	CKeyMgr::GetInstance()->SetKeyState();
	CTimeMgr::GetInstance()->SetTime();
	SceneMgr->Update();
	return 0;
}

void CMainGame::Render(void)
{
	if (SceneMgr->GetSceneType() != SCENE_INTRO)
	{
		m_pDevice->Render_Begin();

		SceneMgr->Render();

		m_pDevice->Render_End();
	}
}

void CMainGame::Release(void)
{
	m_pDevice->DestroyInstance();
	ObjMgr->DestroyInstance();
	SceneMgr->DestroyInstance();
	TextureMgr->DestroyInstance();
	CTimeMgr::GetInstance()->DestroyInstance();
	CKeyMgr::GetInstance()->DestroyInstance();
	SoundMgr->DestroyInstance();
}