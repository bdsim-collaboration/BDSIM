#include "BDSDebug.hh"
#include "BDSLine.hh"

#include <vector>


BDSLine::BDSLine(G4String name):
  BDSAcceleratorComponent(name,0,0,"line")
{;}

void BDSLine::addComponent(BDSAcceleratorComponent* component)
{
  line.push_back(component);
}

void BDSLine::Intialise()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  for (BDSLineIterator it = begin(); it != end(); ++it)
    {(*it)->Initialise();}
}
