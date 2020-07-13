#pragma once

#include "Include.h"

class CKeyMgr
{
	DECLARE_SINGLETON(CKeyMgr)

private:
	DWORD	m_dwKey;

public:
	void	SetKeyState(void);
	DWORD	GetKeyState(void);
public:
	CKeyMgr();
	~CKeyMgr();
};

