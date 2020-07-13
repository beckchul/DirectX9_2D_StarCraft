#include "stdafx.h"
#include "Intro.h"
#include "SceneMgr.h"

CIntro::CIntro()
{
}

CIntro::~CIntro()
{
	Release();
}

HRESULT CIntro::Initialize(void)
{
	m_hVideo = MCIWndCreate(g_hWnd, NULL, MCIWNDF_NOPLAYBAR | WS_VISIBLE | WS_CHILD,
		L"../Texture/Intro&Logo/Intro.wmv");
	MoveWindow(m_hVideo, 0, 0, WINCX, WINCY, NULL);

	MCIWndPlay(m_hVideo);
	return S_OK;
}

int CIntro::Update(void)
{
	if (GetAsyncKeyState(VK_SPACE))
	{
		SceneMgr->SetChangeScene(SCENE_LOGO);
	}
	return 0;
}

void CIntro::Render(void)
{
}

void CIntro::Release(void)
{
	MCIWndClose(m_hVideo);
}