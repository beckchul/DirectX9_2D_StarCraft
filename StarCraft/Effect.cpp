#include "stdafx.h"
#include "Effect.h"
#include "TimeMgr.h"
#include "TextureMgr.h"
#include "Device.h"
#include "ObjMgr.h"
#include "BackGround.h"
#include "Factory.h"

CEffect::CEffect()
{
}


CEffect::~CEffect()
{
	Release();
}


HRESULT CEffect::Initialize(void)
{
	m_iAlphCount = 0;
	switch (m_eEffectType)
	{
	case EFFECT_PROBE:
		m_tFrame = FRAME(0.f, 30.f, 11.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Probe_Att");
		m_eRenderType = RENDER_GROUND_EFFECT;
		m_iFrameCount = 0;
		m_iAlphCount = 40;
		break;
	case EFFECT_PROBE_DEAD:
		m_tFrame = FRAME(0.f, 30.f, 8.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Explosion_S");
		m_eRenderType = RENDER_GROUND_EFFECT;
		m_iFrameCount = 0;
		break;
	case EFFECT_DRAGOON_ATT_EX:
		m_tFrame = FRAME(0.f, 40.f, 13.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"BallEx");
		m_eRenderType = RENDER_GROUND_EFFECT;
		m_iFrameCount = 0;
		break;
	case EFFECT_DRAGOON_DEAD:
		m_tFrame = FRAME(0.f, 0.5f, 4.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"DragoonDeath");
		m_eRenderType = RENDER_BACKGROUND;
		m_iFrameCount = 0;
		break;
	case EFFECT_ARCHON_ATT:
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Shockwave");
		m_eRenderType = RENDER_BACKGROUND;
		m_iFrameCount = 0;
		Angle();
		break;
	case EFFECT_ARCHON_DEAD:
		m_tFrame = FRAME(0.f, 30.f, 9.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Explosion_M");
		m_eRenderType = RENDER_GROUND_EFFECT;
		m_iFrameCount = 0;
		break;
	case EFFECT_OBSERVER_DEAD:
		m_tFrame = FRAME(0.f, 30.f, 8.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Explosion_S");
		m_eRenderType = RENDER_AIR_EFFECT;
		m_iFrameCount = 0;
		break;
	case EFFECT_SCOUT_ATT:
		m_tFrame = FRAME(0.f, 30.f, 7.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Scout_Ground");
		m_eRenderType = RENDER_GROUND_EFFECT;
		m_iFrameCount = 0;
		break;
	case EFFECT_SCOUT_DEAD:
		m_tFrame = FRAME(0.f, 30.f, 9.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Explosion_M");
		m_eRenderType = RENDER_AIR_EFFECT;
		m_iFrameCount = 0;
		break;
	case EFFECT_SCOUT_ATT_EX:
		m_tFrame = FRAME(0.f, 40.f, 13.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"BallEx");
		m_eRenderType = RENDER_AIR_EFFECT;
		m_iFrameCount = 0;
		break;
	case EFFECT_TEMPLAR_SHAD:
		lstrcpy(m_tInfo.szObjKey, L"High Templar");
		lstrcpy(m_tInfo.szStateKey, L"Shad");
		m_eRenderType = RENDER_GROUND;
		m_iFrameCount = 0;
		Shad();
		m_iAlphCount = 30;
		break;
	case EFFECT_DARKARCHON_FEEDBACK:
		m_tFrame = FRAME(0.f, 44.f, 21.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Feedback");
		m_eRenderType = RENDER_GROUND_EFFECT;
		m_iFrameCount = 0;
		m_iAlphCount = 50;
		break;
	case EFFECT_DARKARCHON_MINDCONTROL:
		m_tFrame = FRAME(0.f, 20.f, 19.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Mindcontrol");
		m_eRenderType = RENDER_AIR_EFFECT;
		m_iFrameCount = 0;
		m_iAlphCount = 20;
		break;
	case EFFECT_DARKARCHON_MAELSTORM:
		m_tFrame = FRAME(0.f, 40.f, 8.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"maelhit");
		m_eRenderType = RENDER_AIR_EFFECT;
		m_iFrameCount = 0;
		m_iAlphCount = 120;
		break;
	case EFFECT_DARKARCHON_DEAD:
		m_tFrame = FRAME(0.f, 42.f, 14.f);
		lstrcpy(m_tInfo.szObjKey, L"Dark Archon");
		lstrcpy(m_tInfo.szStateKey, L"Die");
		m_eRenderType = RENDER_GROUND_EFFECT;
		m_iFrameCount = 0;
		break;
	case EFFECT_ARBITER_STASISFIELD:
		m_tFrame = FRAME(0.f, 40.f, 8.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Stasis");
		m_eRenderType = RENDER_AIR_EFFECT;
		m_iFrameCount = 0;
		m_iAlphCount = 120;
		break;
	case EFFECT_ARBITER_RECALL_START:
		m_tFrame = FRAME(0.f, 36.f, 18.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Recall");
		m_eRenderType = RENDER_AIR_EFFECT;
		m_iFrameCount = 0;
		m_iAlphCount = 170;
		break;
	case EFFECT_ARBITER_RECALL_END:
		m_tFrame = FRAME(0.f, 36.f, 18.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Recall");
		m_eRenderType = RENDER_AIR_EFFECT;
		m_iFrameCount = 0;
		m_iAlphCount = 170;
		break;
	case EFFECT_CARRIER_DEAD:
		m_tFrame = FRAME(0.f, 26.f, 13.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Explosion_L");
		m_eRenderType = RENDER_AIR_EFFECT;
		m_iFrameCount = 0;
		break;
	case EFFECT_INTERCEPTOR_DEAD:
		m_tFrame = FRAME(0.f, 16.f, 8.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Explosion_S");
		m_eRenderType = RENDER_AIR_EFFECT;
		m_iFrameCount = 0;
		break;
	case EFFECT_SCARAB_DEAD:
		m_tFrame = FRAME(0.f, 54.f, 9.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Scarab_Explosion");
		m_eRenderType = RENDER_GROUND_EFFECT;
		m_iFrameCount = 0;
		break;
	case EFFECT_SCARAB_MOVE:
		m_tFrame = FRAME(0.f, 28.f, 7.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Scarab_Move");
		m_eRenderType = RENDER_GROUND_EFFECT;
		m_iFrameCount = 0;
		break;
	case EFFECT_PING:
		m_tFrame = FRAME(0.f, 20.f, 18.f);
		lstrcpy(m_tInfo.szObjKey, L"Map");
		lstrcpy(m_tInfo.szStateKey, L"ping");
		m_eRenderType = RENDER_INTERPACE;
		m_iFrameCount = 0;
		m_tInfo.vPos = D3DXVECTOR3(m_tInfo.vPos.x / 32.f + 6.f, m_tInfo.vPos.y / 32.f + 348.f, 0.f);
		break;
	case EFFECT_BUILD_DEAD:
		m_tFrame = FRAME(0.f, 13.f, 13.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Explosion_L");
		m_eRenderType = RENDER_GROUND_EFFECT;
		m_iFrameCount = 0;
		break;
	case EFFECT_BUILD_DEAD2:
		m_tFrame = FRAME(0.f, 0.2f, 3.f);
		lstrcpy(m_tInfo.szObjKey, L"Effect");
		lstrcpy(m_tInfo.szStateKey, L"Vestige");
		m_eRenderType = RENDER_BACKGROUND;
		m_iFrameCount = 0;
		break;
	}



	return S_OK;
}

int CEffect::Update(void)
{
	if (m_bDeadCheck)
	{
		if (m_eEffectType == EFFECT_ARBITER_RECALL_START)
		{
			list<CGameObject*> pUnitList = ObjMgr->GetList()[OBJ_UNIT];
			CGameObject* pBakcGround = ObjMgr->GetList()[OBJ_BACKGROUND].front();
			const vector<TILE*> pVector = D_CAST(CBackGround , pBakcGround)->GetTile();

			for (list<CGameObject*>::iterator iter = pUnitList.begin(); iter != pUnitList.end(); ++iter)
			{
				if ((*iter)->GetTeamType() != m_eTeamType)
					continue;

				int iX = S_INT_CAST(m_AttTarget->GetInfo().vPos.x / TILECX);
				int iY = S_INT_CAST(m_AttTarget->GetInfo().vPos.y / TILECY);

				int iIndex = iX + (TILEEX * iY);
				if (iIndex < 0 || iIndex >= TILEEX * TILEEY)
					return 0;


				if ((*iter)->GetPos().x >= m_tInfo.vPos.x - 80.f &&
					(*iter)->GetPos().x <= m_tInfo.vPos.x + 80.f &&
					(*iter)->GetPos().y >= m_tInfo.vPos.y - 80.f &&
					(*iter)->GetPos().y <= m_tInfo.vPos.y + 80.f)
				{
					if (pVector[iIndex]->byOption == 1 && (*iter)->GetUnitInfo().eType == UNIT_GROUND)
						continue;

					D3DXVECTOR3 vRecallPos;
					vRecallPos = m_AttTarget->GetPos() - m_tInfo.vPos;
					
					(*iter)->SetPos((*iter)->GetPos() + vRecallPos);

					SoundMgr->PlaySound(L"PAbFol01.wav", CHANNEL_EFFECT2, 0.5f);
					ObjMgr->AddGameObject(CFactory<CEffect>::CreateGameEffect(m_AttTarget->GetPos(), EFFECT_ARBITER_RECALL_END), OBJ_EFFECT);
				}
			}
		}
		return 1;
	}
		

	Frame();
	return 0;
}

void CEffect::Render(void)
{
	if(m_eEffectType == EFFECT_PING)
	{
		D3DXMATRIX matTrans;

		const TEX_INFO* pEffectture = TextureMgr->GetTexture(m_tInfo.szObjKey
			, m_tInfo.szStateKey, S_INT_CAST(m_tFrame.fFrame) + m_iFrameCount);

		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x
			, m_tInfo.vPos.y
			, 0.f);

		float fX = pEffectture->ImageInfo.Width / 2.f;
		float fY = pEffectture->ImageInfo.Height / 2.f;


		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pEffectture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255 - m_iAlphCount, 255, 255, 255));
	}
	else if (m_eEffectType == EFFECT_ARCHON_ATT || m_eEffectType == EFFECT_TEMPLAR_SHAD)
	{
		D3DXMATRIX matTrans, matScale;

		D3DXMatrixScaling(&matScale, m_tInfo.vLook.x, 1.f, 0.f);


		const TEX_INFO* pEffectture = TextureMgr->GetTexture(m_tInfo.szObjKey
			, m_tInfo.szStateKey, S_INT_CAST(m_tFrame.fFrame) + m_iFrameCount * 17);

		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y
			, 0.f);

		matScale = matScale * matTrans;

		float fX = pEffectture->ImageInfo.Width / 2.f;
		float fY = pEffectture->ImageInfo.Height / 2.f;


		Device->GetSprite()->SetTransform(&matScale);
		Device->GetSprite()->Draw(pEffectture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255 - m_iAlphCount, 255, 255, 255));
		
	}
	else
	{
		D3DXMATRIX matTrans;

		const TEX_INFO* pEffectture = TextureMgr->GetTexture(m_tInfo.szObjKey
			, m_tInfo.szStateKey, S_INT_CAST(m_tFrame.fFrame) + m_iFrameCount);

		D3DXMatrixTranslation(&matTrans
			, m_tInfo.vPos.x - m_vScroll.x
			, m_tInfo.vPos.y - m_vScroll.y
			, 0.f);

		float fX = pEffectture->ImageInfo.Width / 2.f;
		float fY = pEffectture->ImageInfo.Height / 2.f;


		Device->GetSprite()->SetTransform(&matTrans);
		Device->GetSprite()->Draw(pEffectture->pTexture
			, NULL
			, &D3DXVECTOR3(fX, fY, 0.f)
			, NULL
			, D3DCOLOR_ARGB(255 - m_iAlphCount, 255, 255, 255));
	}

	
}

void CEffect::Release(void)
{
}

void CEffect::Shad(void)
{
	m_tInfo.vPos = m_AttTarget->GetPos();
	m_fAngle = m_AttTarget->GetAngle();

	if (m_fAngle < -168.75 || m_fAngle > 168.75)
	{
		m_tInfo.vPos.x += 3;
		m_tFrame = FRAME(8.f, 15.f, 59.f);
		m_tInfo.vLook.x = -1;
	}
	else if (m_fAngle >= -168.75 && m_fAngle < -146.25)
	{
		m_tInfo.vPos.x += 3;
		m_tInfo.vPos.y += 3;
		m_tFrame = FRAME(6.f, 15.f, 57.f);
		m_tInfo.vLook.x = -1;
	}
	else if (m_fAngle >= -146.25 && m_fAngle < -123.75)
	{
		m_tInfo.vPos.x += 3;
		m_tInfo.vPos.y += 3;
		m_tFrame = FRAME(4.f, 15.f, 55.f);
		m_tInfo.vLook.x = -1;
	}
	else if (m_fAngle >= -123.75 && m_fAngle < -101.25)
	{
		m_tInfo.vPos.x += 3;
		m_tInfo.vPos.y += 3;
		m_tFrame = FRAME(2.f, 15.f, 53.f);
		m_tInfo.vLook.x = -1;
	}
	else if (m_fAngle >= -101.25 && m_fAngle < -78.75)
	{
		m_tInfo.vPos.y += 3;
		m_tFrame = FRAME(0.f, 15.f, 51.f);
		m_tInfo.vLook.x = 1;
	}
	else if (m_fAngle >= -78.75 && m_fAngle < -56.25)
	{
		m_tInfo.vPos.x -= 3;
		m_tInfo.vPos.y += 3;
		m_tFrame = FRAME(2.f, 15.f, 53.f);
		m_tInfo.vLook.x = 1;
	}
	else if (m_fAngle >= -56.25 && m_fAngle < -33.75)
	{
		m_tInfo.vPos.x -= 3;
		m_tInfo.vPos.y += 3;
		m_tFrame = FRAME(4.f, 15.f, 55.f);
		m_tInfo.vLook.x = 1;
	}
	else if (m_fAngle >= -33.75 && m_fAngle < -11.25)
	{
		m_tInfo.vPos.x -= 3;
		m_tInfo.vPos.y += 3;
		m_tFrame = FRAME(6.f, 15.f, 57.f);
		m_tInfo.vLook.x = 1;
	}
	else if (m_fAngle >= -11.25 && m_fAngle < 11.25)
	{
		m_tInfo.vPos.x -= 3;
		m_tFrame = FRAME(8.f, 15.f, 59.f);
		m_tInfo.vLook.x = 1;
	}
	else if (m_fAngle >= 11.25 && m_fAngle < 33.75)
	{
		m_tInfo.vPos.x -= 3;
		m_tInfo.vPos.y -= 3;
		m_tFrame = FRAME(10.f, 15.f, 61.f);
		m_tInfo.vLook.x = 1;
	}
	else if (m_fAngle >= 33.75 && m_fAngle < 56.25)
	{
		m_tInfo.vPos.x -= 3;
		m_tInfo.vPos.y -= 3;
		m_tFrame = FRAME(12.f, 15.f, 63.f);
		m_tInfo.vLook.x = 1;
	}
	else if (m_fAngle >= 56.25 && m_fAngle < 78.75)
	{
		m_tInfo.vPos.x -= 3;
		m_tInfo.vPos.y -= 3;
		m_tFrame = FRAME(14.f, 15.f, 65.f);
		m_tInfo.vLook.x = 1;
	}
	else if (m_fAngle >= 78.75 && m_fAngle < 101.25)
	{
		m_tInfo.vPos.y -= 3;
		m_tFrame = FRAME(16.f, 15.f, 67.f);
		m_tInfo.vLook.x = 1;
	}
	else if (m_fAngle >= 101.25 && m_fAngle < 123.75)
	{
		m_tInfo.vPos.x += 3;
		m_tInfo.vPos.y -= 3;
		m_tFrame = FRAME(14.f, 15.f, 65.f);
		m_tInfo.vLook.x = -1;
	}
	else if (m_fAngle >= 123.75 && m_fAngle < 146.25)
	{
		m_tInfo.vPos.x += 3;
		m_tInfo.vPos.y -= 3;
		m_tFrame = FRAME(12.f, 15.f, 63.f);
		m_tInfo.vLook.x = -1;
	}
	else if (m_fAngle >= 146.25 && m_fAngle < 168.75)
	{
		m_tInfo.vPos.x += 3;
		m_tInfo.vPos.y -= 3;
		m_tFrame = FRAME(10.f, 15.f, 61.f);
		m_tInfo.vLook.x = -1;
	}
}

void CEffect::Angle(void)
{
	switch (m_eEffectType)
	{
	case EFFECT_ARCHON_ATT:
		if (m_fAngle < -168.75 || m_fAngle > 168.75)
		{
			m_tFrame = FRAME(8.f, 20.f, 59.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -168.75 && m_fAngle < -146.25)
		{
			m_tFrame = FRAME(6.f, 20.f, 57.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -146.25 && m_fAngle < -123.75)
		{
			m_tFrame = FRAME(4.f, 20.f, 55.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -123.75 && m_fAngle < -101.25)
		{
			m_tFrame = FRAME(2.f, 20.f, 53.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= -101.25 && m_fAngle < -78.75)
		{
			m_tFrame = FRAME(0.f, 20.f, 51.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -78.75 && m_fAngle < -56.25)
		{
			m_tFrame = FRAME(2.f, 20.f, 53.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -56.25 && m_fAngle < -33.75)
		{
			m_tFrame = FRAME(4.f, 20.f, 55.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -33.75 && m_fAngle < -11.25)
		{
			m_tFrame = FRAME(6.f, 20.f, 57.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= -11.25 && m_fAngle < 11.25)
		{
			m_tFrame = FRAME(8.f, 20.f, 59.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 11.25 && m_fAngle < 33.75)
		{
			m_tFrame = FRAME(10.f, 20.f, 61.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 33.75 && m_fAngle < 56.25)
		{
			m_tFrame = FRAME(12.f, 20.f, 63.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 56.25 && m_fAngle < 78.75)
		{
			m_tFrame = FRAME(14.f, 20.f, 65.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 78.75 && m_fAngle < 101.25)
		{
			m_tFrame = FRAME(16.f, 20.f, 67.f);
			m_tInfo.vLook.x = 1;
		}
		else if (m_fAngle >= 101.25 && m_fAngle < 123.75)
		{
			m_tFrame = FRAME(14.f, 20.f, 65.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= 123.75 && m_fAngle < 146.25)
		{
			m_tFrame = FRAME(12.f, 20.f, 63.f);
			m_tInfo.vLook.x = -1;
		}
		else if (m_fAngle >= 146.25 && m_fAngle < 168.75)
		{
			m_tFrame = FRAME(10.f, 20.f, 61.f);
			m_tInfo.vLook.x = -1;
		}
		break;
	}
}

void CEffect::Frame(void)
{
	m_fFrameTime += GET_TIME * m_tFrame.fCount;

	if (m_fFrameTime >= 1.f)
	{
		m_fFrameTime = 0.f;
		++m_iFrameCount;
	}
	
	if (m_eEffectType == EFFECT_ARCHON_ATT || m_eEffectType == EFFECT_TEMPLAR_SHAD)
	{
		if (m_tFrame.fFrame + m_iFrameCount * 17 > m_tFrame.fMax)
		{
			m_bDeadCheck = true;
			m_iFrameCount = 0;
		}
	}
	else if(m_tFrame.fFrame + m_iFrameCount > m_tFrame.fMax)
	{
		m_bDeadCheck = true;
		m_iFrameCount = 0;
	}
	
	if (m_eEffectType == EFFECT_DARKARCHON_FEEDBACK || m_eEffectType == EFFECT_DARKARCHON_MINDCONTROL)
	{
		m_tInfo.vPos = m_AttTarget->GetPos();
	}
}