#pragma once

#include "Include.h"

class CTimeMgr
{
	DECLARE_SINGLETON(CTimeMgr);

private:
	LARGE_INTEGER	m_FrameTime;
	LARGE_INTEGER	m_FixTime;
	LARGE_INTEGER	m_LastTime;
	LARGE_INTEGER	m_CurTick;

	float			m_fTime;

public:
	float GetTime(void);

public:
	void InitTime(void);
	void SetTime(void);
	void Release(void);
private:
	CTimeMgr();
public:
	~CTimeMgr();
};

