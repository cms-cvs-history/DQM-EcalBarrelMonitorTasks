/*
 * \file EBTriggerTowerTask.cc
 *
 * $Date: 2006/09/29 12:43:50 $
 * $Revision: 1.19 $
 * \author G. Della Ricca
 *
*/

#include <DQM/EcalBarrelMonitorTasks/interface/EBTriggerTowerTask.h>

EBTriggerTowerTask::EBTriggerTowerTask(const ParameterSet& ps){

  init_ = false;

  for (int i = 0; i < 36 ; i++) {
    meEtMap_[i] = 0;
    meVeto_[i] = 0;
    meFlags_[i] = 0;
    for (int j = 0; j < 68 ; j++) {
      meEtMapT_[i][j] = 0;
      meEtMapR_[i][j] = 0;
    }
  }

}

EBTriggerTowerTask::~EBTriggerTowerTask(){

}

void EBTriggerTowerTask::beginJob(const EventSetup& c){

  ievt_ = 0;

  DaqMonitorBEInterface* dbe = 0;

  // get hold of back-end interface
  dbe = Service<DaqMonitorBEInterface>().operator->();

  if ( dbe ) {
    dbe->setCurrentFolder("EcalBarrel/EBTriggerTowerTask");
    dbe->rmdir("EcalBarrel/EBTriggerTowerTask");
  }

}

void EBTriggerTowerTask::setup(void){

  init_ = true;

  Char_t histo[200];

  DaqMonitorBEInterface* dbe = 0;

  // get hold of back-end interface
  dbe = Service<DaqMonitorBEInterface>().operator->();

  if ( dbe ) {
    dbe->setCurrentFolder("EcalBarrel/EBTriggerTowerTask");

    for (int i = 0; i < 36 ; i++) {
      sprintf(histo, "EBTTT Et map SM%02d", i+1);
      meEtMap_[i] = dbe->bookProfile2D(histo, histo, 17, 0., 17., 4, 0., 4., 128, 0., 512., "s");
      sprintf(histo, "EBTTT FineGrainVeto SM%02d", i+1);
      meVeto_[i] = dbe->book3D(histo, histo, 17, 0., 17., 4, 0., 4., 2, 0., 2.);
      sprintf(histo, "EBTTT Flags SM%02d", i+1);
      meFlags_[i] = dbe->book3D(histo, histo, 17, 0., 17., 4, 0., 4., 8, 0., 8.);
    }

    dbe->setCurrentFolder("EcalBarrel/EBTriggerTowerTask/EnergyMaps");

    for (int i = 0; i < 36 ; i++) {
      for (int j = 0; j < 68 ; j++) {
        sprintf(histo, "EBTTT Et T SM%02d TT%02d", i+1, j+1);
        meEtMapT_[i][j] = dbe->book1D(histo, histo, 128, 0., 512.);
        sprintf(histo, "EBTTT Et R SM%02d TT%02d", i+1, j+1);
        meEtMapR_[i][j] = dbe->book1D(histo, histo, 128, 0., 512.);
      }
    }

  }

}

void EBTriggerTowerTask::cleanup(void){

  DaqMonitorBEInterface* dbe = 0;

  // get hold of back-end interface
  dbe = Service<DaqMonitorBEInterface>().operator->();

  if ( dbe ) {
    dbe->setCurrentFolder("EcalBarrel/EBTriggerTowerTask");

    for ( int i = 0; i < 36; i++ ) {
      if ( meEtMap_[i] ) dbe->removeElement( meEtMap_[i]->getName() );
      meEtMap_[i] = 0;
      if ( meVeto_[i] ) dbe->removeElement( meVeto_[i]->getName() );
      meVeto_[i] = 0; 
      if ( meFlags_[i] ) dbe->removeElement( meFlags_[i]->getName() );
      meFlags_[i] = 0;
    }

    dbe->setCurrentFolder("EcalBarrel/EBTriggerTowerTask/EnergyMaps");

    for ( int i = 0; i < 36; i++ ) {
      for ( int j = 0; j < 36; j++ ) {
        if ( meEtMapT_[i][j] ) dbe->removeElement( meEtMapT_[i][j]->getName() );
        meEtMapT_[i][j] = 0;
        if ( meEtMapR_[i][j] ) dbe->removeElement( meEtMapR_[i][j]->getName() );
        meEtMapR_[i][j] = 0;
      }      
    }

  }

  init_ = false;

}

void EBTriggerTowerTask::endJob(void){

  LogInfo("EBTriggerTowerTask") << "analyzed " << ievt_ << " events";

  if ( init_ ) this->cleanup();

}

void EBTriggerTowerTask::analyze(const Event& e, const EventSetup& c){

  if ( ! init_ ) this->setup();

  ievt_++;

  try {

    Handle<EcalTrigPrimDigiCollection> tpdigis;
    e.getByLabel("ecalEBunpacker", tpdigis);

    int nebtpd = tpdigis->size();
    LogDebug("EBTriggerTowerTask") << "event " << ievt_ << " trigger primitive digi collection size " << nebtpd;

    for ( EcalTrigPrimDigiCollection::const_iterator tpdigiItr = tpdigis->begin(); tpdigiItr != tpdigis->end(); ++tpdigiItr ) {

      EcalTriggerPrimitiveDigi data = (*tpdigiItr);
      EcalTrigTowerDetId id = data.id();

      int iet = id.ieta();
      int ipt = id.iphi();

      // phi_tower: change the range from global to SM-local
      ipt     = ( (ipt-1) % 4) +1;

      // phi_tower: range matters too
      //    if ( id.zside() >0)
      //      { ipt = 5 - ipt;      }

      int ismt = id.iDCC();

      int itt = 4*(iet-1)+(ipt-1)+1;

      float xiet = iet - 0.5;
      float xipt = ipt - 0.5;

      LogDebug("EBTriggerTowerTask") << " det id = " << id;
      LogDebug("EBTriggerTowerTask") << " sm, eta, phi " << ismt << " " << iet << " " << ipt;

      float xval;

      xval = data.compressedEt();
      if ( meEtMap_[ismt-1] ) meEtMap_[ismt-1]->Fill(xiet, xipt, xval);

      xval = 0.5 + data.fineGrain();
      if ( meVeto_[ismt-1] ) meVeto_[ismt-1]->Fill(xiet, xipt, xval);

      xval = 0.5 + data.ttFlag();
      if ( meFlags_[ismt-1] ) meFlags_[ismt-1]->Fill(xiet, xipt, xval);

      xval = data.compressedEt();
      if ( meEtMapT_[ismt-1][itt-1] ) meEtMapT_[ismt-1][itt-1]->Fill(xval);

    }

  } catch ( std::exception& ex) {
    LogDebug("EBTriggerTowerTask") << " EcalTrigPrimDigiCollection not in event.";
  }

  float xmap[36][68];

  for (int i = 0; i < 36 ; i++) {
    for (int j = 0; j < 68 ; j++) {
      xmap[i][j] = 0.;
    }
  }

  try {

    Handle<EcalUncalibratedRecHitCollection> hits;
    e.getByLabel("ecalUncalibHitMaker", "EcalUncalibRecHitsEB", hits);

    int nebh = hits->size();
    LogDebug("EBTriggerTowerTask") << "event " << ievt_ << " hits collection size " << nebh;

    for ( EcalUncalibratedRecHitCollection::const_iterator hitItr = hits->begin(); hitItr != hits->end(); ++hitItr ) {

      EcalUncalibratedRecHit hit = (*hitItr);
      EBDetId id = hit.id();

      int ic = id.ic();
      int ie = (ic-1)/20 + 1;
      int ip = (ic-1)%20 + 1;

      int ism = id.ism();

      int iet = 1 + ((ie-1)/5);
      int ipt = 1 + ((ip-1)/5);

      int itt = 4*(iet-1) + (ipt-1) + 1;

      LogDebug("EBTriggerTowerTask") << " det id = " << id;
      LogDebug("EBTriggerTowerTask") << " sm, eta, phi " << ism << " " << ie << " " << ip;

      float xval = 0.;

      xval = hit.amplitude();

      xval = xval * (1./16.) * TMath::Sin(2*TMath::ATan(TMath::Exp(-0.0174*(ie-0.5))));

      xmap[ism-1][itt-1] = xmap[ism-1][itt-1] + xval;

    }

    for (int i = 0; i < 36 ; i++) {
      for (int j = 0; j < 68 ; j++) {
         float xval = xmap[i][j];
         if ( meEtMapR_[i][j] && xval != 0 ) meEtMapR_[i][j]->Fill(xval);
      }
    }

  } catch ( std::exception& ex) {
    LogDebug("EBTriggerTowerTask") << " EcalUncalibratedRecHitCollection not in event.";
  }

}

