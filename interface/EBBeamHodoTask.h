#ifndef EBBeamHodoTask_H
#define EBBeamHodoTask_H

/*
 * \file EBBeamHodoTask.h
 *
 * $Date: 2006/06/26 22:51:29 $
 * $Revision: 1.5 $
 * \author G. Della Ricca
 * \author G. Franzoni
 *
*/

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DQMServices/Core/interface/DaqMonitorBEInterface.h"
#include "DQMServices/Daemon/interface/MonitorDaemon.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include <DQM/EcalBarrelMonitorTasks/interface/EBMUtilsTasks.h>

#include "DataFormats/EcalRawData/interface/EcalRawDataCollections.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDigi/interface/EBDataFrame.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"
#include "DataFormats/EcalRawData/interface/EcalDCCHeaderBlock.h"

#include "TBDataFormats/EcalTBObjects/interface/EcalTBCollections.h"

#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include <iostream>
#include <fstream>
#include <vector>

using namespace cms;
using namespace edm;
using namespace std;

class EBBeamHodoTask: public EDAnalyzer{

public:

/// Constructor
EBBeamHodoTask(const ParameterSet& ps);

/// Destructor
virtual ~EBBeamHodoTask();

protected:

/// Analyze
void analyze(const Event& e, const EventSetup& c);

/// BeginJob
void beginJob(const EventSetup& c);

/// EndJob
void endJob(void);

/// Setup
void setup(void);

/// Cleanup
void cleanup(void);

private:
 
string outputFile_;
 
 int ievt_;
 int LV1_;
 bool tableIsMoving_;
 bool resetNow_;
 int    cryInBeam_;
 int    previousCryInBeam_;
 int    cryInBeamCounter_;
 
 //  ME type I
MonitorElement* meHodoOcc_[4];
MonitorElement* meHodoRaw_[4];
MonitorElement* meHodoPosRecXY_;
MonitorElement* meHodoPosRecX_;
MonitorElement* meHodoPosRecY_;
MonitorElement* meHodoSloXRec_;
MonitorElement* meHodoSloYRec_;
MonitorElement* meHodoQuaXRec_;
MonitorElement* meHodoQuaYRec_;
MonitorElement* meTDCRec_;
MonitorElement* meHodoPosXMinusCaloPosXVsCry_;
MonitorElement* meHodoPosYMinusCaloPosYVsCry_;
MonitorElement* meTDCTimeMinusCaloTimeVsCry_;



 //  ME type II
MonitorElement* meEvsXRecProf_;
MonitorElement* meEvsYRecProf_;
MonitorElement* meEvsXRecHis_;
MonitorElement* meEvsYRecHis_;
MonitorElement* meCaloVsHodoXPos_;
MonitorElement* meCaloVsHodoYPos_;
MonitorElement* meCaloVsTDCTime_;

bool init_;

int smId;

};

#endif
