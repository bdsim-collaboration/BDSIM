#include "BDSSampler.hh"
#include "BDSSamplerRegistry.hh"

#include "globals.hh"
#include "G4Transform3D.hh"

#include <vector>

class BDSBeamlineElement;

BDSSamplerRegistry* BDSSamplerRegistry::instance = nullptr;

BDSSamplerRegistry* BDSSamplerRegistry::Instance()
{
  if (!instance)
    {instance = new BDSSamplerRegistry();}
  return instance;
}

BDSSamplerRegistry::BDSSamplerRegistry():
  numberOfEntries(0)
{;}

BDSSamplerRegistry::~BDSSamplerRegistry()
{
  instance = nullptr;
}

G4int BDSSamplerRegistry::RegisterSampler(G4String            name,
					  BDSSampler*         sampler,
					  G4Transform3D       transform,
					  G4double            S,
					  BDSBeamlineElement* element)
{
  BDSSamplerInfo info = BDSSamplerInfo(name, sampler, transform, S, element);
  return RegisterSampler(info);
}

G4int BDSSamplerRegistry::RegisterSampler(BDSSamplerInfo& info)
{
  infos.push_back(info);

  G4int index = numberOfEntries; // copy the number of entires / the index of this entry
  numberOfEntries++;
  return index;
}

std::vector<G4String> BDSSamplerRegistry::GetNames() const
{
  std::vector<G4String> names;
  for (auto info : infos)
    {names.push_back(info.Name());}
  return names;
}
