#include "stdafx.h"
#include "HighTemplar.h"
#include "TextureMgr.h"
#include "Device.h"
#include "ObjMgr.h"
#include "TimeMgr.h"
#include "AStar.h"
#include "KeyMgr.h"
#include "Mouse.h"
#include "BackGround.h"
#include "Interpace.h"
#include "AttEffect.h"
#include "Factory.h"
#include "Effect.h"
#include "Archon.h"

CHighTemplar::CHighTemplar()
{
}


CHighTemplar::~CHighTemplar()
{
	Release();
}

HRESULT CHighTemplar::Initialize(void)
{
	list<UNIT*> UnitList = ObjMgr->GetUnitList();

	list<UNIT*>::iterator iter = UnitList.begin();
	list<UNIT*>::iterator iter_end = UnitList.end();

	lstrcpy(m_tInfo.szObjKey, L"HighTemplar");
	lstrcpy(m_tInfo.szStateKey, L"Normal");

	for (; iter != iter_end; ++iter)
	{
		if (!lstrcmp((*iter)->szName, m_tInfo.szObjKey))
		{
			m_tUnitInfo = (*(*iter));
			break;
		}
	}
	m_tUnitInfo.eType = UNIT_GROUND;
	m_dwState = UNIT_STAND;
	m_pAStar = new CAStar;
	m_bClickKey = true;
	m_tInfo.vSize = D3DXVECTOR3(24.f, 24.f, 0.f);
	m_tInfo.vLook = D3DXVECTOR3(1.f, 0.f, 0.f);
	m_tClick = FRAME(0.f, 10.f, 2.f);
	m_tFrame = FRAME(8.f, 5.f, 9.f);
	m_fSpeed = 80.f;
	m_fFrameTime = 0.f;
	m_iFrameCount = 0;
	m_eRenderType = RENDER_GROUND;
	m_tCreateTime = FRAME(0.f, 1.f, 1.f);
	m_fMoveCount = 0.f;
	m_iCreateNumber = 0;
	m_CreateTemplar = NULL;
	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();
	if (D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_HIGHTEMPLAR_MPMAX) == 1)
		m_tUnitInfo.iMp = 62;

	m_tStateCount = FRAME(0.f, 24.f, 24.f);
	m_iStateCount = 0;
	m_iView = 9;
	return S_OK;
}

int CHighTemplar::Update(void)
{
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

	if (D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_HIGHTEMPLAR_MPMAX) == 1)
	{
		m_tUnitInfo.iMpMax = 250;
	}


	AStarMove();
	if (m_tUnitInfo.iHp <= 0)
		m_dwState = UNIT_DEAD;
	
	if (m_bDeadCheck)
	{
		
		D_CAST(CInterpace, pInterpace)->EraseUnitList(this);
		return 1;
	}

	Clooking();
	m_tCreateTime.fFrame += GET_TIME;
	if (m_tCreateTime.fFrame > 2.f)
	{
		m_tCreateTime.fFrame = 0;
		if (m_tUnitInfo.iMp < m_tUnitInfo.iMpMax)
		{
			++m_tUnitInfo.iMp;
		}
	}
	SelectKeyCheck();
	Function();
	Frame();
	return 0;
}

void CHighTemplar::Render(void)
{
	D3DXMATRIX matScale, matTrans;

	if (m_bSelect)
	{
		CObjectRenderMgr::SelectUnitUIRender(this, 1, 7.f, 0, 25.f);

		///Mp Frame
		const TEX_INFO* pSelecttexture = TextureMgr->GetTexture(L"Bar"
			, L"MPFrame", 0);

		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y + 30
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
			, L"MPBar", 0);

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

	const TEX_INFO* pTemplartexture = NULL;
	float fX = 0.f, fY = 0.f;

	if (m_dwState == UNIT_DEAD)
	{
		pTemplartexture = TextureMgr->GetTexture(L"High Templar"
			, m_tInfo.szStateKey, S_INT_CAST(m_tFrame.fFrame) + S_INT_CAST(m_iFrameCount));

		D3DXMatrixScaling(&matScale, m_tInfo.vLook.x, 1.f, 0.f);
		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y
			, 0.f);

		matScale *= matTrans;

		fX = pTemplartexture->ImageInfo.Width / 2.f;
		fY = pTemplartexture->ImageInfo.Height / 2.f;
	}
	else
	{
		pTemplartexture = TextureMgr->GetTexture(L"High Templar"
			, m_tInfo.szStateKey, S_INT_CAST(m_tFrame.fFrame) + S_INT_CAST(m_iFrameCount * 17));

		D3DXMatrixScaling(&matScale, m_tInfo.vLook.x, 1.f, 0.f);
		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y
			, 0.f);

		matScale *= matTrans;

		fX = pTemplartexture->ImageInfo.Width / 2.f;
		fY = pTemplartexture->ImageInfo.Height / 2.f;
	}

	if (m_eTeamType == TEAM_PLAYER)
	{
		Device->GetSprite()->SetTransform(&matScale);
		Device->GetSprite()->Draw(pTemplartexture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame), 255, 255, 255));
	}
	else if (m_eTeamType == TEAM_ENEMY)
	{
		if (m_eAlphType == ALPH_CREATE_CLOOCKING && m_dwUnitState != UNIT_MAELSTORM)
		{
			Device->GetSprite()->SetTransform(&matScale);
			Device->GetSprite()->Draw(pTemplartexture->pTexture
				, NULL
				, &D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame - 85), 255, 160, 160));
		}
		else
		{
			Device->GetSprite()->SetTransform(&matScale);
			Device->GetSprite()->Draw(pTemplartexture->pTexture
				, NULL
				, &D3DXVECTOR3(fX, fY, 0.f)
				, NULL
				, D3DCOLOR_ARGB(S_INT_CAST(255 - m_tAlphCount.fFrame), 255, 160, 160));
		}
	}

	if (m_dwUnitState == UNIT_MAELSTORM && m_dwState != UNIT_DEAD)
	{
		const TEX_INFO* pProbetexture = TextureMgr->GetTexture(L"Effect"
			, L"Maelstorm", S_INT_CAST(m_tStateCount.fFrame) + m_iStateCount);

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
			, D3DCOLOR_ARGB(180, 255, 160, 160));
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

void CHighTemplar::Release(void)
{
}

void CHighTemplar::SelectKeyCheck(void)
{
	if (m_dwUnitState == UNIT_MAELSTORM || m_dwUnitState == UNIT_STASISFIELD)
		return;
	if (m_dwState == UNIT_DEAD)
		return;

	if (!m_bSelect)
	{
		return;
	}
		
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
		m_dwSubState = UNIT_MOVE;
		m_bUnitAtt = false;
		m_AttTarget = NULL;
		m_pAStar->StartAStarPos(m_tInfo.vPos, vMouse);
	}

	if (dwKey & KEY_P && m_bClickKey)
	{
		m_bClickKey = false;
		ptMouse->SetState(MOUSE_ATTACK_POINT_PATOL);
	}

	if (dwKey & KEY_LBUTTON && ptMouse->GetState() == MOUSE_ATTACK_POINT_NOMAL)
	{
		m_bClickKey = false;
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
		m_bClickKey = false;
		m_dwState = UNIT_STAND;
		m_dwSubState = UNIT_HOLD;
		m_pAStar->AStarClear();
	}

	if (dwKey & KEY_S && m_bClickKey)
	{
		m_bClickKey = false;
		m_bUnitAtt = false;
		m_AttTarget = NULL;
		m_dwState = UNIT_STAND;
		m_dwSubState = UNIT_STAND;
		m_pAStar->AStarClear();
		for (int i = 0; i < 9; ++i)
			m_iClickMyInterpace[i] = 0;
		m_iClickMyInterpace[1] = 1;
	}

	CGameObject* pInterpace = ObjMgr->GetList()[OBJ_INTERPACE].front();

	D_CAST(CInterpace, pInterpace)->SetUpgradeCount(m_eTeamType, UPGRADE_HIGHTEMPLAR_STORM, 1);
	m_tUnitInfo.iMp = 100;



	if (dwKey & KEY_T && m_bClickKey &&
		1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_HIGHTEMPLAR_STORM))
	{
		m_bClickKey = false;
		if (m_tUnitInfo.iMp >= 75)
		{
			m_bUnitAtt = false;
			D_CAST(CMouse, ptMouse)->SetState(MOUSE_SKILL_POINT);
		}
		else
		{

		}
	}

	if (dwKey & KEY_LBUTTON && m_bClickKey)
	{
		m_bClickKey = false;
		if (D_CAST(CMouse, ptMouse)->GetState() == MOUSE_SKILL_POINT)
		{
			m_vPatrolEndPoint = vMouse;
			m_dwSubState = UNIT_SKILL1;
			m_dwState = UNIT_MOVE;
			m_pAStar->StartAStarPos(m_tInfo.vPos, vMouse);
			ptMouse->SetState(MOUSE_CLICK);
		}


	}

	if (dwKey & KEY_R && m_bClickKey)
	{
		m_bClickKey = false;

		if (m_iCreateNumber == 0)
		{
			list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];

			for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
			{
				if ((*iter)->GetTeamType() != m_eTeamType ||
					(*iter) == this ||
					lstrcmp((*iter)->GetUnitInfo().szName, L"HighTemplar") ||
					D_CAST(CHighTemplar, (*iter))->GetCreateTemplar() != NULL)
					continue;

				m_CreateTemplar = (*iter);
				D_CAST(CHighTemplar, (*iter))->SetCreateTemplar(this);

				m_iCreateNumber = 1;
				if (D_CAST(CHighTemplar, (*iter))->GetCreateNumber() == 0)
				{
					D_CAST(CHighTemplar, (*iter))->SetCreateNumber(100);
				}

				m_dwState = UNIT_MOVE;
				m_dwSubState = UNIT_SKILL3;
				return;
			}
		}
	}
	if (!dwKey)
	{
		m_tClick.fFrame += GET_TIME * m_tClick.fCount;

		if (m_tClick.fFrame > m_tClick.fMax)
			m_bClickKey = true;
	}
}

void CHighTemplar::CheckOneClick(void)
{
	if (m_dwUnitState == UNIT_MAELSTORM || m_dwUnitState == UNIT_STASISFIELD)
		return;
	if (m_dwState == UNIT_DEAD)
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

	if (1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_HIGHTEMPLAR_STORM))
	{
		m_iClickMyInterpace[6] = 0;
	}
	else
	{
		m_iClickMyInterpace[6] = 2;
	}

	if (1 == D_CAST(CInterpace, pInterpace)->GetUpgradeCount(m_eTeamType, UPGRADE_HIGHTEMPLAR_ILLUSION))
	{
		m_iClickMyInterpace[7] = 0;
	}
	else
	{
		m_iClickMyInterpace[7] = 1;
	}
		// 7¹øÂ° Ä­
		pInterpacetexture = TextureMgr->GetTexture(L"Button"
			, L"Storm", m_iClickMyInterpace[6]);

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
			, L"illusion", m_iClickMyInterpace[7]);

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
			, L"Archon", 2);

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
		// T I R / R / F C E
}

void CHighTemplar::Function(void)
{
	if (m_dwState == UNIT_DEAD || m_dwUnitState == UNIT_STASISFIELD)
		return;

	CGameObject* pMouse = ObjMgr->GetList()[OBJ_MOUSE].front();

	if (m_dwSubState == UNIT_SKILL1)
	{
		if (fabs(m_tInfo.vPos.x - m_vPatrolEndPoint.x) < 150.f &&
			fabs(m_tInfo.vPos.y - m_vPatrolEndPoint.y) < 150.f)
		{
			SoundMgr->PlaySound(L"PTeSto00.wav", CHANNEL_EFFECT3, 0.7f, m_vPatrolEndPoint);
			m_tUnitInfo.iMp -= 75;
			m_dwState = UNIT_ATTACK;
			m_dwSubState = UNIT_STAND;
			m_pAStar->AStarClear();
			ObjMgr->AddGameObject(CFactory<CAttEffect>::CreateGameUnitSkillEffect(m_vPatrolEndPoint, EFFECT_STORM), OBJ_EFFECT);
		}
	}

	if (m_dwSubState == UNIT_SKILL3)
	{

		if (m_CreateTemplar == NULL || m_CreateTemplar->GetState() == UNIT_DEAD)
		{
			m_iCreateNumber = 0;
			m_CreateTemplar = NULL;
			m_dwState = UNIT_STAND;
			m_dwSubState = UNIT_STAND;
		}
		else
		{
			D_CAST(CHighTemplar, m_CreateTemplar)->GetAStar()->StartAStarPos(m_CreateTemplar->GetPos(), m_tInfo.vPos);
			m_pAStar->StartAStarPos(m_tInfo.vPos, m_CreateTemplar->GetPos());
		}
		if (fabs(m_CreateTemplar->GetPos().x - m_tInfo.vPos.x) < 40.f &&
			fabs(m_CreateTemplar->GetPos().y - m_tInfo.vPos.y) < 40.f)
		{
			if (m_iCreateNumber == 1)
			{
				SoundMgr->PlaySound(L"pteSum00.wav", CHANNEL_EFFECT3, 0.7f, m_tInfo.vPos);
				ObjMgr->AddGameObject(CFactory<CArchon>::CreateGameObject(m_tInfo.vPos, m_eTeamType), OBJ_UNIT);
				m_bDeadCheck = true;
				m_CreateTemplar->SetDeadCheck(true);
			}
		}
	}
}

void CHighTemplar::Frame(void)
{
	switch (m_dwState)
	{
	case UNIT_STAND:
		if (m_fAngle < -168.75 || m_fAngle > 168.75)
		{
			m_tFrame = FRAME(8.f, 10.f, 110.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -168.75 && m_fAngle < -146.25)
		{
			m_tFrame = FRAME(6.f, 10.f, 108.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -146.25 && m_fAngle < -123.75)
		{
			m_tFrame = FRAME(4.f, 10.f, 106.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -123.75 && m_fAngle < -101.25)
		{
			m_tFrame = FRAME(2.f, 10.f, 104.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -101.25 && m_fAngle < -78.75)
		{
			m_tFrame = FRAME(0.f, 10.f, 102.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -78.75 && m_fAngle < -56.25)
		{
			m_tFrame = FRAME(2.f, 10.f, 104.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -56.25 && m_fAngle < -33.75)
		{
			m_tFrame = FRAME(4.f, 10.f, 106.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -33.75 && m_fAngle < -11.25)
		{
			m_tFrame = FRAME(6.f, 10.f, 108.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -11.25 && m_fAngle < 11.25)
		{
			m_tFrame = FRAME(8.f, 10.f, 110.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 11.25 && m_fAngle < 33.75)
		{
			m_tFrame = FRAME(10.f, 10.f, 112.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 33.75 && m_fAngle < 56.25)
		{
			m_tFrame = FRAME(12.f, 10.f, 114.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 56.25 && m_fAngle < 78.75)
		{
			m_tFrame = FRAME(14.f, 10.f, 116.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 78.75 && m_fAngle < 101.25)
		{
			m_tFrame = FRAME(16.f, 10.f, 118.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 101.25 && m_fAngle < 123.75)
		{
			m_tFrame = FRAME(14.f, 10.f, 116.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= 123.75 && m_fAngle < 146.25)
		{
			m_tFrame = FRAME(12.f, 10.f, 114.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= 146.25 && m_fAngle < 168.75)
		{
			m_tFrame = FRAME(10.f, 10.f, 112.f);
			m_tInfo.vLook.x = -1;
		}
		break;
	case UNIT_MOVE:
		if (m_fAngle < -168.75 || m_fAngle > 168.75)
		{
			m_tFrame = FRAME(8.f, 15.f, 76.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -168.75 && m_fAngle < -146.25)
		{
			m_tFrame = FRAME(6.f, 15.f, 74.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -146.25 && m_fAngle < -123.75)
		{
			m_tFrame = FRAME(4.f, 15.f, 72.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -123.75 && m_fAngle < -101.25)
		{
			m_tFrame = FRAME(2.f, 15.f, 70.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -101.25 && m_fAngle < -78.75)
		{
			m_tFrame = FRAME(0.f, 15.f, 68.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -78.75 && m_fAngle < -56.25)
		{
			m_tFrame = FRAME(2.f, 15.f, 70.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -56.25 && m_fAngle < -33.75)
		{
			m_tFrame = FRAME(4.f, 15.f, 72.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -33.75 && m_fAngle < -11.25)
		{
			m_tFrame = FRAME(6.f, 15.f, 74.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -11.25 && m_fAngle < 11.25)
		{
			m_tFrame = FRAME(8.f, 15.f, 76.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 11.25 && m_fAngle < 33.75)
		{
			m_tFrame = FRAME(10.f, 15.f, 78.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 33.75 && m_fAngle < 56.25)
		{
			m_tFrame = FRAME(12.f, 15.f, 80.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 56.25 && m_fAngle < 78.75)
		{
			m_tFrame = FRAME(14.f, 15.f, 82.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 78.75 && m_fAngle < 101.25)
		{
			m_tFrame = FRAME(16.f, 15.f, 84.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 101.25 && m_fAngle < 123.75)
		{
			m_tFrame = FRAME(14.f, 15.f, 82.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= 123.75 && m_fAngle < 146.25)
		{
			m_tFrame = FRAME(12.f, 15.f, 80.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= 146.25 && m_fAngle < 168.75)
		{
			m_tFrame = FRAME(10.f, 15.f, 78.f);
			m_tInfo.vLook.x = -1;
		}
		break;
	case UNIT_ATTACK:
		if (m_fAngle < -168.75 || m_fAngle > 168.75)
		{
			m_tFrame = FRAME(8.f, 20.f, 76.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -168.75 && m_fAngle < -146.25)
		{
			m_tFrame = FRAME(6.f, 20.f, 74.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -146.25 && m_fAngle < -123.75)
		{
			m_tFrame = FRAME(4.f, 20.f, 72.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -123.75 && m_fAngle < -101.25)
		{
			m_tFrame = FRAME(2.f, 20.f, 70.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -101.25 && m_fAngle < -78.75)
		{
			m_tFrame = FRAME(0.f, 20.f, 68.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -78.75 && m_fAngle < -56.25)
		{
			m_tFrame = FRAME(2.f, 20.f, 70.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -56.25 && m_fAngle < -33.75)
		{
			m_tFrame = FRAME(4.f, 20.f, 72.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -33.75 && m_fAngle < -11.25)
		{
			m_tFrame = FRAME(6.f, 20.f, 74.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -11.25 && m_fAngle < 11.25)
		{
			m_tFrame = FRAME(8.f, 20.f, 76.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 11.25 && m_fAngle < 33.75)
		{
			m_tFrame = FRAME(10.f, 20.f, 78.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 33.75 && m_fAngle < 56.25)
		{
			m_tFrame = FRAME(12.f, 20.f, 80.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 56.25 && m_fAngle < 78.75)
		{
			m_tFrame = FRAME(14.f, 20.f, 82.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 78.75 && m_fAngle < 101.25)
		{
			m_tFrame = FRAME(16.f, 20.f, 84.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 101.25 && m_fAngle < 123.75)
		{
			m_tFrame = FRAME(14.f, 20.f, 82.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= 123.75 && m_fAngle < 146.25)
		{
			m_tFrame = FRAME(12.f, 20.f, 80.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= 146.25 && m_fAngle < 168.75)
		{
			m_tFrame = FRAME(10.f, 20.f, 78.f);
			m_tInfo.vLook.x = -1;
		}
		break;
	case UNIT_DEAD:
		m_tFrame = FRAME(0.f, 15.f, 6.f);
		break;
	}

	if (m_dwState == UNIT_ATTACK)
	{
		lstrcpy(m_tInfo.szStateKey, L"Att");
	}
	else if (m_dwState == UNIT_DEAD)
	{
		lstrcpy(m_tInfo.szStateKey, L"Die");
	}
	else if(m_dwState == UNIT_MOVE)
	{
		lstrcpy(m_tInfo.szStateKey, L"Move");
	}
	else
	{
		lstrcpy(m_tInfo.szStateKey, L"Normal");
	}

	if (m_dwPreState != m_dwState)
	{
		m_iFrameCount = 0;
		m_dwPreState = m_dwState;
	}

	m_fFrameTime += GET_TIME * m_tFrame.fCount;
	if (m_fFrameTime >= 1)
	{
		m_fFrameTime = 0;
		++m_iFrameCount;
	}

	if (m_dwState != UNIT_DEAD)
	{
		if (m_tFrame.fFrame + m_iFrameCount * 17 > m_tFrame.fMax)
		{
			if (m_dwState == UNIT_MOVE)
				--m_iFrameCount;
			else
				m_iFrameCount = 0;

			if (m_dwState == UNIT_ATTACK)
				m_dwState = UNIT_STAND;
		}
	}
	else if (m_dwState == UNIT_DEAD)
	{
		if (m_tFrame.fFrame + m_iFrameCount > m_tFrame.fMax)
		{
			m_iFrameCount = 0;
			m_bDeadCheck = true;
		}
	}
	else if (m_tFrame.fFrame + m_iFrameCount > m_tFrame.fMax)
	{
		m_iFrameCount = 0;
	}

	if(m_dwUnitState == UNIT_MAELSTORM || m_dwUnitState == UNIT_STASISFIELD)
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
			if (m_dwUnitState == UNIT_MAELSTORM)
			{
				if (m_tStateCount.fFrame + m_iStateCount > m_tStateCount.fMax)
				{
					m_iStateCount = 0;
					--m_iStateTime;
				}
			}
			else if (m_dwUnitState == UNIT_STASISFIELD)
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

void CHighTemplar::Clooking(void)
{
	if (m_dwState == UNIT_DEAD || m_dwUnitState == UNIT_STASISFIELD)
		return;

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

void CHighTemplar::AStarMove(void)
{
	if (m_dwUnitState == UNIT_MAELSTORM || m_dwUnitState == UNIT_STASISFIELD)
		return;
	if (m_dwState == UNIT_DEAD)
		return;

	list<int>* pBestList = m_pAStar->GetBestList();

	if (pBestList->empty())
	{
		if (m_dwState != UNIT_ATTACK &&
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

	m_fMoveCount += GET_TIME * 8.f;

	if (m_fMoveCount >= 1.f)
	{
		m_fMoveCount = 0.f;
		if(m_eAlphType == ALPH_NOMAL)
		ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameUnitEffectTarget(this, EFFECT_TEMPLAR_SHAD, m_eTeamType), OBJ_EFFECT);
	}
		

	if (fDist < 5.f)
	{
		pBestList->pop_front();
	}
}
