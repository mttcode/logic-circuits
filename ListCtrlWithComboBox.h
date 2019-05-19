#if !defined(AFX_LISTCTRLWITHCOMBOBOX_H__FD0F6EFC_91A0_41CE_9B39_A32F6A164DF8__INCLUDED_)
#define AFX_LISTCTRLWITHCOMBOBOX_H__FD0F6EFC_91A0_41CE_9B39_A32F6A164DF8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListCtrlWithComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CListCtrlWithComboBox window

class CListCtrlWithComboBox : public CListCtrl
{
// Construction
public:
	CListCtrlWithComboBox();

// Attributes
public:
	CComboBox *m_ComboBox;

// Operations
public:
	void CreateColumns(DWORD countOfVariables);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlWithComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CListCtrlWithComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CListCtrlWithComboBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_LISTCTRLWITHCOMBOBOX_H__FD0F6EFC_91A0_41CE_9B39_A32F6A164DF8__INCLUDED_)
