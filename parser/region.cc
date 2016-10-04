#include "region.h"

using namespace GMAD;

Region::Region()
{
  clear();
  PublishMembers();
}

void Region::clear()
{
  prodCutPhotons   = 0.0;
  prodCutElectrons = 0.0;
  prodCutPositrons = 0.0;
  prodCutHadrons   = 0.0;
}

void Region::PublishMembers()
{
  publish("name",            &Region::name);
  publish("prodCutPhotons",  &Region::prodCutPhotons);
  publish("prodCutElectrons",&Region::prodCutElectrons);
  publish("prodCutPositrons",&Region::prodCutPositrons);
  publish("prodCutHadrons",  &Region::prodCutHadrons);
}

void Region::print()const
{
  std::cout << "region: "
	    << name             << " "
	    << prodCutPhotons   << " "
	    << prodCutElectrons << " "
	    << prodCutPositrons << " "
	    << prodCutHadrons
	    << std::endl;
}
