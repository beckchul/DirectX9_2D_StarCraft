#include "StdAfx.h"
#include "MultiTexture.h"
#include "Device.h"

HRESULT CMultiTexture::InsertTexture(const TCHAR* pPath		//.../Tile%d
									 , const TCHAR* pStateKey /*= L""*/
									 , const int& iCount /*= 0*/)
{
	TCHAR szPath[MAX_STR] = L"";

	vector<TEX_INFO*>	vecTexture;

	for(int i = 0; i < iCount; ++i)
	{
		//swprintf_s()
		wsprintf(szPath, pPath, i);

		TEX_INFO* pTexInfo = new TEX_INFO;
		ZeroMemory(pTexInfo, sizeof(TEX_INFO));

		if(FAILED( D3DXGetImageInfoFromFile(szPath, &pTexInfo->ImageInfo) ))
		{
			MessageBox(g_hWnd, pPath, L"이미지 정보 실패", MB_OK);
			return E_FAIL;
		}

		//얻어온 이미지 정보로 Texture를 만들어 준다.
		if( FAILED(D3DXCreateTextureFromFileEx(
			Device->GetDevice()
			,szPath
			,pTexInfo->ImageInfo.Width
			,pTexInfo->ImageInfo.Height
			,pTexInfo->ImageInfo.MipLevels
			,0		/*이미지를 읽는 방식 -> 0*/
			,pTexInfo->ImageInfo.Format
			,D3DPOOL_MANAGED
			/*
			D3DPOOL_DEFAULT : 그래픽 카드 자원이 없다면 출력 X.
			D3DPOOL_MANAGED : 그래픽 카드 자원이 모두 사용하고 있다면 시스템 메모리를 사용하는 옵션.
			*/
			,D3DX_DEFAULT		//테두리 표현
			,D3DX_DEFAULT		//확대, 축소할때 어찌표현할.
			,NULL		//제거할 이미지의 색상.
			,&pTexInfo->ImageInfo
			,NULL  //8비트 이하의 이미지 처리방식
			,&pTexInfo->pTexture
			)) )
		{
			MessageBox(g_hWnd, szPath, L"단일 이미지 생성 실패", MB_OK);
			return E_FAIL;
		}

		vecTexture.push_back(pTexInfo);
	}

	TCHAR* pKey = new TCHAR[MAX_STR];

	lstrcpy(pKey, pStateKey);

	m_mapMultiTexture.insert( make_pair(pKey, vecTexture) );

	return S_OK;
}

const TEX_INFO* CMultiTexture::GetTexture(const TCHAR* pKey /*= L"" */,const int& iCount /*= 0*/)
{
	map<const TCHAR*, vector<TEX_INFO*>>::iterator iter;

	iter = find_if(m_mapMultiTexture.begin(), m_mapMultiTexture.end(), STRING_CMP(pKey));

	if(iter == m_mapMultiTexture.end())
		return NULL;

	return iter->second[iCount];
}

void CMultiTexture::Release(void)
{
	map<const TCHAR*, vector<TEX_INFO*>>::iterator iter;
	map<const TCHAR*, vector<TEX_INFO*>>::iterator iter_end;

	iter = m_mapMultiTexture.begin();
	iter_end = m_mapMultiTexture.end();

	for(iter; iter != iter_end; ++iter)
	{
		delete [] iter->first;

		for(size_t i = 0; i < iter->second.size(); ++i)
		{
			iter->second[i]->pTexture->Release();
			delete iter->second[i];
		}
		iter->second.clear();
	}
	m_mapMultiTexture.clear();
}

CMultiTexture::CMultiTexture(void)
{
}

CMultiTexture::~CMultiTexture(void)
{
	Release();
}
