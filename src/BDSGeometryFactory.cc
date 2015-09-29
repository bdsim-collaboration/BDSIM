#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGeometryFactory.hh"
#include "BDSGeometryType.hh"
#include "BDSUtilities.hh"

#include "ggmad.hh"

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
{
  fileName = "";
}

BDSGeometryFactory::~BDSGeometryFactory(){}

BDSGeometry* BDSGeometryFactory::BuildGeometry(G4String formatAndFilePath)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "format and path: " << formatAndFilePath << G4endl;
#endif
  
  std::pair<G4String, G4String> ff = BDS::SplitOnColon(formatAndFilePath);
  fileName = BDS::GetFullPath(ff.first);
  BDSGeometryType format = BDS::DetermineGeometryType(ff.second);

  switch(format.underlying())
    {/*
    case BDSGeometryType::gmad:
      {return BuildGmad(); break;}
#ifdef USE_LCDD
    case BDSGeometryType::lcdd:
      {return BuildLCDD(); break;}
#endif
     */
    case BDSGeometryType::mokka:
      {return BuildMokka(); break;}

#ifdef USE_GDML
    case BDSGeometryType::gdml:
      {return BuildGDML(); break;}
#endif
      
    default:
#ifdef BDSDEBUG
      G4cout << __METHOD_NAME__ << "no geometry format specified - not building anything" << G4endl;
#endif
      return nullptr;
      break;
    }
}
/*
BDSGeometry* BDSGeometryFactory::BuildGmad()
{return new GGmadDriver(fileName);}

#ifdef USE_LCDD
BDSGeometry* BDSGeometryFactory::BuildLCDD()
{return new BDSGeometryLCDD(fileName);}
#endif
*/
BDSGeometry* BDSGeometryFactory::BuildMokka()
{return new BDSGeometrySQL(fileName);}

#ifdef USE_GDML
BDSGeometry* BDSGeometryFactory::BuildGDML()
{return new BDSGeometryGDML(fileName);}
#endif


