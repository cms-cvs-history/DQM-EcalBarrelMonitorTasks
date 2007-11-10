/*
 * \file EBClusterTask.cc
 *
 * $Date: 2007/11/09 19:19:56 $
 * $Revision: 1.35 $
 * \author G. Della Ricca
 * \author E. Di Marco
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
#include "DataFormats/EgammaReco/interface/BasicCluster.h"
#include "DataFormats/EgammaReco/interface/SuperCluster.h"
#include "DataFormats/EgammaReco/interface/SuperClusterFwd.h"
#include "DataFormats/EgammaReco/interface/ClusterShape.h"
#include "DataFormats/EgammaReco/interface/BasicClusterShapeAssociation.h"
#include "DataFormats/Math/interface/Point3D.h"

#include <DQM/EcalCommon/interface/Numbers.h>

#include <DQM/EcalBarrelMonitorTasks/interface/EBClusterTask.h>

#include <TLorentzVector.h>

using namespace cms;
using namespace edm;
using namespace std;
using namespace reco;

EBClusterTask::EBClusterTask(const ParameterSet& ps){

  init_ = false;

  // get hold of back-end interface
  dbe_ = Service<DaqMonitorBEInterface>().operator->();

  enableCleanup_ = ps.getUntrackedParameter<bool>("enableCleanup", true);

  // parameters...
  BasicClusterCollection_ = ps.getParameter<edm::InputTag>("BasicClusterCollection");
  SuperClusterCollection_ = ps.getParameter<edm::InputTag>("SuperClusterCollection");
  ClusterShapeAssociation_ = ps.getParameter<edm::InputTag>("ClusterShapeAssociation");

  // histograms...
  meBCEne_ = 0;
  meBCNum_ = 0;
  meBCSiz_ = 0;

  meBCEneMap_ = 0;
  meBCNumMap_ = 0;
  meBCETMap_  = 0;
  meBCSizMap_ = 0;

  meBCEneMapProjEta_ = 0;
  meBCNumMapProjEta_ = 0;
  meBCETMapProjEta_  = 0;
  meBCSizMapProjEta_ = 0;

  meBCEneMapProjPhi_ = 0;
  meBCNumMapProjPhi_ = 0;
  meBCETMapProjPhi_  = 0;
  meBCSizMapProjPhi_ = 0;

  meSCEne_ = 0;
  meSCNum_ = 0;
  meSCSiz_ = 0;

  mes1s9_  = 0;
  mes9s25_  = 0;
  meInvMass_ = 0;

}

EBClusterTask::~EBClusterTask(){

}

void EBClusterTask::beginJob(const EventSetup& c){

  ievt_ = 0;

  if ( dbe_ ) {
    dbe_->setCurrentFolder("EcalBarrel/EBClusterTask");
    dbe_->rmdir("EcalBarrel/EBClusterTask");
  }

}

void EBClusterTask::setup(void){

  init_ = true;

  Char_t histo[200];

  if ( dbe_ ) {
    dbe_->setCurrentFolder("EcalBarrel/EBClusterTask");

    sprintf(histo, "EBCLT island BC energy");
    meBCEne_ = dbe_->book1D(histo, histo, 100, 0., 150.);
    meBCEne_->setAxisTitle("energy (GeV)", 1);

    sprintf(histo, "EBCLT island BC number");
    meBCNum_ = dbe_->book1D(histo, histo, 100, 0., 100.);
    meBCNum_->setAxisTitle("number of clusters", 1);

    sprintf(histo, "EBCLT island BC size");
    meBCSiz_ = dbe_->book1D(histo, histo, 100, 0., 100.);
    meBCSiz_->setAxisTitle("cluster size", 1);

    sprintf(histo, "EBCLT island BC energy map");
    meBCEneMap_ = dbe_->bookProfile2D(histo, histo, 72, -M_PI*(9+1.5)/9, M_PI*(9-1.5)/9, 34, -1.479, 1.479, 100, 0., 500., "s");
    meBCEneMap_->setAxisTitle("phi", 1);
    meBCEneMap_->setAxisTitle("eta", 2);

    sprintf(histo, "EBCLT island BC number map");
    meBCNumMap_ = dbe_->book2D(histo, histo, 72, -M_PI*(9+1.5)/9, M_PI*(9-1.5)/9, 34, -1.479, 1.479);
    meBCNumMap_->setAxisTitle("phi", 1);
    meBCNumMap_->setAxisTitle("eta", 2);

    sprintf(histo, "EBCLT island BC ET map");
    meBCETMap_ = dbe_->bookProfile2D(histo, histo, 72, -M_PI*(9+1.5)/9, M_PI*(9-1.5)/9, 34, -1.479, 1.479, 100, 0., 500., "s");
    meBCETMap_->setAxisTitle("phi", 1);
    meBCETMap_->setAxisTitle("eta", 2);

    sprintf(histo, "EBCLT island BC size map");
    meBCSizMap_ = dbe_->bookProfile2D(histo, histo, 72, -M_PI*(9+1.5)/9, M_PI*(9-1.5)/9, 34, -1.479, 1.479, 100, 0., 100., "s");
    meBCSizMap_->setAxisTitle("phi", 1);
    meBCSizMap_->setAxisTitle("eta", 2);

    sprintf(histo, "EBCLT island BC energy projection eta");
    meBCEneMapProjEta_ = dbe_->bookProfile(histo, histo, 34, -1.479, 1.479, 100, 0., 500., "s");
    meBCEneMapProjEta_->setAxisTitle("eta", 1);
    meBCEneMapProjEta_->setAxisTitle("energy (GeV)", 2);

    sprintf(histo, "EBCLT island BC energy projection phi");
    meBCEneMapProjPhi_ = dbe_->bookProfile(histo, histo, 72, -M_PI*(9+1.5)/9, M_PI*(9-1.5)/9, 100, 0., 500., "s");
    meBCEneMapProjPhi_->setAxisTitle("phi", 1);
    meBCEneMapProjPhi_->setAxisTitle("energy (GeV)", 2);

    sprintf(histo, "EBCLT island BC number projection eta");
    meBCNumMapProjEta_ = dbe_->book1D(histo, histo, 34, -1.479, 1.479);
    meBCNumMapProjEta_->setAxisTitle("eta", 1);
    meBCNumMapProjEta_->setAxisTitle("number of clusters", 2);

    sprintf(histo, "EBCLT island BC number projection phi");
    meBCNumMapProjPhi_ = dbe_->book1D(histo, histo, 72, -M_PI*(9+1.5)/9, M_PI*(9-1.5)/9);
    meBCNumMapProjPhi_->setAxisTitle("phi", 1);
    meBCNumMapProjPhi_->setAxisTitle("number of clusters", 2);

    sprintf(histo, "EBCLT island BC ET projection eta");
    meBCETMapProjEta_ = dbe_->bookProfile(histo, histo, 34, -1.479, 1.479, 100, 0., 500., "s");
    meBCETMapProjEta_->setAxisTitle("eta", 1);
    meBCETMapProjEta_->setAxisTitle("transverse energy (GeV)", 2);

    sprintf(histo, "EBCLT island BC ET projection phi");
    meBCETMapProjPhi_ = dbe_->bookProfile(histo, histo, 72, -M_PI*(9+1.5)/9, M_PI*(9-1.5)/9, 100, 0., 500., "s");
    meBCETMapProjPhi_->setAxisTitle("phi", 1);
    meBCETMapProjPhi_->setAxisTitle("transverse energy (GeV)", 2);

    sprintf(histo, "EBCLT island BC size projection eta");
    meBCSizMapProjEta_ = dbe_->bookProfile(histo, histo, 34, -1.479, 1.479, 100, 0., 100., "s");
    meBCSizMapProjEta_->setAxisTitle("eta", 1);
    meBCSizMapProjEta_->setAxisTitle("cluster size", 2);

    sprintf(histo, "EBCLT island BC size projection phi");
    meBCSizMapProjPhi_ = dbe_->bookProfile(histo, histo, 72, -M_PI*(9+1.5)/9, M_PI*(9-1.5)/9, 100, 0., 100., "s");
    meBCSizMapProjPhi_->setAxisTitle("phi", 1);
    meBCSizMapProjPhi_->setAxisTitle("cluster size", 2);

    sprintf(histo, "EBCLT hybrid SC energy");
    meSCEne_ = dbe_->book1D(histo, histo, 100, 0., 150.);
    meSCEne_->setAxisTitle("energy (GeV)", 1);

    sprintf(histo, "EBCLT hybrid SC number");
    meSCNum_ = dbe_->book1D(histo, histo, 50, 0., 50.);
    meSCNum_->setAxisTitle("number of clusters", 1);

    sprintf(histo, "EBCLT hybrid SC size");
    meSCSiz_ = dbe_->book1D(histo, histo, 10, 0., 10.);
    meSCSiz_->setAxisTitle("cluster size", 1);

    sprintf(histo, "EBCLT hybrid s1s9");
    mes1s9_ = dbe_->book1D(histo, histo, 50, 0., 1.);
    mes1s9_->setAxisTitle("s1/s9", 1);

    sprintf(histo, "EBCLT hybrid s9s25");
    mes9s25_ = dbe_->book1D(histo, histo, 75, 0., 1.5);
    mes9s25_->setAxisTitle("s9/s25", 1);

    sprintf(histo, "EBCLT dicluster invariant mass");
    meInvMass_ = dbe_->book1D(histo, histo, 100, 0., 200.);
    meInvMass_->setAxisTitle("mass (GeV)", 1);

  }

}

void EBClusterTask::cleanup(void){

  if ( ! enableCleanup_ ) return;

  if ( dbe_ ) {
    dbe_->setCurrentFolder("EcalBarrel/EBClusterTask");

    if ( meBCEne_ ) dbe_->removeElement( meBCEne_->getName() );
    meBCEne_ = 0;

    if ( meBCNum_ ) dbe_->removeElement( meBCNum_->getName() );
    meBCNum_ = 0;

    if ( meBCSiz_ ) dbe_->removeElement( meBCSiz_->getName() );
    meBCSiz_ = 0;

    if ( meBCEneMap_ ) dbe_->removeElement( meBCEneMap_->getName() );
    meBCEneMap_ = 0;

    if ( meBCNumMap_ ) dbe_->removeElement( meBCNumMap_->getName() );
    meBCNumMap_ = 0;

    if ( meBCETMap_ ) dbe_->removeElement( meBCETMap_->getName() );
    meBCETMap_ = 0;

    if ( meBCSizMap_ ) dbe_->removeElement( meBCSizMap_->getName() );
    meBCSizMap_ = 0;

    if ( meBCEneMapProjEta_ ) dbe_->removeElement( meBCEneMapProjEta_->getName() );
    meBCEneMapProjEta_ = 0;

    if ( meBCEneMapProjPhi_ ) dbe_->removeElement( meBCEneMapProjPhi_->getName() );
    meBCEneMapProjPhi_ = 0;

    if ( meBCNumMapProjEta_ ) dbe_->removeElement( meBCNumMapProjEta_->getName() );
    meBCNumMapProjEta_ = 0;

    if ( meBCNumMapProjPhi_ ) dbe_->removeElement( meBCNumMapProjPhi_->getName() );
    meBCNumMapProjPhi_ = 0;

    if ( meBCETMapProjEta_ ) dbe_->removeElement( meBCETMapProjEta_->getName() );
    meBCETMapProjEta_ = 0;

    if ( meBCETMapProjPhi_ ) dbe_->removeElement( meBCETMapProjPhi_->getName() );
    meBCETMapProjPhi_ = 0;

    if ( meBCSizMapProjEta_ ) dbe_->removeElement( meBCSizMapProjEta_->getName() );
    meBCSizMapProjEta_ = 0;

    if ( meBCSizMapProjPhi_ ) dbe_->removeElement( meBCSizMapProjPhi_->getName() );
    meBCSizMapProjPhi_ = 0;

    if ( meSCEne_ ) dbe_->removeElement( meSCEne_->getName() );
    meSCEne_ = 0;

    if ( meSCNum_ ) dbe_->removeElement( meSCNum_->getName() );
    meSCNum_ = 0;

    if ( meSCSiz_ ) dbe_->removeElement( meSCSiz_->getName() );
    meSCSiz_ = 0;

    if ( mes1s9_ ) dbe_->removeElement( mes1s9_->getName() );
    mes1s9_ = 0;

    if ( mes9s25_ ) dbe_->removeElement( mes9s25_->getName() );
    mes9s25_ = 0;

    if ( meInvMass_ ) dbe_->removeElement( meInvMass_->getName() );
    meInvMass_ = 0;

  }

  init_ = false;

}

void EBClusterTask::endJob(void){

  LogInfo("EBClusterTask") << "analyzed " << ievt_ << " events";

  if ( init_ ) this->cleanup();

}

void EBClusterTask::analyze(const Event& e, const EventSetup& c){

  Numbers::initGeometry(c);

  if ( ! init_ ) this->setup();

  ievt_++;

  // --- Barrel "Hybrid" Basic Clusters ---
  try {

    Handle<BasicClusterCollection> pBasicClusters;
    e.getByLabel(BasicClusterCollection_, pBasicClusters);

    Int_t nbcc = pBasicClusters->size();
    meBCNum_->Fill(float(nbcc));

    BasicClusterCollection::const_iterator bclusterItr;
    for ( bclusterItr = pBasicClusters->begin(); bclusterItr != pBasicClusters->end(); ++bclusterItr ) {

      BasicCluster bcluster = *(bclusterItr);

      meBCEne_->Fill(bcluster.energy());
      meBCSiz_->Fill(float(bcluster.getHitsByDetId().size()));

      float xphi = bcluster.phi();
      if ( xphi > M_PI*(9-1.5)/9 ) xphi = xphi - M_PI*2;

      meBCEneMap_->Fill(xphi, bcluster.eta(), bcluster.energy());
      meBCEneMapProjEta_->Fill(bcluster.eta(), bcluster.energy());
      meBCEneMapProjPhi_->Fill(xphi, bcluster.energy());

      meBCNumMap_->Fill(xphi, bcluster.eta());
      meBCNumMapProjEta_->Fill(bcluster.eta());
      meBCNumMapProjPhi_->Fill(xphi);

      meBCSizMap_->Fill(xphi, bcluster.eta(), float(bcluster.getHitsByDetId().size()));
      meBCSizMapProjEta_->Fill(bcluster.eta(), float(bcluster.getHitsByDetId().size()));
      meBCSizMapProjPhi_->Fill(xphi, float(bcluster.getHitsByDetId().size()));

      meBCETMap_->Fill(xphi, bcluster.eta(), float(bcluster.energy()) * sin(bcluster.position().theta()));
      meBCETMapProjEta_->Fill(bcluster.eta(), float(bcluster.energy()) * sin(bcluster.position().theta()));
      meBCETMapProjPhi_->Fill(xphi, float(bcluster.energy()) * sin(bcluster.position().theta()));

    }

  } catch ( exception& ex ) {
    LogWarning("EBClusterTask") << " BasicClusterCollection: " << BasicClusterCollection_ << " not in event.";
  }


  // --- Barrel "Hybrid" Super Clusters ---
  try {

    Handle<SuperClusterCollection> pSuperClusters;
    e.getByLabel(SuperClusterCollection_, pSuperClusters);

    Int_t nscc = pSuperClusters->size();
    meSCNum_->Fill(float(nscc));

    Handle<BasicClusterShapeAssociationCollection> pClusterShapeAssociation;
    try	{
      e.getByLabel(ClusterShapeAssociation_, pClusterShapeAssociation);
    }	catch ( exception& ex )	{
      LogWarning("EBClusterTask") << "Can't get collection with label "   << ClusterShapeAssociation_.label();
    }


    TLorentzVector sc1_p(0,0,0,0);
    TLorentzVector sc2_p(0,0,0,0);

    SuperClusterCollection::const_iterator sClusterItr;
    for(  sClusterItr = pSuperClusters->begin(); sClusterItr != pSuperClusters->end(); ++sClusterItr ) {


      // energy, size
      meSCEne_->Fill( sClusterItr->energy() );
      meSCSiz_->Fill( float(sClusterItr->clustersSize()) );

      // seed and shapes
      const ClusterShapeRef& shape = pClusterShapeAssociation->find(sClusterItr->seed())->val;
      mes1s9_->Fill(shape->eMax()/shape->e3x3());
      mes9s25_->Fill(shape->e3x3()/shape->e5x5());

      // look for the two most energetic super clusters
      if (nscc>1) {
	if (sClusterItr->energy()>sc1_p.Energy()) {
	  sc2_p=sc1_p;
	  sc1_p.SetPtEtaPhiE(sClusterItr->energy()*sin(sClusterItr->position().theta()),
			     sClusterItr->eta(), sClusterItr->phi(), sClusterItr->energy());
	}
	else if (sClusterItr->energy()>sc2_p.Energy()) {
	  sc2_p.SetPtEtaPhiE(sClusterItr->energy()*sin(sClusterItr->position().theta()),
			     sClusterItr->eta(), sClusterItr->phi(), sClusterItr->energy());
	}
      }
    }
    // Get the invariant mass of the two most energetic super clusters
    if (nscc>1) {
      TLorentzVector sum = sc1_p+sc2_p;
      meInvMass_->Fill(sum.M());
    }

  } catch ( exception& ex ) {
    LogWarning("EBClusterTask") << " SuperClusterCollection: not in event.";
  }

}

