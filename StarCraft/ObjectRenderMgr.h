#pragma once

class CGameObject;

class CObjectRenderMgr
{
public:
	static void SelectUnitUIRender(CGameObject* pObject1
		, int _iSelectNumber, float _fSelectPos
		, int _iBarNumber, float _fBarPos);

	static void SelectBuildUIRender(CGameObject* pObject1
		, int _iSelectNumber, float _fSelectPos
		, int _iBarNumber, float _fBarPos);
public:
	CObjectRenderMgr();
	~CObjectRenderMgr();
};

