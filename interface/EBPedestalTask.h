#ifndef EBPedestalTask_H
#define EBPedestalTask_H

/*
 * \file EBPedestalTask.h
 *
 * $Date: 2005/10/16 12:35:44 $
 * $Revision: 1.9 $
 * \author G. Della Ricca
 *
*/

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include <FWCore/Framework/interface/EDAnalyzer.h>

#include <FWCore/Framework/interface/Event.h>
#include <FWCore/Framework/interface/MakerMacros.h>

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DQMServices/Core/interface/DaqMonitorBEInterface.h"
#include "DQMServices/Daemon/interface/MonitorDaemon.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include <DataFormats/EcalDigi/interface/EcalDigiCollections.h>
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include <DataFormats/EcalDetId/interface/EBDetId.h>
#include <DataFormats/EcalDigi/interface/EBDataFrame.h>

#include <iostream>
#include <fstream>
#include <vector>

using namespace cms;
using namespace std;

class EBPedestalTask: public edm::EDAnalyzer{

friend class EcalBarrelMonitorModule;

public:

/// Constructor
EBPedestalTask(const edm::ParameterSet& ps, DaqMonitorBEInterface* dbe);

/// Destructor
virtual ~EBPedestalTask();

protected:

/// Analyze
void analyze(const edm::Event& e, const edm::EventSetup& c);

// BeginJob
void beginJob(const edm::EventSetup& c);

// EndJob
void endJob(void);

private:

int ievt;

MonitorElement* mePedMapG01[36];
MonitorElement* mePedMapG06[36];
MonitorElement* mePedMapG12[36];

ofstream logFile;

};

#endif
