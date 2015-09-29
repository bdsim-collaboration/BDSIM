#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGeometryGMad.hh"
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

#include <utility>

class BDSGeometry;

BDSGeometryFactory::BDSGeometryFactory()
{;}

BDSGeometryFactory::~BDSGeometryFactory()
{;}

BDSGeometry* BDSGeometryFactory::BuildGeometry(G4String formatAndFilePath)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "format and path: " << formatAndFilePath << G4endl;
#endif
  
  std::pair<G4String, G4String> ff = BDS::SplitOnColon(formatAndFilePath);
  G4String fileName = BDS::GetFullPath(ff.second);
  BDSGeometryType format = BDS::DetermineGeometryType(ff.first);

  switch(format.underlying())
    {
    case BDSGeometryType::gmad:
      {return BuildGmad(fileName); break;}
      
#ifdef USE_LCDD
    case BDSGeometryType::lcdd:
      {return BuildLCDD(fileName); break;}
#endif

    case BDSGeometryType::mokka:
      {return BuildMokka(fileName); break;}

#ifdef USE_GDML
    case BDSGeometryType::gdml:
      {return BuildGDML(fileName); break;}
#endif
      
    default:
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "no geometry format specified - not building anything" << G4endl;
#endif
      return nullptr;
      break;
    }
}

BDSGeometry* BDSGeometryFactory::BuildGmad(G4String fileName)
{return new BDSGeometryGMad(fileName);}

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


