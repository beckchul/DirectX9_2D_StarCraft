#include "stdafx.h"
#include "DarkArchon.h"
#include "TextureMgr.h"
#include "Device.h"
#include "ObjMgr.h"
#include "TimeMgr.h"
#include "AStar.h"
#include "KeyMgr.h"
#include "Mouse.h"
#include "BackGround.h"
#include "Interpace.h"
#include "Effect.h"
#include "Factory.h"

CDarkArchon::CDarkArchon()
{
}


CDarkArchon::~CDarkArchon()
{
	Release();
}



HRESULT CDarkArchon::Initialize(void)
{
	list<UNIT*> UnitList = ObjMgr->GetUnitList();

	list<UNIT*>::iterator iter = UnitList.begin();
	list<UNIT*>::iterator iter_end = UnitList.end();

	lstrcpy(m_tInfo.szObjKey, L"DarkArchon");
	lstrcpy(m_tInfo.szStateKey, L"Create");

	for (; iter != iter_end; ++iter)
	{
		if (!lstrcmp((*iter)->szName, m_tInfo.szObjKey))
		{
			m_tUnitInfo = (*(*iter));
			break;
		}
	}
	m_tUnitInfo.eType = UNIT_GROUND;
	m_dwState = UNIT_CREATE;
	m_pAStar = new CAStar;
	m_bClickKey = true;
	m_tInfo.vSize = D3DXVECTOR3(24.f, 24.f, 0.f);
	m_tInfo.vLook = D3DXVECTOR3(1.f, 0.f, 0.f);
	m_tClick = FRAME(0.f, 10.f, 2.f);
	m_tFrame = FRAME(0.f, 10.f, 7.f);
	m_fSpeed = 150.f;
	m_fFrameTime = 0.f;
	m_iFrameCount = 0;
	m_eRenderType = RENDER_GROUND;
	m_tBodyFrame = FRAME(0.f, 9.f, 9.f);
	m_tCreateBodyFrame = FRAME(0.f, 3.f, 7.f);
	m_tAuraFrame = FRAME(0.f, 28.f, 14.f);
	m_tCreateTime = FRAME(0.f, 1.f, S_FLOAT_CAST(m_tUnitInfo.iMakeTime));
	m_bCreate = false;
	m_tMpFrame = FRAME(0.f, 1.f, 1.f);
	m_iView = 9;
	return S_OK;
}

int CDarkArchon::Update(void)
{
	if (m_dwState != UNIT_CREATE)
	{
		m_tMpFrame.fFrame += GET_TIME;
		if (m_tMpFrame.fFrame > 2.f)
		{
			m_tMpFrame.fFrame = 0;
			if (m_tUnitInfo.iMp < m_tUnitInfo.iMpMax)
			{
				++m_tUnitInfo.iMp;
			}
		}
	}
	
	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
	switch (D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_GROUND_ARMOR))
	{
	case 1:
		m_iArmor = 1;
		break;
	case 2:
		m_iArmor = 2;
		break;
	case 3:
		m_iArmor = 3;
		break;
	}

	switch (D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_SHILD))
	{
	case 1:
		m_iShield = 1;
		break;
	case 2:
		m_iShield = 2;
		break;
	case 3:
		m_iShield = 3;
		break;
	}

	if (D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_DARKARCHON_MPMAX) == 1)
	{
		m_tUnitInfo.iMpMax = 250;
	}

	m_tUnitInfo.iMp = 150;
	
	if (m_dwState == UNIT_DEAD || m_tUnitInfo.iHp <= 0)
		m_bDeadCheck = true;

	if (m_bDeadCheck)
	{
		SoundMgr->PlaySound(L"explo5.wav", CHANNEL_EFFECT_ATT2, 0.5f, m_tInfo.vPos);
		D_CAST(CInterpace, pInterpace)->EraseUnitList(this);
		ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_DARKARCHON_DEAD), OBJ_EFFECT);
		return 1;
	}
	AStarMove();
	Clooking();
	SelectKeyCheck();
	Function();
	Frame();
	return 0;
}

void CDarkArchon::Render(void)
{
	D3DXMATRIX matScale, matTrans;

	if (m_bSelect)
	{
		CObjectRenderMgr::SelectUnitUIRender(this, 4, 7.f, 4, 40.f);

		///Mp Frame
		const TEX_INFO* pSelecttexture = TextureMgr->GetTexture(L"Bar"
			, L"MPFrame", 2);

		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y + 50
			, 0.f);

		float fX = pSelecttexture->ImageInfo.Width / 2.f;
		float fY = pSelecttexture->ImageInfo.Height / 2.f;

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pSelecttexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));

		pSelecttexture = TextureMgr->GetTexture(L"Bar"
			, L"MPBar", 2);

		RECT rc = { NULL
			, NULL
			, S_LONG_CAST(fX * 2 * S_FLOAT_CAST(m_tUnitInfo.iMp) / S_FLOAT_CAST(m_tUnitInfo.iMpMax))
			, S_LONG_CAST(fY * 2) };

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pSelecttexture->pTexture
			, &rc
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255, 255, 255, 255));
	}

	const TEX_INFO* pDargoontexture = NULL;
	float fX = 0.f, fY = 0.f;

	if (m_dwState == UNIT_CREATE)
	{
		pDargoontexture = TextureMgr->GetTexture(L"Dark Archon"
			, m_tInfo.szStateKey, S_INT_CAST(m_tCreateBodyFrame.fFrame));

		D3DXMatrixScaling(&matScale, m_tInfo.vLook.x, 1.f, 0.f);
		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y
			, 0.f);

		matScale *= matTrans;

		fX = pDargoontexture->ImageInfo.Width / 2.f;
		fY = pDargoontexture->ImageInfo.Height / 2.f;
	}
	else
	{
		pDargoontexture = TextureMgr->GetTexture(L"Dark Archon"
			, L"Body", S_INT_CAST(m_tBodyFrame.fFrame));

		D3DXMatrixScaling(&matScale, m_tInfo.vLook.x, 1.f, 0.f);
		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y
			, 0.f);

		matScale *= matTrans;

		fX = pDargoontexture->ImageInfo.Width / 2.f;
		fY = pDargoontexture->ImageInfo.Height / 2.f;

		if (m_eTeamType == TEAM_PLAYER)
		{
			Device->GetSprite()->SetTransform(&matScale);
			Device->GetSprite()->Draw(pDargoontexture->pTexture
				, NULL
				, &D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame), 255, 255, 255));
		}
		else if (m_eTeamType == TEAM_ENEMY)
		{
			if (m_eAlphType == ALPH_NOMAL_CLOOCKING)
			{
				Device->GetSprite()->SetTransform(&matScale);
				Device->GetSprite()->Draw(pDargoontexture->pTexture
					, NULL
					, &D3DXVECTOR3(fX, fY, 0.f)
					, NULL
					, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame - 85), 255, 160, 160));
			}
			else
			{
				Device->GetSprite()->SetTransform(&matScale);
				Device->GetSprite()->Draw(pDargoontexture->pTexture
					, NULL
					, &D3DXVECTOR3(fX, fY, 0.f)
					, NULL
					, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame), 255, 160, 160));
			}
		}
	}

	if (m_dwState == UNIT_MOVE || m_dwState == UNIT_STAND)
	{
		pDargoontexture = TextureMgr->GetTexture(L"Dark Archon"
			, m_tInfo.szStateKey, S_INT_CAST(m_tFrame.fFrame) + S_INT_CAST(m_iFrameCount * 17));

		D3DXMatrixScaling(&matScale, m_tInfo.vLook.x, 1.f, 0.f);
		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y
			, 0.f);

		matScale *= matTrans;

		fX = pDargoontexture->ImageInfo.Width / 2.f;
		fY = pDargoontexture->ImageInfo.Height / 2.f;
	}

	if (m_eTeamType == TEAM_PLAYER)
	{
		Device->GetSprite()->SetTransform(&matScale);
		Device->GetSprite()->Draw(pDargoontexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame), 255, 255, 255));
	}
	else if (m_eTeamType == TEAM_ENEMY)
	{
		if (m_eAlphType == ALPH_NOMAL_CLOOCKING)
		{
			Device->GetSprite()->SetTransform(&matScale);
			Device->GetSprite()->Draw(pDargoontexture->pTexture
				, NULL
				, &D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame - 85), 255, 160, 160));
		}
		else
		{
			Device->GetSprite()->SetTransform(&matScale);
			Device->GetSprite()->Draw(pDargoontexture->pTexture
				, NULL
				, &D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame), 255, 160, 160));
		}
	}

	if (m_dwState != UNIT_CREATE)
	{
		pDargoontexture = TextureMgr->GetTexture(L"Dark Archon"
			, L"Aura", S_INT_CAST(m_tAuraFrame.fFrame));

		D3DXMatrixScaling(&matScale, m_tInfo.vLook.x, 1.f, 0.f);
		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y
			, 0.f);

		matScale *= matTrans;

		fX = pDargoontexture->ImageInfo.Width / 2.f;
		fY = pDargoontexture->ImageInfo.Height / 2.f;

		if (m_eTeamType == TEAM_PLAYER)
		{
			Device->GetSprite()->SetTransform(&matScale);
			Device->GetSprite()->Draw(pDargoontexture->pTexture
				, NULL
				, &D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame), 255, 255, 255));
		}
		else if (m_eTeamType == TEAM_ENEMY)
		{
			if (m_eAlphType == ALPH_NOMAL_CLOOCKING)
			{
				Device->GetSprite()->SetTransform(&matScale);
				Device->GetSprite()->Draw(pDargoontexture->pTexture
					, NULL
					, &D3DXVECTOR3(fX, fY, 0.f)
					, NULL
					, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame - 85), 255, 160, 160));
			}
			else
			{
				Device->GetSprite()->SetTransform(&matScale);
				Device->GetSprite()->Draw(pDargoontexture->pTexture
					, NULL
					, &D3DXVECTOR3(fX, fY, 0.f)
					, NULL
					, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame), 255, 160, 160));
			}
		}
	}

	if (m_dwUnitState == UNIT_STASISFIELD && m_dwState != UNIT_DEAD)
	{
		const TEX_INFO* pProbetexture = TextureMgr->GetTexture(L"Effect"
			, L"Frozen", S_INT_CAST(m_tStateCount.fFrame) + m_iStasis);

		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y
			, 0.f);

		fX = pProbetexture->ImageInfo.Width / 2.f;
		fY = pProbetexture->ImageInfo.Height / 2.f;

		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pProbetexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(80, 255, 255, 255));
	}
}

void CDarkArchon::Release(void)
{
}

void CDarkArchon::SelectKeyCheck(void)
{
	if (m_dwState == UNIT_DEAD || m_dwUnitState == UNIT_STASISFIELD)
		return;
	if(m_dwState == UNIT_CREATE)
		return;

	if (!m_bSelect)
		return;
	DWORD dwKey = GET_KEY;
	CGameObject* ptMouse = ObjMgr->GetList()[OBJ_MOUSE].front();
	D3DXVECTOR3 vMouse = D_CAST(CMouse, ptMouse)->GetMousePos();

	if (dwKey & KEY_RBUTTON && m_bClickKey)
	{
		m_bClickKey = false;

		// ¸¶¿ì½º ÀÎÅÍÆäÀÌ½º Å¬¸¯ ¹æÁö
		if (vMouse.x - m_vScroll.x >= 490 && vMouse.y - m_vScroll.y >= 340 &&
			vMouse.x - m_vScroll.x <= 640 && vMouse.y - m_vScroll.y <= 480)
			return;

		if (vMouse.x - m_vScroll.x >= 150 && vMouse.y - m_vScroll.y >= 380 &&
			vMouse.x - m_vScroll.x <= 490 && vMouse.y - m_vScroll.y <= 480)
			return;

		if (vMouse.x - m_vScroll.x >= 0 && vMouse.y - m_vScroll.y >= 320 &&
			vMouse.x - m_vScroll.x <= 150 && vMouse.y - m_vScroll.y <= 480)
			return;

		/////////

		ptMouse->SetState(MOUSE_CLICK);
		m_dwState = UNIT_MOVE;
		m_bUnitAtt = false;
		m_AttTarget = NULL;
		m_pAStar->StartAStarPos(m_tInfo.vPos, vMouse);
	}

	if (dwKey & KEY_P && m_bClickKey)
	{
		ptMouse->SetState(MOUSE_ATTACK_POINT_PATOL);
	}

	if(dwKey & KEY_LBUTTON && ptMouse->GetState() == MOUSE_ATTACK_POINT_NOMAL)
	{
		m_vPatrolEndPoint = vMouse;
		m_dwState = UNIT_MOVE;
		m_dwSubState = UNIT_MOVE;
		m_bUnitAtt = false;
		m_AttTarget = NULL;
		m_pAStar->StartAStarPos(m_tInfo.vPos, vMouse);
		ptMouse->SetState(MOUSE_CLICK);
	}

	if (dwKey & KEY_H && m_bClickKey)
	{
		m_dwState = UNIT_STAND;
		m_dwSubState = UNIT_HOLD;
		m_pAStar->AStarClear();
	}

	if (dwKey & KEY_S && m_bClickKey)
	{
		m_bUnitAtt = false;
		m_AttTarget = NULL;
		m_dwState = UNIT_STAND;
		m_dwSubState = UNIT_STAND;
		m_pAStar->AStarClear();
		for (int i = 0; i < 9; ++i)
			m_iClickMyInterpace[i] = 0;
		m_iClickMyInterpace[1] = 1;
	}

	if (dwKey & KEY_F && m_bClickKey)
	{
		if (m_tUnitInfo.iMp >= 50)
		{
			m_bUnitAtt = false;
			D_CAST(CMouse, ptMouse)->SetState(MOUSE_SKILL_POINT);
		}
		else
		{

		}
	}

	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
	if (dwKey & KEY_C && m_bClickKey &&
		1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_DARKARCHON_MINDCONTRAL))
	{
		if (m_tUnitInfo.iMp >= 150)
		{
			m_bUnitAtt = false;
			D_CAST(CMouse, ptMouse)->SetState(MOUSE_SKILL_POINT2);
		}
		else
		{

		}
	}

	if (dwKey & KEY_E && m_bClickKey &&
		1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_DARKARCHON_CONFUSION))
	{
		if (m_tUnitInfo.iMp >= 100)
		{
			m_bUnitAtt = false;
			D_CAST(CMouse, ptMouse)->SetState(MOUSE_SKILL_POINT3);
		}
		else
		{

		}
	}

	if (dwKey & KEY_LBUTTON && m_bClickKey)
	{
		if (D_CAST(CMouse, ptMouse)->GetState() == MOUSE_SKILL_POINT)
		{
			m_AttTarget = NULL;
			list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

			list<CGameObject*>::iterator iter = pUnitList.begin();
			list<CGameObject*>::iterator iter_end = pUnitList.end();

			for (; iter != iter_end; ++iter)
			{
				if (vMouse.x >= (*iter)->GetPos().x - (*iter)->GetInfo().vSize.x / 2.f &&
					vMouse.x <= (*iter)->GetPos().x + (*iter)->GetInfo().vSize.x / 2.f &&
					vMouse.y >= (*iter)->GetPos().y - (*iter)->GetInfo().vSize.y / 2.f &&
					vMouse.y <= (*iter)->GetPos().y + (*iter)->GetInfo().vSize.y / 2.f)
				{
					if((*iter) != this)
						m_AttTarget = (*iter);


					break;
				}
			}
			if (m_AttTarget != NULL)
			{
				if (m_AttTarget->GetUnitInfo().iMpMax > 0 &&
					((m_AttTarget->GetAlphType() != ALPH_CREATE_CLOOCKING &&
					m_AttTarget->GetAlphType() != ALPH_NOMAL_CLOOCKING)
						|| m_AttTarget->GetTeamType() == m_eTeamType))
				{
					m_vPatrolEndPoint = vMouse;
					m_dwSubState = UNIT_SKILL1;
					m_dwState = UNIT_MOVE;
					m_pAStar->StartAStarPos(m_tInfo.vPos, m_AttTarget->GetPos());
					ptMouse->SetState(MOUSE_CLICK);
				}
			}
			else
			{
				m_dwSubState = UNIT_STAND;
			}
		}
	}

	if (dwKey & KEY_LBUTTON && m_bClickKey)
	{
		if (D_CAST(CMouse, ptMouse)->GetState() == MOUSE_SKILL_POINT2)
		{
			m_AttTarget = NULL;
			list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

			list<CGameObject*>::iterator iter = pUnitList.begin();
			list<CGameObject*>::iterator iter_end = pUnitList.end();

			for (; iter != iter_end; ++iter)
			{
				if (vMouse.x >= (*iter)->GetPos().x - (*iter)->GetInfo().vSize.x / 2.f &&
					vMouse.x <= (*iter)->GetPos().x + (*iter)->GetInfo().vSize.x / 2.f &&
					vMouse.y >= (*iter)->GetPos().y - (*iter)->GetInfo().vSize.y / 2.f &&
					vMouse.y <= (*iter)->GetPos().y + (*iter)->GetInfo().vSize.y / 2.f)
				{
					if ((*iter) != this)
						m_AttTarget = (*iter);


					break;
				}
			}
			if (m_AttTarget != NULL)
			{
				if ((*iter)->GetTeamType() != m_eTeamType &&
					m_AttTarget->GetAlphType() != ALPH_CREATE_CLOOCKING &&
					m_AttTarget->GetAlphType() != ALPH_NOMAL_CLOOCKING)
				{
					m_vPatrolEndPoint = vMouse;
					m_dwSubState = UNIT_SKILL2;
					m_dwState = UNIT_MOVE;
					m_pAStar->StartAStarPos(m_tInfo.vPos, m_AttTarget->GetPos());
					ptMouse->SetState(MOUSE_CLICK);
				}
			}
			else
			{
				m_dwSubState = UNIT_STAND;
			}
		}
	}


	if (dwKey & KEY_LBUTTON && m_bClickKey)
	{
		if (D_CAST(CMouse, ptMouse)->GetState() == MOUSE_SKILL_POINT3)
		{
			m_vPatrolEndPoint = vMouse;
			m_dwSubState = UNIT_SKILL3;
			m_dwState = UNIT_MOVE;
			m_pAStar->StartAStarPos(m_tInfo.vPos, vMouse);
			ptMouse->SetState(MOUSE_CLICK);
		}


	}

	if (!dwKey)
	{
		m_tClick.fFrame += GET_TIME * m_tClick.fCount;

		if (m_tClick.fFrame > m_tClick.fMax)
			m_bClickKey = true;
	}
}
void CDarkArchon::CheckOneClick(void)
{
	if (m_dwState == UNIT_CREATE || m_dwUnitState == UNIT_STASISFIELD)
		return;
	D3DXMATRIX matTrans;

	// 1¹øÂ° Ä­
	const TEX_INFO* pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"Move", m_iClickMyInterpace[0]);

	float fX = pInterpacetexture->ImageInfo.Width / 2.f;
	float fY = pInterpacetexture->ImageInfo.Height / 2.f;

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

	// 2¹øÂ° Ä­
	pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"Stop", m_iClickMyInterpace[1]);

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

	// 4¹øÂ° Ä­
	pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"Patrol", m_iClickMyInterpace[3]);

	fX = pInterpacetexture->ImageInfo.Width / 2.f;
	fY = pInterpacetexture->ImageInfo.Height / 2.f;

	D3DXMatrixTranslation(&matTrans
		, 505.f + fX
		, 397.f + fY
		, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pInterpacetexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));

	// 5¹øÂ° Ä­
	pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"Hold", m_iClickMyInterpace[4]);

	fX = pInterpacetexture->ImageInfo.Width / 2.f;
	fY = pInterpacetexture->ImageInfo.Height / 2.f;

	D3DXMatrixTranslation(&matTrans
		, 550.f + fX
		, 397.f + fY
		, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pInterpacetexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));

	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();

	if (1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_DARKARCHON_MINDCONTRAL))
	{
		m_iClickMyInterpace[7] = 0;
	}
	else
	{
		m_iClickMyInterpace[7] = 2;
	}

	if (1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_DARKARCHON_CONFUSION))
	{
		m_iClickMyInterpace[8] = 0;
	}
	else
	{
		m_iClickMyInterpace[8] = 1;
	}

	// 7¹øÂ° Ä­
	pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"FeedBack", m_iClickMyInterpace[6]);

	fX = pInterpacetexture->ImageInfo.Width / 2.f;
	fY = pInterpacetexture->ImageInfo.Height / 2.f;

	D3DXMatrixTranslation(&matTrans
		, 505.f + fX
		, 440.f + fY
		, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pInterpacetexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));


	// 8¹øÂ° Ä­
	pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"MindControl", m_iClickMyInterpace[7]);

	fX = pInterpacetexture->ImageInfo.Width / 2.f;
	fY = pInterpacetexture->ImageInfo.Height / 2.f;

	D3DXMatrixTranslation(&matTrans
		, 550.f + fX
		, 440.f + fY
		, 0.f);

	Device->GetSprite()->SetTransform(&matTrans);
	Device->GetSprite()->Draw(pInterpacetexture->pTexture
		, NULL
		, &D3DXVECTOR3(fX, fY, 0.f)
		, NULL
		, D3DCOLOR_ARGB(255, 255, 255, 255));

	// 9¹øÂ° Ä­
	pInterpacetexture = TextureMgr->GetTexture(L"Button"
		, L"ConfusionUpgrade", m_iClickMyInterpace[8]);

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

void CDarkArchon::Function(void)
{
	if (m_dwState == UNIT_DEAD || m_dwUnitState == UNIT_STASISFIELD)
		return;

	CGameObject* pMouse = ObjMgr->GetList()[OBJ_MOUSE].front();

	if (m_dwSubState == UNIT_SKILL1)
	{
		if (fabs(m_tInfo.vPos.x - m_AttTarget->GetPos().x) < 220.f &&
			fabs(m_tInfo.vPos.y - m_AttTarget->GetPos().y) < 220.f)
		{
			m_dwState = UNIT_STAND;
			m_dwSubState = UNIT_STAND;
			m_pAStar->AStarClear();
			m_tUnitInfo.iMp -= 50;
			//m_vPatrolEndPoint
			ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameUnitEffectTarget(m_AttTarget, EFFECT_DARKARCHON_FEEDBACK, m_eTeamType), OBJ_EFFECT);
			SoundMgr->PlaySound(L"feedback.wav", CHANNEL_EFFECT3, 0.7f);
			CDamageMgr::DamageCalculation(m_AttTarget->GetUnitInfo().iMp, m_AttTarget);
			m_AttTarget->SetUnitMp(m_AttTarget->GetUnitInfo().iMp);
		}
	}

	if (m_dwSubState == UNIT_SKILL2 && m_AttTarget != NULL && m_AttTarget->GetUnitState() != UNIT_STASISFIELD)
	{
		if (fabs(m_tInfo.vPos.x - m_AttTarget->GetPos().x) < 220.f &&
			fabs(m_tInfo.vPos.y - m_AttTarget->GetPos().y) < 220.f)
		{
			m_dwState = UNIT_STAND;
			m_dwSubState = UNIT_STAND;
			m_pAStar->AStarClear();
			m_tUnitInfo.iMp -= 150;
			m_tUnitInfo.iShield = 0;
			//m_vPatrolEndPoint
			ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameUnitEffectTarget(m_AttTarget, EFFECT_DARKARCHON_MINDCONTROL, m_eTeamType), OBJ_EFFECT);
			SoundMgr->PlaySound(L"mind.wav", CHANNEL_EFFECT3, 0.7f);
			m_AttTarget->SetTeamType(m_eTeamType);
		}
	}

	if (m_dwSubState == UNIT_SKILL3)
	{
		if (fabs(m_tInfo.vPos.x - m_vPatrolEndPoint.x) < 200.f &&
			fabs(m_tInfo.vPos.y - m_vPatrolEndPoint.y) < 200.f)
		{
			m_tUnitInfo.iMp -= 100;
			m_dwState = UNIT_STAND;
			m_dwSubState = UNIT_STAND;
			m_pAStar->AStarClear();
			ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_vPatrolEndPoint, EFFECT_DARKARCHON_MAELSTORM), OBJ_EFFECT);
			SoundMgr->PlaySound(L"parahit.wav", CHANNEL_EFFECT3, 0.7f);
			list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

			list<CGameObject*>::iterator iter = pUnitList.begin();
			list<CGameObject*>::iterator iter_end = pUnitList.end();

			for (; iter != iter_end; ++iter)
			{
				if ((*iter)->GetPos().x >= m_vPatrolEndPoint.x - 60.f &&
					(*iter)->GetPos().x <= m_vPatrolEndPoint.x + 60.f &&
					(*iter)->GetPos().y >= m_vPatrolEndPoint.y - 60.f &&
					(*iter)->GetPos().y <= m_vPatrolEndPoint.y + 60.f)
				{
					if (!lstrcmp((*iter)->GetUnitInfo().szName, L"Probe") ||
						!lstrcmp((*iter)->GetUnitInfo().szName, L"Zealot") ||
						!lstrcmp((*iter)->GetUnitInfo().szName, L"HighTemplar") ||
						!lstrcmp((*iter)->GetUnitInfo().szName, L"DarkTemplar"))
					{
						(*iter)->SetStateTime(10);
						(*iter)->SetUnitState(UNIT_MAELSTORM);
					}
					
				}
			}
		}
	}
}

void CDarkArchon::Clooking(void)
{
	list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];
	list<CGameObject*> pBuildList = ObjMgr->GetList()[OBJ_BUILD];
	CDistanceMgr::CheckNoamlCloking(this, pUnitList);
	CDistanceMgr::CheckCloking(this, pUnitList, pBuildList);

	if ((m_eAlphType == ALPH_NOMAL_CLOOCKING || m_eAlphType == LOOK_ALPH_NOMAL_CLOOCKING) && m_tAlphCount.fFrame < m_tAlphCount.fMax)
	{
		m_tAlphCount.fFrame += m_tAlphCount.fCount * GET_TIME;

		if (m_tAlphCount.fFrame >= m_tAlphCount.fMax)
		{
			m_tAlphCount.fFrame = m_tAlphCount.fMax;
		}
	}
	else if (m_eAlphType == ALPH_NOMAL && m_tAlphCount.fFrame > 0)
	{
		m_tAlphCount.fFrame -= m_tAlphCount.fCount * GET_TIME;

		if (m_tAlphCount.fFrame <= 0)
		{
			m_tAlphCount.fFrame = 0;
		}
	}
}

void CDarkArchon::Frame(void)
{
	switch (m_dwState)
	{
	case UNIT_MOVE:
	case UNIT_STAND:
		if (m_fAngle < -168.75 || m_fAngle > 168.75)
		{
			m_tFrame = FRAME(8.f, 25.f, 178.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -168.75 && m_fAngle < -146.25)
		{
			m_tFrame = FRAME(6.f, 25.f, 176.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -146.25 && m_fAngle < -123.75)
		{
			m_tFrame = FRAME(4.f, 25.f, 174.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -123.75 && m_fAngle < -101.25)
		{
			m_tFrame = FRAME(2.f, 25.f, 172.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -101.25 && m_fAngle < -78.75)
		{
			m_tFrame = FRAME(0.f, 25.f, 170.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -78.75 && m_fAngle < -56.25)
		{
			m_tFrame = FRAME(2.f, 25.f, 172.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -56.25 && m_fAngle < -33.75)
		{
			m_tFrame = FRAME(4.f, 25.f, 174.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -33.75 && m_fAngle < -11.25)
		{
			m_tFrame = FRAME(6.f, 25.f, 176.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -11.25 && m_fAngle < 11.25)
		{
			m_tFrame = FRAME(8.f, 25.f, 178.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 11.25 && m_fAngle < 33.75)
		{
			m_tFrame = FRAME(10.f, 25.f, 180.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 33.75 && m_fAngle < 56.25)
		{
			m_tFrame = FRAME(12.f, 25.f, 182.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 56.25 && m_fAngle < 78.75)
		{
			m_tFrame = FRAME(14.f, 25.f, 184.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 78.75 && m_fAngle < 101.25)
		{
			m_tFrame = FRAME(16.f, 25.f, 186.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 101.25 && m_fAngle < 123.75)
		{
			m_tFrame = FRAME(14.f, 25.f, 184.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= 123.75 && m_fAngle < 146.25)
		{
			m_tFrame = FRAME(12.f, 25.f, 182.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= 146.25 && m_fAngle < 168.75)
		{
			m_tFrame = FRAME(10.f, 25.f, 180.f);
			m_tInfo.vLook.x = -1;
		}
		break;
	case UNIT_DEAD:
		m_tFrame = FRAME(0.f, 5.f, 14.f);
		break;
	}

	if (m_dwState == UNIT_STAND ||
		m_dwState == UNIT_MOVE)
	{
		lstrcpy(m_tInfo.szStateKey, L"Normal");
	}
	else if (m_dwState == UNIT_CREATE)
	{
		lstrcpy(m_tInfo.szStateKey, L"Create");
	}

	if (m_dwState == UNIT_CREATE)
	{
		m_tCreateTime.fFrame += GET_TIME * m_tCreateTime.fCount;

		if (m_tCreateTime.fFrame < m_tCreateTime.fMax)
		{

			m_tCreateBodyFrame.fFrame += GET_TIME * m_tCreateBodyFrame.fCount;

			if (m_tCreateBodyFrame.fFrame > m_tCreateBodyFrame.fMax)
			{
				m_tCreateBodyFrame.fFrame = 3;
			}
		}
		else
		{
			m_tCreateBodyFrame.fFrame += GET_TIME * m_tCreateBodyFrame.fCount * 4;
			m_tCreateBodyFrame.fMax = 16.f;

			if (m_tCreateBodyFrame.fFrame > m_tCreateBodyFrame.fMax)
			{
				if (m_eTeamType == TEAM_PLAYER)
				{
					ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_tInfo.vPos, EFFECT_PING), OBJ_EFFECT);
					SoundMgr->PlaySound(L"pdardy00.wav", CHANNEL_MAIN, 0.9f);
				}
				
				m_dwState = UNIT_NORMAL;
				return;
			}
		}

	}

	m_tBodyFrame.fFrame += m_tBodyFrame.fCount * GET_TIME;

	if (m_tBodyFrame.fFrame > m_tBodyFrame.fMax)
	{
		m_tBodyFrame.fFrame = 0.f;
	}

	m_tAuraFrame.fFrame += m_tAuraFrame.fCount * GET_TIME;

	if (m_tAuraFrame.fFrame > m_tAuraFrame.fMax)
	{
		m_tAuraFrame.fFrame = 0.f;
	}

	m_fFrameTime += GET_TIME * m_tFrame.fCount;
	if (m_fFrameTime >= 1)
	{
		m_fFrameTime = 0;
		++m_iFrameCount;
	}

	if (m_tFrame.fFrame + m_iFrameCount * 17 > m_tFrame.fMax)
	{
		m_iFrameCount = 0;
	}

	if (m_dwUnitState == UNIT_STASISFIELD)
	{
		m_fStateCount += GET_TIME * m_tStateCount.fCount;
		m_AttTarget = NULL;
		m_bUnitAtt = false;
		m_dwState = UNIT_STAND;
		m_dwSubState = UNIT_STAND;

		if (m_fStateCount >= 1.f)
		{
			m_fStateCount = 0.f;
			++m_iStateCount;
			if (m_iStasis < 12)
				++m_iStasis;
			if (m_dwUnitState == UNIT_STASISFIELD)
			{
				if (m_tStateCount.fFrame + m_iStateCount >= (m_tStateCount.fMax / 2))
				{
					m_iStateCount = 0;
					--m_iStateTime;
				}
			}
		}
		if (m_iStateTime <= 0)
		{
			m_iStasis = 0;
			m_dwUnitState = UNIT_NORMAL;
		}
	}
}

void CDarkArchon::AStarMove(void)
{
	if (m_dwUnitState == UNIT_STASISFIELD)
		return;
	list<int>* pBestList = m_pAStar->GetBestList();

	if (pBestList->empty())
	{
		if (m_dwState != UNIT_CREATE &&
			m_dwState != UNIT_ATTACK &&
			m_dwState != UNIT_HOLD)
		{
			m_dwState = UNIT_STAND;
			for (int i = 0; i < 9; ++i)
				m_iClickMyInterpace[i] = 0;
			m_iClickMyInterpace[1] = 1;
		}

		return;
	}

	if (m_dwState != UNIT_ATTACK &&
		m_dwState != UNIT_HOLD)
	{
		for (int i = 0; i < 9; ++i)
			m_iClickMyInterpace[i] = 0;
		m_iClickMyInterpace[0] = 1;
	}


	const vector<TILE*> pVecTile
		= (ObjMgr->FindGameObject<CBackGround>())->GetTile();

	int iNextTileIndex = pBestList->front();

	//°¢µµ
	float fWidth = pVecTile[iNextTileIndex]->vPos.x - m_tInfo.vPos.x;
	float fHeight = pVecTile[iNextTileIndex]->vPos.y - m_tInfo.vPos.y;

	float fDistance = sqrtf((fWidth * fWidth) + (fHeight * fHeight));

	float fAngle = acosf(fWidth / fDistance);

	if (pVecTile[iNextTileIndex]->vPos.y < m_tInfo.vPos.y)
		fAngle *= -1.f;

	m_fAngle = D3DXToDegree(fAngle);

	//¹æÇâ
	m_tInfo.vDir = pVecTile[iNextTileIndex]->vPos - m_tInfo.vPos;

	float fDist = D3DXVec3Length(&m_tInfo.vDir);

	D3DXVec3Normalize(&m_tInfo.vDir, &m_tInfo.vDir);

	m_tInfo.vPos += m_tInfo.vDir * m_fSpeed * GET_TIME;

	if (fDist < 5.f)
	{
		pBestList->pop_front();
	}
}