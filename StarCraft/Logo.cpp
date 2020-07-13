#include "stdafx.h"
#include "Logo.h"
#include "TextureMgr.h"
#include "Device.h"
#include "ObjMgr.h"
#include "SceneMgr.h"

CLogo::CLogo()
{
	memset(&m_Crt, 0, sizeof(CRITICAL_SECTION));
}

CLogo::~CLogo()
{
	Release();
}

unsigned int CLogo::ImageLoadThreadFunction(void * pArg)
{
	CLogo* pLogo = (CLogo*)pArg;

	LeaveCriticalSection(&pLogo->GetCrt());

	ObjMgr->LoadData();

	lstrcpy(TextureMgr->GetCurrentPath(), L"Image Load Complate!");

	_endthreadex(0);

	return 0;
}

HRESULT CLogo::Initialize(void)
{
	TextureMgr->InsertTexture(L"../Texture/Intro&Logo/0.png", L"Logo", TEX_SINGLE);

	InitializeCriticalSection(&m_Crt);

	m_hThread = (HANDLE)_beginthreadex(
		NULL
		, 0
		, ImageLoadThreadFunction
		, this
		, 0
		, NULL);

	return S_OK;
}

int CLogo::Update(void)
{
	if (GetAsyncKeyState(VK_RETURN))
	{
		WaitForSingleObject(m_hThread, INFINITE);
		CloseHandle(m_hThread);

		DeleteCriticalSection(&m_Crt);

		SceneMgr->SetChangeScene(SCENE_STAGE);
	}
	return 0;
}

void CLogo::Render(void)
{
	D3DXMATRIX matTrans;

	D3DXMatrixTranslation(&matTrans, WINCX / 2.f, WINCY / 2.f, 0.f);

	const TEX_INFO* pIntroTexture = TextureMgr->GetTexture(L"Logo");

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pIntroTexture->pTexture
		, NULL
		, &D3DXVECTOR3(WINCX / 2.f, WINCY / 2.f, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));

	D3DXMatrixTranslation(&matTrans, 50, 430, 0.f);
	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetFont()->DrawTextW(
		Device->GetSprite()
		, TextureMgr->GetCurrentPath()
		, lstrlen(TextureMgr->GetCurrentPath())
		, NULL, NULL, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CLogo::Release(void)
{
}