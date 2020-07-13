// MyForm.cpp : 구현 파일입니다.
//

#include "stdafx.h"
#include "StarTool.h"
#include "MyForm.h"


// CMyForm

IMPLEMENT_DYNCREATE(CMyForm, CFormView)

CMyForm::CMyForm()
	: CFormView(CMyForm::IDD)
{

}

CMyForm::~CMyForm()
{
	Release();
}

void CMyForm::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_IMAGE_PATH, m_ListBoxImagePath);
}

BEGIN_MESSAGE_MAP(CMyForm, CFormView)
	ON_BN_CLICKED(IDC_BUTTON_MAPTOOL, &CMyForm::OnBnClickedButtonMaptool)
	ON_BN_CLICKED(IDC_BUTTON_UNITTOOL, &CMyForm::OnBnClickedButtonUnittool)
	ON_BN_CLICKED(IDC_BUTTON_BUILDTOOL, &CMyForm::OnBnClickedButtonBuildtool)
	ON_WM_DROPFILES()
	ON_BN_CLICKED(IDC_BUTTON_SAVE_IMAGE, &CMyForm::OnBnClickedButtonSaveImage)
	ON_BN_CLICKED(IDC_BUTTON_LOAD_IMAGE, &CMyForm::OnBnClickedButtonLoadImage)
	ON_BN_CLICKED(IDC_BUTTON_OBJECTTOOL, &CMyForm::OnBnClickedButtonObjecttool)
END_MESSAGE_MAP()


// CMyForm 진단입니다.

#ifdef _DEBUG
void CMyForm::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CMyForm::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG


// CMyForm 메시지 처리기입니다.

void CMyForm::OnBnClickedButtonMaptool()
{
	if(m_MapTool.GetSafeHwnd() == NULL)
		m_MapTool.Create(IDD_MAPTOOL);

	m_MapTool.ShowWindow(SW_SHOW);
}

void CMyForm::OnBnClickedButtonUnittool()
{
	if (m_UnitTool.GetSafeHwnd() == NULL)
		m_UnitTool.Create(IDD_UNITTOOL);

	m_UnitTool.ShowWindow(SW_SHOW);
}

void CMyForm::OnBnClickedButtonBuildtool()
{
	if (m_BuildTool.GetSafeHwnd() == NULL)
		m_BuildTool.Create(IDD_BUILDTOOL);

	m_BuildTool.ShowWindow(SW_SHOW);
}

void CMyForm::OnBnClickedButtonObjecttool()
{
	if (m_ObjectTool.GetSafeHwnd() == NULL)
		m_ObjectTool.Create(IDD_OBJECTTOOL);

	m_ObjectTool.ShowWindow(SW_SHOW);
}



void CMyForm::OnDropFiles(HDROP hDropInfo)
{
	CFormView::OnDropFiles(hDropInfo);

	UpdateData(TRUE);
	
	Release();
	m_ListBoxImagePath.ResetContent();

	int iFileNum = DragQueryFile(hDropInfo, -1, NULL, 0);

	TCHAR szFullPath[MAX_STR] = L"";
	m_ListBoxImagePath.ResetContent();

	int iCount = 0;

	for (int i = 0; i < iFileNum; ++i)
	{
		DragQueryFile(hDropInfo, i, (LPWSTR)szFullPath, MAX_STR);
		DirectoryInfo(szFullPath, m_listImagePath);

		wstring wstrCombine = L"";
		TCHAR szBuf[MIN_STR] = L"";

		list<IMAGE_PATH*>::iterator iter = m_listImagePath.begin();
		list<IMAGE_PATH*>::iterator iter_end = m_listImagePath.end();

		for (; iter != iter_end; ++iter)
		{
			_itow_s((*iter)->iCount, szBuf, 10);

			wstrCombine = (*iter)->szObjKey;
			wstrCombine += L"|";
			wstrCombine += (*iter)->szStateKey;
			wstrCombine += L"|";
			wstrCombine += szBuf;
			wstrCombine += L"|";
			wstrCombine += (*iter)->szPath;

			m_ListBoxImagePath.AddString(wstrCombine.c_str());
		}
	}

	UpdateData(FALSE);
}

void CMyForm::Release(void)
{
	list<IMAGE_PATH*>::iterator iter = m_listImagePath.begin();
	list<IMAGE_PATH*>::iterator iter_end = m_listImagePath.end();

	for (; iter != iter_end; ++iter)
		SAFE_DELETE(*iter);

	list<IMAGE_PATH*> Temp;
	m_listImagePath.swap(Temp);
}

void CMyForm::OnBnClickedButtonSaveImage()
{
	if (m_ListBoxImagePath.GetCount() <= NULL)
		return;

	IXMLDOMDocument2Ptr pDoc;

	MSXML2::IXMLDOMProcessingInstructionPtr	pPl;

	pDoc.CreateInstance(__uuidof(MSXML2::DOMDocument));

	//Root
	MSXML2::IXMLDOMElementPtr	pRootElement_IMAGE;

	//Element
	MSXML2::IXMLDOMElementPtr	pElement_OBJKEY;
	MSXML2::IXMLDOMElementPtr	pElement_STATEKEY;
	MSXML2::IXMLDOMElementPtr	pElement_PATH;
	MSXML2::IXMLDOMElementPtr	pElement_COUNT;

	//Line
	MSXML2::IXMLDOMElementPtr	pElement_LINE;

	MSXML2::IXMLDOMTextPtr	pTextObjKey;
	MSXML2::IXMLDOMTextPtr	pTextStateKey;
	MSXML2::IXMLDOMTextPtr	pTextPath;
	MSXML2::IXMLDOMTextPtr	pTextCount;

	pPl = pDoc->createProcessingInstruction(
		L"xml", L"version=\"1.0\" encoding=\"euc-kr\"");

	pRootElement_IMAGE = pDoc->createElement(L"IMAPE_PATH");

	for (list<IMAGE_PATH*>::iterator iter = m_listImagePath.begin();
		iter != m_listImagePath.end(); ++iter)
	{
		pElement_OBJKEY		= pDoc->createElement(L"OBJKEY");
		pElement_STATEKEY	= pDoc->createElement(L"STATEKEY");
		pElement_PATH		= pDoc->createElement(L"PATH");
		pElement_COUNT		= pDoc->createElement(L"COUNT");

		pElement_LINE		= pDoc->createElement(L"IMAGE");

		pTextObjKey		= pDoc->createTextNode((*iter)->szObjKey);
		pTextStateKey	= pDoc->createTextNode((*iter)->szStateKey);
		pTextPath		= pDoc->createTextNode((*iter)->szPath);
		pTextCount		= pDoc->createTextNode((_bstr_t)(*iter)->iCount);

		pDoc->appendChild(pPl);

		pElement_OBJKEY->appendChild(pTextObjKey);
		pElement_STATEKEY->appendChild(pTextStateKey);
		pElement_PATH->appendChild(pTextPath);
		pElement_COUNT->appendChild(pTextCount);

		pRootElement_IMAGE->appendChild(pElement_LINE);

		pElement_LINE->appendChild(pElement_OBJKEY);
		pElement_LINE->appendChild(pElement_STATEKEY);
		pElement_LINE->appendChild(pElement_PATH);
		pElement_LINE->appendChild(pElement_COUNT);
	}

	pDoc->appendChild(pRootElement_IMAGE);

	if (pDoc->save((_variant_t)L"../Data/ImagePath.xml") == S_OK)
	{
		MessageBox(L"Xml저장이 완료 되었습니다!", L"저장 성공");
	}
	else
		MessageBox(L"실패함!", L"저장 실패");
}


void CMyForm::OnBnClickedButtonLoadImage()
{
	Release();
	m_ListBoxImagePath.ResetContent();

	UpdateData(TRUE);

	CoInitialize(NULL);

	MSXML2::IXMLDOMDocument2Ptr pDoc;

	pDoc.CreateInstance(__uuidof(DOMDocument40));

	pDoc->async = VARIANT_FALSE;
	pDoc->validateOnParse = VARIANT_TRUE;
	pDoc->resolveExternals = VARIANT_TRUE;

	if (pDoc->load((_variant_t)"../Data/ImagePath.xml") == E_FAIL)
	{
		MessageBox(L"Xml Load Failed", L"Xml Load Failed", MB_OK);
		return;
	}

	MSXML2::IXMLDOMNodeListPtr pElementList = NULL;

	CString strTemp = L"";

	pElementList = pDoc->selectNodes(L"//IMAGE");

	int iCount = 0;
	int iTotal = 0;

	iTotal = pElementList->Getlength();

	for (int i = 0; i < iTotal; ++i)
	{
		IMAGE_PATH* pData = new IMAGE_PATH;
		CString strTemp = L"";

		strTemp
			= pElementList->Getitem(i)->selectNodes(L"//OBJKEY")->Getitem(i)->Gettext().operator LPCSTR();
		lstrcpy(pData->szObjKey, strTemp);

		strTemp
			= pElementList->Getitem(i)->selectNodes(L"//STATEKEY")->Getitem(i)->Gettext().operator LPCSTR();
		lstrcpy(pData->szStateKey, strTemp);
		strTemp
			= pElementList->Getitem(i)->selectNodes(L"//PATH")->Getitem(i)->Gettext().operator LPCSTR();
		lstrcpy(pData->szPath, strTemp);

		pData->iCount
			= atoi(pElementList->Getitem(i)->selectNodes(L"//COUNT")->Getitem(i)->Gettext());
		
		m_listImagePath.push_back(pData);
	}
		
	list<IMAGE_PATH*>::iterator iter = m_listImagePath.begin();
	list<IMAGE_PATH*>::iterator iter_end = m_listImagePath.end();

	for (; iter != iter_end; ++iter)
	{
		wstring wstrCombine = L"";
		TCHAR szBuf[MIN_STR] = L"";
		_itow_s((*iter)->iCount, szBuf, 10);

		wstrCombine = (*iter)->szObjKey;
		wstrCombine += L"|";
		wstrCombine += (*iter)->szStateKey;
		wstrCombine += L"|";
		wstrCombine += szBuf;
		wstrCombine += L"|";
		wstrCombine += (*iter)->szPath;

		m_ListBoxImagePath.AddString(wstrCombine.c_str());	
	}
	UpdateData(FALSE);
}
