#include "BDSMagnetType.hh"

#include <map>

// dictionary for BDSMagnetType
template<>
std::map<BDSMagnetType, std::string>* BDSMagnetType::dictionary =
  new std::map<BDSMagnetType, std::string> ({
  {BDSMagnetType::decapole,        "decapole"},
  {BDSMagnetType::hkicker,         "hkicker"},
  {BDSMagnetType::vkicker,         "vkicker"},
  {BDSMagnetType::muonspoiler,     "muonspoiler"},
  {BDSMagnetType::octupole,        "octupole"},
  {BDSMagnetType::quadrupole,      "quadrupole"},
  {BDSMagnetType::sectorbend,      "sbend"},
  {BDSMagnetType::rectangularbend, "rbend"},
  {BDSMagnetType::rfcavity,        "rfcavity"},
  {BDSMagnetType::sextupole,       "sextupole"},
  {BDSMagnetType::solenoid,        "solenoid"},
  {BDSMagnetType::multipole,       "multipole"}
});
