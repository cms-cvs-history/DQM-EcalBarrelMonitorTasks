/*
 * \file EBOccupancyTask.cc
 *
 * $Date: 2006/07/08 15:31:38 $
 * $Revision: 1.10 $
 * \author G. Della Ricca
 * \author G. Franzoni
 *
*/

#include <DQM/EcalBarrelMonitorTasks/interface/EBOccupancyTask.h>

EBOccupancyTask::EBOccupancyTask(const ParameterSet& ps){

  init_ = false;

  for (int i = 0; i < 36; i++) {
    meOccupancy_[i]    = 0;
    meOccupancyMem_[i] = 0;
  }

}

EBOccupancyTask::~EBOccupancyTask(){

}

void EBOccupancyTask::beginJob(const EventSetup& c){

  ievt_ = 0;

  DaqMonitorBEInterface* dbe = 0;

  // get hold of back-end interface
  dbe = Service<DaqMonitorBEInterface>().operator->();

  if ( dbe ) {
    dbe->setCurrentFolder("EcalBarrel/EBOccupancyTask");
    dbe->rmdir("EcalBarrel/EBOccupancyTask");
  }

}

void EBOccupancyTask::setup(void){

  init_ = true;

  Char_t histo[200];

  DaqMonitorBEInterface* dbe = 0;

  // get hold of back-end interface
  dbe = Service<DaqMonitorBEInterface>().operator->();

  if ( dbe ) {
    dbe->setCurrentFolder("EcalBarrel/EBOccupancyTask");

    for (int i = 0; i < 36; i++) {
      sprintf(histo, "EBOT occupancy SM%02d", i+1);
      meOccupancy_[i] = dbe->book2D(histo, histo, 85, 0., 85., 20, 0., 20.);
    }
    for (int i = 0; i < 36; i++) {
      sprintf(histo, "EBOT MEM occupancy SM%02d", i+1);
      meOccupancyMem_[i] = dbe->book2D(histo, histo, 10, 0., 10., 5, 0., 5.);
    }

  }

}

void EBOccupancyTask::cleanup(void){

  DaqMonitorBEInterface* dbe = 0;

  // get hold of back-end interface
  dbe = Service<DaqMonitorBEInterface>().operator->();

  if ( dbe ) {
    dbe->setCurrentFolder("EcalBarrel/EBOccupancyTask");

    for (int i = 0; i < 36; i++) {
      if ( meOccupancy_[i] ) dbe->removeElement( meOccupancy_[i]->getName() );
      meOccupancy_[i] = 0;
      if ( meOccupancyMem_[i] ) dbe->removeElement( meOccupancyMem_[i]->getName() );
      meOccupancyMem_[i] = 0;
    }

  }

  init_ = false;

}

void EBOccupancyTask::endJob(void) {

  LogInfo("EBOccupancyTask") << "analyzed " << ievt_ << " events";

  this->cleanup();

}

void EBOccupancyTask::analyze(const Event& e, const EventSetup& c){

  if ( ! init_ ) this->setup();

  ievt_++;

  Handle<EBDigiCollection> digis;
  //  e.getByLabel("ecalEBunpacker", digis);
  e.getByType(digis);

  int nebd = digis->size();
  LogDebug("EBOccupancyTask") << "event " << ievt_ << " digi collection size " << nebd;

  for ( EBDigiCollection::const_iterator digiItr = digis->begin(); digiItr != digis->end(); ++digiItr ) {

    EBDataFrame dataframe = (*digiItr);
    EBDetId id = dataframe.id();

    int ic = id.ic();
    int ie = (ic-1)/20 + 1;
    int ip = (ic-1)%20 + 1;

    int ism = id.ism();

    float xie = ie - 0.5;
    float xip = ip - 0.5;

    LogDebug("EBOccupancyTask") << " det id = " << id;
    LogDebug("EBOccupancyTask") << " sm, eta, phi " << ism << " " << ie << " " << ip;

    if ( xie <= 0. || xie >= 85. || xip <= 0. || xip >= 20. ) {
      LogWarning("EBOccupancyTask") << " det id = " << id;
      LogWarning("EBOccupancyTask") << " sm, eta, phi " << ism << " " << ie << " " << ip;
      LogWarning("EBOccupancyTask") << " xie, xip " << xie << " " << xip;
    }

    if ( meOccupancy_[ism-1] ) meOccupancy_[ism-1]->Fill(xie, xip);

  }

  Handle<EcalPnDiodeDigiCollection> PNs;
  //  e.getByLabel("ecalEBunpacker", PNs);
  e.getByType(PNs);

  // filling mem occupancy only for the 5 channels belonging
  // to a fully reconstructed PN's
  for ( EcalPnDiodeDigiCollection::const_iterator pnItr = PNs->begin(); pnItr != PNs->end(); ++pnItr ) {

    int   ism   = (*pnItr).id().iDCCId();
    float PnId  = (*pnItr).id().iPnId();
    PnId        = PnId - 0.5;
    float st    = 0.0;

    for (int chInStrip = 1; chInStrip <= 5; chInStrip++){
      if ( meOccupancyMem_[ism-1] ) {
         st = chInStrip - 0.5;
	 meOccupancyMem_[ism-1]->Fill(PnId, st);
      }
    }

  }

}

