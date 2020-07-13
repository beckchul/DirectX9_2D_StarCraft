#include "stdafx.h"
#include "Fog.h"
#include "Device.h"
#include "TextureMgr.h"
#include "ObjMgr.h"
#include "TimeMgr.h"
#include "Interpace.h"

CFog::CFog()
{
}

CFog::~CFog()
{
	Release();
}

HRESULT CFog::Initialize(void)
{
	//m_tInfo.vPos = D3DXVECTOR3(0.f, 0.f, 0.f);
	//m_iAlphCount = 0;

	int iIndex = 0;
	float fX = 0.f, fY = 0.f;
	for (int i = 0; i < 128; ++i)
	{
		for (int j = 0; j < 128; ++j)
		{
			iIndex = i + j * TILEEX;

			if (iIndex < 0 || iIndex >= TILEEX * TILEEY)
				continue;

			PTILE pTile = new TILE;

			fX = S_FLOAT_CAST(TILECX * j + TILECX / 2.f);
			fY = S_FLOAT_CAST(TILECY * i + TILECY / 2.f);

			pTile->vPos = D3DXVECTOR3(fX, fY, 0.f);
			pTile->vSize = D3DXVECTOR3(8.f, 8.f, 0.f);
			pTile->byOption = 0;
			m_vecTile.push_back(pTile);
		}
	}
	
	m_eRenderType = RENDER_FOG;
	return S_OK;
}

int CFog::Update(void)
{
	m_fStateCount += GET_TIME * 2.f;
	if (m_fStateCount >= 1.f)
	{
		m_fStateCount = 0.f;
		list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];
		int iX = 0, iY = 0, iX2 = 0, iY2 = 0, iIndex = 0, iIndex2 = 0;


		for(size_t i = 0; i < m_vecTile.size(); ++i)
		{ 
			if (m_vecTile[i]->byOption == 2)
				m_vecTile[i]->byOption = 1;
		}

		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if ((*iter)->GetTeamType() != TEAM_PLAYER)
				continue;

			iX = S_INT_CAST((*iter)->GetPos().x - (*iter)->GetView() * TILECX) / TILECX;
			iY = S_INT_CAST((*iter)->GetPos().y - (*iter)->GetView() * TILECX) / TILECX;

			iX2 = S_INT_CAST((*iter)->GetPos().x + (*iter)->GetView() * TILECX) / TILECX;
			iY2 = S_INT_CAST((*iter)->GetPos().y + (*iter)->GetView() * TILECX) / TILECX;


			for (int i = iX; i < iX2; ++i)
			{
				if (i >= TILEEX)
					continue;

				for (int j = iY; j < iY2; ++j)
				{
					if (j >= TILEEY)
						continue;

					if ((i == iX && j == iY) ||
						(i == iX2 - 1 && j == iY) ||
						(i == iX && j == iY2 - 1) ||
						(i == iX2 - 1 && j == iY2 - 1) ||
						(i == iX + 1 && j == iY) ||
						(i == iX2 - 2 && j == iY) ||
						(i == iX + 1&& j == iY2 - 1) ||
						(i == iX2 - 2 && j == iY2 - 1) ||
						(i == iX && j == iY + 1) ||
						(i == iX2 - 1 && j == iY + 1) ||
						(i == iX && j == iY2 - 2) ||
						(i == iX2 - 1 && j == iY2 - 2))
						continue;

					iIndex2 = iIndex + i + j * TILEEX;

					if (iIndex2 < 0 || iIndex2 >= TILEEX * TILEEY)
						continue;

					m_vecTile[iIndex2]->byOption = 2;
				}
			}
		}

		pUnitList = ObjMgr->GetList()[OBJ_BUILD];


		for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
		{
			if ((*iter)->GetTeamType() != TEAM_PLAYER)
				continue;

			iX = S_INT_CAST((*iter)->GetPos().x - (*iter)->GetView() * TILECX) / TILECX;
			iY = S_INT_CAST((*iter)->GetPos().y - (*iter)->GetView() * TILECX) / TILECX;

			iX2 = S_INT_CAST((*iter)->GetPos().x + (*iter)->GetView() * TILECX) / TILECX;
			iY2 = S_INT_CAST((*iter)->GetPos().y + (*iter)->GetView() * TILECX) / TILECX;


			for (int i = iX; i < iX2; ++i)
			{
				if (i >= TILEEX)
					continue;

				for (int j = iY; j < iY2; ++j)
				{
					if (j >= TILEEY)
						continue;

					if ((i == iX && j == iY) ||
						(i == iX2 - 1 && j == iY) ||
						(i == iX && j == iY2 - 1) ||
						(i == iX2 - 1 && j == iY2 - 1) ||
						(i == iX + 1 && j == iY) ||
						(i == iX2 - 2 && j == iY) ||
						(i == iX + 1 && j == iY2 - 1) ||
						(i == iX2 - 2 && j == iY2 - 1) ||
						(i == iX && j == iY + 1) ||
						(i == iX2 - 1 && j == iY + 1) ||
						(i == iX && j == iY2 - 2) ||
						(i == iX2 - 1 && j == iY2 - 2))
						continue;

					iIndex2 = iIndex + i + j * TILEEX;

					if (iIndex2 < 0 || iIndex2 >= TILEEX * TILEEY)
						continue;

					m_vecTile[iIndex2]->byOption = 2;
				}
			}
		}
	}
	return 0;
}

void CFog::Render(void)
{
	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();

	if (D_CAST(CInterpace, pInterpace)->GetMapLook())
		return;

	D3DXMATRIX matTrans;
	const TEX_INFO* pFogtexture = pFogtexture = TextureMgr->GetTexture(L"Map", L"Main", 2);
	float fX = pFogtexture->ImageInfo.Width / 2.f;
	float fY = pFogtexture->ImageInfo.Height / 2.f;

	int iCullX = int(m_vScroll.x / TILECX);
	int iCullY = int(m_vScroll.y / TILECY);

	int iCullEndX = (WINCX / TILECX) + 2;
	int iCullEndY = (WINCY / TILECY) + 2;

	int iIndex = 0;
	for (int i = iCullX; i < iCullX + iCullEndX; ++i)
	{
		for (int j = iCullY; j < iCullY + iCullEndY; ++j)
		{
			iIndex = i + (TILEEX * j);

			if (iIndex < 0 || TILEEX * TILEEY <= iIndex)
				continue;

			D3DXMatrixTranslation(&matTrans
				, m_vecTile[iIndex]->vPos.x - m_vScroll.x
				, m_vecTile[iIndex]->vPos.y - m_vScroll.y
				, 0.f);

			Device->GetSprite()->SetTransform(&matTrans);

			if (m_vecTile[iIndex]->byOption == 0)
			{
				Device->GetSprite()->Draw(pFogtexture->pTexture
					, NULL
					, &D3DXVECTOR3(fX, fY, 0.f)
					, NULL
					, D3DCOLOR_ARGB(255, 255, 255, 255));
			}
			else if (m_vecTile[iIndex]->byOption == 1)
			{
				Device->GetSprite()->Draw(pFogtexture->pTexture
					, NULL
					, &D3DXVECTOR3(fX, fY, 0.f)
					, NULL
					, D3DCOLOR_ARGB(150, 255, 255, 255));
			}
		}
	}	
}

void CFog::Release(void)
{
}





