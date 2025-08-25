#pragma once
// dlgLayerNodes.h : header file
//
#include "VelMod.h"

/////////////////////////////////////////////////////////////////////////////
// CdlgLayerNodes dialog

class CdlgLayerNodes : public CDialog
{
// Construction
public:
	CdlgLayerNodes(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CdlgLayerNodes)
	enum { IDD = IDD_LAYER_NODES };
//	CSpinButtonCtrl	m_spin;
	int		m_layerNum;
	CVelMod::VELMOD_X *m_modX;  // this would solve the prob: it contains both the depth and time domain values

	//}}AFX_DATA

	// will disable these
	//	float m_regionalSedVmin;
	//	float m_regionalSedVmax;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgLayerNodes)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
//	void CreateSpinCtrl();
//	CSpinButtonCtrl	m_Spin;
//	CRect			m_EditRect;

	// Generated message map functions
	//{{AFX_MSG(CdlgLayerNodes)
	float m_layerX;  // will point to m_modX->dist
	float m_layerY;  // will be calculated in OnInitDialog(), private
	float	m_layerV2; // in meters!!!
	// float m_waterDepth;
	virtual BOOL OnInitDialog();
	float m_layerV_backup;  // for undo operation
//	afx_msg void OnOutofmemorySpin(NMHDR* pNMHDR, LRESULT* pResult);
//	afx_msg void OnDeltaposSpin(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	// Velocity will be deducted by this amount of the current layer during time-to-depth conversion
	float m_anisotropy;
	BOOL m_apply4All;
	int m_anisotropy_layer0;
	afx_msg void OnBnClickedButtonAuto();
	afx_msg void OnBnClickedButtonUndo();
	CString m_strRegression;
	BOOL m_isRegressionChecked;
	inline BOOL isSedimentLayer2() {
		return m_modX->vTop[m_layerNum - 1] >= 1.53 && m_modX->vBot[m_layerNum - 1] <= 4.2;
	};  // only when it is a sediment layer, can it be regression-ed
	bool m_isTop;
	bool m_isTimeDomain;  // assumed true
	CString m_strRegressionX;

	double getLayerV();
	void setLayerV(double v);

	// void calcRegression();  // based on *m_modX
	// void calcRegressionOrig();  // based on *m_modX

	afx_msg void OnEnChangeLayerv();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
};
