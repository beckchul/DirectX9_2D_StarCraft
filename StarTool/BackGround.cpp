#include "StdAfx.h"
#include "BackGround.h"
#include "StarToolView.h"

CBackGround::CBackGround(void)
{
}

CBackGround::~CBackGround(void)
{
	Release();
}

HRESULT CBackGround::Initialize( void )
{
	float fX = 0.f, fY = 0.f;

	for(int i = 0; i < TILEEY; ++i)
	{
		for(int j = 0; j < TILEEX; ++j)
		{
			PTILE pTile = new TILE;

			fX = S_FLOAT_CAST(TILECX * j + TILECX / 2.f);
			fY = S_FLOAT_CAST(TILECY * i + TILECY / 2.f);

			pTile->vPos	    = D3DXVECTOR3(fX, fY, 0.f);
			pTile->vSize    = D3DXVECTOR3(S_FLOAT_CAST(TILECX), S_FLOAT_CAST(TILECY), 0.f);
			pTile->byDrawID = 0;
			pTile->byOption = 0;

			m_vecTile.push_back(pTile);
		}
	}
	return S_OK;
}

int CBackGround::Update( void )
{
	return 0;
}

void CBackGround::Render( void )
{
	
	D3DXMATRIX matTrans;

	const TEX_INFO* pMapTexture = TextureMgr->GetTexture(L"Map");

	D3DXMatrixTranslation( &matTrans
		, pMapTexture->ImageInfo.Width / 2.f - m_pView->GetScrollPos(0)
		, pMapTexture->ImageInfo.Height / 2.f - m_pView->GetScrollPos(1)
		, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pMapTexture->pTexture
		,NULL
		,&D3DXVECTOR3(MAPCX / 2.f, MAPCY / 2.f, 0.f)
		,NULL
		,D3DCOLOR_ARGB(255, 255, 255, 255));



	int iIndex = 0;
	TCHAR szIndex[MID_STR] = L"";
	for(int i = 0; i < TILEEY; ++i)
	{
		for(int j = 0; j < TILEEX; ++j)
		{
			iIndex = i * TILEEX + j;

			if(iIndex < 0 || iIndex >= TILEEX * TILEEY)
				break;

			const TEX_INFO* pMapTexture = TextureMgr->GetTexture(L"Tile", L"Back", m_vecTile[iIndex]->byOption);

			D3DXMatrixTranslation( &matTrans
				, m_vecTile[iIndex]->vPos.x - m_pView->GetScrollPos(0)
				, m_vecTile[iIndex]->vPos.y - m_pView->GetScrollPos(1)
				, 0.f);

			Device->GetSprite()->SetTransform(&matTrans);
			Device->GetSprite()->Draw(pMapTexture->pTexture
				,NULL
				,&D3DXVECTOR3(TILECX / 2.f, TILECY / 2.f, 0.f)
				,NULL
				,D3DCOLOR_ARGB(255, 255, 255, 255));

			if(m_vecTile[iIndex]->byOption != 0 && m_vecTile[iIndex]->byOption != 4)
			{
				D3DXMatrixTranslation( &matTrans
					, m_vecTile[iIndex]->vPos.x - m_pView->GetScrollPos(0) - 13
					, m_vecTile[iIndex]->vPos.y - m_pView->GetScrollPos(1) - 13
					, 0.f);
				Device->GetSprite()->SetTransform(&matTrans);

				wsprintf(szIndex, L"%d", iIndex);

				Device->GetFont()->DrawTextW(Device->GetSprite(), szIndex
					,lstrlen(szIndex), NULL, NULL, D3DCOLOR_ARGB(255, 155, 255, 32));
			}
		}
	}
}

void CBackGround::Release( void )
{
	for(size_t i = 0; i < m_vecTile.size(); ++i)
		SAFE_DELETE(m_vecTile[i]);

	m_vecTile.clear();
}

void CBackGround::TilePicking( D3DXVECTOR3 _vPos, int _iOption )
{
	int iX = S_INT_CAST(_vPos.x / TILECX);
	int iY = S_INT_CAST(_vPos.y / TILECY);

	int iIndex = iX + (TILEEX * iY);

	m_vecTile[iIndex]->byOption = _iOption;
}

void CBackGround::MiniView(void)
{
	D3DXMATRIX matTrans, matScale;

	const TEX_INFO* pMapTexture = TextureMgr->GetTexture(L"Map");

	D3DXMatrixScaling(&matScale, 0.1568f, 0.12f, 0.f);
	D3DXMatrixTranslation(&matTrans
		, pMapTexture->ImageInfo.Width / 12.8f
		, pMapTexture->ImageInfo.Height / 17.f
		, 0.f);

	matScale *= matTrans;

	Device->GetSprite()->SetTransform(&matScale);
	Device->GetSprite()->Draw(pMapTexture->pTexture
		, NULL
		, &D3DXVECTOR3(MAPCX / 2.f, MAPCY / 2.f, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));
}
