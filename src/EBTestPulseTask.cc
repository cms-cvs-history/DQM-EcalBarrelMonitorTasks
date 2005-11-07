/*
 * \file EBTestPulseTask.cc
 * 
 * $Date: 2005/10/31 12:42:26 $
 * $Revision: 1.23 $
 * \author G. Della Ricca
 *
*/

#include <DQM/EcalBarrelMonitorTasks/interface/EBTestPulseTask.h>

EBTestPulseTask::EBTestPulseTask(const edm::ParameterSet& ps, DaqMonitorBEInterface* dbe){

  logFile.open("EBTestPulseTask.log");

  Char_t histo[20];

  if ( dbe ) {
    dbe->setCurrentFolder("EcalBarrel/EBTestPulseTask");

    dbe->setCurrentFolder("EcalBarrel/EBTestPulseTask/Gain01");
    for (int i = 0; i < 36 ; i++) {
      sprintf(histo, "EBTT shape SM%02d G01", i+1);
      meShapeMapG01[i] = dbe->bookProfile2D(histo, histo, 1700, 0., 1700., 10, 0., 10., 4096, 0., 4096.);
      sprintf(histo, "EBTT amplitude SM%02d G01", i+1);
      meAmplMapG01[i] = dbe->bookProfile2D(histo, histo, 85, 0., 85., 20, 0., 20., 4096, 0., 4096.);
    }

    dbe->setCurrentFolder("EcalBarrel/EBTestPulseTask/Gain06");
    for (int i = 0; i < 36 ; i++) {
      sprintf(histo, "EBTT shape SM%02d G06", i+1);
      meShapeMapG06[i] = dbe->bookProfile2D(histo, histo, 1700, 0., 1700., 10, 0., 10., 4096, 0., 4096.);
      sprintf(histo, "EBTT amplitude SM%02d G06", i+1);
      meAmplMapG06[i] = dbe->bookProfile2D(histo, histo, 85, 0., 85., 20, 0., 20., 4096, 0., 4096.);
    }

    dbe->setCurrentFolder("EcalBarrel/EBTestPulseTask/Gain12");
    for (int i = 0; i < 36 ; i++) {
      sprintf(histo, "EBTT shape SM%02d G12", i+1);
      meShapeMapG12[i] = dbe->bookProfile2D(histo, histo, 1700, 0., 1700., 10, 0., 10., 4096, 0., 4096.);
      sprintf(histo, "EBTT amplitude SM%02d G12", i+1);
      meAmplMapG12[i] = dbe->bookProfile2D(histo, histo, 85, 0., 85., 20, 0., 20., 4096, 0., 4096.);
    }
  }

}

EBTestPulseTask::~EBTestPulseTask(){

  logFile.close();

}

void EBTestPulseTask::beginJob(const edm::EventSetup& c){

  ievt = 0;
    
}

void EBTestPulseTask::endJob(){

  cout << "EBTestPulseTask: analyzed " << ievt << " events" << endl;

}

void EBTestPulseTask::analyze(const edm::Event& e, const edm::EventSetup& c){

  ievt++;

  edm::Handle<EBDigiCollection>  digis;
  e.getByLabel("ecalEBunpacker", digis);

//  int nebd = digis->size();
//  cout << "EBTestPulseTask: event " << ievt << " digi collection size " << nebd << endl;

  for ( EBDigiCollection::const_iterator digiItr = digis->begin(); digiItr != digis->end(); ++digiItr ) {

    EBDataFrame dataframe = (*digiItr);
    EBDetId id = dataframe.id();

//    int ie = id.ieta();
//    int ip = id.iphi();
//    int iz = id.zside();

//    float xie = iz * (ie - 0.5);
//    float xip = ip - 0.5;

    int ism = id.ism();

    int ic = id.ic();

//    logFile << " det id = " << id << endl;
//    logFile << " sm, eta, phi " << ism << " " << ie*iz << " " << ip << endl;

    for (int i = 0; i < 10; i++) {

      EcalMGPASample sample = dataframe.sample(i);
      int adc = sample.adc();
      float gain = 1.;

      MonitorElement* meShapeMap = 0;

      if ( sample.gainId() == 1 ) {
        gain = 1./12.;
      }
      if ( sample.gainId() == 2 ) {
        gain = 1./ 6.;
      }
      if ( sample.gainId() == 3 ) {
        gain = 1./ 1.;
      }

      if ( ievt >=   1 && ievt <=  50 ) meShapeMap = meShapeMapG01[ism-1];
      if ( ievt >=  51 && ievt <= 100 ) meShapeMap = meShapeMapG06[ism-1];
      if ( ievt >= 101 && ievt <= 150 ) meShapeMap = meShapeMapG12[ism-1];

      float xval = adc * gain;

      if ( meShapeMap ) meShapeMap->Fill( ic - 0.5, i + 0.5, xval);

    }

  }

  edm::Handle<EcalUncalibratedRecHitCollection>  hits;
  e.getByLabel("ecalUncalibHitMaker", "EcalEBUncalibRecHits", hits);

//  int neh = hits->size();
//  cout << "EBTestPulseTask: event " << ievt << " hits collection size " << neb << endl;

  for ( EcalUncalibratedRecHitCollection::const_iterator hitItr = hits->begin(); hitItr != hits->end(); ++hitItr ) {

    EcalUncalibratedRecHit hit = (*hitItr);
    EBDetId id = hit.id();

    int ie = id.ieta();
    int ip = id.iphi();
    int iz = id.zside();

    float xie = iz * (ie - 0.5);
    float xip = ip - 0.5;

    int ism = id.ism();

//    logFile << " det id = " << id << endl;
//    logFile << " sm, eta, phi " << ism << " " << ie*iz << " " << ip << endl;

    MonitorElement* meAmplMap = 0;

    if (                ievt <=  50 ) meAmplMap = meAmplMapG01[ism-1];
    if ( ievt >=  50 && ievt <= 100 ) meAmplMap = meAmplMapG06[ism-1];
    if ( ievt >= 100                ) meAmplMap = meAmplMapG12[ism-1];

    float xval = 0.001 * hit.amplitude();

//    logFile << " hit amplitude " << xval << endl;

    if ( meAmplMap ) meAmplMap->Fill(xie, xip, xval);

  }

}

