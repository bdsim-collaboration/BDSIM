#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGeometryFactoryBase.hh"
#include "BDSGeometryFactoryGDML.hh"
#include "BDSGeometryGMAD.hh"
#include "BDSGeometryType.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals

#ifdef USE_LCDD
#include "BDSGeometryLCDD.hh"
#endif
#include "BDSGeometrySQL.hh"
#ifdef USE_GDML
#include "BDSGeometryGDML.hh"
#endif

#include <string>
#include <unordered_map>
#include <utility>

class BDSGeometry;

BDSGeometryFactory* BDSGeometryFactory::instance = nullptr;

BDSGeometryFactory* BDSGeometryFactory::Instance()
{
  if (!instance)
    {instance = new BDSGeometryFactory();}
  return instance;
}

BDSGeometryFactory::BDSGeometryFactory()
{;}

BDSGeometryFactory::~BDSGeometryFactory()
{
  instance = nullptr;
  for (auto& geom : storage)
    {delete geom;}
}

BDSGeometryFactoryBase* BDSGeometryFactory::GetAppropriateFactory(BDSGeometryType type)
{
  switch(type.underlying())
    {
    case BDSGeometryType::gdml:
      {return BDSGeometryFactoryGDML::Instance(); break;}
    default:
      {
	G4cout << "Unsupported factory type " << type;
	return nullptr;
	break;
      }
    }
}

BDSGeometryExternal* BDSGeometryFactory::BuildGeometry(G4String formatAndFileName,
						       std::map<G4String, G4Colour*>* colourMapping)
{
  std::pair<G4String, G4String> ff = BDS::SplitOnColon(formatAndFileName);
  G4String      fileName = BDS::GetFullPath(ff.second);

  const auto search = registry.find(fileName);
  if (search != registry.end())
    {// it was found already in registry
      return search->second;
    }
  // else wasn't found so continue
  
  BDSGeometryType format = BDS::DetermineGeometryType(ff.first);
  BDSGeometryFactoryBase* fac = GetAppropriateFactory(format);
  BDSGeometryExternal* result = fac->Build(fileName, colourMapping);

  if (result)
    {
      registry[(std::string)fileName] = result;
      storage.push_back(result);
    }
  
  return result;
}

BDSGeometry* BDSGeometryFactory::BuildGeometryOld(G4String formatAndFilePath)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "format and path: " << formatAndFilePath << G4endl;
#endif
  
  std::pair<G4String, G4String> ff = BDS::SplitOnColon(formatAndFilePath);
  G4String      fileName = BDS::GetFullPath(ff.second);
  BDSGeometryType format = BDS::DetermineGeometryType(ff.first);
  BDSGeometry* result = nullptr;
  
  switch(format.underlying())
    {
    case BDSGeometryType::gmad:
      {result = BuildGMAD(fileName); break;}
      
#ifdef USE_LCDD
    case BDSGeometryType::lcdd:
      {result = BuildLCDD(fileName); break;}
#endif

    case BDSGeometryType::mokka:
      {result = BuildMokka(fileName); break;}

#ifdef USE_GDML
    case BDSGeometryType::gdml:
      {result =  BuildGDML(fileName); break;}
#endif
      
    default:
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "no geometry format specified - not building anything" << G4endl;
#endif
      break;
    }
  return result;
}

BDSGeometry* BDSGeometryFactory::BuildGMAD(G4String fileName)
{return new BDSGeometryGMAD(fileName);}

#ifdef USE_LCDD
BDSGeometry* BDSGeometryFactory::BuildLCDD(G4String fileName)
{return new BDSGeometryLCDD(fileName);}
#endif

BDSGeometry* BDSGeometryFactory::BuildMokka(G4String fileName)
{return new BDSGeometrySQL(fileName);}

#ifdef USE_GDML
BDSGeometry* BDSGeometryFactory::BuildGDML(G4String fileName)
{return new BDSGeometryGDML(fileName);}
#endif


