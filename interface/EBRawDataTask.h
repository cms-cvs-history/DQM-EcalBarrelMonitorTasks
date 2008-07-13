#ifndef EBRawDataTask_H
#define EBRawDataTask_H

/*
 * \file EBRawDataTask.h
 *
 * $Date: 2008/05/11 09:35:09 $
 * $Revision: 1.105 $
 * \author E. Di Marco
 *
*/

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

class MonitorElement;
class DQMStore;

class EBRawDataTask : public edm::EDAnalyzer {

public:

/// Constructor
EBRawDataTask(const edm::ParameterSet& ps);

/// Destructor
virtual ~EBRawDataTask();

protected:

/// Analyze
void analyze(const edm::Event& e, const edm::EventSetup& c);

/// BeginJob
void beginJob(const edm::EventSetup& c);

/// EndJob
void endJob(void);

/// BeginRun
void beginRun(const edm::Run & r, const edm::EventSetup & c);

/// EndRun
void endRun(const edm::Run & r, const edm::EventSetup & c);

/// Reset
void reset(void);

/// Setup
void setup(void);

/// Cleanup
void cleanup(void);

private:

int ievt_;

DQMStore* dqmStore_;

std::string prefixME_;

bool enableCleanup_;

bool mergeRuns_;

edm::InputTag FEDRawDataCollection_;
edm::InputTag EcalRawDataCollection_;

MonitorElement* meEBCRCErrors_;

MonitorElement* meEBRunNumberErrors_;
MonitorElement* meEBL1AErrors_;
MonitorElement* meEBOrbitNumberErrors_;
MonitorElement* meEBBunchCrossingErrors_;
MonitorElement* meEBTriggerTypeErrors_;

bool init_;

};

#endif