#ifndef EBPedestalOnlineTask_H
#define EBPedestalOnlineTask_H

/*
 * \file EBPedestalOnlineTask.h
 *
 * $Date: 2007/03/13 10:53:16 $
 * $Revision: 1.9 $
 * \author G. Della Ricca
 *
*/

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "DQMServices/Core/interface/MonitorElement.h"

class EBPedestalOnlineTask: public edm::EDAnalyzer{

public:

/// Constructor
EBPedestalOnlineTask(const edm::ParameterSet& ps);

/// Destructor
virtual ~EBPedestalOnlineTask();

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

edm::InputTag EBDigiCollection_;

MonitorElement* mePedMapG12_[36];

bool init_;

};

#endif
