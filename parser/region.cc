#include "region.h"

using namespace GMAD;

Region::Region()
{
  clear();
  PublishMembers();
}

void Region::clear()
{
  // In case these aren't explictly set, these are set to a sensible
  // default of 1mm (SI units here) - the Geant4 default.  All are copied
  // to a region definition in BDSDetectorConstruction.
  prodCutPhotons   = 0.001;
  prodCutElectrons = 0.001;
  prodCutPositrons = 0.001;
  prodCutProtons   = 0.001;
}

void Region::PublishMembers()
{
  publish("name",            &Region::name);
  publish("prodCutPhotons",  &Region::prodCutPhotons);
  publish("prodCutElectrons",&Region::prodCutElectrons);
  publish("prodCutPositrons",&Region::prodCutPositrons);
  publish("prodCutProtons",  &Region::prodCutProtons);
}

void Region::print()const
{
  std::cout << "region: "
	    << name             << " "
	    << prodCutPhotons   << " "
	    << prodCutElectrons << " "
	    << prodCutPositrons << " "
	    << prodCutProtons
	    << std::endl;
}
