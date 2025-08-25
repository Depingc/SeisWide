#pragma once


// CdlgSegyColor2 dialog
#include "segy.h"
#include "colorHelper.h"

#define NUMCOLORS_WORKING 11

class CdlgSegyColor2 : public CDialog
{
	DECLARE_DYNAMIC(CdlgSegyColor2)

public:
	CdlgSegyColor2(CWnd* pParent = NULL);   // standard constructor
	CdlgSegyColor2(CString aname);
	virtual ~CdlgSegyColor2();

// Dialog Data
	enum { IDD = IDD_DLG_SEGYCOLOR2 };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	int m_count;

	// these 3 values are for holding the coord of the IDC_BUTTON_CURVE button, updated in ::OnDrawItem()
	int m_pointLeft;
	int m_pointXCenter;
	int m_pointRight;
	bool m_isDrag; // controls drag and drop movements on screen
	int  m_iBtnDrag; // keep track of which button got mouse focus; -1=none

	// cannot move m_ampPercent[] and m_ampColorref[] to inside CColorHelper::AMPCOLORDYN struct
	// since they are the interpolated outputs to file. The AMPCOLORDYN struct can contain more than 11
	// elements, but only use 11 out of them. The colpallete.txt file can contain more than 11 colors as well.
	//float m_ampPercent[NUMCOLORS_WORKING];
	//COLORREF m_ampColorref[NUMCOLORS_WORKING];  // this is the 11 color indexes for the buttons.
	CRect m_rectButtons[NUMCOLORS_WORKING];  // this stores the color button sizes, for drag and drop calculations

	// these colors are read from file. Can be more or less than 11 colors
	// void percentInterpolate(int iFrom, int iTo);


	DECLARE_MESSAGE_MAP()
	afx_msg void OnBnClickedButtonBrowset();
	afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnBnClickedButtonCol0();
	afx_msg void OnBnClickedButtonCol10();
	afx_msg void OnBnClickedButtonCol9();
	afx_msg void OnBnClickedButtonCol1();
	afx_msg void OnBnClickedButtonCol2();
	afx_msg void OnBnClickedButtonCol3();
	afx_msg void OnBnClickedButtonCol4();
	afx_msg void OnBnClickedButtonCol5();
	afx_msg void OnBnClickedButtonCol6();
	afx_msg void OnBnClickedButtonCol7();
	afx_msg void OnBnClickedButtonCol8();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButtonUpdate();
	afx_msg void OnBnClickedButtonReload();
	afx_msg void OnBnClickedButtonReloadBw();
	afx_msg void OnBnClickedButtonLow();
	afx_msg void OnBnClickedButtonFull();
	afx_msg void OnBnClickedButtonLower();
	afx_msg void OnBnClickedButtonHigher();
	afx_msg void OnEnUpdateAmplevel0();
	afx_msg void OnEnUpdateAmplevel1();
	afx_msg void OnEnUpdateAmplevel2();
	afx_msg void OnEnUpdateAmplevel3();
	afx_msg void OnEnUpdateAmplevel4();
	afx_msg void OnEnUpdateAmplevel5();
	afx_msg void OnEnUpdateAmplevel6();
	afx_msg void OnEnUpdateAmplevel7();
	afx_msg void OnEnUpdateAmplevel8();
	afx_msg void OnEnUpdateAmplevel9();
	afx_msg void OnEnUpdateAmplevel10();
	afx_msg void OnBnClickedButtonMoreColor();
	afx_msg void OnBnClickedButtonBkRed();
	afx_msg void OnBnClickedButtonSmallbk();
	afx_msg void OnBnClickedButtonSmallbk2();

	CColorHelper::AMPCOLORDYN ampColor11;  // must be 11 colors

public:

	CString m_colFileName;  // this will be changed to fileName
	BOOL saveAmpColorToFile(CString colFileName);

	virtual INT_PTR DoModal();

	afx_msg void OnBnClickedButtonMoreColor3();
	afx_msg void OnBnClickedButtonReloadRedyellow();
	afx_msg void OnBnClickedButtonReloadRedyellow2();
};

