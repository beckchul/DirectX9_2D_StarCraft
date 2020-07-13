#include "StdAfx.h"
#include "SingleTexture.h"
#include "Device.h"

HRESULT CSingleTexture::InsertTexture(const TCHAR* pPath, const TCHAR* pStateKey /*= L""*/, const int& iCount /*= 0*/)
{
	m_pTexInfo = new TEX_INFO;
	ZeroMemory(m_pTexInfo, sizeof(TEX_INFO));

	//�̹����� ������ ������ �Լ�.
	if(FAILED( D3DXGetImageInfoFromFile(pPath, &m_pTexInfo->ImageInfo) ))
	{
		MessageBox(g_hWnd, pPath, L"�̹��� ���� ����", MB_OK);
		return E_FAIL;
	}

	//���� �̹��� ������ Texture�� ����� �ش�.
	if( FAILED(D3DXCreateTextureFromFileEx(
		Device->GetDevice()
		,pPath
		,m_pTexInfo->ImageInfo.Width
		,m_pTexInfo->ImageInfo.Height
		,m_pTexInfo->ImageInfo.MipLevels
		,0		/*�̹����� �д� ��� -> 0*/
		,m_pTexInfo->ImageInfo.Format
		,D3DPOOL_MANAGED
		/*
			D3DPOOL_DEFAULT : �׷��� ī�� �ڿ��� ���ٸ� ��� X.
			D3DPOOL_MANAGED : �׷��� ī�� �ڿ��� ��� ����ϰ� �ִٸ� �ý��� �޸𸮸� ����ϴ� �ɼ�.
		*/
		,D3DX_DEFAULT		//�׵θ� ǥ��
		,D3DX_DEFAULT		//Ȯ��, ����Ҷ� ����ǥ����.
		,NULL		//������ �̹����� ����.
		,&m_pTexInfo->ImageInfo
		,NULL  //8��Ʈ ������ �̹��� ó�����
		,&m_pTexInfo->pTexture
		)) )
	{
		MessageBox(g_hWnd, pPath, L"���� �̹��� ���� ����", MB_OK);
		return E_FAIL;
	}

	return S_OK;
}

const TEX_INFO* CSingleTexture::GetTexture(const TCHAR* pKey /*= L"" */,const int& iCount /*= 0*/)
{
	return m_pTexInfo;
}

void CSingleTexture::Release(void)
{
	if(m_pTexInfo != NULL)
	{
		m_pTexInfo->pTexture->Release();
		Safe_Delete(m_pTexInfo);
	}
}

CSingleTexture::CSingleTexture(void)
:m_pTexInfo(NULL)
{
}

CSingleTexture::~CSingleTexture(void)
{
	Release();
}
