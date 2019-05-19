// LogObvodyView.h : interface of the CLogObvodyView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGOBVODYVIEW_H__D5C610D5_931B_437B_8105_741856945EA1__INCLUDED_)
#define AFX_LOGOBVODYVIEW_H__D5C610D5_931B_437B_8105_741856945EA1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QuinMcCluskey.h"

#define FONT_COUNT	3	//pocet fontov

class CLogObvodyView : public CScrollView
{
protected: // create from serialization only
	CLogObvodyView();
	DECLARE_DYNCREATE(CLogObvodyView)

// Attributes
public:
	CLogObvodyDoc* GetDocument();
	CQuinMcCluskey *m_QuinMcCluskey;

	int m_FontHeight[FONT_COUNT];
	int m_FontWeight[FONT_COUNT];
	CFont m_Fonts[FONT_COUNT];

	//!!! ak tu nieco pridas -> zmen pocet stran na tlac (OnPreparePrinting)
	//co sa ma zobrazovat
	//BOOL m_ViewHelp;
	BOOL m_ViewTableFunctionValues;
	BOOL m_ViewCalculateSteps;
	BOOL m_ViewTableMPI;
	BOOL m_View_AND_OR_INVERT_Scheme;
	BOOL m_View_NAND_Scheme;
	BOOL m_View_NOR_Scheme;
	BOOL m_ViewFunctionBool;
	BOOL m_ViewFunctionSheffer;
	BOOL m_ViewFunctionPierce;

	//velkosti jednotlivych prvkov na vykreslovanie
	CSize m_SizeTableFunctionValues;
	CSize m_SizeCalculateSteps;
	CSize m_SizeTableMPI;
	CSize m_Size_AND_OR_INVERT_Scheme;
	CSize m_Size_NAND_Scheme;
	CSize m_Size_NOR_Scheme;
	CSize m_SizeFunctionBool;
	CSize m_SizeFunctionSheffer;
	CSize m_SizeFunctionPierce;

	//TRUE ak prebieha vypocet
	BOOL m_Calculating;

	//char m_HelpText[10000];	//help
	//int m_HelpTextCount;	//pocet znakov helpu

// Operations
public:
	//metody na vypis postupu pocitania
	void TextOut(CDC* pDC,char *str,int *posX,int *posY);

	//vypis implikantu (sucin premennych)
	CSize DrawImplikant(CDC* pDC,int startX,int startY,PSImplikant impl);

	//vypis implicentu (sucet premennych) !POZOR! vstup je implikant (negovanim vznikne implicent)
	CSize DrawImplicent(CDC* pDC,int startX,int startY,PSImplikant impl);

	CSize CalculateExtentImplikant(CDC* pDC,PSImplikant impl);

	//vykreslenie obdlznika
	void DrawRect(CDC *pDC,CRect rect);

	//vykresli clen s n vstupmi
	void DrawLogicalElement(CDC *pDC,int ofsX,int ofsY,int contactsCount,char elmType,BOOL invert);

	//vypise dany znak s diakritikou
	//CSize DrawSlovakChar(CDC *pDC,int x,int y,char znak);

	//vykresli konektor na dany bod
	void DrawConector(CDC *pDC,int x,int y);

	//zobrazenie popisu ovaladania progamu
	//CSize DrawHelp(CDC *pDC,int ofsX,int ofsY);

	//zobrazenie tabulky funkcnych hodnot
	CSize DrawTableOfFunctionValues(CDC *pDC,int ofsX,int ofsY);

	//zobrazenie postupu vypoctu
	CSize DrawCalculateSteps(CDC *pDC,int ofsX,int ofsY);

	//zobrazenie tabulky pokrytia (mriezka prostych implikantov
	CSize DrawTableMPI(CDC *pDC,int ofsX,int ofsY);

	//vykreslenie schemy AND-OR-INVERT (Booleaova algebra)
	CSize Draw_AND_OR_INVERT(CDC *pDC,int ofsX,int ofsY);
	CSize DrawFunctionAND_OR_INVERT(CDC *pDC,int ofsX,int ofsY);

	//vykreslenie schemy NAND (Shefferova algebra)
	CSize Draw_NAND(CDC *pDC,int ofsX,int ofsY);		//vykresli schemu pomocou NAND clenov
	CSize DrawFunctionNAND(CDC *pDC,int ofsX,int ofsY);	//vypise funkciu pomocou NAND clenov

	//vykreslenie schemy NOR (Pierceho algebra)
	CSize Draw_NOR(CDC *pDC,int ofsX,int ofsY);			//vykresli schemu pomocou NOR clenov
	CSize DrawFunctionNOR(CDC *pDC,int ofsX,int ofsY);	//vypise funkciu pomocou NOR clenov

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogObvodyView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLogObvodyView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CLogObvodyView)
	afx_msg void OnTableChange();
	afx_msg void OnViewSettings();
	afx_msg void OnFilePrintPreview();
	afx_msg void OnFileSave();
	afx_msg void OnHelpProgram();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in LogObvodyView.cpp
inline CLogObvodyDoc* CLogObvodyView::GetDocument()
   { return (CLogObvodyDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGOBVODYVIEW_H__D5C610D5_931B_437B_8105_741856945EA1__INCLUDED_)
