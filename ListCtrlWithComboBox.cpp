// ListCtrlWithComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "LogObvody.h"
#include "ListCtrlWithComboBox.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCtrlWithComboBox

CListCtrlWithComboBox::CListCtrlWithComboBox()
{
	m_ComboBox=NULL;
}

CListCtrlWithComboBox::~CListCtrlWithComboBox()
{
	if(m_ComboBox!=NULL)delete []m_ComboBox;
}

void CListCtrlWithComboBox::CreateColumns(DWORD countOfVariables)
{
	DWORD k,i;
	char name[256];
	InsertColumn(0,"DE",LVCFMT_CENTER,50,-1);
	for(k=0;k<countOfVariables;k++){
		sprintf(name,"x%d",k+1);
		InsertColumn(k+1,name,LVCFMT_CENTER,50,-1);
	}
	InsertColumn(countOfVariables+1,"y",LVCFMT_CENTER,50,-1);
	//nastavi format stlpcov
	LVCOLUMN column;
	column.mask=LVCF_FMT;
	column.fmt=LVCFMT_CENTER;
	for(k=0;k<countOfVariables+1;k++)SetColumn(k,&column);

	DWORD itemsCount=pow2(countOfVariables);
	for(k=0;k<itemsCount;k++){
		//sprintf(name,"%d",getBitValue(k,count-1));
		sprintf(name,"%d",k);
		InsertItem(k,name);

		//vyplenenie ostatnych hodnot
		for(i=0;i<countOfVariables;i++){
			sprintf(name,"%d",getBitValue(k,countOfVariables-i-1));
			SetItemText(k,i+1,name);
		}
		//nastavenie hodnoty funkcie
		//SetItemText(k,countOfVariables+1,"0");
	}
	m_ComboBox=new CComboBox[itemsCount];
	if(m_ComboBox==NULL){
		MessageBox("Nedostatok pamate","Error",MB_OK|MB_ICONERROR);
		return; //!!! CHYBA !!!
	}



	for(k=0;k<itemsCount;k++){
		CFont font;
		VERIFY(font.CreateFont(
		   10,                        // nHeight
		   0,                         // nWidth
		   0,                         // nEscapement
		   0,                         // nOrientation
		   FW_NORMAL,                 // nWeight
		   FALSE,                     // bItalic
		   FALSE,                     // bUnderline
		   0,                         // cStrikeOut
		   ANSI_CHARSET,              // nCharSet
		   OUT_DEFAULT_PRECIS,        // nOutPrecision
		   CLIP_DEFAULT_PRECIS,       // nClipPrecision
		   DEFAULT_QUALITY,           // nQuality
		   DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		   "Arial"));                 // lpszFacename

		CRect rect;
		int width;
		GetItemRect(k,&rect,LVIR_BOUNDS);
		width=GetColumnWidth(countOfVariables+1);
		rect.left=rect.right-width;
		rect.bottom+=100;
		m_ComboBox[k].Create(WS_CHILD|WS_VISIBLE|WS_VSCROLL|CBS_DROPDOWNLIST,rect,this,k+1000);
		//vytvorenie fontu
		m_ComboBox[k].SetFont(&font,TRUE);
	}
}

BEGIN_MESSAGE_MAP(CListCtrlWithComboBox, CListCtrl)
	//{{AFX_MSG_MAP(CListCtrlWithComboBox)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCtrlWithComboBox message handlers
