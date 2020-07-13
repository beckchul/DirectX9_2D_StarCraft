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
			MessageBox(g_hWnd, pPath, L"�̹��� ���� ����", MB_OK);
			return E_FAIL;
		}

		//���� �̹��� ������ Texture�� ����� �ش�.
		if( FAILED(D3DXCreateTextureFromFileEx(
			Device->GetDevice()
			,szPath
			,pTexInfo->ImageInfo.Width
			,pTexInfo->ImageInfo.Height
			,pTexInfo->ImageInfo.MipLevels
			,0		/*�̹����� �д� ��� -> 0*/
			,pTexInfo->ImageInfo.Format
			,D3DPOOL_MANAGED
			/*
			D3DPOOL_DEFAULT : �׷��� ī�� �ڿ��� ���ٸ� ��� X.
			D3DPOOL_MANAGED : �׷��� ī�� �ڿ��� ��� ����ϰ� �ִٸ� �ý��� �޸𸮸� ����ϴ� �ɼ�.
			*/
			,D3DX_DEFAULT		//�׵θ� ǥ��
			,D3DX_DEFAULT		//Ȯ��, ����Ҷ� ����ǥ����.
			,NULL		//������ �̹����� ����.
			,&pTexInfo->ImageInfo
			,NULL  //8��Ʈ ������ �̹��� ó�����
			,&pTexInfo->pTexture
			)) )
		{
			MessageBox(g_hWnd, szPath, L"���� �̹��� ���� ����", MB_OK);
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
