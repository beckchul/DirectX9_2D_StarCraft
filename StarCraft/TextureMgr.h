#pragma once

#include "Include.h"
class CTexture;

class CTextureMgr
{
DECLARE_SINGLETON(CTextureMgr);

private:
	map<const TCHAR*, CTexture*>		m_matTexture;
	TCHAR	m_szCurrentPath[MAX_STR];
public:
	HRESULT InsertTexture(const TCHAR* pFilePath,
							const TCHAR* pObjKey,
							eTEXTURE_TYPE eType,
							const TCHAR* pStateKey = L"",
							const int& iCount = 0);

	const TEX_INFO* GetTexture(const TCHAR* pObjKey = L""
							  ,const TCHAR* pStateKey = L""
							  ,const int& iCount = 0);

	TCHAR* GetCurrentPath(void) { return m_szCurrentPath;  }
	void Release(void);

private:
	CTextureMgr(void);
public:
	~CTextureMgr(void);
};
