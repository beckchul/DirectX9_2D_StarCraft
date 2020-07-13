#include "StdAfx.h"
#include "SingleTexture.h"
#include "Device.h"

HRESULT CSingleTexture::InsertTexture(const TCHAR* pPath, const TCHAR* pStateKey /*= L""*/, const int& iCount /*= 0*/)
{
	m_pTexInfo = new TEX_INFO;
	ZeroMemory(m_pTexInfo, sizeof(TEX_INFO));

	//이미지의 정보를 얻어오는 함수.
	if(FAILED( D3DXGetImageInfoFromFile(pPath, &m_pTexInfo->ImageInfo) ))
	{
		MessageBox(g_hWnd, pPath, L"이미지 정보 실패", MB_OK);
		return E_FAIL;
	}

	//얻어온 이미지 정보로 Texture를 만들어 준다.
	if( FAILED(D3DXCreateTextureFromFileEx(
		Device->GetDevice()
		,pPath
		,m_pTexInfo->ImageInfo.Width
		,m_pTexInfo->ImageInfo.Height
		,m_pTexInfo->ImageInfo.MipLevels
		,0		/*이미지를 읽는 방식 -> 0*/
		,m_pTexInfo->ImageInfo.Format
		,D3DPOOL_MANAGED
		/*
			D3DPOOL_DEFAULT : 그래픽 카드 자원이 없다면 출력 X.
			D3DPOOL_MANAGED : 그래픽 카드 자원이 모두 사용하고 있다면 시스템 메모리를 사용하는 옵션.
		*/
		,D3DX_DEFAULT		//테두리 표현
		,D3DX_DEFAULT		//확대, 축소할때 어찌표현할.
		,NULL		//제거할 이미지의 색상.
		,&m_pTexInfo->ImageInfo
		,NULL  //8비트 이하의 이미지 처리방식
		,&m_pTexInfo->pTexture
		)) )
	{
		MessageBox(g_hWnd, pPath, L"단일 이미지 생성 실패", MB_OK);
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
