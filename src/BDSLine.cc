#include "BDSDebug.hh"
#include "BDSLine.hh"

#include <vector>

BDSLine::BDSLine(G4String name):
  BDSAcceleratorComponent(name,0,0,"line")
{;}

void BDSLine::AddComponent(BDSAcceleratorComponent* component)
{line.push_back(component);}

void BDSLine::Initialise()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  for (iterator it = begin(); it != end(); ++it)
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "Initialising component named: " << (*it)->GetName() << G4endl;
#endif
      (*it)->Initialise();
    }
}
