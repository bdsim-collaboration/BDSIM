#include "BDSDebug.hh"
#include "BDSExecOptions.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSGeometryDriver.hh"

BDSGeometryDriver::BDSGeometryDriver(){
}

BDSGeometryDriver::BDSGeometryDriver(G4String geomFile, G4LogicalVolume* markerVol, G4double markerLength):
  itsGeomFile(geomFile), itsMarkerLength(markerLength), itsMarkerVol(markerVol){
}

BDSGeometryDriver::~BDSGeometryDriver(){
}

void BDSGeometryDriver::Construct()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
}
 
