#include "BDSAcceleratorComponentRegistry.hh"
#include "BDSDebug.hh"
#include "BDSLine.hh"

#include <ostream>
#include <iomanip>
#include <stdexcept>


BDSAcceleratorComponentRegistry* BDSAcceleratorComponentRegistry::_instance = nullptr;

BDSAcceleratorComponentRegistry* BDSAcceleratorComponentRegistry::Instance()
{
  if (_instance == nullptr)
    {_instance = new BDSAcceleratorComponentRegistry();}
  return _instance;
}

BDSAcceleratorComponentRegistry::BDSAcceleratorComponentRegistry()
{;}

BDSAcceleratorComponentRegistry::~BDSAcceleratorComponentRegistry()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "size of registry " << registry.size() << G4endl;
#endif
  iterator i = registry.begin();
  for (; i != registry.end(); ++i)
    {delete i->second;}
  for (auto ac : allocatedComponents)
    {delete ac;}
  
  _instance = nullptr;
}

void BDSAcceleratorComponentRegistry::RegisterComponent(BDSAcceleratorComponent* component,
							bool isModified)
{
  // If the component was modified beyond its original element definition in the parser,
  // ie a drift was modified to match a pole face of a bend, then store if for memory
  // management, but not in the registry
  if (isModified)
    {
      allocatedComponents.push_back(component);
      // if line then also add constituents
      if (BDSLine* line = dynamic_cast<BDSLine*>(component))
	{
	  for (const auto element : *line)
	    {allocatedComponents.push_back(element);}
	}
      return;
    }
  
  if (IsRegistered(component->GetName()))
    {// don't register something that's already registered
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "already registered - not registering again" << G4endl;
#endif
      return;
    } 

  if (BDSLine* line = dynamic_cast<BDSLine*>(component))
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "component is a line - registering the line and its contents" << G4endl;
#endif
      // register the line object itself
      registry[component->GetName()] = component;
      // now add all the components of the line individually using this very function
      for (const auto element : *line)
	{RegisterComponent(element, false);}
    }
  else
    {
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "registering component \"" << component->GetName() << "\"" << G4endl;
#endif
      registry[component->GetName()] = component;}
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "size of registry " << registry.size() << G4endl;
#endif
}

G4bool BDSAcceleratorComponentRegistry::IsRegistered(BDSAcceleratorComponent* component)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "(BDSAcceleratorComponent*)" << G4endl;
#endif
  return IsRegistered(component->GetName());
}

G4bool BDSAcceleratorComponentRegistry::IsRegistered(G4String name)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "(G4String) named \"" << name << "\" -> ";
#endif
  iterator search = registry.find(name);
  if (search == registry.end())
    {
#ifdef BDSDEBUG
      G4cout << "not registered" << G4endl;
#endif
      return false;
    }
  else
    {
#ifdef BDSDEBUG
      G4cout << "registered" << G4endl;
#endif
      return true;
    }
}

BDSAcceleratorComponent* BDSAcceleratorComponentRegistry::GetComponent(G4String name)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  try
    {return registry.at(name);}
  catch (const std::out_of_range& /*oor*/)
    {
      G4cerr << __METHOD_NAME__ << "unkown component named: \"" << name << "\"" << G4endl;
      return nullptr;
    }
}  

std::ostream& operator<< (std::ostream &out, BDSAcceleratorComponentRegistry const &r)
{
  // save flags since std::left changes the stream
  std::ios_base::fmtflags ff = out.flags();
  out << "Accelerator Component Registry:" << G4endl;
  BDSAcceleratorComponentRegistry::const_iterator it = r.registry.begin();
  for (; it != r.registry.end(); ++it)
    {out << std::left << std::setw(15) << it->second->GetType() << " \"" << it->first << "\"" << G4endl;;}
  // reset flags
  out.flags(ff);
  return out;
}
