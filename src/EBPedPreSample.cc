/*
 * \file EBPedPreSampleTask.cc
 * 
 * $Date: 2005/11/14 08:52:30 $
 * $Revision: 1.3 $
 * \author G. Della Ricca
 *
*/

#include <DQM/EcalBarrelMonitorTasks/interface/EBPedPreSampleTask.h>

EBPedPreSampleTask::EBPedPreSampleTask(const edm::ParameterSet& ps, DaqMonitorBEInterface* dbe){

//  logFile_.open("EBPedPreSampleTask.log");

  Char_t histo[20];

  if ( dbe ) {
    dbe->setCurrentFolder("EcalBarrel/EBPedPreSampleTask");

    dbe->setCurrentFolder("EcalBarrel/EBPedPreSampleTask/Gain12");
    for (int i = 0; i < 36 ; i++) {
      sprintf(histo, "EBPT pedestal PreSample SM%02d G12", i+1);
      mePedMapG12_[i] = dbe->bookProfile2D(histo, histo, 85, 0., 85., 20, 0., 20., 4096, 0., 4096.);
    }

  }

}

EBPedPreSampleTask::~EBPedPreSampleTask(){

//  logFile_.close();

}

void EBPedPreSampleTask::beginJob(const edm::EventSetup& c){

  ievt_ = 0;
    
}

void EBPedPreSampleTask::endJob(){

  cout << "EBPedPreSampleTask: analyzed " << ievt_ << " events" << endl;
}

void EBPedPreSampleTask::analyze(const edm::Event& e, const edm::EventSetup& c){

  ievt_++;

  edm::Handle<EBDigiCollection>  digis;
  e.getByLabel("ecalEBunpacker", digis);

//  int nebd = digis->size();
//  cout << "EBPedPreSampleTask: event " << ievt_ << " digi collection size " << nebd << endl;

  for ( EBDigiCollection::const_iterator digiItr = digis->begin(); digiItr != digis->end(); ++digiItr ) {

    EBDataFrame dataframe = (*digiItr);
    EBDetId id = dataframe.id();

    int ie = id.ieta();
    int ip = id.iphi();
    int iz = id.zside();

    float xie = iz * (ie - 0.5);
    float xip = ip - 0.5;

    int ism = id.ism();

//    logFile_ << " det id = " << id << endl;
//    logFile_ << " sm, eta, phi " << ism << " " << ie*iz << " " << ip << endl;

    for (int i = 0; i < 3; i++) {

      EcalMGPASample sample = dataframe.sample(i);
      int adc = sample.adc();
      float gain = 1.;

      MonitorElement* mePedMap = 0;

      if ( sample.gainId() == 1 ) {
        gain = 1./12.;
        mePedMap = 0;
      }
      if ( sample.gainId() == 2 ) {
        gain = 1./ 6.;
        mePedMap = 0;
      }
      if ( sample.gainId() == 3 ) {
        gain = 1./ 1.;
        mePedMap = mePedMapG12_[ism-1];
      }

      float xval = adc * gain;

      if ( mePedMap ) mePedMap->Fill(xie, xip, xval);

    }

  }

}

