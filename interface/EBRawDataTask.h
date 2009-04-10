#ifndef EBRawDataTask_H
#define EBRawDataTask_H

/*
 * \file EBRawDataTask.h
 *
 * $Date: 2008/08/07 10:06:14 $
 * $Revision: 1.3 $
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
edm::InputTag GTEvmSource_;

MonitorElement* meEBCRCErrors_;
MonitorElement* meEBEventTypePreCalibrationBX_;
MonitorElement* meEBEventTypeCalibrationBX_;
MonitorElement* meEBEventTypePostCalibrationBX_;
MonitorElement* meEBRunNumberErrors_;
MonitorElement* meEBL1AErrors_;
MonitorElement* meEBOrbitNumberErrors_;
MonitorElement* meEBBunchCrossingErrors_;
MonitorElement* meEBTriggerTypeErrors_;
MonitorElement* meEBGapErrors_;

bool init_;

float calibrationBX_; 

enum activeEVM { TCS, FDLEVM };

};

#endif
