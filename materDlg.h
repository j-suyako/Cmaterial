//
#if !defined(AFX_MATERDLG_H__5E14C0EB_C70A_458D_B689_E14242AAA589__INCLUDED_)
#define AFX_MATERDLG_H__5E14C0EB_C70A_458D_B689_E14242AAA589__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif

#include "afxwin.h"


// CMaterDlg dialog

class CMaterDlg : public CDialog
{
	DECLARE_DYNAMIC(CMaterDlg)

public:
	CMaterDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~CMaterDlg();

// Dialog Data
	enum { IDD = IDD_MATERINFO };

	double	m_dConcretF;
	double	m_dMortarF;

	double m_dMortarE;
	double m_dMortarV;
	double m_dMortarFt;
	double m_dMortarFc;
	double m_dInterfaceFt;
	double m_dInterfaceFc;

	double m_dMortarD;
	double m_dAggD;

	double m_dBondRatio;

	double m_dNormal;
	double m_dWeibull;

	double m_dInterfaceE;
	double m_dInterfaceV;
	double m_dAggE;
	double m_dAggV;

	int m_iDistributionType;
	int m_iAggRigid;

	 BOOL m_bMatVariation;  //是否考虑材料变异性

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	// Generated message map functions
	//{{AFX_MSG(CMaterDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnDismatVaria();
	afx_msg void OnNormal();
	afx_msg void OnWeibull();
	afx_msg void OnAggrigid();
	afx_msg void OnAggdisrigid();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()	

};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MATERDLG_H__5E14C0EB_C70A_458D_B689_E14242AAA589__INCLUDED_)
