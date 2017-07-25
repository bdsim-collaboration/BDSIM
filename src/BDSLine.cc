#include "BDSDebug.hh"
#include "BDSLine.hh"

#include <vector>

BDSLine::BDSLine(G4String nameIn):
  BDSAcceleratorComponent(nameIn,0,0,"line")
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

void BDSLine::SetBiasVacuumList(std::list<std::string> biasVacuumListIn)
{
  BDSAcceleratorComponent::SetBiasVacuumList(biasVacuumListIn); // set if for this object
  for (auto component : *this)
    {component->SetBiasVacuumList(biasVacuumListIn);}
}

void BDSLine::SetBiasMaterialList(std::list<std::string> biasMaterialListIn)
{
  BDSAcceleratorComponent::SetBiasMaterialList(biasMaterialListIn);
  for (auto component : *this)
    {component->SetBiasMaterialList(biasMaterialListIn);}
}

void BDSLine::SetRegion(G4String regionIn)
{
  BDSAcceleratorComponent::SetRegion(regionIn); // set it for this object
  for (auto component: *this)
    {component->SetRegion(regionIn);}
}

  
