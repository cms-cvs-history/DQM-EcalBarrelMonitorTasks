#include <iostream>

#include <DQM/EcalBarrelMonitorTasks/interface/EBMonitorUtils.h>

int main() {

  //EBMonitorUtils util;

  int eta = 1, phi = 1, zed = 1;
  int sm  = 1, cry = 1;
  int test;

  std::cout << "getSuperModuleID (1), getCrystalID (2), getEtaPhi (3)? ";
  std::cin >> test;
  
  if( test ==  1 ) {
    while( zed != 0 ) {
      std::cout << "Phi? ";
      std::cin >> phi;
      std::cout << "Zed? ";
      std::cin >> zed;
      sm = EBMonitorUtils::getSuperModuleID( phi, zed );
      std::cout << phi << " " << zed << " " << sm << std::endl;
    }
  }
  else if( test == 2 ) {
    while( phi != 0 ) {
      std::cout << "Eta? ";
      std::cin >> eta;
      std::cout << "Phi? ";
      std::cin >> phi;
      cry = EBMonitorUtils::getCrystalID( eta, phi );
      std::cout << eta << " " << phi << " " << cry << std::endl;
    }
  }
  else if( test == 3 ) {
    while( phi != 0 ) {
      std::cout << "Crystal? ";
      std::cin >> cry;
      std::cout << "SM?      ";
      std::cin >> sm;
      EBMonitorUtils::getEtaPhi( cry, sm, eta, phi );
      std::cout << cry << " " << sm << " " << eta << " " << phi << std::endl;
    }
  }

  return 0;
}
