#include "stdafx.h"
#include "Pylon.h"
#include "ObjMgr.h"
#include "Device.h"
#include "TextureMgr.h"
#include "BackGround.h"
#include "Mouse.h"
#include "Effect.h"
#include "Factory.h"

CPylon::CPylon()
{
}


CPylon::~CPylon()
{
	Release();
}


HRESULT CPylon::Initialize(void)
{
	list<BUILD*> BuildList = ObjMgr->GetBuildList();

	list<BUILD*>::iterator iter = BuildList.begin();
	list<BUILD*>::iterator iter_end = BuildList.end();

	lstrcpy(m_tInfo.szObjKey, L"Building");
	lstrcpy(m_tInfo.szStateKey, L"Pylon");

	for (; iter != iter_end; ++iter)
	{
		if (!lstrcmp((*iter)->szName, m_tInfo.szStateKey))
		{
			m_tBuildInfo = (*(*iter));
			break;
		}
	}

	m_tInfo.vSize.x = S_FLOAT_CAST(m_tBuildInfo.iSizeX * TILECX) + 40;
	m_tInfo.vSize.y = S_FLOAT_CAST(m_tBuildInfo.iSizeY * TILECY);
	m_eRenderType = RENDER_GROUND;
	m_eBuildCreateType = BUILD_CREATE_NOMAL;
	m_eBuildType = BUILD_PYLON;

	SettingTile();
	m_iView = 10;
	return S_OK;
}

int CPylon::Update(void)
{
	if (m_dwState == UNIT_DEAD)
		m_bDeadCheck = true;

	if (m_bDeadCheck)
	{
		SoundMgr->PlaySound(L"explo4.wav", CHANNEL_EFFECT_CREATE, 0.9f, m_tInfo.vPos);
		ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_BUILD_DEAD), OBJ_EFFECT);
		SettingTile();
		return 1;
	}
	return 0;
}

void CPylon::Render(void)
{
	D3DXMATRIX matScale, matTrans;

	if (m_bSelect)
	{
		CObjectRenderMgr::SelectBuildUIRender(this, 4, 20.f, 4, 60.f);

		list<CGameObject*> pBuildList = ObjMgr->GetList()[OBJ_BUILD];

		list<CGameObject*>::iterator iter = pBuildList.begin();
		list<CGameObject*>::iterator iter_end = pBuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->GetBuildInfo().szName, L"Pylon"))
			{
				const TEX_INFO* pBuildtexture = TextureMgr->GetTexture(L"Building"
					, L"Pylon", 1);

				D3DXMatrixTranslation(&matTrans
					, (*iter)->GetPos().x - m_vScroll.x
					, (*iter)->GetPos().y - m_vScroll.y
					, 0.f);

				float fX = pBuildtexture->ImageInfo.Width / 2.f;
				float fY = pBuildtexture->ImageInfo.Height / 2.f;

				Device->GetSprite()->SetTransform(&matTrans);
				Device->GetSprite()->Draw(pBuildtexture->pTexture
					, NULL
					, &D3DXVECTOR3(fX, fY, 0.f)
					, NULL
					, D3DCOLOR_ARGB(100, 255, 255, 255));
			}
		}
	}
	const TEX_INFO* pBuildtexture = TextureMgr->GetTexture(L"Shad"
		, L"Pylon", 0);

	float fX = pBuildtexture->ImageInfo.Width / 2.f;
	float fY = pBuildtexture->ImageInfo.Height / 2.f;

	D3DXMatrixTranslation(&matTrans
		, m_tInfo.vPos.x - m_vScroll.x
		, m_tInfo.vPos.y - m_vScroll.y
		, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pBuildtexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(230, 255, 255, 255));

	pBuildtexture = TextureMgr->GetTexture(L"Building"
		, L"Pylon", 0);

	D3DXMatrixTranslation(&matTrans
		, m_tInfo.vPos.x - m_vScroll.x
		, m_tInfo.vPos.y - m_vScroll.y
		, 0.f);

	fX = pBuildtexture->ImageInfo.Width / 2.f;
	fY = pBuildtexture->ImageInfo.Height / 2.f;

	Device->GetSprite()->SetTransform(&matTrans);
	if (m_eTeamType == TEAM_PLAYER)
	{
		Device->GetSprite()->Draw(pBuildtexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
	else
	{
		Device->GetSprite()->Draw(pBuildtexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 160, 160));
	}

	CGameObject* pMouse = ObjMgr->GetList()[OBJ_MOUSE].front();

	if (D_CAST(CMouse, pMouse)->GetBuildClick())
	{
		list<CGameObject*> pBuildList = ObjMgr->GetList()[OBJ_BUILD];

		list<CGameObject*>::iterator iter = pBuildList.begin();
		list<CGameObject*>::iterator iter_end = pBuildList.end();

		for (; iter != iter_end; ++iter)
		{
			if (!lstrcmp((*iter)->GetBuildInfo().szName, L"Pylon") &&
				(*iter)->GetBuildCreateType() != BUILD_CREATE_CREATE)
			{
				const TEX_INFO* pBuildtexture = TextureMgr->GetTexture(L"Building"
					, L"Pylon", 1);

				D3DXMatrixTranslation(&matTrans
					, (*iter)->GetPos().x - m_vScroll.x
					, (*iter)->GetPos().y - m_vScroll.y
					, 0.f);

				fX = pBuildtexture->ImageInfo.Width / 2.f;
				fY = pBuildtexture->ImageInfo.Height / 2.f;

				Device->GetSprite()->SetTransform(&matTrans);
				Device->GetSprite()->Draw(pBuildtexture->pTexture
					, NULL
					, &D3DXVECTOR3(fX, fY, 0.f)
					, NULL
					, D3DCOLOR_ARGB(100, 255, 255, 255));
			}
		}
	}
}

void CPylon::Release(void)
{
}

void CPylon::CheckOneClick(void)
{
}

void CPylon::CreateUnit(bool Check)
{
}

void CPylon::SettingTile(void)
{
	int iX = S_INT_CAST(m_tInfo.vPos.x / TILECX);
	int iY = S_INT_CAST(m_tInfo.vPos.y / TILECY);

	int iIndex = iX + (TILEEX * iY);

	if (iIndex < 0 || iIndex >= TILEEX * TILEEY)
		return;

	CGameObject* pObj = ObjMgr->GetList()[OBJ_BACKGROUND].front();

	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - 1);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX - 1);

	int iIndex2 = 0;
	for (int i = -8; i < 7; ++i)
	{
		for (int j = -5; j < 5; ++j)
		{
			iIndex2 = iIndex + i + j * TILEEX;

			if (iIndex2 < 0 || iIndex2 >= TILEEX * TILEEY)
				continue;

			if(!m_bDeadCheck)
				D_CAST(CBackGround, pObj)->SetTileDrawIDCreate(iIndex2);
			else
				D_CAST(CBackGround, pObj)->SetTIleDrawIDDelete(iIndex2);
		}
	}
	
}