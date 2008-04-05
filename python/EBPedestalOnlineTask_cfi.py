import FWCore.ParameterSet.Config as cms

ecalBarrelPedestalOnlineTask = cms.EDFilter("EBPedestalOnlineTask",
    EBDigiCollection = cms.InputTag("ecalEBunpacker","ebDigis"),
    enableCleanup = cms.untracked.bool(True)
)

