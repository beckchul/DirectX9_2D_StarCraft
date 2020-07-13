#include "stdafx.h"
#include "ObjectRenderMgr.h"
#include "GameObject.h"
#include "TextureMgr.h"
#include "Device.h"

void CObjectRenderMgr::SelectUnitUIRender(CGameObject * pObject1, int _iSelectNumber
	, float _fSelectPos, int _iBarNumber, float _fBarPos)
{
	if (pObject1->GetState() == UNIT_DEAD)
		return;

	const TEX_INFO* pSelecttexture = NULL;
	D3DXMATRIX matScale, matTrans;
	
	switch (pObject1->GetTeamType())
	{
	case TEAM_PLAYER:
		pSelecttexture = TextureMgr->GetTexture(L"Select"
			, L"Ally", _iSelectNumber);
		break;
	case TEAM_ENEMY:
		pSelecttexture = TextureMgr->GetTexture(L"Select"
			, L"Enemy", _iSelectNumber);
		break;
	}

	float fX = pSelecttexture->ImageInfo.Width / 2.f;
	float fY = pSelecttexture->ImageInfo.Height / 2.f;

	D3DXMatrixTranslation(&matTrans
		, pObject1->GetPos().x - pObject1->GetScrollPos().x
		, pObject1->GetPos().y - pObject1->GetScrollPos().y + _fSelectPos
		, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pSelecttexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));

	float fPercent = S_FLOAT_CAST(pObject1->GetUnitInfo().iHp) / S_FLOAT_CAST(pObject1->GetUnitInfo().iHpMax) * 100.f;
	int iR = 0, iG = 0, iB = 0;
	if (S_INT_CAST(fPercent) <= 33)
	{
		iR = 224, iG = 44, iB = 44;
	}
	else if (S_INT_CAST(fPercent) <= 66)
	{
		iR = 252, iG = 252, iB = 56;
	}
	else
	{
		iR = 36, iG = 152, iB = 36;
	}

	// Hp Frame
	pSelecttexture = TextureMgr->GetTexture(L"Bar"
		, L"HPFrame", _iBarNumber);

	
	D3DXMatrixTranslation(&matTrans
		, pObject1->GetPos().x - pObject1->GetScrollPos().x
		, pObject1->GetPos().y - pObject1->GetScrollPos().y + _fBarPos
		, 0.f);

	fX = pSelecttexture->ImageInfo.Width / 2.f;
	fY = pSelecttexture->ImageInfo.Height / 2.f;

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pSelecttexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));

	// Hp Bar	
	pSelecttexture = TextureMgr->GetTexture(L"Bar"
		, L"HPBar", _iBarNumber);

	fX = pSelecttexture->ImageInfo.Width / 2.f;
	fY = pSelecttexture->ImageInfo.Height / 2.f;

	RECT rc = { NULL
		, NULL
		, S_LONG_CAST(fX * 2 * S_FLOAT_CAST(pObject1->GetUnitInfo().iHp) / S_FLOAT_CAST(pObject1->GetUnitInfo().iHpMax))
		, S_LONG_CAST(fY * 2) };

	Device->GetSprite()->Draw(pSelecttexture->pTexture
		, &rc
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, iR, iG, iB));

	// Shild Bar
	pSelecttexture = TextureMgr->GetTexture(L"Bar"
		, L"ShieldBar", _iBarNumber);

	rc = { NULL
		, NULL
		, S_LONG_CAST(fX * 2 * S_FLOAT_CAST(pObject1->GetUnitInfo().iShield) / S_FLOAT_CAST(pObject1->GetUnitInfo().iShieldMax))
		, S_LONG_CAST(fY * 2) };

	Device->GetSprite()->Draw(pSelecttexture->pTexture
		, &rc
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));
}

void CObjectRenderMgr::SelectBuildUIRender(CGameObject * pObject1, int _iSelectNumber, float _fSelectPos, int _iBarNumber, float _fBarPos)
{
	const TEX_INFO* pSelecttexture = NULL;
	D3DXMATRIX matScale, matTrans;

	switch (pObject1->GetTeamType())
	{
	case TEAM_PLAYER:
		pSelecttexture = TextureMgr->GetTexture(L"Select"
			, L"Ally", _iSelectNumber);
		break;
	case TEAM_ENEMY:
		pSelecttexture = TextureMgr->GetTexture(L"Select"
			, L"Enemy", _iSelectNumber);
		break;
	}

	float fX = pSelecttexture->ImageInfo.Width / 2.f;
	float fY = pSelecttexture->ImageInfo.Height / 2.f;

	D3DXMatrixTranslation(&matTrans
		, pObject1->GetPos().x - pObject1->GetScrollPos().x
		, pObject1->GetPos().y - pObject1->GetScrollPos().y + _fSelectPos
		, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pSelecttexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));

	float fPercent = S_FLOAT_CAST(pObject1->GetBuildInfo().iHp) / S_FLOAT_CAST(pObject1->GetBuildInfo().iHpMax) * 100.f;
	int iR = 0, iG = 0, iB = 0;
	if (S_INT_CAST(fPercent) <= 33)
	{
		iR = 224, iG = 44, iB = 44;
	}
	else if (S_INT_CAST(fPercent) <= 66)
	{
		iR = 252, iG = 252, iB = 56;
	}
	else
	{
		iR = 36, iG = 152, iB = 36;
	}

	// Hp Frame
	pSelecttexture = TextureMgr->GetTexture(L"Bar"
		, L"HPFrame", _iBarNumber);


	D3DXMatrixTranslation(&matTrans
		, pObject1->GetPos().x - pObject1->GetScrollPos().x
		, pObject1->GetPos().y - pObject1->GetScrollPos().y + _fBarPos
		, 0.f);

	fX = pSelecttexture->ImageInfo.Width / 2.f;
	fY = pSelecttexture->ImageInfo.Height / 2.f;

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pSelecttexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));

	// Hp Bar	
	pSelecttexture = TextureMgr->GetTexture(L"Bar"
		, L"HPBar", _iBarNumber);

	fX = pSelecttexture->ImageInfo.Width / 2.f;
	fY = pSelecttexture->ImageInfo.Height / 2.f;

	RECT rc = { NULL
		, NULL
		, S_LONG_CAST(fX * 2 * S_FLOAT_CAST(pObject1->GetBuildInfo().iHp) / S_FLOAT_CAST(pObject1->GetBuildInfo().iHpMax))
		, S_LONG_CAST(fY * 2) };

	Device->GetSprite()->Draw(pSelecttexture->pTexture
		, &rc
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, iR, iG, iB));

	// Shild Bar
	pSelecttexture = TextureMgr->GetTexture(L"Bar"
		, L"ShieldBar", _iBarNumber);

	rc = { NULL
		, NULL
		, S_LONG_CAST(fX * 2 * S_FLOAT_CAST(pObject1->GetBuildInfo().iShield) / S_FLOAT_CAST(pObject1->GetBuildInfo().iShieldMax))
		, S_LONG_CAST(fY * 2) };

	Device->GetSprite()->Draw(pSelecttexture->pTexture
		, &rc
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));
}

CObjectRenderMgr::CObjectRenderMgr()
{
}


CObjectRenderMgr::~CObjectRenderMgr()
{
}
