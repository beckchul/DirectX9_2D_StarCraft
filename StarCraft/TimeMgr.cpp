#include "stdafx.h"
#include "TimeMgr.h"

IMPLEMENT_SINGLETON(CTimeMgr);

CTimeMgr::CTimeMgr()
{
}

CTimeMgr::~CTimeMgr()
{
	Release();
}

float CTimeMgr::GetTime(void)
{
	return m_fTime;
}

void CTimeMgr::InitTime(void)
{
	//QueryPerformanceCounter() : 타이머의 현재값을 반환받는 함수이다.
	//						->1 / 10000초를 반환한다.

	//QueryPerformanceFrequency() : 실제 CPU의 1초당 진동수를 반환받는 함수이다.

	QueryPerformanceCounter(&m_FrameTime);
	QueryPerformanceCounter(&m_FixTime);
	QueryPerformanceCounter(&m_LastTime);

	QueryPerformanceFrequency(&m_CurTick);
}

void CTimeMgr::SetTime(void)
{
	//호출이 될때마다 새로운 시간을 구한다.
	QueryPerformanceCounter(&m_FrameTime);

	//새로 구한 시간과 초기(Init)되었때와의 시간 차이를 구한다.
	//m_CurTick : 1초당 진동수를 뜻한다.
	if (m_FrameTime.QuadPart - m_LastTime.QuadPart > m_CurTick.QuadPart)
	{
		//프로그램이 부하가걸렸을때 시간이 뒤로 밀리는것을 방지하는 코드를 여기 작성한다.

		//Cpu Tick을 갱신한다.
		QueryPerformanceFrequency(&m_CurTick);

		//1초 뒤의 값을 새롭게 갱신하도록 한다.
		m_LastTime.QuadPart = m_FrameTime.QuadPart;
	}

	//메인게임의 Update()한번 호출되지?
	// 다음진동수까지의 인터벌값을 진동수로 나누면 1초동안 Frame값이 반환된다.
	m_fTime = float(m_FrameTime.QuadPart - m_FixTime.QuadPart) / m_CurTick.QuadPart;

	m_FixTime = m_FrameTime;
}

void CTimeMgr::Release(void)
{

}