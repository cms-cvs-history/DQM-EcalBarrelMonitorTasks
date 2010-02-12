/*
 * \file EBTestPulseTask.cc
 *
 * $Date: 2009/10/26 17:33:48 $
 * $Revision: 1.111 $
 * \author G. Della Ricca
 * \author G. Franzoni
 *
*/

#include <iostream>
#include <fstream>
#include <vector>

#include "FWCore/ServiceRegistry/interface/Service.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DQMServices/Core/interface/MonitorElement.h"

#include "DQMServices/Core/interface/DQMStore.h"

#include "DataFormats/EcalRawData/interface/EcalRawDataCollections.h"
#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDigi/interface/EBDataFrame.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include <DQM/EcalCommon/interface/Numbers.h>

#include <DQM/EcalBarrelMonitorTasks/interface/EBTestPulseTask.h>

using namespace cms;
using namespace edm;
using namespace std;

EBTestPulseTask::EBTestPulseTask(const ParameterSet& ps){

  init_ = false;

  dqmStore_ = Service<DQMStore>().operator->();

  prefixME_ = ps.getUntrackedParameter<string>("prefixME", "");

  enableCleanup_ = ps.getUntrackedParameter<bool>("enableCleanup", false);

  mergeRuns_ = ps.getUntrackedParameter<bool>("mergeRuns", false);

  EcalRawDataCollection_ = ps.getParameter<edm::InputTag>("EcalRawDataCollection");
  EBDigiCollection_ = ps.getParameter<edm::InputTag>("EBDigiCollection");
  EcalPnDiodeDigiCollection_ = ps.getParameter<edm::InputTag>("EcalPnDiodeDigiCollection");
  EcalUncalibratedRecHitCollection_ = ps.getParameter<edm::InputTag>("EcalUncalibratedRecHitCollection");

  MGPAGains_.reserve(3);
  for ( unsigned int i = 1; i <= 3; i++ ) MGPAGains_.push_back(i);
  MGPAGains_ = ps.getUntrackedParameter<vector<int> >("MGPAGains", MGPAGains_);

  MGPAGainsPN_.reserve(2);
  for ( unsigned int i = 1; i <= 3; i++ ) MGPAGainsPN_.push_back(i);
  MGPAGainsPN_ = ps.getUntrackedParameter<vector<int> >("MGPAGainsPN", MGPAGainsPN_);

  for (int i = 0; i < 36; i++) {
    meShapeMapG01_[i] = 0;
    meAmplMapG01_[i] = 0;
    meShapeMapG06_[i] = 0;
    meAmplMapG06_[i] = 0;
    meShapeMapG12_[i] = 0;
    meAmplMapG12_[i] = 0;
    mePnAmplMapG01_[i] = 0;
    mePnPedMapG01_[i] = 0;
    mePnAmplMapG16_[i] = 0;
    mePnPedMapG16_[i] = 0;
  }


}

EBTestPulseTask::~EBTestPulseTask(){

}

void EBTestPulseTask::beginJob(void){

  ievt_ = 0;

  if ( dqmStore_ ) {
    dqmStore_->setCurrentFolder(prefixME_ + "/EBTestPulseTask");
    dqmStore_->rmdir(prefixME_ + "/EBTestPulseTask");
  }

}

void EBTestPulseTask::beginRun(const Run& r, const EventSetup& c) {

  Numbers::initGeometry(c, false);

  if ( ! mergeRuns_ ) this->reset();

}

void EBTestPulseTask::endRun(const Run& r, const EventSetup& c) {

}

void EBTestPulseTask::reset(void) {

  for (int i = 0; i < 36; i++) {
    if (find(MGPAGains_.begin(), MGPAGains_.end(), 1) != MGPAGains_.end() ) {
      if ( meShapeMapG01_[i] ) meShapeMapG01_[i]->Reset();
      if ( meAmplMapG01_[i] ) meAmplMapG01_[i]->Reset();
    }
    if (find(MGPAGains_.begin(), MGPAGains_.end(), 6) != MGPAGains_.end() ) {
      if ( meShapeMapG06_[i] ) meShapeMapG06_[i]->Reset();
      if ( meAmplMapG06_[i] ) meAmplMapG06_[i]->Reset();
    }
    if (find(MGPAGains_.begin(), MGPAGains_.end(), 12) != MGPAGains_.end() ) {
      if ( meShapeMapG12_[i] ) meShapeMapG12_[i]->Reset();
      if ( meAmplMapG12_[i] ) meAmplMapG12_[i]->Reset();
    }
    if (find(MGPAGainsPN_.begin(), MGPAGainsPN_.end(), 1) != MGPAGainsPN_.end() ) {
      if ( mePnAmplMapG01_[i] ) mePnAmplMapG01_[i]->Reset();
      if ( mePnPedMapG01_[i] ) mePnPedMapG01_[i]->Reset();
    }
    if (find(MGPAGainsPN_.begin(), MGPAGainsPN_.end(), 16) != MGPAGainsPN_.end() ) {
      if ( mePnAmplMapG16_[i] ) mePnAmplMapG16_[i]->Reset();
      if ( mePnPedMapG16_[i] ) mePnPedMapG16_[i]->Reset();
    }
  }

}

void EBTestPulseTask::setup(void){

  init_ = true;

  char histo[200];

  if ( dqmStore_ ) {
    dqmStore_->setCurrentFolder(prefixME_ + "/EBTestPulseTask");

    if (find(MGPAGains_.begin(), MGPAGains_.end(), 1) != MGPAGains_.end() ) {

      dqmStore_->setCurrentFolder(prefixME_ + "/EBTestPulseTask/Gain01");
      for (int i = 0; i < 36; i++) {
        sprintf(histo, "EBTPT shape %s G01", Numbers::sEB(i+1).c_str());
        meShapeMapG01_[i] = dqmStore_->bookProfile2D(histo, histo, 1700, 0., 1700., 10, 0., 10., 4096, 0., 4096., "s");
        meShapeMapG01_[i]->setAxisTitle("channel", 1);
        meShapeMapG01_[i]->setAxisTitle("sample", 2);
        meShapeMapG01_[i]->setAxisTitle("amplitude", 3);
        dqmStore_->tag(meShapeMapG01_[i], i+1);
        sprintf(histo, "EBTPT amplitude %s G01", Numbers::sEB(i+1).c_str());
        meAmplMapG01_[i] = dqmStore_->bookProfile2D(histo, histo, 85, 0., 85., 20, 0., 20., 4096, 0., 4096.*12., "s");
        meAmplMapG01_[i]->setAxisTitle("ieta", 1);
        meAmplMapG01_[i]->setAxisTitle("iphi", 2);
        dqmStore_->tag(meAmplMapG01_[i], i+1);
      }

    }

    if (find(MGPAGains_.begin(), MGPAGains_.end(), 6) != MGPAGains_.end() ) {

      dqmStore_->setCurrentFolder(prefixME_ + "/EBTestPulseTask/Gain06");
      for (int i = 0; i < 36; i++) {
        sprintf(histo, "EBTPT shape %s G06", Numbers::sEB(i+1).c_str());
        meShapeMapG06_[i] = dqmStore_->bookProfile2D(histo, histo, 1700, 0., 1700., 10, 0., 10., 4096, 0., 4096., "s");
        meShapeMapG06_[i]->setAxisTitle("channel", 1);
        meShapeMapG06_[i]->setAxisTitle("sample", 2);
        meShapeMapG06_[i]->setAxisTitle("amplitude", 3);
        dqmStore_->tag(meShapeMapG06_[i], i+1);
        sprintf(histo, "EBTPT amplitude %s G06", Numbers::sEB(i+1).c_str());
        meAmplMapG06_[i] = dqmStore_->bookProfile2D(histo, histo, 85, 0., 85., 20, 0., 20., 4096, 0., 4096.*12., "s");
        meAmplMapG06_[i]->setAxisTitle("ieta", 1);
        meAmplMapG06_[i]->setAxisTitle("iphi", 2);
        dqmStore_->tag(meAmplMapG06_[i], i+1);
      }

    }

    if (find(MGPAGains_.begin(), MGPAGains_.end(), 12) != MGPAGains_.end() ) {

      dqmStore_->setCurrentFolder(prefixME_ + "/EBTestPulseTask/Gain12");
      for (int i = 0; i < 36; i++) {
        sprintf(histo, "EBTPT shape %s G12", Numbers::sEB(i+1).c_str());
        meShapeMapG12_[i] = dqmStore_->bookProfile2D(histo, histo, 1700, 0., 1700., 10, 0., 10., 4096, 0., 4096., "s");
        meShapeMapG12_[i]->setAxisTitle("channel", 1);
        meShapeMapG12_[i]->setAxisTitle("sample", 2);
        meShapeMapG12_[i]->setAxisTitle("amplitude", 3);
        dqmStore_->tag(meShapeMapG12_[i], i+1);
        sprintf(histo, "EBTPT amplitude %s G12", Numbers::sEB(i+1).c_str());
        meAmplMapG12_[i] = dqmStore_->bookProfile2D(histo, histo, 85, 0., 85., 20, 0., 20., 4096, 0., 4096.*12., "s");
        meAmplMapG12_[i]->setAxisTitle("ieta", 1);
        meAmplMapG12_[i]->setAxisTitle("iphi", 2);
        dqmStore_->tag(meAmplMapG12_[i], i+1);
      }

    }

    dqmStore_->setCurrentFolder(prefixME_ + "/EBTestPulseTask/PN");

    if (find(MGPAGainsPN_.begin(), MGPAGainsPN_.end(), 1) != MGPAGainsPN_.end() ) {

      dqmStore_->setCurrentFolder(prefixME_ + "/EBTestPulseTask/PN/Gain01");
      for (int i = 0; i < 36; i++) {
        sprintf(histo, "EBTPT PNs amplitude %s G01", Numbers::sEB(i+1).c_str());
        mePnAmplMapG01_[i] = dqmStore_->bookProfile(histo, histo, 10, 0., 10., 4096, 0., 4096., "s");
        mePnAmplMapG01_[i]->setAxisTitle("channel", 1);
        mePnAmplMapG01_[i]->setAxisTitle("amplitude", 2);
        dqmStore_->tag(mePnAmplMapG01_[i], i+1);
        sprintf(histo, "EBTPT PNs pedestal %s G01", Numbers::sEB(i+1).c_str());
        mePnPedMapG01_[i] =  dqmStore_->bookProfile(histo, histo, 10, 0., 10., 4096, 0., 4096., "s");
        mePnPedMapG01_[i]->setAxisTitle("channel", 1);
        mePnPedMapG01_[i]->setAxisTitle("pedestal", 2);
        dqmStore_->tag(mePnPedMapG01_[i], i+1);
      }

    }

    if (find(MGPAGainsPN_.begin(), MGPAGainsPN_.end(), 16) != MGPAGainsPN_.end() ) {

      dqmStore_->setCurrentFolder(prefixME_ + "/EBTestPulseTask/PN/Gain16");
      for (int i = 0; i < 36; i++) {
        sprintf(histo, "EBTPT PNs amplitude %s G16", Numbers::sEB(i+1).c_str());
        mePnAmplMapG16_[i] = dqmStore_->bookProfile(histo, histo, 10, 0., 10., 4096, 0., 4096., "s");
        mePnAmplMapG16_[i]->setAxisTitle("channel", 1);
        mePnAmplMapG16_[i]->setAxisTitle("amplitude", 2);
        dqmStore_->tag(mePnAmplMapG16_[i], i+1);
        sprintf(histo, "EBTPT PNs pedestal %s G16", Numbers::sEB(i+1).c_str());
        mePnPedMapG16_[i] =  dqmStore_->bookProfile(histo, histo, 10, 0., 10., 4096, 0., 4096., "s");
        mePnPedMapG16_[i]->setAxisTitle("channel", 1);
        mePnPedMapG16_[i]->setAxisTitle("pedestal", 2);
        dqmStore_->tag(mePnPedMapG16_[i], i+1);
      }

    }

  }

}

void EBTestPulseTask::cleanup(void){

  if ( ! init_ ) return;

  if ( dqmStore_ ) {
    dqmStore_->setCurrentFolder(prefixME_ + "/EBTestPulseTask");

    if (find(MGPAGains_.begin(), MGPAGains_.end(), 1) != MGPAGains_.end() ) {

      dqmStore_->setCurrentFolder(prefixME_ + "/EBTestPulseTask/Gain01");
      for (int i = 0; i < 36; i++) {
        if ( meShapeMapG01_[i] ) dqmStore_->removeElement( meShapeMapG01_[i]->getName() );
        meShapeMapG01_[i] = 0;
        if ( meAmplMapG01_[i] ) dqmStore_->removeElement( meAmplMapG01_[i]->getName() );
        meAmplMapG01_[i] = 0;
      }

    }

    if (find(MGPAGains_.begin(), MGPAGains_.end(), 6) != MGPAGains_.end() ) {

      dqmStore_->setCurrentFolder(prefixME_ + "/EBTestPulseTask/Gain06");
      for (int i = 0; i < 36; i++) {
        if ( meShapeMapG06_[i] ) dqmStore_->removeElement( meShapeMapG06_[i]->getName() );
        meShapeMapG06_[i] = 0;
        if ( meAmplMapG06_[i] ) dqmStore_->removeElement( meAmplMapG06_[i]->getName() );
        meAmplMapG06_[i] = 0;
      }

    }

    if (find(MGPAGains_.begin(), MGPAGains_.end(), 12) != MGPAGains_.end() ) {

      dqmStore_->setCurrentFolder(prefixME_ + "/EBTestPulseTask/Gain12");
      for (int i = 0; i < 36; i++) {
        if ( meShapeMapG12_[i] ) dqmStore_->removeElement( meShapeMapG12_[i]->getName() );
        meShapeMapG12_[i] = 0;
        if ( meAmplMapG12_[i] ) dqmStore_->removeElement( meAmplMapG12_[i]->getName() );
        meAmplMapG12_[i] = 0;
      }

    }

    dqmStore_->setCurrentFolder(prefixME_ + "/EBTestPulseTask/PN");

    if (find(MGPAGainsPN_.begin(), MGPAGainsPN_.end(), 1) != MGPAGainsPN_.end() ) {

      dqmStore_->setCurrentFolder(prefixME_ + "/EBTestPulseTask/PN/Gain01");
      for (int i = 0; i < 36; i++) {
        if ( mePnAmplMapG01_[i] ) dqmStore_->removeElement( mePnAmplMapG01_[i]->getName() );
        mePnAmplMapG01_[i] = 0;
        if ( mePnPedMapG01_[i] ) dqmStore_->removeElement( mePnPedMapG01_[i]->getName() );
        mePnPedMapG01_[i] = 0;
      }

    }


    if (find(MGPAGainsPN_.begin(), MGPAGainsPN_.end(), 16) != MGPAGainsPN_.end() ) {

      dqmStore_->setCurrentFolder(prefixME_ + "/EBTestPulseTask/PN/Gain16");
      for (int i = 0; i < 36; i++) {
        if ( mePnAmplMapG16_[i] ) dqmStore_->removeElement( mePnAmplMapG16_[i]->getName() );
        mePnAmplMapG16_[i] = 0;
        if ( mePnPedMapG16_[i] ) dqmStore_->removeElement( mePnPedMapG16_[i]->getName() );
        mePnPedMapG16_[i] = 0;
      }

    }

  }

  init_ = false;

}

void EBTestPulseTask::endJob(void){

  LogInfo("EBTestPulseTask") << "analyzed " << ievt_ << " events";

  if ( enableCleanup_ ) this->cleanup();

}

void EBTestPulseTask::analyze(const Event& e, const EventSetup& c){

  bool enable = false;
  int runType[36];
  for (int i=0; i<36; i++) runType[i] = -1;
  int mgpaGain[36];
  for (int i=0; i<36; i++) mgpaGain[i] = -1;

  Handle<EcalRawDataCollection> dcchs;

  if ( e.getByLabel(EcalRawDataCollection_, dcchs) ) {

    for ( EcalRawDataCollection::const_iterator dcchItr = dcchs->begin(); dcchItr != dcchs->end(); ++dcchItr ) {

      if ( Numbers::subDet( *dcchItr ) != EcalBarrel ) continue;

      int ism = Numbers::iSM( *dcchItr, EcalBarrel );

      runType[ism-1] = dcchItr->getRunType();
      mgpaGain[ism-1] = dcchItr->getMgpaGain();

      if ( dcchItr->getRunType() == EcalDCCHeaderBlock::TESTPULSE_MGPA ||
           dcchItr->getRunType() == EcalDCCHeaderBlock::TESTPULSE_GAP ) enable = true;

    }

  } else {

    LogWarning("EBTestPulseTask") << EcalRawDataCollection_ << " not available";

  }

  if ( ! enable ) return;

  if ( ! init_ ) this->setup();

  ievt_++;

  Handle<EBDigiCollection> digis;

  if ( e.getByLabel(EBDigiCollection_, digis) ) {

    int nebd = digis->size();
    LogDebug("EBTestPulseTask") << "event " << ievt_ << " digi collection size " << nebd;

    for ( EBDigiCollection::const_iterator digiItr = digis->begin(); digiItr != digis->end(); ++digiItr ) {

      EBDetId id = digiItr->id();

      int ic = id.ic();
      int ie = (ic-1)/20 + 1;
      int ip = (ic-1)%20 + 1;

      int ism = Numbers::iSM( id );

      if ( ! ( runType[ism-1] == EcalDCCHeaderBlock::TESTPULSE_MGPA ||
               runType[ism-1] == EcalDCCHeaderBlock::TESTPULSE_GAP ) ) continue;

      EBDataFrame dataframe = (*digiItr);

      for (int i = 0; i < 10; i++) {

        int adc = dataframe.sample(i).adc();
        float gain = 1.;

        MonitorElement* meShapeMap = 0;

        if ( dataframe.sample(i).gainId() == 1 ) gain = 1./12.;
        if ( dataframe.sample(i).gainId() == 2 ) gain = 1./ 6.;
        if ( dataframe.sample(i).gainId() == 3 ) gain = 1./ 1.;

        if ( mgpaGain[ism-1] == 3 ) meShapeMap = meShapeMapG01_[ism-1];
        if ( mgpaGain[ism-1] == 2 ) meShapeMap = meShapeMapG06_[ism-1];
        if ( mgpaGain[ism-1] == 1 ) meShapeMap = meShapeMapG12_[ism-1];

//        float xval = float(adc) * gain;
        float xval = float(adc);

        if ( meShapeMap ) meShapeMap->Fill(ic - 0.5, i + 0.5, xval);

      }

    }

  } else {

    LogWarning("EBTestPulseTask") << EBDigiCollection_ << " not available";

  }

  Handle<EcalUncalibratedRecHitCollection> hits;

  if ( e.getByLabel(EcalUncalibratedRecHitCollection_, hits) ) {

    int neh = hits->size();
    LogDebug("EBTestPulseTask") << "event " << ievt_ << " hits collection size " << neh;

    for ( EcalUncalibratedRecHitCollection::const_iterator hitItr = hits->begin(); hitItr != hits->end(); ++hitItr ) {

      EBDetId id = hitItr->id();

      int ic = id.ic();
      int ie = (ic-1)/20 + 1;
      int ip = (ic-1)%20 + 1;

      int ism = Numbers::iSM( id );

      float xie = ie - 0.5;
      float xip = ip - 0.5;

      if ( ! ( runType[ism-1] == EcalDCCHeaderBlock::TESTPULSE_MGPA ||
               runType[ism-1] == EcalDCCHeaderBlock::TESTPULSE_GAP ) ) continue;

      MonitorElement* meAmplMap = 0;

      if ( mgpaGain[ism-1] == 3 ) meAmplMap = meAmplMapG01_[ism-1];
      if ( mgpaGain[ism-1] == 2 ) meAmplMap = meAmplMapG06_[ism-1];
      if ( mgpaGain[ism-1] == 1 ) meAmplMap = meAmplMapG12_[ism-1];

      float xval = hitItr->amplitude();
      if ( xval <= 0. ) xval = 0.0;

//      if ( mgpaGain[ism-1] == 3 ) xval = xval * 1./12.;
//      if ( mgpaGain[ism-1] == 2 ) xval = xval * 1./ 2.;
//      if ( mgpaGain[ism-1] == 1 ) xval = xval * 1./ 1.;

      if ( meAmplMap ) meAmplMap->Fill(xie, xip, xval);

    }

  } else {

    LogWarning("EBTestPulseTask") << EcalUncalibratedRecHitCollection_ << " not available";

  }

  Handle<EcalPnDiodeDigiCollection> pns;

  if ( e.getByLabel(EcalPnDiodeDigiCollection_, pns) ) {

    int nep = pns->size();
    LogDebug("EBTestPulseTask") << "event " << ievt_ << " pns collection size " << nep;

    for ( EcalPnDiodeDigiCollection::const_iterator pnItr = pns->begin(); pnItr != pns->end(); ++pnItr ) {

      if ( Numbers::subDet( pnItr->id() ) != EcalBarrel ) continue;

      int ism = Numbers::iSM( pnItr->id() );

      int num = pnItr->id().iPnId();

      if ( ! ( runType[ism-1] == EcalDCCHeaderBlock::TESTPULSE_MGPA ||
               runType[ism-1] == EcalDCCHeaderBlock::TESTPULSE_GAP ) ) continue;

      float xvalped = 0.;

      for (int i = 0; i < 4; i++) {

        int adc = pnItr->sample(i).adc();

        MonitorElement* mePNPed = 0;

        if ( pnItr->sample(i).gainId() == 0 ) mePNPed = mePnPedMapG01_[ism-1];
        if ( pnItr->sample(i).gainId() == 1 ) mePNPed = mePnPedMapG16_[ism-1];

        float xval = float(adc);

        if ( mePNPed ) mePNPed->Fill(num - 0.5, xval);

        xvalped = xvalped + xval;

      }

      xvalped = xvalped / 4;

      float xvalmax = 0.;

      MonitorElement* mePN = 0;

      for (int i = 0; i < 50; i++) {

        int adc = pnItr->sample(i).adc();

        float xval = float(adc);

        if ( xval >= xvalmax ) xvalmax = xval;

      }

      xvalmax = xvalmax - xvalped;

      if ( pnItr->sample(0).gainId() == 0 ) mePN = mePnAmplMapG01_[ism-1];
      if ( pnItr->sample(0).gainId() == 1 ) mePN = mePnAmplMapG16_[ism-1];

      if ( mePN ) mePN->Fill(num - 0.5, xvalmax);

    }

  } else {

    LogWarning("EBTestPulseTask") << EcalPnDiodeDigiCollection_ << " not available";

  }

}

