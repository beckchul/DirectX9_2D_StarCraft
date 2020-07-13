#include "stdafx.h"
#include "BackGround.h"
#include "Device.h"
#include "TextureMgr.h"
#include "KeyMgr.h"
#include "Mouse.h"
#include "ObjMgr.h"
#include "TimeMgr.h"

CBackGround::CBackGround()
{
}

CBackGround::~CBackGround()
{
	Release();
}

HRESULT CBackGround::Initialize(void)
{
	m_vScroll.x = MAXSCROLLX;
	m_vScroll.y = MAPCY - WINCY;
	LoadTile();
	m_fSpeed = 200.f;
	return S_OK;
}

int CBackGround::Update(void)
{
	MouseScrollCheck();
	KeyScroll();
	SettingScroll();
	return 0;
}

void CBackGround::Render(void)
{
	D3DXMATRIX matTrans;

	const TEX_INFO* pMapTexture = TextureMgr->GetTexture(L"Map", L"Main", 0);

	D3DXMatrixTranslation(&matTrans
		, pMapTexture->ImageInfo.Width / 2.f - m_vScroll.x
		, pMapTexture->ImageInfo.Height / 2.f - m_vScroll.y
		, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pMapTexture->pTexture
		, NULL
		, &D3DXVECTOR3(MAPCX / 2.f, MAPCY / 2.f, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));

}

void CBackGround::Release(void)
{
	for (size_t i = 0; i < m_vecTile.size(); ++i)
		SAFE_DELETE(m_vecTile[i]);

	m_vecTile.clear();
}


void CBackGround::LoadTile(void)
{
	HANDLE hFile = CreateFile(L"../Data/MapData.bat", GENERIC_READ, NULL, NULL
		, OPEN_EXISTING
		, FILE_ATTRIBUTE_NORMAL
		, NULL);

	DWORD dwByte;

	while (true)
	{
		PTILE pTile = new TILE;

		ReadFile(hFile, pTile, sizeof(TILE), &dwByte, NULL);

		if (dwByte == NULL)
		{
			SAFE_DELETE(pTile);
			break;
		}

		m_vecTile.push_back(pTile);
	}
	CloseHandle(hFile);

	SettingTileOption();
}

void CBackGround::MouseScrollCheck(void)
{
	CGameObject* ptMouse = ObjMgr->GetList()[OBJ_MOUSE].front();
	D3DXVECTOR3 vMouse = D_CAST(CMouse, ptMouse)->GetMousePos();

	if (vMouse.x < m_vScroll.x + 15 && m_vScroll.x > 0)
	{
		m_vScroll.x -= GET_TIME * m_fSpeed;
		ptMouse->SetState(MOUSE_L);

		if (vMouse.y < m_vScroll.y + 15)
		{
			m_vScroll.y -= GET_TIME * m_fSpeed;
			ptMouse->SetState(MOUSE_LU);
		}
		else if (vMouse.y > m_vScroll.y + WINCY - 15)
		{
			m_vScroll.y += GET_TIME * m_fSpeed;
			ptMouse->SetState(MOUSE_LD);
		}

		if (vMouse.x < m_vScroll.x + 10)
		{
			m_vScroll.x -= GET_TIME * m_fSpeed * 3;
		}
		else if (vMouse.y < m_vScroll.y + 10)
		{
			m_vScroll.y -= GET_TIME * m_fSpeed * 3;
		}
		else if (vMouse.y > m_vScroll.y + WINCY - 10)
		{
			m_vScroll.y += GET_TIME * m_fSpeed * 3;
		}
		return;
	}
	else if (vMouse.x > m_vScroll.x + WINCX - 30 && m_vScroll.x < MAXSCROLLX)
	{
		m_vScroll.x += GET_TIME * m_fSpeed;
		ptMouse->SetState(MOUSE_R);

		if (vMouse.y < m_vScroll.y + 15)
		{
			m_vScroll.y -= GET_TIME * m_fSpeed;
			ptMouse->SetState(MOUSE_RU);
		}
		else if (vMouse.y > m_vScroll.y + WINCY - 15)
		{
			m_vScroll.y += GET_TIME * m_fSpeed;
			ptMouse->SetState(MOUSE_RD);
		}

		if (vMouse.x > m_vScroll.x + WINCX - 10)
		{
			m_vScroll.x += GET_TIME * m_fSpeed * 3;
		}
		else if (vMouse.y < m_vScroll.y + 10)
		{
			m_vScroll.y -= GET_TIME * m_fSpeed * 3;
		}
		else if (vMouse.y > m_vScroll.y + WINCY - 10)
		{
			m_vScroll.y += GET_TIME * m_fSpeed * 3;
		}
		return;
	}

	if (vMouse.y < m_vScroll.y + 15 && m_vScroll.y > 0)
	{
		m_vScroll.y -= GET_TIME * m_fSpeed;
		ptMouse->SetState(MOUSE_U);

		if(vMouse.y < m_vScroll.y + 10)
			m_vScroll.y -= GET_TIME * m_fSpeed * 3;
	}
	else if (vMouse.y > m_vScroll.y + WINCY - 15 && m_vScroll.y < MAXSCROLLY)
	{
		m_vScroll.y += GET_TIME * m_fSpeed;
		ptMouse->SetState(MOUSE_D);

		if(vMouse.y > m_vScroll.y + WINCY - 10)
			m_vScroll.y += GET_TIME * m_fSpeed * 3;
	}
	
}

void CBackGround::KeyScroll(void)
{
	DWORD dwKey = GET_KEY;

	if (dwKey & KEY_LEFT)
	{
		m_vScroll.x -= GET_TIME * m_fSpeed * 5;
	}
	if (dwKey & KEY_RIGHT)
	{
		m_vScroll.x += GET_TIME * m_fSpeed * 5;
	}
	if (dwKey & KEY_UP)
	{
		m_vScroll.y -= GET_TIME * m_fSpeed * 5;
	}
	if (dwKey & KEY_DOWN)
	{
		m_vScroll.y += GET_TIME * m_fSpeed * 5;
	}
}

void CBackGround::SettingScroll(void)
{
	if (m_vScroll.x < 0)
		m_vScroll.x = 0;
	else if (m_vScroll.x > MAXSCROLLX)
		m_vScroll.x = MAXSCROLLX;
	if (m_vScroll.y < 0)
		m_vScroll.y = 0;
	else if (m_vScroll.y > MAXSCROLLY)
		m_vScroll.y = MAXSCROLLY;
}

void CBackGround::SettingTileOption(void)
{
	for (size_t i = 0; i < m_vecTile.size(); ++i)
	{
		m_vecTile[i]->byDrawID = 0;
	}
	for (size_t i = 0; i < m_vecTile.size(); ++i)
	{
		if (m_vecTile[i]->byOption == 2 ||
			m_vecTile[i]->byOption == 3)
		{
			if (i >= 0 && i < TILEEX * TILEEY)
			{
				m_vecTile[i]->byDrawID = 1;
			}

			for (int j = 0; j < 4; ++j)
			{
				for (int k = 0; k < 3; ++k)
				{
					if (i - TILEEX * k - j >= 0 && i - TILEEX * k - j < TILEEX * TILEEY)
					{
						m_vecTile[i - TILEEX * k - j]->byDrawID = 1;
					}
					if (i - TILEEX * k + j >= 0 && i - TILEEX * k + j < TILEEX * TILEEY)
					{
						m_vecTile[i - TILEEX * k + j]->byDrawID = 1;
					}

					if (i + TILEEX * k - j >= 0 && i + TILEEX * k - j < TILEEX * TILEEY)
					{
						m_vecTile[i + TILEEX * k - j]->byDrawID = 1;
					}
					if (i + TILEEX * k + j >= 0 && i + TILEEX * k + j < TILEEX * TILEEY)
					{
						m_vecTile[i + TILEEX * k + j]->byDrawID = 1;
					}
				}

			}
		}
	}
}