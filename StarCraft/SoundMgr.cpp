#include "StdAfx.h"
#include "SoundMgr.h"
#include "Functor.h"
#include "ObjMgr.h"
#include "GameObject.h"

IMPLEMENT_SINGLETON(CSoundMgr)

CSoundMgr::CSoundMgr(void)
: m_pSystem(NULL)
{
	Initialize();
}

CSoundMgr::~CSoundMgr(void)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter_begin = m_MapSound.begin();
	map<TCHAR*, FMOD_SOUND*>::iterator iter_end = m_MapSound.end();

	for(; iter_begin != iter_end; ++iter_begin)
	{
		delete[] iter_begin->first;
		FMOD_Sound_Release(iter_begin->second);
	}

	m_MapSound.clear();

	FMOD_System_Close(m_pSystem);
	FMOD_System_Release(m_pSystem);
}

void CSoundMgr::Initialize( void )
{
	unsigned int niVersion = NULL;
	FMOD_System_Create(&m_pSystem);
	FMOD_System_GetVersion(m_pSystem, &niVersion);
	FMOD_System_Init(m_pSystem, CHANNEL_END, FMOD_INIT_NORMAL, NULL);
}

void CSoundMgr::LoadSoundFile( void )
{
	_finddata_t	fd = {};
	long handle;
	int iResult = 0;

	handle = _findfirst("../Sound/*.*", &fd);

	if(-1 == handle)
	{
		MessageBox(g_hWnd, L"Not found soundfile", L"Not found soundfile", MB_OK);
		return;
	}

	while(-1 != iResult)
	{
		TCHAR*	pSoundKey = new TCHAR[256];
		ZeroMemory(pSoundKey, sizeof(TCHAR) * 256);

		MultiByteToWideChar(CP_ACP, 0, fd.name, strlen(fd.name) + 1, pSoundKey, 256);

		char szFullPath[256] = "";
		strcpy_s(szFullPath, 256, "../Sound/");
		strcat_s(szFullPath, 256, fd.name);

		FMOD_SOUND*	pSound;

		FMOD_RESULT FResult = FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_HARDWARE, NULL, &pSound);

		if(FMOD_OK == FResult)
			m_MapSound.insert(make_pair(pSoundKey, pSound));
		else
		{
			delete[] pSoundKey;
		}

		iResult = _findnext(handle, &fd);
	}

	_findclose(handle);

	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::PlaySound( TCHAR* pSoundKey, CHANNEL_TYPE eChannel )
{
	
	
	map<TCHAR*, FMOD_SOUND*>::iterator iter = find_if(m_MapSound.begin(),
		m_MapSound.end(), CStringCmp(pSoundKey));

	if(iter == m_MapSound.end())
		return;

	FMOD_BOOL bCheck;
	if (FMOD_Channel_IsPlaying(m_pChannel[eChannel], &bCheck))
		return;
	

	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0, &(m_pChannel[eChannel]));

}

void CSoundMgr::PlaySound( TCHAR* pSoundKey, CHANNEL_TYPE eChannel, float fVolume )
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter = find_if(m_MapSound.begin(),
		m_MapSound.end(), CStringCmp(pSoundKey));

	if(iter == m_MapSound.end())
		return;

	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0, &(m_pChannel[eChannel]));
	FMOD_Channel_SetVolume(m_pChannel[eChannel], fVolume);
}

void CSoundMgr::PlaySound(TCHAR* pSoundKey, CHANNEL_TYPE eChannel, float fVolume, D3DXVECTOR3 vPos)
{
	map<TCHAR*, FMOD_SOUND*>::iterator iter = find_if(m_MapSound.begin(),
		m_MapSound.end(), CStringCmp(pSoundKey));

	if (iter == m_MapSound.end())
		return;

	CGameObject* pObject = ObjMgr->GetList()[OBJ_INTERPACE].front();

	
	D3DXVECTOR3 vScroll = pObject->GetScrollPos();
	float fX = fabs(vPos.x - vScroll.x + WINCX / 2.f);
	float fY = fabs(vPos.y - vScroll.y + WINCY / 2.f);

	float fN = S_FLOAT_CAST(S_INT_CAST(fX) / 100 + S_INT_CAST(fY) / 100);
	fVolume = fVolume - fN * 0.02f;

	if (fVolume < 0.f)
		fVolume = 0.f;

	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0, &(m_pChannel[eChannel]));
	FMOD_Channel_SetVolume(m_pChannel[eChannel], fVolume);
}

void CSoundMgr::PlayBGM( TCHAR* pSoundKey, CHANNEL_TYPE eChannel, float fVolume)
{
	FMOD_Channel_Stop(m_pChannel[eChannel]);
	map<TCHAR*, FMOD_SOUND*>::iterator iter = find_if(m_MapSound.begin(), 
		m_MapSound.end(), CStringCmp(pSoundKey));

	if(iter == m_MapSound.end())
		return;

	FMOD_Sound_SetMode(iter->second, FMOD_LOOP_NORMAL);
	FMOD_System_PlaySound(m_pSystem, FMOD_CHANNEL_REUSE, iter->second, 0, &(m_pChannel[eChannel]));
	FMOD_Channel_SetVolume(m_pChannel[eChannel], fVolume);
}

void CSoundMgr::UpdateSound( void )
{
	FMOD_System_Update(m_pSystem);
}

void CSoundMgr::StopSound( CHANNEL_TYPE eChannel )
{
	FMOD_Channel_Stop(m_pChannel[eChannel]);
}

void CSoundMgr::StopSoundAll( void )
{
	for(int i = 0; i < CHANNEL_END; ++i)
		FMOD_Channel_Stop(m_pChannel[i]);
}

