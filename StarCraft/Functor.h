#pragma once

class CStringCmp
{
private:
	const TCHAR* m_pStr;

public:
	CStringCmp(const TCHAR* pStr)
		: m_pStr(pStr)
	{}

public:
	template <typename T>
	bool operator() (T& obj)
	{
		return !_tcscmp(obj.first, m_pStr);
	}
};