#include "stdafx.h"
#include "MapResource.h"
#include "TextureMgr.h"
#include "Device.h"
#include "TimeMgr.h"

CMapResource::CMapResource()
{
}


CMapResource::~CMapResource()
{
	Release();
}

HRESULT CMapResource::Initialize(void)
{
	m_tFrame = FRAME(0.f, 8.f, 4.f);
	m_iDigUp = 0;
	return S_OK;
}

int CMapResource::Update(void)
{
	if (m_dwPreState == OBJECT_RENDER_TRUE)
	{
		m_fFrameTime += GET_TIME * m_tFrame.fCount;

		if (m_fFrameTime >= 1.f)
		{
			m_fFrameTime = 0.f;
			++m_iFrameCount;
		}

		if (m_tFrame.fFrame + m_iFrameCount > m_tFrame.fMax)
		{
			m_iFrameCount = 0;
			m_dwPreState = OBJECT_RENDER_FALSE;
		}
	}
	
	return 0;
}

void CMapResource::Render(void)
{
	if (m_iFrameCount % 2 == 1 && m_dwPreState == OBJECT_RENDER_TRUE)
	{
		D3DXMATRIX matTrans;

		const TEX_INFO* pEffectture = TextureMgr->GetTexture(L"Select", L"Nertual", 0);

		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y + 10.f
			, 0.f);

		float fX = pEffectture->ImageInfo.Width / 2.f;
		float fY = pEffectture->ImageInfo.Height / 2.f;


		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pEffectture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
}

void CMapResource::Release(void)
{
}