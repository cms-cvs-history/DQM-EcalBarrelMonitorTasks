import FWCore.ParameterSet.Config as cms

ecalBarrelTimingTask = cms.EDAnalyzer("EBTimingTask",
    prefixME = cms.untracked.string('EcalBarrel'),
    enableCleanup = cms.untracked.bool(False),
    mergeRuns = cms.untracked.bool(False),
    energyThreshold = cms.untracked.double(1.0),
    EcalRawDataCollection = cms.InputTag("ecalEBunpacker"),
    EcalRecHitCollection = cms.InputTag("ecalRecHit","EcalRecHitsEB"),
)

