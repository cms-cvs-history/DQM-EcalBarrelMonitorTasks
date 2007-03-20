/*
 * \file EBTimingTask.cc
 *
 * $Date: 2007/03/13 10:53:18 $
 * $Revision: 1.7 $
 * \author G. Della Ricca
 *
*/

#include <iostream>
#include <fstream>
#include <vector>

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DQMServices/Core/interface/DaqMonitorBEInterface.h"
#include "DQMServices/Daemon/interface/MonitorDaemon.h"

#include "DataFormats/EcalRawData/interface/EcalRawDataCollections.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDigi/interface/EBDataFrame.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include <DQM/EcalBarrelMonitorTasks/interface/EBTimingTask.h>

using namespace cms;
using namespace edm;
using namespace std;

EBTimingTask::EBTimingTask(const ParameterSet& ps){

  init_ = false;

  EcalRawDataCollection_ = ps.getParameter<edm::InputTag>("EcalRawDataCollection");
  EcalUncalibratedRecHitCollection_ = ps.getParameter<edm::InputTag>("EcalUncalibratedRecHitCollection");

  for (int i = 0; i < 36 ; i++) {
    meTimeMap_[i] = 0;
  }

}

EBTimingTask::~EBTimingTask(){

}

void EBTimingTask::beginJob(const EventSetup& c){

  ievt_ = 0;

  DaqMonitorBEInterface* dbe = 0;

  // get hold of back-end interface
  dbe = Service<DaqMonitorBEInterface>().operator->();

  if ( dbe ) {
    dbe->setCurrentFolder("EcalBarrel/EBTimingTask");
    dbe->rmdir("EcalBarrel/EBTimingTask");
  }

}

void EBTimingTask::setup(void){

  init_ = true;

  Char_t histo[200];

  DaqMonitorBEInterface* dbe = 0;

  // get hold of back-end interface
  dbe = Service<DaqMonitorBEInterface>().operator->();

  if ( dbe ) {
    dbe->setCurrentFolder("EcalBarrel/EBTimingTask");

    for (int i = 0; i < 36 ; i++) {
      sprintf(histo, "EBTMT timing SM%02d", i+1);
      meTimeMap_[i] = dbe->bookProfile2D(histo, histo, 85, 0., 85., 20, 0., 20., 250, 0., 10., "s");
      dbe->tag(meTimeMap_[i], i+1);
    }

  }

}

void EBTimingTask::cleanup(void){

  DaqMonitorBEInterface* dbe = 0;

  // get hold of back-end interface
  dbe = Service<DaqMonitorBEInterface>().operator->();

  if ( dbe ) {
    dbe->setCurrentFolder("EcalBarrel/EBTimingTask");

    for ( int i = 0; i < 36; i++ ) {
      if ( meTimeMap_[i] ) dbe->removeElement( meTimeMap_[i]->getName() );
      meTimeMap_[i] = 0;
    }

  }

  init_ = false;

}

void EBTimingTask::endJob(void){

  LogInfo("EBTimingTask") << "analyzed " << ievt_ << " events";

  if ( init_ ) this->cleanup();

}

void EBTimingTask::analyze(const Event& e, const EventSetup& c){

  bool enable = false;
  map<int, EcalDCCHeaderBlock> dccMap;

  Handle<EcalRawDataCollection> dcchs;
  e.getByLabel(EcalRawDataCollection_, dcchs);

  for ( EcalRawDataCollection::const_iterator dcchItr = dcchs->begin(); dcchItr != dcchs->end(); ++dcchItr ) {

    EcalDCCHeaderBlock dcch = (*dcchItr);

    map<int, EcalDCCHeaderBlock>::iterator i = dccMap.find(dcch.id());
    if ( i != dccMap.end() ) continue;

    dccMap[dcch.id()] = dcch;

    if ( dcch.getRunType() == EcalDCCHeaderBlock::COSMIC ||
         dcch.getRunType() == EcalDCCHeaderBlock::LASER_STD ||
         dcch.getRunType() == EcalDCCHeaderBlock::BEAMH4 ||
         dcch.getRunType() == EcalDCCHeaderBlock::BEAMH2 ||
         dcch.getRunType() == EcalDCCHeaderBlock::MTCC ) enable = true;

  }

  if ( ! enable ) return;

  if ( ! init_ ) this->setup();

  ievt_++;

  Handle<EcalUncalibratedRecHitCollection> hits;
  e.getByLabel(EcalUncalibratedRecHitCollection_, hits); 

  int neh = hits->size();
  LogDebug("EBTimingTask") << "event " << ievt_ << " hits collection size " << neh;

  for ( EcalUncalibratedRecHitCollection::const_iterator hitItr = hits->begin(); hitItr != hits->end(); ++hitItr ) {
    
    EcalUncalibratedRecHit hit = (*hitItr);
    EBDetId id = hit.id(); 

    int ic = id.ic();
    int ie = (ic-1)/20 + 1;
    int ip = (ic-1)%20 + 1;

    int ism = id.ism();

    float xie = ie - 0.5;
    float xip = ip - 0.5;

    map<int, EcalDCCHeaderBlock>::iterator i = dccMap.find(ism);
    if ( i == dccMap.end() ) continue;

    if ( ! ( dccMap[ism].getRunType() == EcalDCCHeaderBlock::COSMIC ||
             dccMap[ism].getRunType() == EcalDCCHeaderBlock::LASER_STD ||
             dccMap[ism].getRunType() == EcalDCCHeaderBlock::BEAMH4 ||
             dccMap[ism].getRunType() == EcalDCCHeaderBlock::BEAMH2 ||
             dccMap[ism].getRunType() == EcalDCCHeaderBlock::MTCC ) ) continue;

    LogDebug("EBTimingTask") << " det id = " << id;
    LogDebug("EBTimingTask") << " sm, eta, phi " << ism << " " << ie << " " << ip;

    MonitorElement* meTimeMap = 0;

    meTimeMap = meTimeMap_[ism-1];

    float xval = hit.amplitude();
    if ( xval <= 0. ) xval = 0.0;
    float yval = hit.jitter();
    if ( yval <= 0. ) yval = 0.0;
    float zval = hit.pedestal();
    if ( zval <= 0. ) zval = 0.0;

    LogDebug("EBTimingTask") << " hit amplitude " << xval;
    LogDebug("EBTimingTask") << " hit jitter " << yval;
    LogDebug("EBTimingTask") << " hit pedestal " << zval;

    if ( meTimeMap ) meTimeMap->Fill(xie, xip, yval);

  }

}

