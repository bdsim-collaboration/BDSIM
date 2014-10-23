#include "BDSSquareTunnel.hh"

BDSSquareTunnel::BDSSquareTunnel(){
}

BDSSquareTunnel::~BDSSquareTunnel(){
}

void BDSSquareTunnel::CalculateDimensions(){
  _offsetY = _radius - _beamlineCeilingHeight;
  //Set position and rotation.
  tunnelTrans.setX(_offsetX);
  tunnelTrans.setY(_offsetY);
  tunnelTrans.setZ(0);
}

void BDSSquareTunnel::BuildSolidVolumes(){
  
  //Build the geometric solids.
  _solid=new G4Box(_motherName+"_tun_solid",
		   _radius+BDSGlobalConstants::Instance()->GetTunnelThickness(),
		   _floorCeilingHeight/2.0+BDSGlobalConstants::Instance()->GetTunnelThickness(),
		   _length/2.0
		   );
  
  G4VSolid* SoilSolid_tmp=new G4Box(_motherName+"_tun_soil_solid",
				    _radius+BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetLengthSafety()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness(),	
				    _floorCeilingHeight/2.0+BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness(),		
				    _length/2.0
				    );
  
  _innerTunnelSolid=new G4Box(_motherName+"_inner_tun_solid",
			      _radius,
			      _floorCeilingHeight/2.0,
			      _length/2.0
			      );
  
  
#ifdef BDSDEBUG
  G4cout << "Building tunnel walls" << G4endl;
#endif
  _tunnelWallsSolid = new G4SubtractionSolid(
					_motherName+"_tun_walls_log",
					_tunnelSolid,
					_innerTunnelSolid,
					     );
  
#ifdef BDSDEBUG
  G4cout << "Building soil around tunnel" << G4endl;
#endif
  _soilSolid = new G4SubtractionSolid(
				      _motherName+"_tun_soil",
				      SoilSolid_tmp,
				      _tunnelSolid,
				      );

}


