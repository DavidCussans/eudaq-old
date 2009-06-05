 // PixelmanProducerMFCDlg.cpp : implementation file
//

#include "stdafx.h"
#include "PixelmanProducerMFC.h"
#include "PixelmanProducerMFCDlg.h"
 
#include "eudaq/OptionParser.hh"
#include "eudaq/Logger.hh"
#include "TimepixProducer.h"
#include "TimePixDAQStatus.h"
#include <limits>


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


short _stdcall Inp32(short PortAddress);
void _stdcall Out32(short PortAddress, short data);
//TimepixProducer* producer;



//char* sThlMaskFilePath;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CPixelmanProducerMFCDlg dialog


IMPLEMENT_DYNAMIC(CPixelmanProducerMFCDlg, CDialog)

CPixelmanProducerMFCDlg::CPixelmanProducerMFCDlg(CWnd* pParent/*=NULL*/)
	: CDialog(CPixelmanProducerMFCDlg::IDD, pParent), timePixDaqStatus(0x378),
		m_AcquisitionActive(false), m_producer(0)
{
	m_csThlFilePath.Empty();
	m_csMaskFilePath.Empty();
	m_csTestBitMaskFilePath.Empty();
	m_csThhorModeMaskFilePath.Empty();
	//m_lptPort.m_bHex = true;
	mpxCurrSel = 0;
	infDouble  = std::numeric_limits<double>::infinity();

	// Inititalise the mutexes
    pthread_mutex_init( &m_AcquisitionActiveMutex, 0 );
	pthread_mutex_init( &m_producer_mutex, 0);

}



CPixelmanProducerMFCDlg::~CPixelmanProducerMFCDlg()	
{
	//pixelmanProducerCreated = false;
	//if(producerStarted==TRUE)
	//	producer->SetDone(true);
	delete m_producer;
	
	//this->DialogBoxDelete(this);

    pthread_mutex_destroy( &m_AcquisitionActiveMutex );	
    pthread_mutex_destroy( &m_producer_mutex );	
}


void CPixelmanProducerMFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_hostname);
	DDX_Control(pDX, IDC_SPINACQCOUNT, m_SpinAcqCount);
	DDX_Control(pDX, IDC_ACQCOUNT, m_AcqCount);
	DDX_Control(pDX, IDC_EDIT3, m_AcqTime);
	DDX_Control(pDX, IDC_THLMSKLABEL, m_ThlMaskLabel);
	DDX_Control(pDX, IDC_THLASCIIMASK, m_AsciiThlAdjFile);
	DDX_Control(pDX, IDC_CHIPSELECT, m_chipSelect);
	DDX_Control(pDX, IDC_WRITEASCIIMASKS, m_writeMask);
	DDX_Control(pDX, IDC_CONNECT, m_connect);
	DDX_Control(pDX, IDC_EDIT2, m_parPortAddress);
	DDX_Control(pDX, IDC_COMMANDS, m_commHistRunCtrl);
	
}


BEGIN_MESSAGE_MAP(CPixelmanProducerMFCDlg, CDialog)
	/*ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP*/
	ON_BN_CLICKED(IDCANCEL, &CPixelmanProducerMFCDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_QUIT, &CPixelmanProducerMFCDlg::OnBnClickedQuit)

	ON_BN_CLICKED(IDC_THLASCIIMASK, &CPixelmanProducerMFCDlg::OnBnClickedThlAsciiMask)
	ON_BN_CLICKED(IDC_WRITEASCIIMASKS, &CPixelmanProducerMFCDlg::OnBnClickedWriteMasks)

	ON_CBN_SELCHANGE(IDC_CHIPSELECT, &CPixelmanProducerMFCDlg::OnCbnSelchangeChipselect)
	//ON_WM_TIMER()

	ON_BN_CLICKED(IDC_BUTTON1, &CPixelmanProducerMFCDlg::OnBnClickedButton1)
	//ON_EN_CHANGE(IDC_EDIT2, &CPixelmanProducerMFCDlg::OnEnChangeParPortAddr)
	ON_BN_CLICKED(ID_CONNECT, &CPixelmanProducerMFCDlg::OnBnClickedConnect)
END_MESSAGE_MAP()


// CPixelmanProducerMFCDlg message handlers


BOOL CPixelmanProducerMFCDlg::OnInitDialog()
{	//char ** argv = 0;
	SetTimer(1,1000,NULL);
	CDialog::OnInitDialog();
	//OnPaint;

	m_hostname.SetWindowText("mortimer.physik.uni-freiburg.de");	
	
	m_AcqCount.SetWindowText("0");
	m_AcqTime.SetWindowText("0");
	m_SpinAcqCount.SetRange(0, 10000);
	m_SpinAcqCount.SetBuddy(&m_AcqCount);
	m_commHistRunCtrl.AddString("Producer Started");
	producerStarted = false;

	
	for (int i = 0; i<mpxCount; i++)
	{
		static CString chipBuffer;
		chipBuffer.Format("Chip %i",(mpxDevId+i)->chipNo);
		m_chipSelect.AddString(chipBuffer);
		m_chipSelect.SetCurSel(0);
	}

	m_parPortAddress.SetWindowText("0x378");

	m_ThlMaskLabel.SetWindowText("No ASCII Mask selected");
	


	
	//sprintf(buffer, "mpxCount %i deviceId %d chipNo. %d", this->mpxCount, this->mpxDevId[mpxCount-1].deviceId, this->mpxDevId[mpxCount-1].chipNo);
	//MessageBox(buffer,"Test der deviceId", 0);

	return true;
	
}




// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CPixelmanProducerMFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CPixelmanProducerMFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

UINT mpxCtrlPerformAcqLoopThread(LPVOID pParam)
{
	CPixelmanProducerMFCDlg* pMainWnd = (CPixelmanProducerMFCDlg*) pParam;

	bool executeMainLoop = true;
	do
	{
		switch (pMainWnd->getProducer()->PopCommand())
		{
			case TimepixProducer::NONE :
				break; // don't do anything
			
			case TimepixProducer::START_RUN :
				pMainWnd->getProducer()->SetRunStatusFlag(TimepixProducer::RUN_ACTIVE);
				break;

			case TimepixProducer::STOP_RUN :
				pMainWnd->getProducer()->SetRunStatusFlag(TimepixProducer::RUN_STOPPED);
				break;
			
			case TimepixProducer::TERMINATE:
				executeMainLoop = false;
				break;
			
			case TimepixProducer::CONFIGURE:
				// nothing to do here, is done in the communication thread
				break;
			
			case TimepixProducer::RESET:	
				// currently ignored
				break;

			default: 
				AfxMessageBox("Invalid Command", MB_ICONERROR, 0);  // invalid command, this should not be possible

		}

		// if run is active read the next event
		if (pMainWnd->getProducer()->GetRunStatusFlag() == TimepixProducer::RUN_ACTIVE )
		{
			pMainWnd->mpxCtrlPerformTriggeredFrameAcqTimePixProd();	
		}
	} while (executeMainLoop);
	pMainWnd->enablePixelManProdAcqControls();
	
	return 0;
}


void CPixelmanProducerMFCDlg::OnBnClickedConnect()
{
	disablePixelManProdAcqControls();
		
	eudaq::OptionParser op("TimePix Producer", "0.0", "The TimePix Producer");
	eudaq::Option<std::string> rctrl(op, "r", "runcontrol", "tcp://"+m_hostname.getStdStr()+":44000", "address",
                                   "The address of the RunControl application");
	eudaq::Option<std::string> level(op, "l", "log-level", "NONE", "level",
                                   "The minimum level for displaying log messages locally");
	eudaq::Option<std::string> name (op, "n", "name", "TimePix", "string",
                                   "The name of this Producer");
	//op.Parse(argv);
    EUDAQ_LOG_LEVEL(level.Value());
	
	pthread_mutex_lock( & m_producer_mutex );
		m_producer = new TimepixProducer(name.Value(), rctrl.Value(), this);
	pthread_mutex_unlock( & m_producer_mutex );

	CWinThread* pThread = AfxBeginThread(mpxCtrlPerformAcqLoopThread, this);
	
	producerStarted = true;
	m_commHistRunCtrl.AddString(_T("Connected"));
	
	//MessageBox("Goodbye", "HaveFun", NULL);


}




void CPixelmanProducerMFCDlg::OnBnClickedCancel()
{
	//delete timePixDaqStatus;
	
	//producer->SetDone(true);
	//delete getProducer();	
}

void CPixelmanProducerMFCDlg::OnBnClickedQuit()
{
	//producer->SetDone(true);
	DialogBoxDelete(this);
}



void CPixelmanProducerMFCDlg::OnBnClickedThlAsciiMask()
{ 
	if (m_AsciiThlAdjFile.GetCheck() == BST_CHECKED)
	{
		//static CString m_strFilePath;
		
		
		//static CString errorStr;

		CFileDialog m_ThlMask(TRUE,NULL, NULL, OFN_DONTADDTORECENT | OFN_ENABLESIZING, NULL, this,0);
		
		if(m_ThlMask.DoModal()==IDOK)
		{
			m_csThlFilePath.Empty();
			m_csThlFilePath = m_ThlMask.GetPathName();
			m_ThlMaskLabel.SetWindowText(m_ThlMask.GetFileName());
		}
	}
	else
	{
		m_csThlFilePath.Empty();
		m_ThlMaskLabel.SetWindowText(m_csThlFilePath);
	}
		
		
}



void CPixelmanProducerMFCDlg::OnBnClickedWriteMasks()
{
	CT2CA sThlMaskFilePath(m_csThlFilePath);
	static CString errorStr;
	static DEVID devId;
	static int retval;
	devId = (mpxDevId+mpxCurrSel)->deviceId;
	
	
	
	//strcpy_s(sThlMaskFilePath,strlen(sThlMaskFilePath),m_csThlFilePath);
//	m_ThlMaskLabel.SetWindowText(m_csThlFilePath.GetFileName());
	
//	sThlMaskFilePath = new char[m_strFilePath.GetLength()+1];
//	strcpy_s(sThlMaskFilePath,strlen(sThlMaskFilePath),m_strFilePath);
//	m_ThlMaskLabel.SetWindowText(m_ThlMask.GetFileName());
	

	retval = mpxCtrlLoadPixelsCfgAscii(devId, NULL , NULL, sThlMaskFilePath , NULL, 0);
		if (retval != 0)
		{
			errorStr.Format("MpxMgrError: %i", retval);
			AfxMessageBox(errorStr, MB_ICONERROR, 0);
		}
}



void CPixelmanProducerMFCDlg::OnCbnSelchangeChipselect()
{
	mpxCurrSel = m_chipSelect.GetCurSel();
}


//starting mpxCtrlPerformFrameAcq in own thread (DEVID devId, int numberOfFrames, 
//						   double timeOfEachAcq, u32 fileFlags,
//						   const char *fileName);

UINT mpxCtrlPerformFrameAcqThread(LPVOID pParam)//thread der zur normalen Acq. geh�rt
{
	CPixelmanProducerMFCDlg* pMainWnd = (CPixelmanProducerMFCDlg*) pParam;
	
	int threadRetVal = 0;
	int retval, retval2;
	
	pMainWnd->disablePixelManProdAcqControls();


	DEVID devId = pMainWnd->mpxDevId[pMainWnd->mpxCurrSel].deviceId;
	
	int numberOfFrames = pMainWnd->m_AcqCount.getInt();
	int timeOfEachAcq = pMainWnd->m_AcqTime.getInt();
	
	i16 *  databuffer = pMainWnd->mpxDevId[pMainWnd->mpxCurrSel].databuffer;            
	u32 sizeOfDataBuffer = pMainWnd->mpxDevId[pMainWnd->mpxCurrSel].sizeOfDataBuffer;
	retval = pMainWnd->mpxCtrlPerformFrameAcq(devId, numberOfFrames, timeOfEachAcq, NULL, NULL);
	retval2 = pMainWnd->mpxCtrlGetFrame16(devId, databuffer, sizeOfDataBuffer, 0);
	for(u32 i = 256*256; i<sizeOfDataBuffer; i++)
		if (databuffer[i]>0)
			databuffer[i];

			
	
	pMainWnd->enablePixelManProdAcqControls();
	
	if (retval <= retval2)
		threadRetVal = retval;
	else
		threadRetVal = retval2;
	
	return 0;
	
}

	
int CPixelmanProducerMFCDlg::mpxCtrlPerformFrameAcqTimePixProd()
{
	static int retval;
	static CString errorStr;
	static int trigger;


	
	CWinThread* pThread = AfxBeginThread(mpxCtrlPerformFrameAcqThread,this);
	
	/*if (_threadRetVal != 0)
		{
			errorStr.Format("MpxMgrError: %i", retval);
			AfxMessageBox(errorStr, MB_ICONERROR, 0);
		}
		
	return _threadRetVal;*/
	//return value of mpxCtrlPerformFrameAcqThread is per Defualt
	return 0;
}





UINT mpxCtrlPerformTriggeredFrameAcqThread(LPVOID pParam)
{	
	
	CPixelmanProducerMFCDlg* pMainWnd = (CPixelmanProducerMFCDlg*) pParam;
	
	int threadRetVal = 0;
	int retval, retval2;
	static CString errorStr;

	//pMainWnd->disablePixelManProdAcqControls();

	DEVID devId = pMainWnd->mpxDevId[pMainWnd->mpxCurrSel].deviceId;
	i16 *  databuffer = pMainWnd->mpxDevId[pMainWnd->mpxCurrSel].databuffer;            
	u32 sizeOfDataBuffer = pMainWnd->mpxDevId[pMainWnd->mpxCurrSel].sizeOfDataBuffer;
	retval = pMainWnd->mpxCtrlPerformFrameAcq(devId, 1, pMainWnd->infDouble, NULL, NULL);
	retval2 = pMainWnd->mpxCtrlGetFrame16(devId, databuffer, sizeOfDataBuffer, 0);
	
	/////////////

	//more negative Mpx-Error will be returned, zero is okay
	if (retval <= retval2)
		threadRetVal = retval;
	else
		threadRetVal = retval2;
	
	if (threadRetVal<0)
		{
			errorStr.Format("MpxMgrError: %i", threadRetVal);
			AfxMessageBox(errorStr, MB_ICONERROR, 0);
		}

	pMainWnd->enablePixelManProdAcqControls();
	
	return 0;
}




int CPixelmanProducerMFCDlg::mpxCtrlPerformTriggeredFrameAcqTimePixProd()
{
	static int retval, waitForTrigger, numberOfLoops;
	static CString errorStr;
	
	//MessageBox("PerformTriggeredFrameAcqTimePixProd", "PerformTriggeredFrameAcqTimePixProd", 0);
	retval = -1;
	waitForTrigger = -1;
	numberOfLoops = 0;
	
	CWinThread* pThread = AfxBeginThread(mpxCtrlPerformTriggeredFrameAcqThread,this);
	// wait until the acquisition realy is active before lowering the busy flag.
    // (The flag is set via callback function and can be accessed with getAcquisitionActive())
	while(getAcquisitionActive() == false)
			{
				Sleep(1);
			}
	// getProducer()->Event(mpxDevId[mpxCurrSel].databuffer, mpxDevId[mpxCurrSel].sizeOfDataBuffer);
	timePixDaqStatus.parPortSetBusyLineLow();
		
	while (waitForTrigger == -1)
	{
		
			
		
		{
			
			numberOfLoops++;
		}
		waitForTrigger = mpxWaitForTrigger();
	}
	
		
	

	
	
	
	
	if (waitForTrigger<0)
		{
			errorStr.Format("MpxMgrError: %i", retval);
			AfxMessageBox(errorStr, MB_ICONERROR, 0);
		}

	// wait for the mpxCtrl thread to finish
/*	DWORD dwRet;
	do	
	{
		dwRet = WaitForSingleObject(pThread->m_hThread, 100);
		if(dwRet == WAIT_FAILED)
		{
			errorStr.Format("Error waiting for mpxCtrl thread to finish: %li", dwRet);
			AfxMessageBox(errorStr, MB_ICONERROR, 0);			
		}
	} while	((dwRet != WAIT_OBJECT_0) && (dwRet != WAIT_FAILED));
	
	// now data readout is finished, we can clear the AcquisitonActive flag and return
	*/	
	clearAcquisitionActive();
	return retval;

	

	
}
	
int CPixelmanProducerMFCDlg::mpxWaitForTrigger()
{
	DEVID devId = this->mpxDevId[this->mpxCurrSel].deviceId;
	
	//static i16 *  databuffer = mpxDevId[mpxCurrSel].databuffer;            
	//u32 sizeOfDataBuffer = mpxDevId[mpxCurrSel].sizeOfDataBuffer;

	if(timePixDaqStatus.parPortCheckTriggerLine() == HIGH)
	{
		timePixDaqStatus.parPortSetBusyLineHigh();
		// wait until the shutter has closed again
		// (unfortunately 1 ms is the shortest we can get :-( )
		Sleep(1);
		return mpxCtrlTriggerType(devId, TRIGGER_ACQSTOP);
	}
	else
		return  -1;
}

		
	
void CPixelmanProducerMFCDlg::OnTimer(UINT_PTR nIDEvent)
{
	int trigger = this->timePixDaqStatus.parPortCheckTriggerLine();
	if(trigger==1)
		MessageBox("Hallo", "Boss", 0);
	CDialog::OnTimer(nIDEvent);

}
	



/*int CPixelmanProducerMFCDlg::mpxCtrlGetFrame32TimePixProd()
{
	static int retval;
	static CString errorStr;
	
	
	CWinThread* pThread = AfxBeginThread(mpxCtrlPerformFrameAcqThread,this);
	
	if (retval != 0)
		{
			errorStr.Format("MpxMgrError: %i", retval);
			AfxMessageBox(errorStr, MB_ICONERROR, 0);
		}
	return retval;
	
	//return value of mpxCtrlPerformFrameAcqThread is per Defualt
	}*/
void CPixelmanProducerMFCDlg::OnBnClickedButton1()
{	
	/*static CString hexBuffer;
	
	int testHexBuffer = m_parPortAddress.GetValue();;
	timePixDaqStatus.parPortUpdateAddress(testHexBuffer);
	

	int busyLineStatus = this->timePixDaqStatus.parPortCheckBusyLine();
	if (busyLineStatus == HIGH)
		timePixDaqStatus.parPortSetBusyLineLow();
	else
		timePixDaqStatus.parPortSetBusyLineHigh();*/
	this->mpxCtrlPerformTriggeredFrameAcqTimePixProd();
}



void CPixelmanProducerMFCDlg::OnEnChangeParPortAddr()
{
	static int hexBuffer;
	hexBuffer = m_parPortAddress.GetValue();
	timePixDaqStatus.parPortUpdateAddress(hexBuffer);
}

void  CPixelmanProducerMFCDlg::setAcquisitionActive()
{
	pthread_mutex_lock( &m_AcquisitionActiveMutex);
		m_AcquisitionActive = true;
	pthread_mutex_unlock( &m_AcquisitionActiveMutex);
}

void  CPixelmanProducerMFCDlg::clearAcquisitionActive()
{
	pthread_mutex_lock( &m_AcquisitionActiveMutex);
		m_AcquisitionActive = false;
	pthread_mutex_unlock( &m_AcquisitionActiveMutex);
}

bool  CPixelmanProducerMFCDlg::getAcquisitionActive()
{	
	bool retval;
	pthread_mutex_lock( &m_AcquisitionActiveMutex);
		retval = m_AcquisitionActive;
	pthread_mutex_unlock( &m_AcquisitionActiveMutex);
	return retval;
}

void CPixelmanProducerMFCDlg::disablePixelManProdAcqControls()
{
	m_connect.EnableWindow(false);
	m_AsciiThlAdjFile.EnableWindow(false);
	m_writeMask.EnableWindow(false);
	m_chipSelect.EnableWindow(false);
	m_AcqCount.EnableWindow(false);
	m_AcqTime.EnableWindow(false);
	m_SpinAcqCount.EnableWindow(false);
}

void CPixelmanProducerMFCDlg::enablePixelManProdAcqControls()
{	
	m_connect.EnableWindow(true);
	m_AsciiThlAdjFile.EnableWindow(true);
	m_writeMask.EnableWindow(true);
	m_chipSelect.EnableWindow(true);
	m_AcqCount.EnableWindow(true);
	m_AcqTime.EnableWindow(true);
	m_SpinAcqCount.EnableWindow(true);
}

TimepixProducer * CPixelmanProducerMFCDlg::getProducer()
{
	TimepixProducer * retval;
	pthread_mutex_lock( & m_producer_mutex );
		retval = m_producer;
	pthread_mutex_unlock( & m_producer_mutex );
	return retval;

}

void CPixelmanProducerMFCDlg::deleteProducer()
{
	pthread_mutex_lock( & m_producer_mutex );
		delete m_producer;
	pthread_mutex_unlock( & m_producer_mutex );
}
