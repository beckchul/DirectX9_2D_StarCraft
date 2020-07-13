#pragma once

#define WINCX 640
#define WINCY 480

#define TILECX 32
#define TILECY 32

#define MINICX 256
#define MINICY 256

#define TILEEX 128
#define TILEEY 128

#define MAPCX 4096
#define MAPCY 4096

#define MAXSCROLLX MAPCX - WINCX
#define MAXSCROLLY MAPCY - WINCY + 100

#define Device		CDevice::GetInstance()
#define ObjMgr		CObjMgr::GetInstance()
#define TextureMgr	CTextureMgr::GetInstance()
#define SceneMgr	CSceneMgr::GetInstance()
#define SoundMgr	CSoundMgr::GetInstance()
#define ASTAR		CAStar::GetInstance()
#define GET_TIME	CTimeMgr::GetInstance()->GetTime()
#define GET_KEY		CKeyMgr::GetInstance()->GetKeyState()
#define	ERRBOX(Text)		MessageBox(g_hWnd, Text, Text, MB_OK);

#define D_CAST(ClassName, pObj) dynamic_cast<ClassName*>(pObj)
#define S_INT_CAST(T) static_cast<int>(T)
#define S_FLOAT_CAST(T) static_cast<float>(T)
#define S_LONG_CAST(T) static_cast<long>(T)

class CGameObject;
typedef std::list<CGameObject*>::iterator OBJITER;