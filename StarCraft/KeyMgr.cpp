#include "stdafx.h"
#include "KeyMgr.h"

IMPLEMENT_SINGLETON(CKeyMgr)


CKeyMgr::CKeyMgr()
{
}

CKeyMgr::~CKeyMgr()
{
}

void CKeyMgr::SetKeyState(void)
{
	m_dwKey = NULL;

	if (GetAsyncKeyState(VK_RIGHT))
	{
		m_dwKey |= KEY_RIGHT;
	}

	if (GetAsyncKeyState(VK_LEFT))
	{
		m_dwKey |= KEY_LEFT;
	}

	if (GetAsyncKeyState(VK_UP))
	{
		m_dwKey |= KEY_UP;
	}

	if (GetAsyncKeyState(VK_DOWN))
	{
		m_dwKey |= KEY_DOWN;
	}

	if (GetAsyncKeyState('A'))
	{
		m_dwKey |= KEY_A;
	}

	if (GetAsyncKeyState('S'))
	{
		m_dwKey |= KEY_S;
	}

	if (GetAsyncKeyState('D'))
	{
		m_dwKey |= KEY_D;
	}

	if (GetAsyncKeyState('F'))
	{
		m_dwKey |= KEY_F;
	}

	if (GetAsyncKeyState('G'))
	{
		m_dwKey |= KEY_G;
	}

	if (GetAsyncKeyState('H'))
	{
		m_dwKey |= KEY_H;
	}

	if (GetAsyncKeyState('J'))
	{
		m_dwKey |= KEY_J;
	}

	if (GetAsyncKeyState('K'))
	{
		m_dwKey |= KEY_K;
	}

	if (GetAsyncKeyState('L'))
	{
		m_dwKey |= KEY_L;
	}

	if (GetAsyncKeyState('Z'))
	{
		m_dwKey |= KEY_Z;
	}

	if (GetAsyncKeyState('X'))
	{
		m_dwKey |= KEY_X;
	}

	if (GetAsyncKeyState('C'))
	{
		m_dwKey |= KEY_C;
	}

	if (GetAsyncKeyState('V'))
	{
		m_dwKey |= KEY_V;
	}

	if (GetAsyncKeyState('B'))
	{
		m_dwKey |= KEY_B;
	}

	if (GetAsyncKeyState('N'))
	{
		m_dwKey |= KEY_N;
	}

	if (GetAsyncKeyState('M'))
	{
		m_dwKey |= KEY_M;
	}

	if (GetAsyncKeyState('Q'))
	{
		m_dwKey |= KEY_Q;
	}

	if (GetAsyncKeyState('W'))
	{
		m_dwKey |= KEY_W;
	}

	if (GetAsyncKeyState('E'))
	{
		m_dwKey |= KEY_E;
	}

	if (GetAsyncKeyState('R'))
	{
		m_dwKey |= KEY_R;
	}

	if (GetAsyncKeyState('T'))
	{
		m_dwKey |= KEY_T;
	}

	if (GetAsyncKeyState('Y'))
	{
		m_dwKey |= KEY_Y;
	}

	if (GetAsyncKeyState('U'))
	{
		m_dwKey |= KEY_U;
	}

	if (GetAsyncKeyState('I'))
	{
		m_dwKey |= KEY_I;
	}

	if (GetAsyncKeyState('O'))
	{
		m_dwKey |= KEY_O;
	}

	if (GetAsyncKeyState('P'))
	{
		m_dwKey |= KEY_P;
	}

	if (GetAsyncKeyState(VK_LBUTTON))
	{
		m_dwKey |= KEY_LBUTTON;
	}

	if (GetAsyncKeyState(VK_RBUTTON))
	{
		m_dwKey |= KEY_RBUTTON;
	}
}

DWORD CKeyMgr::GetKeyState(void)
{
	return m_dwKey;
}