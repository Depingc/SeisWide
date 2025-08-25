// dlgSegyInfo.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CdlgSegyInfo dialog
#include "LicenseCheck.h"

class CdlgSegyInfo : public CDialog
{
// Construction
public:
	CdlgSegyInfo(CWnd* pParent = NULL);   // standard constructor




// This block is for converting raw DOBS files into segy
#define HTONS(x) (((unsigned short)(x)>>8) | ((x) & 0x00FF)<<8)
#define HTONL(x) ((unsigned long)((((ulong)(x) & 0x000000ffU) << 24) | \
					(((unsigned long)(x) & 0x0000ff00U) <<  8) | \
					(((unsigned long)(x) & 0x00ff0000U) >>  8) | \
					(((unsigned long)(x) & 0xff000000U) >> 24)))
// #define MIL 1000000.


typedef struct {
	short physical_block_size;  // bytes 0-1
	short logical_record_size;
	short records;
	short not_used;
	short number_type;
	short bytes;  // bytes per number
	short channels; // number of channels
	short mux_length;
	short averages; // num of averages
	short x_units;
	short y_units;
	short sequence_number;
	short scaling;
	short hicut;
	short idummy[2]; // bytes 28-31

	float sampling_frequency; // bytes 32-35
	float hetro_frequency; // bytes 36-39
	float reference_level; // bytes 40-43
	float max_magnitude;   // bytes 44-47
	float gain_correction;  // bytes 48-51
	float fdummy[3];  // bytes 52-63

	struct{
		unsigned char number;
		unsigned char gain;
		}chan[4];  // bytes 64-71
	unsigned char bdummy[120];  // byte 72-191

	//the following ascii block can now be accessed as
	//header.ablk.raw[320], OR
	//header.ablk.fields.lat, ...fields.lon, ..etc...

	union{
		char raw[320];
		struct {
			//all the strings to follow should end with a CR ('\n') unless
			//specified as null terminated
			char time[20];			//0   -  19 eg "29-Jan-84 16:07:47"	<- NULL TERMINATED!! bytes 192-211
			char units[6];			//20  -  25 eg "VOLTS"	<- NULL TERMINATED!!
			char instrument[14];	//26  -  39 eg	"DOBS B - 1.04\n"
			char channel_mark[10];	//40  -  49 should be: "Channels:\n"
			char ch_1_name[22];		//50  -  71 should be: "1=HYDROPHONE         \n"
			char ch_2_name[22];		//72  -  93 should be: "2=VERTICAL GEO.      \n"
			char ch_3_name[22];		//94  - 115 should be: "3=HORIZONTAL GEO. 1  \n"
			char ch_4_name[22];		//116 - 137 should be: "4=HORIZONTAL GEO. 2  \n"
			char cruise[41];		//138 - 178 any string
			char line[41];			//179 - 219 any string
			char comment[61];		//220 - 280 any string
			char lat[18];			//281 - 298 should be of form: "LAT: ### ##.### X\n"
			char lon[18];			//299 - 316 should be of form: "LAT: ### ##.### X\n"
			char dummy[3];			//317 - 319
		} ascii;
	}ablk;
	
} Header;
// end of raw-segy conversion block





// Dialog Data
	//{{AFX_DATA(CdlgSegyInfo)
	enum { IDD = IDD_DLG_SEGYINFO };
	CString	m_timeSecName;
	// int		m_shot1;
	float	m_tmax;
	float	m_tmin;
	float	m_xmin;
	int		m_numSamp;
	int		m_sampInt;
	int		m_dataFormat;
	long	m_totNTrace;
	int		m_CDP;
	int		m_FFID;
	float	m_averageAmp;
	float	m_sDepth;
	float	m_gDepth;
	int		m_traceActive;
//	float m_value;
	float m_xShift;  // 
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CdlgSegyInfo)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void CalcSegyInfo(int trc);
	float GetAverageAmp(CString inFile);
	double GetMaxAmp(CString inFile);


	//int totDays(int nYear, int nDay, CString t);
	//long myRound(double x);
//	void showFileInNotePad(CStringA name);


	// Generated message map functions
	//{{AFX_MSG(CdlgSegyInfo)
	afx_msg void OnButton1();
	afx_msg void OnKillfocusViewUpdateTraceactive();
	virtual BOOL OnInitDialog();
	afx_msg void OnButton2();
//	afx_msg void OnButton3();
	afx_msg void OnButtonEbcdic();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void OnBrowseT();
public:
	afx_msg void OnButton8DumpMoreTraceInfo();
	afx_msg void OnBnClickedButtonShottab();
	//afx_msg void OnBnClickedButtonTest();
	//afx_msg void OnBnClickedButtonTest2();
	//afx_msg void OnBnClickedButton10();
	//afx_msg void OnBnClickedButton11();
	// afx_msg void OnBnClickedButtonSegygen();
	// afx_msg void OnBnClickedButton19();
	afx_msg void OnEnChangeEditValue();
	//afx_msg void OnBnClickedButton20();
	afx_msg void OnBnClickedButton20();
	BOOL m_isAuto;
	CString m_strValue;
	afx_msg void OnBnClickedButtonTrace();
};
