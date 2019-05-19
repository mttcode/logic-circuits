// LogObvodyDoc.h : interface of the CLogObvodyDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGOBVODYDOC_H__8B0179DA_FBB9_4C93_BA6E_9BD9E29543E6__INCLUDED_)
#define AFX_LOGOBVODYDOC_H__8B0179DA_FBB9_4C93_BA6E_9BD9E29543E6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "globals.h"

class CLogObvodyDoc : public CDocument
{
protected: // create from serialization only
	CLogObvodyDoc();
	DECLARE_DYNCREATE(CLogObvodyDoc)

// Attributes
public:
	//funkcia -> zoznam funkcnych hodnot
	BYTE *m_BoolFunctionValues;	//hodnoty funkcie v bodoch (0/1 a 2=nedefinovana)
	DWORD m_VariablesCount;		//pocet premennych
	DWORD m_ValuesCount;		//pocet hodnot = 2^m_VariablesCount

	//funkcia po minimalizacii - sucet implikantov (DNF)
	SImplikant *m_Implikants;	//implikanty tvoriace min. dnf
	DWORD m_ImplikantsCount;	//pocet tychto implikantov
	DWORD m_FunctionFlag;		//ak pocet implikantov je 0 -> potom toto udava ci f(x)=0 / 1

	//negovana funkcia po minimalizacii (koli KNF)
	SImplikant *m_ImplikantsInv;//implikanty tvoriace min. dnf
	DWORD m_ImplikantsCountInv;	//pocet tychto implikantov
	DWORD m_FunctionFlagInv;	//ak pocet implikantov je 0 -> potom toto udava ci f(x)=0 / 1

	//zobrazovanie medzivysledkov
	//rozdelenie jednotkovych a neurcenych bodov do skupin
	PSGroupValues m_GroupValues;//jednotlive skupiny
	DWORD m_GroupValuesCount;	//pocet skupin (zhodny s poctom premennych+1)

	//implikanty, ktore vznikaju pocas spajania (zretazeny zoznam_
	PSInterImplikant m_InterImplikants;

	//tabulka MPI + implikanty, ktore v nej vystupuju + horny riadok (dek. ekv. jednotkovych b.)
	DWORD m_TableRowsCount,m_TableColumnsCount;	//pocet riadkov a stlpcov
	BYTE *m_TableMPI;							//samotne hodnoty
	DWORD *m_TableFunctionPoint;				//dekadicke ekvivalenty
	PSImplikant m_TableImplikants;				//implikanty v jednotlivych riadkoch
	BYTE *m_TableRowsSelect;					//oznacenie riadkov

// Operations
public:
	void Kill(void);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogObvodyDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CLogObvodyDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CLogObvodyDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_LOGOBVODYDOC_H__8B0179DA_FBB9_4C93_BA6E_9BD9E29543E6__INCLUDED_)
