/*
 * EUDAQMonitorCollection.cpp
 *
 *  Created on: Sep 27, 2011
 *      Author: stanitz
 */

#include "../include/EUDAQMonitorCollection.hh"
#include "OnlineMon.hh"

EUDAQMonitorCollection::EUDAQMonitorCollection()
{
	histos_init=false;
	cout << " Initialising EUDAQMonitor Collection" <<endl;
	CollectionType=EUDAQMONITOR_COLLECTION_TYPE;

}

EUDAQMonitorCollection::~EUDAQMonitorCollection()
{
	// TODO Auto-generated destructor stub
}

void EUDAQMonitorCollection::fillHistograms(const SimpleStandardEvent & ev)
{
	Fill(ev);
}


void EUDAQMonitorCollection::Reset()
{
	mymonhistos->Reset();
}



void EUDAQMonitorCollection::Write(TFile *file)
{
	if (file==NULL)
	{
		cout << "EUDAQMonitorCollection::Write File pointer is NULL"<<endl;
		exit(-1);
	}
	if (gDirectory!=NULL) //check if this pointer exists
	{
		gDirectory->mkdir("EUDAQMonitor");
		gDirectory->cd("EUDAQMonitor");
		mymonhistos->Write();
		gDirectory->cd("..");
	}
}

void EUDAQMonitorCollection::bookHistograms(const SimpleStandardEvent & simpev)
{
	if (_mon != NULL)
	{
		cout << "EUDAQMonitorCollection:: Monitor running in online-mode" << endl;
		string performance_folder_name="EUDAQ Monitor";
		_mon->getOnlineMon()->registerTreeItem((performance_folder_name+"/Number of Planes"));
		_mon->getOnlineMon()->registerHisto( (performance_folder_name+"/Number of Planes"),mymonhistos->getPlanes_perEventHisto());
		_mon->getOnlineMon()->registerTreeItem((performance_folder_name+"/Hits vs. Plane"));
		_mon->getOnlineMon()->registerHisto( (performance_folder_name+"/Hits vs. Plane"),mymonhistos->getHits_vs_PlaneHisto());
		_mon->getOnlineMon()->registerTreeItem((performance_folder_name+"/Hits vs. Event"));
		_mon->getOnlineMon()->registerHisto( (performance_folder_name+"/Hits vs. Event"),mymonhistos->getHits_vs_EventsTotal());

		_mon->getOnlineMon()->makeTreeItemSummary(performance_folder_name.c_str()); //make summary page

		stringstream namestring;
		string name_root=performance_folder_name+"/Planes";
		for (unsigned int i=0; i<mymonhistos->getNplanes(); i++)
		{
			stringstream namestring;
			namestring<<name_root<<"/Plane "<<i;
			_mon->getOnlineMon()->registerTreeItem(namestring.str());
			_mon->getOnlineMon()->registerHisto(namestring.str(),mymonhistos->getHits_vs_Events(i));
		}
		_mon->getOnlineMon()->makeTreeItemSummary(name_root.c_str()); //make summary page

	}
}

EUDAQMonitorHistos *EUDAQMonitorCollection::getEUDAQMonitorHistos()
{
	return mymonhistos;
}



void EUDAQMonitorCollection::Calculate(const unsigned int currentEventNumber)
{
}



void EUDAQMonitorCollection::Fill(const SimpleStandardEvent & simpev)
{
	if (histos_init==false)
	{
		mymonhistos=new EUDAQMonitorHistos(simpev);
		if (mymonhistos==NULL)
		{
			cout << "EUDAQMonitorCollection:: Can't book histograms " <<endl;
			exit(-1);
		}
		bookHistograms(simpev);
		histos_init=true;
	}
	mymonhistos->Fill(simpev);
}

