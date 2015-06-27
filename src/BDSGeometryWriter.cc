#include "BDSGeometryWriter.hh"

#include "globals.hh" // geant4 types / globals
#include "G4GDMLParser.hh"
#include "G4TransportationManager.hh"

class G4VPhysicalVolume;

BDSGeometryWriter* BDSGeometryWriter::_instance = 0;

BDSGeometryWriter* BDSGeometryWriter::Instance()
{
  if (_instance == 0)
    {_instance = new BDSGeometryWriter();}
  return _instance;
}

BDSGeometryWriter::~BDSGeometryWriter()
{
  _instance = 0;
}

BDSGeometryWriter::BDSGeometryWriter()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
}

void BDSGeometryWriter::ExportGeometry(G4String geometryType,
				       G4String geometryFileName)
{
  if (!strcmp(geometryType, "gdml"))
    {WriteToGDML(geometryFileName);}
}

void BDSGeometryWriter::WriteToGDML(G4String           outputFileName,
				    G4VPhysicalVolume* volume)
{
  if (!volume)
    {volume = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume();}
  G4GDMLParser* parser = new G4GDMLParser();
  parser->Write(outputFileName, volume, true);
}
