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
	//QueryPerformanceCounter() : Ÿ�̸��� ���簪�� ��ȯ�޴� �Լ��̴�.
	//						->1 / 10000�ʸ� ��ȯ�Ѵ�.

	//QueryPerformanceFrequency() : ���� CPU�� 1�ʴ� �������� ��ȯ�޴� �Լ��̴�.

	QueryPerformanceCounter(&m_FrameTime);
	QueryPerformanceCounter(&m_FixTime);
	QueryPerformanceCounter(&m_LastTime);

	QueryPerformanceFrequency(&m_CurTick);
}

void CTimeMgr::SetTime(void)
{
	//ȣ���� �ɶ����� ���ο� �ð��� ���Ѵ�.
	QueryPerformanceCounter(&m_FrameTime);

	//���� ���� �ð��� �ʱ�(Init)�Ǿ������� �ð� ���̸� ���Ѵ�.
	//m_CurTick : 1�ʴ� �������� ���Ѵ�.
	if (m_FrameTime.QuadPart - m_LastTime.QuadPart > m_CurTick.QuadPart)
	{
		//���α׷��� ���ϰ��ɷ����� �ð��� �ڷ� �и��°��� �����ϴ� �ڵ带 ���� �ۼ��Ѵ�.

		//Cpu Tick�� �����Ѵ�.
		QueryPerformanceFrequency(&m_CurTick);

		//1�� ���� ���� ���Ӱ� �����ϵ��� �Ѵ�.
		m_LastTime.QuadPart = m_FrameTime.QuadPart;
	}

	//���ΰ����� Update()�ѹ� ȣ�����?
	// ���������������� ���͹����� �������� ������ 1�ʵ��� Frame���� ��ȯ�ȴ�.
	m_fTime = float(m_FrameTime.QuadPart - m_FixTime.QuadPart) / m_CurTick.QuadPart;

	m_FixTime = m_FrameTime;
}

void CTimeMgr::Release(void)
{

}