#include "BDSDebug.hh"
#include "BDSGeometryExternal.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGeometryFactoryBase.hh"
#ifdef USE_GDML
#include "BDSGeometryFactoryGDML.hh"
#endif
#include "BDSGeometryFactoryGMAD.hh"
#include "BDSGeometryFactorySQL.hh"
#include "BDSGeometryType.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals

#ifdef USE_LCDD
#include "BDSGeometryLCDD.hh"
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
#ifdef USE_GDML
    case BDSGeometryType::gdml:
      {return BDSGeometryFactoryGDML::Instance(); break;}
#endif
    case BDSGeometryType::gmad:
      {return BDSGeometryFactoryGMAD::Instance(); break;}
    case BDSGeometryType::mokka:
      {return BDSGeometryFactorySQL::Instance(); break;}
    default:
      {
	G4cout << "Unsupported factory type " << type;
	return nullptr;
	break;
      }
    }
}

BDSGeometryExternal* BDSGeometryFactory::BuildGeometry(G4String formatAndFileName,
						       std::map<G4String, G4Colour*>* colourMapping,
						       G4double suggestedLength,
						       G4double suggestedOuterDiameter)
{
  std::pair<G4String, G4String> ff = BDS::SplitOnColon(formatAndFileName);
  G4String fileName = BDS::GetFullPath(ff.second);

  const auto search = registry.find(fileName);
  if (search != registry.end())
    {return search->second;}// it was found already in registry
  // else wasn't found so continue

  // Check the file exists.
  if (!BDS::FileExists(fileName))
    {G4cerr << "No such file \"" << fileName << "\"" << G4endl; exit(1);}
  
  BDSGeometryType format = BDS::DetermineGeometryType(ff.first);
  BDSGeometryFactoryBase* factory = GetAppropriateFactory(format);
  if (!factory)
    {return nullptr;}
  
  BDSGeometryExternal* result = factory->Build(fileName, colourMapping,
					       suggestedLength, suggestedOuterDiameter);
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
#ifdef USE_LCDD
    case BDSGeometryType::lcdd:
      {result = BuildLCDD(fileName); break;}
#endif
    default:
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__
	     << "no geometry format specified - not building anything" << G4endl;
#endif
      break;
    }
  return result;
}

#ifdef USE_LCDD
BDSGeometry* BDSGeometryFactory::BuildLCDD(G4String fileName)
{return new BDSGeometryLCDD(fileName);}
#endif
