#include "BDSMagnetType.hh"

#include "globals.hh" // geant4 types / globals

#include <map>

std::map<G4int, G4String> BDSMagnetTypeMap::types = {
  {0,  "decapole"},
  {1,  "hkicker"},
  {2,  "vkicker"},
  {3,  "muspoiler"},
  {4,  "octupole"},
  {5,  "quadrupole"},
  {6,  "sectorbend"},
  {7,  "rectangularbend"},
  {8,  "rfcavity"},
  {9,  "sextupole"},
  {10, "solenoid"},
  {11, "multipole"}
};
