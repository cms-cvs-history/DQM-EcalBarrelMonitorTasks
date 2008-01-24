#ifndef EBOccupancyTask_H
#define EBOccupancyTask_H

/*
 * \file EBOccupancyTask.h
 *
 * $Date: 2008/01/24 16:01:10 $
 * $Revision: 1.13 $
 * \author G. Della Ricca
 *
*/

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class MonitorElement;
class DaqMonitorBEInterface;

class EBOccupancyTask: public edm::EDAnalyzer{

public:

/// Constructor
EBOccupancyTask(const edm::ParameterSet& ps);

/// Destructor
virtual ~EBOccupancyTask();

protected:

/// Analyze
void analyze(const edm::Event& e, const edm::EventSetup& c);

/// BeginJob
void beginJob(const edm::EventSetup& c);

/// EndJob
void endJob(void);

/// Setup
void setup(void);

/// Cleanup
void cleanup(void);

private:

int ievt_;

DaqMonitorBEInterface* dbe_;

bool enableCleanup_;

edm::InputTag EBDigiCollection_;
edm::InputTag EcalPnDiodeDigiCollection_;
edm::InputTag EcalRecHitCollection_;
edm::InputTag EcalTrigPrimDigiCollection_;

MonitorElement* meEvent_[36];
MonitorElement* meOccupancy_[36];
MonitorElement* meOccupancyMem_[36];

MonitorElement* meEBDigiOccupancy_;
MonitorElement* meEBDigiOccupancyProjEta_;
MonitorElement* meEBDigiOccupancyProjPhi_;
MonitorElement* meEBRecHitOccupancy_;
MonitorElement* meEBRecHitOccupancyProjEta_;
MonitorElement* meEBRecHitOccupancyProjPhi_;
MonitorElement* meEBRecHitOccupancyThr_;
MonitorElement* meEBRecHitOccupancyProjEtaThr_;
MonitorElement* meEBRecHitOccupancyProjPhiThr_;
MonitorElement* meEBTrigPrimDigiOccupancy_;
MonitorElement* meEBTrigPrimDigiOccupancyProjEta_;
MonitorElement* meEBTrigPrimDigiOccupancyProjPhi_;
MonitorElement* meEBTrigPrimDigiOccupancyThr_;
MonitorElement* meEBTrigPrimDigiOccupancyProjEtaThr_;
MonitorElement* meEBTrigPrimDigiOccupancyProjPhiThr_;

float recHitEnergyMin_;
float trigPrimEtMin_;

bool init_;

};

#endif
