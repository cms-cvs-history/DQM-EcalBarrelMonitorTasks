
#include "PluginManager/ModuleDef.h"

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_SEAL_MODULE();

#include <DQM/EcalBarrelMonitorTasks/interface/EBCosmicTask.h>
DEFINE_ANOTHER_FWK_MODULE(EBCosmicTask)

#include <DQM/EcalBarrelMonitorTasks/interface/EBIntegrityTask.h>
DEFINE_ANOTHER_FWK_MODULE(EBIntegrityTask)

#include <DQM/EcalBarrelMonitorTasks/interface/EBLaserTask.h>
DEFINE_ANOTHER_FWK_MODULE(EBLaserTask)

#include <DQM/EcalBarrelMonitorTasks/interface/EBOccupancyTask.h>
DEFINE_ANOTHER_FWK_MODULE(EBOccupancyTask)

#include <DQM/EcalBarrelMonitorTasks/interface/EBPedestalOnlineTask.h>
DEFINE_ANOTHER_FWK_MODULE(EBPedestalOnlineTask)

#include <DQM/EcalBarrelMonitorTasks/interface/EBPedestalTask.h>
DEFINE_ANOTHER_FWK_MODULE(EBPedestalTask)

#include <DQM/EcalBarrelMonitorTasks/interface/EBTestPulseTask.h>
DEFINE_ANOTHER_FWK_MODULE(EBTestPulseTask)

#include <DQM/EcalBarrelMonitorTasks/interface/EBBeamHodoTask.h>
DEFINE_ANOTHER_FWK_MODULE(EBBeamHodoTask)

#include <DQM/EcalBarrelMonitorTasks/interface/EBBeamCaloTask.h>
DEFINE_ANOTHER_FWK_MODULE(EBBeamCaloTask)

