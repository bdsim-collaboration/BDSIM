#include "BDSGlobalConstants.hh" 
#include "BDSCCDCamera.hh"
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

BDSCCDCamera::BDSCCDCamera ()
{
  defaultDimensions();
  build();
}

void BDSCCDCamera::defaultDimensions(){
  _size.setX(110.7*mm);
  _size.setY(137.2*mm);
  _size.setZ(231.0*mm);
}

void BDSCCDCamera::build(){
  buildMotherVolume();
  buildObjectLens();
  buildImageLens();
  buildCCDChip();
  placeComponents();
}

void BDSCCDCamera::buildMotherVolume(){
  _solid=new G4Box( _name+"_solid",
		     _size.x()/2.0,
		    _size.y()/2.0,
		    _size.z()/2.0);
  
  _log=new G4LogicalVolume
    (_solid, 
     BDSMaterials::Instance()->GetMaterial("vacuum"),
     _name+"_log");
}

void BDSCCDCamera::buildObjectLens(){
  _objectLens = new BDSLens(_name+"ObjectLens",25.4*mm, 1029.8*mm,2.2*mm); //Focal length 1m lens (Thorlabs LB1409-A)
}
void BDSCCDCamera::buildImageLens(){
  _imageLensnew new BDSLens(_name+"ObjectLens",12.7*mm, 14.6*mm, 4.7*mm); //Focal length 15mm lens (Thorlabs LB1092-A) (magnification factor = 66.4)
}
void BDSCCDCamera::buildCCDChip(){
  G4ThreeVector pixelSize;
  G4ThreeVector nPixels;
  pixelSize.setX(13.5e-3*mm);
  pixelSize.setY(13.5e-3*mm);
  pixelSize.setZ(13.5e-3*mm); //Assume that the pixels are cubes for the moment.
  nPixels.setX(2048);
  nPixels.setY(512);
  _ccdChip = new BDSCCDChip(_name+"_CCDChip", pixelSize, nPixels);
}

void BDSCCDCamera::placeComponents(){
}


void BDSCCDCamera::buildPixels(){
  G4ThreeVector pos, offset;
  offset.setZ(0);
  offset.setX(-_size.x()/2.0+_pixel->size().x());
  offset.setX(_size.y()/2.0+_pixel->size().y());
  pos.setZ(0);
  G4int copyNumber=0;
  for (int i=0; i<_nPixels.x(); i++){
    for(int j=0; j<_nPixels.y(); j++, copyNumber++){
      pos.setX(offset.x()+i*_pixel->size().x());
      pos.setY(offset.y()-j*_pixel->size().y());
      new G4PVPlacement(0,
			pos,
			_pixel->log(),
			_pixel->name()+"_phys",
			_log,
			true,
			copyNumber,
			BDSGlobalConstants::Instance()->GetCheckOverlaps()
			);
    }
  }
}


BDSCCDCamera::~BDSCCDCamera()
{
}

