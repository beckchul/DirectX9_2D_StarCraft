#include "StdAfx.h"
#include "TextureMgr.h"
#include "MultiTexture.h"
#include "SingleTexture.h"

IMPLEMENT_SINGLETON(CTextureMgr);

CTextureMgr::CTextureMgr(void)
{
}

CTextureMgr::~CTextureMgr(void)
{
	Release();
}

HRESULT CTextureMgr::InsertTexture(const TCHAR* pFilePath
								   , const TCHAR* pObjKey
								   , eTEXTURE_TYPE eType
								   , const TCHAR* pStateKey /*= L""*/
								   , const int& iCount /*= 0*/)
{

	map<const TCHAR*, CTexture*>::iterator iter;

	iter = find_if(m_matTexture.begin(), m_matTexture.end(), STRING_CMP(pObjKey));

	if(iter == m_matTexture.end())
	{
		//없는 이미지 키값이면 새롭게 데이터를 생성한다.

		CTexture* pTexture = NULL;

		switch(eType)
		{
		case TEX_MULTI:
			pTexture = new CMultiTexture;
			break;

		case TEX_SINGLE:
			pTexture = new CSingleTexture;
			break;
		}
		
		if(FAILED(pTexture->InsertTexture(pFilePath, pStateKey, iCount) ) )
		{
			MessageBox(g_hWnd, pFilePath, L"이미지 생성 실패", MB_OK);
			return E_FAIL;
		}

		TCHAR* pKey = new TCHAR[MAX_STR];
		lstrcpy(pKey, pObjKey);

		m_matTexture.insert( make_pair(pKey, pTexture) );
	}
	else
	{
		if (TEX_MULTI == eType)
		{
			iter->second->InsertTexture(pFilePath, pStateKey, iCount);
		}
	}

	return S_OK;
}

const TEX_INFO* CTextureMgr::GetTexture(const TCHAR* pObjKey /*= L"" */
										,const TCHAR* pStateKey /*= L"" */
										,const int& iCount /*= 0*/)
{
	map<const TCHAR*, CTexture*>::iterator iter;

	iter = find_if(m_matTexture.begin(), m_matTexture.end(), STRING_CMP(pObjKey));

	if(iter == m_matTexture.end())
		return NULL;

	return iter->second->GetTexture(pStateKey, iCount);
}

void CTextureMgr::Release(void)
{
	map<const TCHAR*, CTexture*>::iterator iter = m_matTexture.begin();
	map<const TCHAR*, CTexture*>::iterator iter_end = m_matTexture.end();

	for(iter; iter != iter_end; ++iter)
	{
		delete [] iter->first;
		delete iter->second;
	}
	m_matTexture.clear();
}


