#include "BDSRectangleTunnel.hh"
#include "G4Box.hh"
#include "G4SubtractionSolid.hh"
#include "BDSTunnel.hh"
#include "BDSGlobalConstants.hh"

BDSRectangleTunnel::~BDSRectangleTunnel(){
}

BDSRectangleTunnel::BDSRectangleTunnel(Element element, G4double length, G4double angle) : BDSTunnel(element, length, angle){
  CalculateDimensions();
}

void BDSRectangleTunnel::CalculateDimensions(){
  _floorCeilingHeight = _floorBeamlineHeight+_beamlineCeilingHeight;
  offsetY(_floorCeilingHeight/2.0-_floorBeamlineHeight);
  //Set position and rotation.
  _trans.setX(offsetX());
  _trans.setY(offsetY());
  _trans.setZ(0);
}

void BDSRectangleTunnel::BuildSolidVolumes(){
  

  //Build the geometric solids.
  _solid=new G4Box(_motherName+"_tun_solid",
		   radius()+thickness(),
		   _floorCeilingHeight/2.0+thickness(),
		   _length/2.0
		   );
  
  G4VSolid* SoilSolid_tmp=new G4Box(_motherName+"_tun_soil_solid",
				    radius()+thickness()+BDSGlobalConstants::Instance()->GetLengthSafety()+soilThickness(),	
				    _floorCeilingHeight/2.0+thickness()+soilThickness(),		
				    _length/2.0
				    );
  
  _innerSolid=new G4Box(_motherName+"_inner_tun_solid",
			      radius(),
			      _floorCeilingHeight/2.0,
			      _length/2.0
			      );
  
  
#ifdef BDSDEBUG
  G4cout << "Building tunnel walls" << G4endl;
#endif
  _wallsSolid = new G4SubtractionSolid(
					_motherName+"_tun_walls_log",
					_solid,
					_innerSolid
					     );
  
#ifdef BDSDEBUG
  G4cout << "Building soil around tunnel" << G4endl;
#endif
  _soilSolid = new G4SubtractionSolid(
				      _motherName+"_tun_soil",
				      SoilSolid_tmp,
				      _solid
				      );

}


