/*
 * \file EBTriggerTowerTask.cc
 *
 * $Date: 2007/10/26 08:44:55 $
 * $Revision: 1.47 $
 * \author C. Bernet
 * \author G. Della Ricca
 * \author E. Di Marco
 *
*/


#include "FWCore/MessageLogger/interface/MessageLogger.h"

#include "DQMServices/Core/interface/DaqMonitorBEInterface.h"
#include "DQMServices/Daemon/interface/MonitorDaemon.h"
#include "FWCore/ServiceRegistry/interface/Service.h"

#include "DataFormats/EcalDetId/interface/EBDetId.h"
#include "DataFormats/EcalDigi/interface/EBDataFrame.h"
#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"
#include "DataFormats/EcalRecHit/interface/EcalUncalibratedRecHit.h"
#include "DataFormats/EcalRecHit/interface/EcalRecHitCollections.h"

#include "DQM/EcalCommon/interface/Numbers.h"

#include "DQM/EcalBarrelMonitorTasks/interface/EBTriggerTowerTask.h"

using namespace cms;
using namespace edm;
using namespace std;


const int EBTriggerTowerTask::nTTEta = 17;
const int EBTriggerTowerTask::nTTPhi = 4;
const int EBTriggerTowerTask::nSM = 36;


EBTriggerTowerTask::EBTriggerTowerTask(const ParameterSet& ps) {

  init_ = false;

  reserveArray(meEtMapReal_);
  reserveArray(meVetoReal_);
  reserveArray(meFlagsReal_);
  reserveArray(meEtMapEmul_);
  reserveArray(meVetoEmul_);
  reserveArray(meFlagsEmul_);
  reserveArray(meEmulError_);
  reserveArray(meVetoEmulError_);
  reserveArray(meFlagEmulError_);

  realCollection_ =  ps.getParameter<InputTag>("EcalTrigPrimDigiCollectionReal");
  emulCollection_ =  ps.getParameter<InputTag>("EcalTrigPrimDigiCollectionEmul");

//   realModuleLabel_
//     = ps.getUntrackedParameter<string>("real_digis_moduleLabel",
// 				       "ecalEBunpacker");
//   emulModuleLabel_
//     = ps.getUntrackedParameter<string>("emulated_digis_moduleLabel",
// 				       "ecalTriggerPrimitiveDigis");
  outputFile_
    = ps.getUntrackedParameter<string>("OutputRootFile",
				       "");


  ostringstream  str;
  str<<"Module label for producer of REAL     digis: "<<realCollection_<<endl;
  str<<"Module label for producer of EMULATED digis: "<<emulCollection_<<endl;

  LogDebug("EBTriggerTowerTask")<<str.str()<<endl;
}


EBTriggerTowerTask::~EBTriggerTowerTask(){

}


void EBTriggerTowerTask::reserveArray( array1& array ) {

  array.reserve( nSM );
  array.resize( nSM, static_cast<MonitorElement*>(0) );

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


//   DaqMonitorBEInterface* dbe = 0;

  // get hold of back-end interface
  DaqMonitorBEInterface* dbe = Service<DaqMonitorBEInterface>().operator->();

  if ( dbe ) {
    // dbe->showDirStructure();

    setup( dbe,
	   "Real Digis",
	   "EcalBarrel/EBTriggerTowerTask", false );

    setup( dbe,
	   "Emulated Digis",
	   "EcalBarrel/EBTriggerTowerTask/Emulated", true);
  }
  else {
    LogError("EBTriggerTowerTask")<<"Bad DaqMonitorBEInterface, "
				  <<"cannot book MonitorElements."<<endl;
  }
}


void EBTriggerTowerTask::setup( DaqMonitorBEInterface* dbe,
				const char* nameext,
				const char* folder,
				bool emulated ) {


  array1*  meEtMap = &meEtMapReal_;
  array1*  meVeto = &meVetoReal_;
  array1*  meFlags = &meFlagsReal_;

  if( emulated ) {
    meEtMap = &meEtMapEmul_;
    meVeto = &meVetoEmul_;
    meFlags= &meFlagsEmul_;
  }


  assert(dbe);

  dbe->setCurrentFolder(folder);


  static const unsigned namesize = 200;

  char histo[namesize];
  sprintf(histo, "EBTTT Et map %s", nameext);
  string etMapName = histo;
  sprintf(histo, "EBTTT FineGrainVeto %s", nameext);
  string fineGrainVetoName = histo;
  sprintf(histo, "EBTTT Flags %s", nameext);
  string flagsName = histo;
  sprintf(histo, "EBTTT EmulError %s", nameext);
  string emulErrorName = histo;
  sprintf(histo, "EBTTT EmulFineGrainVetoError %s", nameext);
  string emulFineGrainVetoErrorName = histo;
  sprintf(histo, "EBTTT EmulFlagError %s", nameext);
  string emulFlagErrorName = histo;

  for (int i = 0; i < 36 ; i++) {

    string etMapNameSM = etMapName;
    etMapNameSM += " " + Numbers::sEB(i+1);

    (*meEtMap)[i] = dbe->book3D(etMapNameSM.c_str(), etMapNameSM.c_str(),
				nTTEta, 0, nTTEta,
				nTTPhi, 0, nTTPhi,
				256, 0, 256.);
    dbe->tag((*meEtMap)[i], i+1);

    string  fineGrainVetoNameSM = fineGrainVetoName;
    fineGrainVetoNameSM += " " + Numbers::sEB(i+1);

    (*meVeto)[i] = dbe->book3D(fineGrainVetoNameSM.c_str(),
			       fineGrainVetoNameSM.c_str(),
			       nTTEta, 0, nTTEta,
			       nTTPhi, 0, nTTPhi,
			       2, 0., 2.);
    dbe->tag((*meVeto)[i], i+1);

    string  flagsNameSM = flagsName;
    flagsNameSM += " " + Numbers::sEB(i+1);

    (*meFlags)[i] = dbe->book3D(flagsNameSM.c_str(), flagsNameSM.c_str(),
				nTTEta, 0, nTTEta,
				nTTPhi, 0, nTTPhi,
				8, 0., 8.);
    dbe->tag((*meFlags)[i], i+1);


    if(!emulated) {

      string  emulErrorNameSM = emulErrorName;
      emulErrorNameSM += " " + Numbers::sEB(i+1);

      meEmulError_[i] = dbe->book2D(emulErrorNameSM.c_str(),
				    emulErrorNameSM.c_str(),
				    nTTEta, 0., nTTEta,
				    nTTPhi, 0., nTTPhi );
      dbe->tag(meEmulError_[i], i+1);

      string  emulFineGrainVetoErrorNameSM = emulFineGrainVetoErrorName;
      emulFineGrainVetoErrorNameSM += " " + Numbers::sEB(i+1);

      meVetoEmulError_[i] = dbe->book3D(emulFineGrainVetoErrorNameSM.c_str(),
					  emulFineGrainVetoErrorNameSM.c_str(),
					  nTTEta, 0., nTTEta,
					  nTTPhi, 0., nTTPhi,
					  8, 0., 8.);
      dbe->tag(meVetoEmulError_[i], i+1);

      string  emulFlagErrorNameSM = emulFlagErrorName;
      emulFlagErrorNameSM += " " + Numbers::sEB(i+1);

      meFlagEmulError_[i] = dbe->book3D(emulFlagErrorNameSM.c_str(),
					  emulFlagErrorNameSM.c_str(),
					  nTTEta, 0., nTTEta,
					  nTTPhi, 0., nTTPhi,
					  8, 0., 8.);
      dbe->tag(meFlagEmulError_[i], i+1);

    }
  }

}


void EBTriggerTowerTask::cleanup(void) {

  DaqMonitorBEInterface* dbe = 0;

  // get hold of back-end interface
  dbe = Service<DaqMonitorBEInterface>().operator->();

  if ( dbe ) {

    if( !outputFile_.empty() )
      dbe->save( outputFile_.c_str() );

    dbe->rmdir( "EcalBarrel/EBTriggerTowerTask" );
  }

  init_ = false;

}




void EBTriggerTowerTask::endJob(void){

  LogInfo("EBTriggerTowerTask") << "analyzed " << ievt_ << " events";

  if ( init_ ) this->cleanup();
}


void EBTriggerTowerTask::analyze(const Event& e, const EventSetup& c){

  Numbers::initGeometry(c);

  if ( ! init_ ) this->setup();

  ievt_++;

  try {

    Handle<EcalTrigPrimDigiCollection> realDigis;
    e.getByLabel(realCollection_, realDigis);


    Handle<EcalTrigPrimDigiCollection> dummy;

    int nebtpd = realDigis->size();
    LogDebug("EBTriggerTowerTask")
      <<"event "
      <<ievt_
      <<" trigger primitive digi collection size: "
      <<nebtpd;


    processDigis( realDigis,
		  meEtMapReal_,
		  meVetoReal_,
		  meFlagsReal_);

    Handle<EcalTrigPrimDigiCollection> emulDigis;
    e.getByLabel(emulCollection_, emulDigis);

    processDigis( emulDigis,
		  meEtMapEmul_,
		  meVetoEmul_,
		  meFlagsEmul_,
		  realDigis);


  } catch ( std::exception& ex) {
    LogError("EBTriggerTowerTask")
      <<ex.what();
  }
}



void
EBTriggerTowerTask::processDigis( const Handle<EcalTrigPrimDigiCollection>&
				  digis,
				  array1& meEtMap,
				  array1& meVeto,
				  array1& meFlags,
				  const Handle<EcalTrigPrimDigiCollection>&
				  compDigis ) {

  LogDebug("EBTriggerTowerTask")<<"processing "<<meEtMap[0]->getName()<<endl;

  ostringstream  str;
  typedef EcalTrigPrimDigiCollection::const_iterator ID;
  for ( ID tpdigiItr = digis->begin();
	tpdigiItr != digis->end(); ++tpdigiItr ) {

    EcalTriggerPrimitiveDigi data = (*tpdigiItr);
    EcalTrigTowerDetId idt = data.id();

    if ( idt.subDet() != EcalBarrel ) continue;

    int iet = abs(idt.ieta());
    int ipt = idt.iphi();

    // phi_tower: change the range from global to SM-local
    // phi==0 is in the middle of a SM
    ipt = ipt + 2;
    if ( ipt > 72 ) ipt = ipt - 72;
    ipt = (ipt-1)%4 + 1;

    // phi_tower: SM-local phi runs opposite to global in EB+
    if ( idt.zside() > 0 ) ipt = 5 - ipt;

    int ismt = Numbers::iSM( idt );

    int itt = Numbers::iTT( idt );

    float xiet = iet+0.5;
    float xipt = ipt+0.5;

    str<<"det id = "<<idt.rawId()<<" "
       <<idt<<" sm, tt, ieta, iphi "<<ismt<<" "<<itt<<" "<<iet<<" "<<ipt<<endl;

    float xval;

    xval = data.compressedEt();
    if ( meEtMap[ismt-1] ) {
      meEtMap[ismt-1]->Fill(xiet-1, xipt-1, xval);
    }
    else {
      LogError("EBTriggerTowerTask")<<"histo does not exist "<<endl;
    }

    xval = 0.5 + data.fineGrain();
    if ( meVeto[ismt-1] ) meVeto[ismt-1]->Fill(xiet-1, xipt-1, xval);

    xval = 0.5 + data.ttFlag();
    if ( meFlags[ismt-1] ) meFlags[ismt-1]->Fill(xiet-1, xipt-1, xval);


    if( compDigis.isValid() ) {
      ID compDigiItr = compDigis->find( idt.rawId() );

      bool good = true;
      bool goodFlag = true;
      bool goodVeto = true;
      if( compDigiItr != compDigis->end() ) {
	str<<"found corresponding digi! "<<*compDigiItr<<endl;
	if( data.compressedEt() != compDigiItr->compressedEt() ) {
	  str<<"but it is different..."<<endl;
	  good = false;
	}
	if( data.ttFlag() != compDigiItr->ttFlag() ) {
	  str<<"but flag is different..."<<endl;
	  goodFlag = false;
	}
	if( data.fineGrain() != compDigiItr->fineGrain() ) {
	  str<<"but fine grain veto is different..."<<endl;
	  goodVeto = false;
	}
      }
      else {
	good = false;
	goodFlag = false;
	goodVeto = false;
	str<<"could not find corresponding digi... "<<endl;
      }
      if(!good ) {
	if ( meEmulError_[ismt-1] ) meEmulError_[ismt-1]->Fill(xiet-1, xipt-1);
      }
      if(!goodFlag) {
	float zval = data.ttFlag();
	if ( meFlagEmulError_[ismt-1] ) meFlagEmulError_[ismt-1]->Fill(xiet-1, xipt-1, zval);
      }
      if(!goodVeto) {
	float zval = data.fineGrain();
	if ( meVetoEmulError_[ismt-1] ) meVetoEmulError_[ismt-1]->Fill(xiet-1, xipt-1, zval);
      }
    }
  }
  LogDebug("EBTriggerTowerTask")<<str.str()<<endl;
}
