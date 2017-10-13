#include "BDSDebug.hh"
#include "BDSIonDefinition.hh"

#include "globals.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <exception>
#include <ostream>
#include <regex>
#include <stdexcept>

BDSIonDefinition::BDSIonDefinition(G4String definition):
  a(1), z(1), charge(1), energy(0)
{
  try
    {Parse(definition);}
  catch (std::exception)
    {
      G4cout << __METHOD_NAME__ << "Invalid ion definition "
	     << definition << G4endl;
      exit(1);
    }
}

std::ostream& operator<< (std::ostream& out, BDSIonDefinition const& io)
{
  out << "A: " << io.a << " Z: " << io.z << " Q: " << io.charge << " E: "
      << io.energy << G4endl;
  return out;
}

void BDSIonDefinition::Parse(const G4String& definition)
{
  std::regex numberMatch("([0-9]+)");

  auto wordsBegin = std::sregex_iterator(definition.begin(), definition.end(), numberMatch);
  auto wordsEnd   = std::sregex_iterator();

  // A Z Q E
  G4int counter = 0; // 0,1
  std::vector<G4int*> vals = {&a, &z, &charge};
  for (auto i = wordsBegin; i != wordsEnd; ++i, ++counter)
    {
      std::smatch match = *i;
      try
	{
	  if (counter > 2) // E - only double
	    {energy = std::stod(match[1])*CLHEP::keV;}
	  else
	    {(*vals[counter]) = std::stoi(match[1]);}
	}
      catch (std::invalid_argument) // if stod can't convert number to double / int
	{throw std::exception();}
    }
}
