/*
 * \file EBPedestalOnlineTask.cc
 *
 * $Date: 2007/02/01 15:43:57 $
 * $Revision: 1.17 $
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

#include <DQM/EcalBarrelMonitorTasks/interface/EBPedestalOnlineTask.h>

using namespace cms;
using namespace edm;
using namespace std;

EBPedestalOnlineTask::EBPedestalOnlineTask(const ParameterSet& ps){

  init_ = false;

  for (int i = 0; i < 36 ; i++) {
    mePedMapG12_[i] = 0;
  }

}

EBPedestalOnlineTask::~EBPedestalOnlineTask(){

}

void EBPedestalOnlineTask::beginJob(const EventSetup& c){

  ievt_ = 0;

  DaqMonitorBEInterface* dbe = 0;

  // get hold of back-end interface
  dbe = Service<DaqMonitorBEInterface>().operator->();

  if ( dbe ) {
    dbe->setCurrentFolder("EcalBarrel/EBPedestalOnlineTask");
    dbe->rmdir("EcalBarrel/EBPedestalOnlineTask");
  }

}

void EBPedestalOnlineTask::setup(void){

  init_ = true;

  Char_t histo[200];

  DaqMonitorBEInterface* dbe = 0;

  // get hold of back-end interface
  dbe = Service<DaqMonitorBEInterface>().operator->();

  if ( dbe ) {
    dbe->setCurrentFolder("EcalBarrel/EBPedestalOnlineTask");

    dbe->setCurrentFolder("EcalBarrel/EBPedestalOnlineTask/Gain12");
    for (int i = 0; i < 36 ; i++) {
      sprintf(histo, "EBPOT pedestal SM%02d G12", i+1);
      mePedMapG12_[i] = dbe->bookProfile2D(histo, histo, 85, 0., 85., 20, 0., 20., 4096, 0., 4096., "s");
      dbe->tag(mePedMapG12_[i], i+1);
    }

  }

}

void EBPedestalOnlineTask::cleanup(void){

  DaqMonitorBEInterface* dbe = 0;

  // get hold of back-end interface
  dbe = Service<DaqMonitorBEInterface>().operator->();

  if ( dbe ) {
    dbe->setCurrentFolder("EcalBarrel/EBPedestalOnlineTask");

    dbe->setCurrentFolder("EcalBarrel/EBPedestalOnlineTask/Gain12");
    for ( int i = 0; i < 36; i++ ) {
      if ( mePedMapG12_[i] ) dbe->removeElement( mePedMapG12_[i]->getName() );
      mePedMapG12_[i] = 0;
    }

  }

  init_ = false;

}

void EBPedestalOnlineTask::endJob(void){

  LogInfo("EBPedestalOnlineTask") << "analyzed " << ievt_ << " events";

  if ( init_ ) this->cleanup();

}

void EBPedestalOnlineTask::analyze(const Event& e, const EventSetup& c){

  if ( ! init_ ) this->setup();

  ievt_++;

  Handle<EBDigiCollection> digis;
  e.getByLabel("ecalEBunpacker", digis);

  int nebd = digis->size();
  LogDebug("EBPedestalOnlineTask") << "event " << ievt_ << " digi collection size " << nebd;

  for ( EBDigiCollection::const_iterator digiItr = digis->begin(); digiItr != digis->end(); ++digiItr ) {

    EBDataFrame dataframe = (*digiItr);
    EBDetId id = dataframe.id();

    int ic = id.ic();
    int ie = (ic-1)/20 + 1;
    int ip = (ic-1)%20 + 1;

    int ism = id.ism();

    float xie = ie - 0.5;
    float xip = ip - 0.5;

    LogDebug("EBPedestalOnlineTask") << " det id = " << id;
    LogDebug("EBPedestalOnlineTask") << " sm, eta, phi " << ism << " " << ie << " " << ip;

    for (int i = 0; i < 3; i++) {

      EcalMGPASample sample = dataframe.sample(i);
      int adc = sample.adc();

      MonitorElement* mePedMap = 0;

      if ( sample.gainId() == 1 ) mePedMap = mePedMapG12_[ism-1];
      if ( sample.gainId() == 2 ) mePedMap = 0;
      if ( sample.gainId() == 3 ) mePedMap = 0;

      float xval = float(adc);

      if ( mePedMap ) mePedMap->Fill(xie, xip, xval);

    }

  }

}

