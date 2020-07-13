#include "StdAfx.h"
#include "Device.h"

IMPLEMENT_SINGLETON(CDevice);

HRESULT CDevice::InitD3D(void)
{
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	D3DCAPS9 DeviceCaps;
	ZeroMemory(&DeviceCaps, sizeof(D3DCAPS9));

	if( FAILED(m_pD3D->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &DeviceCaps)) )
	{
		MessageBox(g_hWnd, L"장치 초기화 실패", L"장치 초기화 실패", MB_OK);

		return E_FAIL;
	}

	DWORD vp;

	if(DeviceCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
		vp = D3DCREATE_HARDWARE_VERTEXPROCESSING;	
	else
		vp = D3DCREATE_SOFTWARE_VERTEXPROCESSING;	

	D3DPRESENT_PARAMETERS  d3dpp;

	ZeroMemory(&d3dpp, sizeof(D3DPRESENT_PARAMETERS));

	SetParameters(d3dpp);			

	if(FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, g_hWnd, vp,
								&d3dpp, &m_pD3dDevice) ))
	{
		MessageBox(g_hWnd, L"디바이스 초기화 실패", L"디바이스 초기화 실패", MB_OK);

		return E_FAIL;
	}

	if(FAILED(D3DXCreateSprite(m_pD3dDevice, &m_pSprite)))
	{
		MessageBox(g_hWnd, L"스프라이트 객체 생성 실패", L"스프라이트 객체 생성 실패", MB_OK);
		return E_FAIL;
	}

	D3DXFONT_DESC	FontInfo;
	ZeroMemory(&FontInfo, sizeof(D3DXFONT_DESC));

	FontInfo.Height = 8;
	FontInfo.Width  = 8;

	FontInfo.Weight = FW_NORMAL;		

	FontInfo.CharSet = HANGUL_CHARSET;		
	lstrcpy(FontInfo.FaceName,	L"굴림" );		

	if(FAILED( D3DXCreateFontIndirect(m_pD3dDevice
		,&FontInfo, &m_pFont) ))
	{
		AfxMessageBox(L"폰트 객체 생성 실패");

		return E_FAIL;
	}
	return S_OK;
}

void CDevice::SetParameters(D3DPRESENT_PARAMETERS& d3dp)
{
	d3dp.BackBufferWidth  = WINCX;		
	d3dp.BackBufferHeight = WINCY;		
	d3dp.BackBufferCount  = 1;			
	d3dp.BackBufferFormat = D3DFMT_A8R8G8B8;
	d3dp.MultiSampleType = D3DMULTISAMPLE_NONE;		
	d3dp.MultiSampleQuality = 0;					
	d3dp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dp.hDeviceWindow = g_hWnd;	
	d3dp.Windowed	   = TRUE;			//TRUE -> 창모드, FALSE -> 전체화면.
	d3dp.EnableAutoDepthStencil = TRUE;
	d3dp.AutoDepthStencilFormat = D3DFMT_D24S8;	
	d3dp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	d3dp.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;
}

void CDevice::Render_Begin(void)
{
	m_pD3dDevice->Clear(0
						,NULL
						,D3DCLEAR_TARGET | D3DCLEAR_STENCIL | D3DCLEAR_ZBUFFER
						,D3DCOLOR_ARGB(255, 0, 0, 255)		
						,1.f	
						,0);

	m_pD3dDevice->BeginScene();

	m_pSprite->Begin(D3DXSPRITE_ALPHABLEND);
}

void CDevice::Render_End(void)
{
	m_pSprite->End();
	m_pD3dDevice->EndScene();
	m_pD3dDevice->Present(NULL, NULL, NULL, NULL);
}

void CDevice::Render_End( HWND& _hWnd )
{
	m_pSprite->End();
	m_pD3dDevice->EndScene();
	m_pD3dDevice->Present(NULL, NULL, _hWnd, NULL);
}
void CDevice::Release(void)
{
	m_pFont->Release();
	m_pSprite->Release();
	m_pD3dDevice->Release();
	m_pD3D->Release();
}

CDevice::CDevice(void)
:m_pD3D(NULL)
,m_pD3dDevice(NULL)
{
}

CDevice::~CDevice(void)
{
	Release();
}
