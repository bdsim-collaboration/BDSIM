#include "BDSIntegratorType.hh"
#include "BDSDebug.hh"

#include "globals.hh" // geant4 types / globals

#include <map>
#include <string>

// dictionary for BDSIntegratorType for reflexivity
template<>
std::map<BDSIntegratorType, std::string>* BDSIntegratorType::dictionary =
  new std::map<BDSIntegratorType, std::string> ({
      {BDSIntegratorType::solenoid,             "solenoid"},
      {BDSIntegratorType::dipole,               "dipole"},
      {BDSIntegratorType::quadrupole,           "quadrupole"},
      {BDSIntegratorType::sextupole,            "sextupole"},
      {BDSIntegratorType::octupole,             "octupole"},
      {BDSIntegratorType::decapole,             "decapole"},
      {BDSIntegratorType::multipole,            "multipole"},
      {BDSIntegratorType::decapole,             "fringe"},
      {BDSIntegratorType::g4cashkarprkf45,      "g4cashkarprkf45"},
      {BDSIntegratorType::g4classicalrk4,       "g4classicalrk4"},
      {BDSIntegratorType::g4constrk4,           "g4constrk4"},
      {BDSIntegratorType::g4expliciteuler,      "g4expliciteuler"},
      {BDSIntegratorType::g4impliciteuler,      "g4impliciteuler"},
      {BDSIntegratorType::g4simpleheum,         "g4simpleheum"},
      {BDSIntegratorType::g4simplerunge,        "g4simplerunge"},
      {BDSIntegratorType::g4exacthelixstepper,  "g4exacthelixstepper"},
      {BDSIntegratorType::g4helixexpliciteuler, "g4helixexpliciteuler"},
      {BDSIntegratorType::g4helixheum,          "g4helixheum"},
      {BDSIntegratorType::g4heliximpliciteuler, "g4heliximpliciteuler"},
      {BDSIntegratorType::g4helixmixedstepper,  "g4helixmixedstepper"},
      {BDSIntegratorType::g4helixsimplerunge,   "g4helixsimplerunge"},
      {BDSIntegratorType::g4nystromrk4,         "g4nystromrk4"},
      {BDSIntegratorType::g4rkg3stepper,        "g4rkg3stepper"}
    });

BDSIntegratorType BDS::DetermineIntegratorType(G4String integratorType)
{
  std::map<G4String, BDSIntegratorType> types;
  types["solenoid"]             = BDSIntegratorType::solenoid;
  types["dipole"]               = BDSIntegratorType::dipole;
  types["quadrupole"]           = BDSIntegratorType::quadrupole;
  types["sextupole"]            = BDSIntegratorType::sextupole;
  types["octupole"]             = BDSIntegratorType::octupole;
  types["decapole"]             = BDSIntegratorType::decapole;
  types["multipole"]            = BDSIntegratorType::multipole;
  types["fringe"]               = BDSIntegratorType::fringe;
  types["g4cashkarprkf45"]      = BDSIntegratorType::g4cashkarprkf45;
  types["g4classicalrk4"]       = BDSIntegratorType::g4classicalrk4;
  types["g4constrk4"]           = BDSIntegratorType::g4constrk4;
  types["g4expliciteuler"]      = BDSIntegratorType::g4expliciteuler;
  types["g4impliciteuler"]      = BDSIntegratorType::g4impliciteuler;
  types["g4simpleheum"]         = BDSIntegratorType::g4simpleheum;
  types["g4simplerunge"]        = BDSIntegratorType::g4simplerunge;
  types["g4exacthelixstepper"]  = BDSIntegratorType::g4exacthelixstepper;
  types["g4helixexpliciteuler"] = BDSIntegratorType::g4helixexpliciteuler;
  types["g4helixheum"]          = BDSIntegratorType::g4helixheum;
  types["g4heliximpliciteuler"] = BDSIntegratorType::g4heliximpliciteuler;
  types["g4helixmixedstepper"]  = BDSIntegratorType::g4helixmixedstepper;
  types["g4helixsimplerunge"]   = BDSIntegratorType::g4helixsimplerunge;
  types["g4nystromrk4"]         = BDSIntegratorType::g4nystromrk4;
  types["g4rkg3stepper"]        = BDSIntegratorType::g4rkg3stepper;

  integratorType.toLower();

  auto result = types.find(integratorType);
  if (result == types.end())
    {
      // it's not a valid key
      G4cerr << __METHOD_NAME__ << integratorType << " is not a valid field type" << G4endl;

      G4cout << "Available integrator types are:" << G4endl;
      for (auto it : types)
	{G4cout << "\"" << it.first << "\"" << G4endl;}
      exit(1);
    }

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "determined integrator type to be " << result->second << G4endl;
#endif
  return result->second;
}
