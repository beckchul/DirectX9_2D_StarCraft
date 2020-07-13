#include "stdafx.h"
#include "MiniResource.h"
#include "TextureMgr.h"
#include "Device.h"
#include "Probe.h"
#include "Interpace.h"
#include "ObjMgr.h"

CMiniResource::CMiniResource()
{
}


CMiniResource::~CMiniResource()
{
	Release();
}

HRESULT CMiniResource::Initialize(void)
{
	m_tFrame.fFrame = 0;
	return S_OK;
}

int CMiniResource::Update(void)
{
	if (m_pTarget == NULL)
		return 1;

	m_fAngle = m_pTarget->GetAngle();

	if ((m_pTarget->GetState() == UNIT_DIGUP_RESOURCE_M ||
		m_pTarget->GetState() == UNIT_DIGUP_RESOURCE_G) &&
		D_CAST(CProbe, m_pTarget)->GetProbeGetResource() == 5)
	{
		CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();

		if (eType == RESOURCE_MINERAL)
		{
			D_CAST(CInterpace, pInterpace)->PlusWorldMineral(m_pTarget->GetTeamType(), 8);
		}
		else if (eType == RESOURCE_GAS)
		{
			D_CAST(CInterpace, pInterpace)->PlusWorldGas(m_pTarget->GetTeamType(), 8);
		}

		D_CAST(CProbe, m_pTarget)->SetProbeGetRsource(0);
		return 1;
	}
		
	Frame();

	return 0;
}

void CMiniResource::Render(void)
{
	D3DXMATRIX matTrans;

	D3DXMatrixTranslation(&matTrans
		, m_tInfo.vPos.x - m_vScroll.x
		, m_tInfo.vPos.y - m_vScroll.y
		, 0.f);

	const TEX_INFO* pSelecttexture;
	if (eType == RESOURCE_MINERAL)
	{
		pSelecttexture = TextureMgr->GetTexture(L"Resource"
			, L"MiniMineral", S_INT_CAST(m_tFrame.fFrame));
	}
	else
	{
		pSelecttexture = TextureMgr->GetTexture(L"Resource"
			, L"MiniGas", S_INT_CAST(m_tFrame.fFrame));
	}

	float fX = pSelecttexture->ImageInfo.Width / 2.f;
	float fY = pSelecttexture->ImageInfo.Height / 2.f;

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pSelecttexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CMiniResource::Release(void)
{
}

void CMiniResource::Frame(void)
{
	if (m_fAngle < -168.75 || m_fAngle > 168.75)
	{
		m_tInfo.vPos = m_pTarget->GetInfo().vPos;
		m_tInfo.vPos.x -= 16.f;
		m_tFrame.fFrame = 0.f;
	}
	else if (m_fAngle >= -168.75 && m_fAngle < -146.25)
	{
		m_tInfo.vPos = m_pTarget->GetInfo().vPos;
		m_tInfo.vPos.x -= 12.f;
		m_tInfo.vPos.y -= 4.f;
		m_tFrame.fFrame = 1.f;
	}
	else if (m_fAngle >= -146.25 && m_fAngle < -123.75)
	{
		m_tInfo.vPos = m_pTarget->GetInfo().vPos;
		m_tInfo.vPos.x -= 8.f;
		m_tInfo.vPos.y -= 8.f;
		m_tFrame.fFrame = 2.f;
	}
	else if (m_fAngle >= -123.75 && m_fAngle < -101.25)
	{
		m_tInfo.vPos = m_pTarget->GetInfo().vPos;
		m_tInfo.vPos.x -= 4.f;
		m_tInfo.vPos.y -= 12.f;
		m_tFrame.fFrame = 3.f;
	}
	else if (m_fAngle >= -101.25 && m_fAngle < -78.75)
	{
		m_tInfo.vPos = m_pTarget->GetInfo().vPos;
		m_tInfo.vPos.y -= 16.f;
		m_tFrame.fFrame = 4.f;
	}
	else if (m_fAngle >= -78.75 && m_fAngle < -56.25)
	{
		m_tInfo.vPos = m_pTarget->GetInfo().vPos;
		m_tInfo.vPos.x += 4.f;
		m_tInfo.vPos.y -= 12.f;
		m_tFrame.fFrame = 5.f;
	}
	else if (m_fAngle >= -56.25 && m_fAngle < -33.75)
	{
		m_tInfo.vPos = m_pTarget->GetInfo().vPos;
		m_tInfo.vPos.x += 8.f;
		m_tInfo.vPos.y -= 8.f;
		m_tFrame.fFrame = 6.f;
	}
	else if (m_fAngle >= -33.75 && m_fAngle < -11.25)
	{
		m_tInfo.vPos = m_pTarget->GetInfo().vPos;
		m_tInfo.vPos.x += 12.f;
		m_tInfo.vPos.y -= 4.f;
		m_tFrame.fFrame = 7.f;
	}
	else if (m_fAngle >= -11.25 && m_fAngle < 11.25)
	{
		m_tInfo.vPos = m_pTarget->GetInfo().vPos;
		m_tInfo.vPos.x += 16.f;
		m_tFrame.fFrame = 8.f;
	}
	else if (m_fAngle >= 11.25 && m_fAngle < 33.75)
	{
		m_tInfo.vPos = m_pTarget->GetInfo().vPos;
		m_tInfo.vPos.x += 12.f;
		m_tInfo.vPos.y += 4.f;
		m_tFrame.fFrame = 9.f;
	}
	else if (m_fAngle >= 33.75 && m_fAngle < 56.25)
	{
		m_tInfo.vPos = m_pTarget->GetInfo().vPos;
		m_tInfo.vPos.x += 8.f;
		m_tInfo.vPos.y += 8.f;
		m_tFrame.fFrame = 10.f;
	}
	else if (m_fAngle >= 56.25 && m_fAngle < 78.75)
	{
		m_tInfo.vPos = m_pTarget->GetInfo().vPos;
		m_tInfo.vPos.x += 4.f;
		m_tInfo.vPos.y += 12.f;
		m_tFrame.fFrame = 11.f;
	}
	else if (m_fAngle >= 78.75 && m_fAngle < 101.25)
	{
		m_tInfo.vPos = m_pTarget->GetInfo().vPos;
		m_tInfo.vPos.y += 16.f;
		m_tFrame.fFrame = 12.f;
	}
	else if (m_fAngle >= 101.25 && m_fAngle < 123.75)
	{
		m_tInfo.vPos = m_pTarget->GetInfo().vPos;
		m_tInfo.vPos.x -= 4.f;
		m_tInfo.vPos.y += 12.f;
		m_tFrame.fFrame = 13.f;
	}
	else if (m_fAngle >= 123.75 && m_fAngle < 146.25)
	{
		m_tInfo.vPos = m_pTarget->GetInfo().vPos;
		m_tInfo.vPos.x -= 8.f;
		m_tInfo.vPos.y += 8.f;
		m_tFrame.fFrame = 14.f;
	}
	else if (m_fAngle >= 146.25 && m_fAngle < 168.75)
	{
		m_tInfo.vPos = m_pTarget->GetInfo().vPos;
		m_tInfo.vPos.x -= 12.f;
		m_tInfo.vPos.y += 4.f;
		m_tFrame.fFrame = 15.f;
	}
}