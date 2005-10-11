/*
 * \file EBLaserTask.cc
 * 
 * $Date: 2005/10/08 15:18:28 $
 * $Revision: 1.4 $
 * \author G. Della Ricca
 *
*/

#include <DQM/EcalBarrelMonitorTasks/interface/EBLaserTask.h>

EBLaserTask::EBLaserTask(const edm::ParameterSet& ps, DaqMonitorBEInterface* dbe){

  logFile.open("EBLaserTask.log");

  ievt = 0;

  Char_t histo[20];

  dbe->setCurrentFolder("EcalBarrel/EBLaserTask");

  dbe->setCurrentFolder("EcalBarrel/EBLaserTask/Laser1");
  for (int i = 0; i < 36 ; i++) {
    sprintf(histo, "EBLT shape SM%02d L1", i+1);
    meShapeMapL1[i] = dbe->bookProfile2D(histo, histo, 1700, 0., 1700., 10, 0., 10., 4096, 0., 4096.);
    sprintf(histo, "EBLT amplitude SM%02d L1", i+1);
    meAmplMapL1[i] = dbe->bookProfile2D(histo, histo, 20, 0., 20., 85, 0., 85., 4096, 0., 4096.);
  }

  dbe->setCurrentFolder("EcalBarrel/EBLaserTask/Laser2");
  for (int i = 0; i < 36 ; i++) {
    sprintf(histo, "EBLT shape SM%02d L2", i+1);
    meShapeMapL2[i] = dbe->bookProfile2D(histo, histo, 1700, 0., 1700., 10, 0., 10., 4096, 0., 4096.);
    sprintf(histo, "EBLT amplitude SM%02d L2", i+1);
    meAmplMapL2[i] = dbe->bookProfile2D(histo, histo, 20, 0., 20., 85, 0., 85., 4096, 0., 4096.);
  }

}

EBLaserTask::~EBLaserTask(){

  logFile.close();

  cout << "EBLaserTask: analyzed " << ievt << " events" << endl;
}

void EBLaserTask::analyze(const edm::Event& e, const edm::EventSetup& c){

  ievt++;

  edm::Handle<EBDigiCollection>  digis;
  e.getByLabel("ecalEBunpacker", digis);

  int neb = digis->size();

  cout << "EBLaserTask: event " << ievt << " collection size " << neb << endl;

  for ( EBDigiCollection::const_iterator digiItr = digis->begin(); digiItr != digis->end(); ++digiItr ) {

    EBDataFrame dataframe = (*digiItr);
    EBDetId id = dataframe.id();

    int ie = id.ieta();
    int ip = id.iphi();
    int iz = id.zside();

    float xie = iz * (ie - 0.5);
    float xip = ip - 0.5;

    int ism = EBMonitorUtils::getSuperModuleID(ip, iz);

    logFile << " det id = " << id << endl;
    logFile << " sm, eta, phi " << ism << " " << ie*iz << " " << ip << endl;

    float xvalmax = 0.;

    MonitorElement* meAmplMap = 0;

    for (int i = 0; i < 10; i++) {

      EcalMGPASample sample = dataframe.sample(i);
      int adc = sample.adc();
      float gain = 1.;

      MonitorElement* meShapeMap = 0;

      int il = 1;

      if ( sample.gainId() == 1 ) {
        gain = 1./12.;
      }
      if ( sample.gainId() == 2 ) {
        gain = 1./ 6.;
      }
      if ( sample.gainId() == 3 ) {
        gain = 1./ 1.;
      }

      if ( il == 1 ) {
          meShapeMap = meShapeMapL1[ism-1];
          meAmplMap = meAmplMapL1[ism-1];
      }
      if ( il == 2 ) {
        meShapeMap = meShapeMapL2[ism-1];
        meAmplMap = meAmplMapL2[ism-1];
      }

      float xval = adc * gain;

      int ic = EBMonitorUtils::getCrystalID(ie, ip);

      if ( meShapeMap ) meShapeMap->Fill( ic - 0.5, i + 0.5, xval);

      float xrms = 1.0;

      if ( xval >= 3.0 * xrms && xval >= xvalmax ) xvalmax = xval;

    }

    if ( meAmplMap ) meAmplMap->Fill(xip, xie, xvalmax);

  }

}

