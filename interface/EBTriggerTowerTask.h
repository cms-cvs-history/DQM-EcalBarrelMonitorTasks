#ifndef EBTriggerTowerTask_H
#define EBTriggerTowerTask_H

/*
 * \file EBTriggerTowerTask.h
 *
 * $Date: 2007/11/13 13:20:50 $
 * $Revision: 1.10 $
 * \author C. Bernet
 *
*/

#include "FWCore/Framework/interface/EDAnalyzer.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/EcalDigi/interface/EcalDigiCollections.h"

#include <vector>

class MonitorElement;
class DaqMonitorBEInterface;

class EBTriggerTowerTask : public edm::EDAnalyzer {

 public:

  /// Constructor
  EBTriggerTowerTask(const edm::ParameterSet& ps);

  /// Destructor
  virtual ~EBTriggerTowerTask();

  /// number of trigger towers in eta
  static const int nTTEta; 

  /// number of trigger towers in phi
  static const int nTTPhi; 

  /// number of supermodules
  static const int nSM; 

 protected:

  /// Analyze
  void analyze(const edm::Event& e, 
	       const edm::EventSetup& c);

  /// BeginJob
  void beginJob(const edm::EventSetup& c);

  /// EndJob
  void endJob(void);

  /// Setup
  void setup(void);

  /// Cleanup
  void cleanup(void);

 private:
  
  /// 1D array
  typedef std::vector<MonitorElement*> array1;

  /// reserve an array to hold one histogram per supermodule
  void reserveArray( array1& array );

  /// process a collection of digis, either real or emulated
  void processDigis( const edm::Handle<EcalTrigPrimDigiCollection>& digis, 
		     array1& meEtMap,
		     array1& meVeto,
		     array1& meFlags,
		     const edm::Handle<EcalTrigPrimDigiCollection>& digis
		     = edm::Handle<EcalTrigPrimDigiCollection>());


  /// book monitor elements for real, or emulated digis
  void setup( DaqMonitorBEInterface* dbe,
	      const char* nameext,
	      const char* folder, 
	      bool emulated);
  

  /// local event counter
  int ievt_;

  /// Et vs iphi vs ieta, for each SM 
  array1 meEtMapReal_;

  /// fine grain veto vs iphi vs ieta, for each SM 
  array1 meVetoReal_;

  /// flag vs iphi vs ieta, for each SM   
  array1 meFlagsReal_;
  
  /// Emulated Et vs iphi vs ieta, for each SM 
  array1 meEtMapEmul_;

  /// Emulated fine grain veto vs iphi vs ieta, for each SM 
  array1 meVetoEmul_;

  /// Emulated flag vs iphi vs ieta, for each SM   
  array1 meFlagsEmul_;
  
  /// error flag vs iphi vs ieta, for each SM
  /// the error flag is set to true in case of a discrepancy between 
  /// the emulator and the real data
  array1 meEmulError_;
  array1 meVetoEmulError_;
  array1 meFlagEmulError_;

  /// not sure this is necessary
  bool init_;

  /// to find the input collection of real digis 
  edm::InputTag  realCollection_;

  /// to find the input collection of emulated digis
  edm::InputTag  emulCollection_;
  
  /// debug output root file. if empty, no output file created.
  std::string   outputFile_;

  bool enableCleanup_;

};

#endif
