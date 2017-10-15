#include "BDSDebug.hh"
#include "BDSIonDefinition.hh"

#include "globals.hh"

#include "CLHEP/Units/SystemOfUnits.h"

#include <exception>
#include <ostream>
#include <regex>
#include <stdexcept>

BDSIonDefinition::BDSIonDefinition(G4String definition):
  a(1), z(1), charge(1), energy(0), overrideCharge(false)
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
  G4int counter = 0;
  std::vector<G4int*> vals = {&a, &z};
  for (auto i = wordsBegin; i != wordsEnd; ++i, ++counter)
    {
      std::smatch match = *i;
      if (counter == 2) // ie > 1
	{overrideCharge = true;} // charge is specified
      try
	{
	  if (counter > 1)
	    {// double
	      G4double value = std::stod(match[1]);
	      if (counter == 1)
		{energy = value*CLHEP::keV;}
	      else
		{charge = value*CLHEP::eplus;}
	    }
	  else
	    {// integer
	      (*vals[counter]) = std::stoi(match[1]);
	      if (counter ==1) // by default copy Z as value of Q
		{charge = (G4double)*vals[counter] * CLHEP::eplus;}
	    }
	}
      catch (std::invalid_argument) // if stod can't convert number to double / int
	{throw std::exception();}
    }
}
