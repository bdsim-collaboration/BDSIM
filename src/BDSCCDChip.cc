#include "BDSGlobalConstants.hh" 
#include "BDSCCDChip.hh"
#include "BDSCCDPixel.hh"
#include "BDSSampler.hh"
#include "BDSSamplerSD.hh"
#include "BDSOutput.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"               
#include "G4SubtractionSolid.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"
#include "G4OpticalSurface.hh"
#include "G4LogicalBorderSurface.hh"
#include "BDSDebug.hh"

#include "G4SDManager.hh"
#include "G4UserLimits.hh"
#include "G4Version.hh"
#include "parser/gmad.h"
#include <map>
#include "G4TwoVector.hh"
#include "BDSMaterials.hh"

BDSCCDChip::BDSCCDChip (G4String aName, G4ThreeVector pixelSize, G4TwoVector nPixels):
  _name(aName), _pixel(new BDSCCDPixel(pixelSize,aName+"pixel")),_nPixels(nPixels)
{
  computeDimensions();  
  build();
}

void BDSCCDChip::computeDimensions(){
  G4cout << __METHOD_NAME__ << G4endl;
  _size.setX(_pixel->size().x()*_nPixels.x());
  _size.setY(_pixel->size().y()*_nPixels.y());
  _size.setZ(_pixel->size().z());
  G4cout << __METHOD_END__ << G4endl;

}


void BDSCCDChip::build(){
  G4cout << __METHOD_NAME__ << G4endl;
  buildMotherVolume();
  buildPixels();
  G4cout << __METHOD_END__ << G4endl;
}

void BDSCCDChip::buildMotherVolume(){
  G4cout << __METHOD_NAME__ << G4endl;
  _solid=new G4Box( _name+"_solid",
		     _size.x()/2.0,
		    _size.y()/2.0,
		    _size.z()/2.0);
  
  _log=new G4LogicalVolume
    (_solid, 
     BDSMaterials::Instance()->GetMaterial("vacuum"),
     _name+"_log");
  G4cout << __METHOD_END__ << G4endl;
}



void BDSCCDChip::buildPixels(){
  G4cout << __METHOD_NAME__ << G4endl;
  G4ThreeVector pos, offset;
  offset.setZ(0);
  offset.setX(-_size.x()/2.0+_pixel->size().x()/2.0);
  offset.setY(_size.y()/2.0-_pixel->size().y()/2.0);
  pos.setZ(0);
  G4int copyNumber=0;
  std::stringstream ss;
  for (int i=0; i<_nPixels.x(); i++){
    for(int j=0; j<_nPixels.y(); j++, copyNumber++){
      ss.str("");
      ss << _pixel->name().data() << "_phys_" << copyNumber;
      pos.setX(offset.x()+i*_pixel->size().x());
      pos.setY(offset.y()-j*_pixel->size().y());
      
      new G4PVPlacement(0,
			pos,
			_pixel->log(),
			ss.str(),
			_log,
			true,
			copyNumber,
			false//BDSGlobalConstants::Instance()->GetCheckOverlaps()
			);
    }
  }
  G4cout << __METHOD_END__ << G4endl;
}


BDSCCDChip::~BDSCCDChip()
{
}

