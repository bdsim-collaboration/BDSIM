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

void BDSLine::SetBiasVacuumList(std::list<std::string> biasVacuumList)
{
  BDSAcceleratorComponent::SetBiasVacuumList(biasVacuumList); // set if for this object
  for (auto component : *this)
    {component->SetBiasVacuumList(biasVacuumList);}
}

void BDSLine::SetBiasMaterialList(std::list<std::string> biasMaterialList)
{
  BDSAcceleratorComponent::SetBiasMaterialList(biasMaterialList);
  for (auto component : *this)
    {component->SetBiasMaterialList(biasMaterialList);}
}

void BDSLine::SetRegion(G4String region)
{
  BDSAcceleratorComponent::SetRegion(region); // set it for this object
  for (auto component: *this)
    {component->SetRegion(region);}
}

  
