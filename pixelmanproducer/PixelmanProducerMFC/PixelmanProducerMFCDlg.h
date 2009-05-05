// PixelmanProducerMFCDlg.h : header file
//

#pragma once
//#define WINVER 0x0500
#include "resource.h"
#include "eudaq/DummyProducer.hh"
#include "ceditextended.h"
#include "afxcmn.h"
#include "afxwin.h"
#include "mpxpluginmgrapi.h"
#include "medipixChipId.h"
#include "hexedit.h"
#include "TimePixDAQStatus.h"



// CPixelmanProducerMFCDlg dialog
class CPixelmanProducerMFCDlg : public CDialog
{
	DECLARE_DYNAMIC(CPixelmanProducerMFCDlg)
// Construction
public:
	CPixelmanProducerMFCDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CPixelmanProducerMFCDlg();
	void (*DialogBoxDelete)(CWnd*);
	int (*mpxCtrlLoadPixelsCfgAscii) (DEVID devID, const char *maskBitFile,
						   const char *testBitFile, const char *thlFile,
						   const char *thhOrModeFile, BOOL loadDacs);
	int (*mpxCtrlPerformFrameAcq)(DEVID devId, int numberOfFrames, 
						   double timeOfEachAcq, u32 fileFlags,
						   const char *fileName);
	int (*mpxCtrlGetFrame16)(DEVID devId, i16 *buffer, u32 size,
							 u32 frameNumber);
	int (*mpxCtrlTriggerType)(DEVID devId, int trigger);

	int mpxCtrlPerformFrameAcqTimePixProd();
	int mpxCtrlPerformTriggeredFrameAcqTimePixProd();
	int mpxCtrlGetFrame32TimePixProd();
	int mpxWaitForTrigger();

		
	CSpinButtonCtrl m_SpinAcqCount;
	CEditExtended m_AcqCount;
	CEditExtended m_AcqTime;
	CEditExtended m_hostname;
	CHexEdit m_parPortAddress;
	int _threadRetVal;
	
	CComboBox m_chipSelect;
	
	CButton m_AsciiThlAdjFile;

	TimePixDAQStatus timePixDaqStatus;
	

	int mpxCount;//needed to now how long medipixChipId is
	int mpxCurrSel;
	medipixChipId* mpxDevId;
	double infDouble;

// Dialog Data
	enum { IDD = IDD_PIXELMANPRODUCERMFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
	protected:
	HICON m_hIcon;

	//DECLARE_MESSAGE_MAP()
protected:
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	
	
	


	DECLARE_MESSAGE_MAP()
	/*afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	*/
public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	
	
protected:
	CStatic m_ThlMaskLabel;
	CString m_csThlFilePath;
	CString m_csMaskFilePath;
	CString m_csTestBitMaskFilePath;
	CString m_csThhorModeMaskFilePath;


public:
	afx_msg void OnBnClickedQuit();
	afx_msg void OnBnClickedThlAsciiMask();
	afx_msg void OnBnClickedWriteMasks();
		
	afx_msg void OnCbnSelchangeChipselect();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	CButton m_writeMask;
	afx_msg void OnBnClickedButton1();
	
	afx_msg void OnEnChangeParPortAddr();
};
