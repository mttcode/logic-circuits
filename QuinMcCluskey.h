// QuinMcCluskey.h: interface for the CQuinMcCluskey class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_QUINMCCLUSKEY_H__7F036B19_4F6B_41AB_91A6_203AE87CDF74__INCLUDED_)
#define AFX_QUINMCCLUSKEY_H__7F036B19_4F6B_41AB_91A6_203AE87CDF74__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "globals.h"
#include "LogObvodyDoc.h"
#include "StatusDlg.h"

#define STATUS_NONE		0	//vypocet prebieha
#define STATUS_STOP		1	//vypocet ma byt zastaveny
#define STATUS_FINISH	2	//vypocet uspesne dokonceny
#define STATUS_STOPPED	3	//vypocet bol zastaveny

class CQuinMcCluskey  
{
public:
	CQuinMcCluskey(CWnd *parent);
	virtual ~CQuinMcCluskey();

	//nastavi dokument, do ktoreho ma zapisovat medzivysledky
	void SetDocument(CLogObvodyDoc *pDoc){m_pDoc=pDoc;}

	//nastavi dialog na ukazovanie stavu
	void SetStatusDialog(CStatusDlg *dlg){m_pStatusDlg=dlg;}

	//nastavi novu funkciu a pripravi sa na vypocet (varti -1 pri chybe)
	int SetFunction(BYTE *values,DWORD variablesCount);

	//minimalizuje funkciu a vrati jednotlive implikanty a ich pocet
	void MinimalizeFunction(SImplikant *finalImplikants,DWORD *count);

private:
	//aby reagoval na vstupy
	void MessagePump(void);

	//usporiada pole podla poctu jednotiek do skupin a podla indexu v ramci skupiny
	void Sort(SImplikant *arr,DWORD arrLength);

	//TRUE ak je mozne 2 implikanty spojit
	BOOL CanMerge(SImplikant impl1,SImplikant impl2);

	//spoji 2 implikanty do jedneho
	void MergeImplikants(SImplikant *outimpl,SImplikant impl1,SImplikant impl2);

	//vyberie implikanty, tak aby bola pokryta cela fcia
	void UrcenieIDNF(SImplikant *prosteImplikanty,DWORD *pocetProstychImplikantov);

	//test ci su implikanty rovnake
	BOOL isIdentical(SImplikant *impl1,SImplikant *impl2);

	//test ci implikant s danym indexom je podstatny
	//BOOL isBaseImplikant(SImplikant *implikants,BYTE *tables,DWORD rowsCount,DWORD columnsCount,BOOL *rowsSelect,*columnSelect);

	//test ci implikant obsahuje dany index
	//BOOL ImplikantHasIndex(SImplikant *implikant,DWORD index);

public:
	//indikator stavu vypoctu
	int m_Status;

private:
	CLogObvodyDoc *m_pDoc;
	CWnd *m_pParent;			//rodicovske okno
	CStatusDlg *m_pStatusDlg;

	BYTE *m_BoolFunctionValues;	//hodnoty funkcie v bodoch (0/1 a 2=nedefinovana)
	DWORD m_VariablesCount;		//pocet premennych
	DWORD m_ValuesCount;		//pocet hodnot = 2^m_VariablesCount
};

#endif // !defined(AFX_QUINMCCLUSKEY_H__7F036B19_4F6B_41AB_91A6_203AE87CDF74__INCLUDED_)
