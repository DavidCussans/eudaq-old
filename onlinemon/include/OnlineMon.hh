#ifndef ONLINE_MON_H
#define ONLINE_MON_H

#include <cmath>

//ROOT includes
#include <TSystem.h>
#include <TInterpreter.h>
#include <TQObject.h>
#include <RQ_OBJECT.h>
#include <TPRegexp.h>
#include <TObjString.h>
#include <TStopwatch.h>

//EUDAQ includes
#include "eudaq/Monitor.hh"
#include "eudaq/DetectorEvent.hh"
#include "eudaq/Logger.hh"
#include "eudaq/Utils.hh"
#include "eudaq/OptionParser.hh"

//Project Includes

#include "BaseCollection.hh"

#include "OnlineMonWindow.hh"
#include "OnlineHistograms.hh"
#include "SimpleStandardEvent.hh"
#include "EventSanityChecker.hh"

#include "CheckEOF.hh"

//STL includes
#include <string>

using namespace std;

class OnlineMonWindow;
class BaseCollection;
class CheckEOF;

class RootMonitor : private eudaq::Holder<int>,
                    //public TApplication,
                    //public TGMainFrame,
                    public eudaq::Monitor {
	RQ_OBJECT("RootMonitor")
protected:
	bool histos_booked;
	//HitmapCollection *hmCollection;
	//CorrelationCollection *corrCollection;
	std::vector <BaseCollection*> _colls;
	OnlineMonWindow *onlinemon;
	std::string rootfilename;
	int runnumber;
	bool _writeRoot;
	int _offline;
	CheckEOF _checkEOF;

	//bool _autoReset;
	
public:
 
	RootMonitor(const std::string & runcontrol, const std::string & datafile, int x, int y, int w, int h, int argc, int offline);
	~RootMonitor() {gApplication->Terminate();}
	void registerSensorInGUI(std::string name, int id);
  
 
	virtual void StartIdleing() { }
	OnlineMonWindow * getOnlineMon() { return onlinemon; }
  
	virtual void OnConfigure(const eudaq::Configuration & param) {
		std::cout << "Configure: " << param.Name() << std::endl;
		SetStatus(eudaq::Status::LVL_OK, "Configured (" + param.Name() + ")");
	}
	virtual void OnTerminate() {
		std::cout << "Terminating" << std::endl;
		sleep(1);
		gApplication->Terminate();
	}
	virtual void OnReset() {
		std::cout << "Reset" << std::endl;
		SetStatus(eudaq::Status::LVL_OK);
	}
	  virtual void OnStartRun(unsigned param);
	virtual void OnEvent(const eudaq::StandardEvent & ev);

	virtual void OnBadEvent(counted_ptr<eudaq::Event> ev) {
		EUDAQ_ERROR("Bad event type found in data file");
		std::cout << "Bad Event: " << *ev << std::endl;
	}
  
	virtual void OnStopRun();
	void setWriteRoot(const bool write) {_writeRoot = write; } 
	void autoReset(const bool reset);
	void setReduce(const unsigned int red);
	void setUpdate(const unsigned int up);
	
	void SetSnapShotDir(string s);
	string GetSnapShotDir();
private:
	string snapshotdir; //FIXME
	EventSanityChecker myevent; //FIXME
	TStopwatch my_event_processing_time;
  
};


#ifdef __CINT__
#pragma link C++ class RootMonitor-;
#endif

#endif