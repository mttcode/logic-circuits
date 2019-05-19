// LogObvody.h : main header file for the LOGOBVODY application
//

#if !defined(AFX_LOGOBVODY_H__F683F26D_4E42_4BC8_9EC4_A8A787FDC063__INCLUDED_)
#define AFX_LOGOBVODY_H__F683F26D_4E42_4BC8_9EC4_A8A787FDC063__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CLogObvodyApp:
// See LogObvody.cpp for the implementation of this class
//

class CLogObvodyApp : public CWinApp
{
public:
	CLogObvodyApp();
	
	void SetPrintOrientation(int mode) ;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogObvodyApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
	//{{AFX_MSG(CLogObvodyApp)
	afx_msg void OnAppAbout();
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGOBVODY_H__F683F26D_4E42_4BC8_9EC4_A8A787FDC063__INCLUDED_)
