
// StarToolDoc.cpp : CStarToolDoc Ŭ������ ����
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


// CStarToolDoc ����/�Ҹ�

CStarToolDoc::CStarToolDoc()
{
	// TODO: ���⿡ ��ȸ�� ���� �ڵ带 �߰��մϴ�.

}

CStarToolDoc::~CStarToolDoc()
{
}

BOOL CStarToolDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: ���⿡ ���ʱ�ȭ �ڵ带 �߰��մϴ�.
	// SDI ������ �� ������ �ٽ� ����մϴ�.

	return TRUE;
}




// CStarToolDoc serialization

void CStarToolDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: ���⿡ ���� �ڵ带 �߰��մϴ�.
	}
	else
	{
		// TODO: ���⿡ �ε� �ڵ带 �߰��մϴ�.
	}
}


// CStarToolDoc ����

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


// CStarToolDoc ���
