
// StarToolDoc.h : CStarToolDoc Ŭ������ �������̽�
//


#pragma once


class CStarToolDoc : public CDocument
{
protected: // serialization������ ��������ϴ�.
	CStarToolDoc();
	DECLARE_DYNCREATE(CStarToolDoc)

// Ư���Դϴ�.
public:

// �۾��Դϴ�.
public:

// �������Դϴ�.
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);

// �����Դϴ�.
public:
	virtual ~CStarToolDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ������ �޽��� �� �Լ�
protected:
	DECLARE_MESSAGE_MAP()
};


