// LogObvodyDoc.cpp : implementation of the CLogObvodyDoc class
//

#include "stdafx.h"
#include "LogObvody.h"

#include "LogObvodyDoc.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogObvodyDoc

IMPLEMENT_DYNCREATE(CLogObvodyDoc, CDocument)

BEGIN_MESSAGE_MAP(CLogObvodyDoc, CDocument)
	//{{AFX_MSG_MAP(CLogObvodyDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogObvodyDoc construction/destruction

CLogObvodyDoc::CLogObvodyDoc()
{
	// TODO: add one-time construction code here
	m_BoolFunctionValues=NULL;
	m_VariablesCount=0;
	m_ValuesCount=0;
	m_Implikants=NULL;
	m_ImplikantsCount=0;
	m_ImplikantsInv=NULL;
	m_ImplikantsCountInv=0;

	m_GroupValues=NULL;
	m_GroupValuesCount=0;

	m_InterImplikants=NULL;

	m_TableRowsCount=0;
	m_TableColumnsCount=0;
	m_TableMPI=NULL;
	m_TableFunctionPoint=NULL;
	m_TableImplikants=NULL;
	m_TableRowsSelect=NULL;
}

CLogObvodyDoc::~CLogObvodyDoc()
{
	Kill();
}

void CLogObvodyDoc::Kill(void)
{
	DWORD k;
	SAFE_DELETE(m_BoolFunctionValues);
	m_VariablesCount=0;
	m_ValuesCount=0;

	for(k=0;k<m_ImplikantsCount;k++)KillImplikant(&m_Implikants[k]);
	SAFE_DELETE(m_Implikants);
	m_ImplikantsCount=0;
	for(k=0;k<m_ImplikantsCountInv;k++)KillImplikant(&m_ImplikantsInv[k]);
	SAFE_DELETE(m_ImplikantsInv);
	m_ImplikantsCountInv=0;

	//zoskupenie podla poctu jedniciek
	for(k=0;k<m_GroupValuesCount;k++)SAFE_DELETE(m_GroupValues[k].de);
	SAFE_DELETE(m_GroupValues);
	m_GroupValuesCount=0;
	
	//medzivysledky
	PSInterImplikant akt=m_InterImplikants,tmp;
	while(akt!=NULL){
		for(k=0;k<akt->countImplikants;k++)KillImplikant(&akt->implikants[k]);
		akt->countImplikants=0;
		SAFE_DELETE(akt->implikants);
		tmp=akt->next;
		SAFE_DELETE(akt);
		akt=tmp;
	}
	m_InterImplikants=NULL;

	//tabulka MPI
	SAFE_DELETE(m_TableMPI);
	for(k=0;k<m_TableRowsCount;k++)KillImplikant(&m_TableImplikants[k]);
	SAFE_DELETE(m_TableImplikants);
	SAFE_DELETE(m_TableFunctionPoint);
	SAFE_DELETE(m_TableRowsSelect);
	m_TableRowsCount=0;
	m_TableColumnsCount=0;
}

BOOL CLogObvodyDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CLogObvodyDoc serialization

void CLogObvodyDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CLogObvodyDoc diagnostics

#ifdef _DEBUG
void CLogObvodyDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLogObvodyDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CLogObvodyDoc commands
