#include "BDSSampler.hh"
#include "BDSSamplerRegistry.hh"

#include "globals.hh"
#include "G4Transform3D.hh"

#include <vector>

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

G4int BDSSamplerRegistry::RegisterSampler(G4String      name,
					  BDSSampler*   sampler,
					  G4Transform3D transform,
					  G4double      S)
{
  names.push_back(name);
  samplers.push_back(sampler);
  transforms.push_back(transform);
  transformInverses.push_back(transform.inverse());
  sPosition.push_back(S);

  G4int index = numberOfEntries; // copy the number of entires / the index of this entry
  numberOfEntries++;
  return index;
}
  
