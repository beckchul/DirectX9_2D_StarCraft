#include "stdafx.h"
#include "RoboticsSupportBay.h"
#include "ObjMgr.h"
#include "Device.h"
#include "TextureMgr.h"
#include "BackGround.h"
#include "TimeMgr.h"
#include "Mouse.h"
#include "Factory.h"
#include "Effect.h"

CRoboticsSupportBay::CRoboticsSupportBay()
{
}


CRoboticsSupportBay::~CRoboticsSupportBay()
{
	Release();
}

HRESULT CRoboticsSupportBay::Initialize(void)
{
	list<BUILD*> BuildList = ObjMgr->GetBuildList();

	list<BUILD*>::iterator iter = BuildList.begin();
	list<BUILD*>::iterator iter_end = BuildList.end();

	lstrcpy(m_tInfo.szObjKey, L"Building");
	lstrcpy(m_tInfo.szStateKey, L"RoboticsSupportBay");

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
	m_tClick = FRAME(0.f, 10.f, 2.f);
	m_bKeyInput = false;
	m_bCreateUpgrade = false;
	m_eBuildCreateType = BUILD_CREATE_NOMAL;
	m_eBuildType = BUILD_ROBOTICS_SUPPORT_BAY;
	m_vStartingPoint = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_eRenderType = RENDER_GROUND;
	SettingTile();
	m_iView = 10;
	return S_OK;
}

int CRoboticsSupportBay::Update(void)
{
	if (m_dwState == UNIT_DEAD)
		m_bDeadCheck = true;

	if (m_bDeadCheck)
	{
		SoundMgr->PlaySound(L"explo4.wav", CHANNEL_EFFECT_CREATE, 0.9f, m_tInfo.vPos);
		ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_BUILD_DEAD), OBJ_EFFECT);
		ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_BUILD_DEAD2), OBJ_EFFECT);
		SettingTile();
		return 1;
	}

	Function();
	CreateUnit(true);

	return 0;
}

void CRoboticsSupportBay::Render(void)
{
	D3DXMATRIX matScale, matTrans;

	if (m_bSelect)
	{
		CObjectRenderMgr::SelectBuildUIRender(this, 6, 0.f, 6, 40.f);
	}

	const TEX_INFO* pBuildtexture = TextureMgr->GetTexture(L"Shad"
		, L"Robotics Support Bay", 0);

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
		, D3DCOLOR_ARGB(255, 255, 255, 255));

	pBuildtexture = TextureMgr->GetTexture(L"Building"
		, L"Robotics Support Bay", 0);

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
}

void CRoboticsSupportBay::Release(void)
{
}

void CRoboticsSupportBay::Function(void)
{

}

void CRoboticsSupportBay::CheckOneClick(void)
{
	D3DXMATRIX matTrans;
	float fX = 0.f, fY = 0.f;
	const TEX_INFO* pInterpacetexture = NULL;
	if (m_eBuildCreateType != BUILD_CREATE_NOMAL)
	{
		// 9��° ĭ
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Cancel", m_iClickMyInterpace[8]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 595.f + fX
			, 440.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	else
	{
		// 1��° ĭ
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"ReverAttUp", m_iClickMyInterpace[0]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 505.f + fX
			, 357.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		// 2��° ĭ
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"ReverTongUp", m_iClickMyInterpace[1]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 550.f + fX
			, 357.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		// 3��° ĭ
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"SuttleSpeedUp", m_iClickMyInterpace[2]);

		fX = pInterpacetexture->ImageInfo.Width / 2.f;
		fY = pInterpacetexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 595.f + fX
			, 357.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pInterpacetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
}

void CRoboticsSupportBay::CreateUnit(bool Check)
{
	if (Check) // ���� ���� �ð� ���
	{
		if (!m_bCreateUpgrade)
		{
			// ���׷��̵� ��� �������� // �ؼ��� ���ݸ� ����
		}
		else
		{

		}
	}
	else	// �������̽� �����ִ°�	// ���׷��̵� �ϴ³� ����
	{
		D3DXMATRIX matScale, matTrans;
		float fX = 0.f, fY = 0.f;
		const TEX_INFO* pBigWireTexture = TextureMgr->GetTexture(L"Produce"
			, L"Frame", 0);

		fX = pBigWireTexture->ImageInfo.Width / 2.f;
		fY = pBigWireTexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 230.f + fX
			, 395.f + fY
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pBigWireTexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		pBigWireTexture = TextureMgr->GetTexture(L"Produce"
			, L"Bar", 0);

		fX = pBigWireTexture->ImageInfo.Width / 2.f;
		fY = pBigWireTexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, fX + 270.f
			, fY + 418.f
			, 0.f);

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pBigWireTexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		pBigWireTexture = TextureMgr->GetTexture(L"Produce"
			, L"Bar", 1);

		fX = pBigWireTexture->ImageInfo.Width / 2.f;
		fY = pBigWireTexture->ImageInfo.Height / 2.f;

		D3DXMatrixTranslation(&matTrans
			, 270.f
			, 418.f
			, 0.f);


		float fPercent = m_tCreateTime.fFrame / m_tCreateTime.fMax;

		RECT rc = { NULL, NULL
			, S_LONG_CAST(fPercent * fX * 2), S_LONG_CAST(fY * 2) };

		if (fPercent > 1.f)
			fPercent = 1.f;

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pBigWireTexture->pTexture
			, &rc
			, NULL //&D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));
	}
}

void CRoboticsSupportBay::SettingTile(void)
{
	int iX = S_INT_CAST(m_tInfo.vPos.x / TILECX);
	int iY = S_INT_CAST(m_tInfo.vPos.y / TILECY);

	int iIndex = iX + (TILEEX * iY);

	if (iIndex < 0 || iIndex >= TILEEX * TILEEY)
		return;

	CGameObject* pObj = ObjMgr->GetList()[OBJ_BACKGROUND].front();

	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - 1);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex + 1);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX - 1);
	D_CAST(CBackGround, pObj)->SetTileBuildOption(iIndex - TILEEX + 1);
}