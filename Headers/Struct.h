#ifndef __STRUCT_H__
#define __STRUCT_H__

// Texture 관련 (##Texture)
typedef struct tagSringCmp
{
	const TCHAR* m_pString;

	template<class T>
	bool operator () (T data)
	{
		return !lstrcmp(data.first, m_pString);
	}

	tagSringCmp(const TCHAR* pKey)
		:m_pString(pKey){}

}STRING_CMP;

typedef struct tagTexture
{
	LPDIRECT3DTEXTURE9	pTexture;	//이미지 한장을 저장.
	D3DXIMAGE_INFO		ImageInfo;	//이미지 정보를 저장.
}TEX_INFO;

//INFO
typedef struct tagInfo
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vDir;
	D3DXVECTOR3 vLook;
	D3DXVECTOR3 vSize;

	D3DXMATRIX	matWorld;

	TCHAR		szObjKey[MIN_STR];
	TCHAR		szStateKey[MIN_STR];

	tagInfo(void)
	{
		D3DXMatrixIdentity(&matWorld);
		ZeroMemory(szObjKey, sizeof(TCHAR) * MIN_STR);
		ZeroMemory(szStateKey, sizeof(TCHAR) * MIN_STR);
	}

	tagInfo(D3DXVECTOR3 _vPos
		, D3DXVECTOR3 _vDir
		, D3DXVECTOR3 _vLook
		, D3DXVECTOR3 _vSize)
		: vPos(_vPos), vDir(_vDir), vLook(_vLook), vSize(_vSize)
	{
		D3DXMatrixIdentity(&matWorld);
	}

}INFO, *PINFO;

// 타일
typedef struct tagTile
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vSize;

	WORD		byDrawID;
	WORD		byOption;
}TILE, *PTILE;

typedef struct tagUnitInfo
{
	TCHAR	szName[MIN_STR];
	int		iAtt;
	int		iShield;
	int		iShieldMax;
	int		iHp;
	int		iHpMax;
	int		iMp;
	int		iMpMax;
	int		iMakeTime;
	int		iDrop;
	int		iPeople;
	int		iMineral;
	int		iGas;
	
	bool	bAttGround;
	bool	bAttArr;
	eUNIT_TYPE	eType;
}UNIT;

typedef struct tagBuildInfo
{
	TCHAR	szName[MIN_STR];
	int		iShield;
	int		iShieldMax;
	int		iHp;
	int		iHpMax;
	int		iMp;
	int		iMpMax;
	int		iMakeTime;
	int		iMineral;
	int		iGas;
	int		iSizeX;
	int		iSizeY;
}BUILD;

typedef struct tagTexturePath
{
	TCHAR szObjKey[MIN_STR];
	TCHAR szStateKey[MIN_STR];
	TCHAR szPath[MAX_STR];
	int	iCount;

	tagTexturePath(void)
		:iCount(0)
	{
		ZeroMemory(szObjKey, sizeof(TCHAR) * MAX_STR);
		ZeroMemory(szStateKey, sizeof(TCHAR) * MAX_STR);
		ZeroMemory(szPath, sizeof(TCHAR) * MAX_STR);
	}

}IMAGE_PATH;

typedef struct tagFrame
{
	float fFrame;
	float fCount;
	float fMax;

	tagFrame(float _fFrame = 0, float _fCount = 0, float _fMax = 0)
		:fFrame(_fFrame)
		, fCount(_fCount)
		, fMax(_fMax)
	{}

}FRAME;

typedef struct tagAstar
{
	tagAstar() {}
	tagAstar(int _G, int _H, int _vertex[2], tagAstar* _pNode)
		:G(_G), H(_H), pNode(_pNode)
	{
		vertex[0] = _vertex[0];
		vertex[1] = _vertex[1];
	}

	int		G;
	int		H;
	int vertex[2];
	tagAstar*	pNode;
}NODE;

typedef struct tagResourceInfo
{
	D3DXVECTOR3 vPos;
	D3DXVECTOR3 vSize;
	eRESOURCE_TYPE eType;
	int			   m_iMineral;
	int			   m_iGas;

	tagResourceInfo(void)
	{}

	tagResourceInfo(D3DXVECTOR3 _vPos
		, D3DXVECTOR3 _vSize)
		: vPos(_vPos), vSize(_vSize)
	{}
}RESOURCE;

#endif