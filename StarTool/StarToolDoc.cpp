
// StarToolDoc.cpp : CStarToolDoc 클래스의 구현
//

#include "stdafx.h"
#include "StarTool.h"

#include "StarToolDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CStarToolDoc

IMPLEMENT_DYNCREATE(CStarToolDoc, CDocument)

BEGIN_MESSAGE_MAP(CStarToolDoc, CDocument)
END_MESSAGE_MAP()


// CStarToolDoc 생성/소멸

CStarToolDoc::CStarToolDoc()
{
	// TODO: 여기에 일회성 생성 코드를 추가합니다.

}

CStarToolDoc::~CStarToolDoc()
{
}

BOOL CStarToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: 여기에 재초기화 코드를 추가합니다.
	// SDI 문서는 이 문서를 다시 사용합니다.

	return TRUE;
}




// CStarToolDoc serialization

void CStarToolDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 여기에 저장 코드를 추가합니다.
	}
	else
	{
		// TODO: 여기에 로딩 코드를 추가합니다.
	}
}


// CStarToolDoc 진단

#ifdef _DEBUG
void CStarToolDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CStarToolDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CStarToolDoc 명령
