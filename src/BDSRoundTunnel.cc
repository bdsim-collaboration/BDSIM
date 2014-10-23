#include "BDSRoundTunnel.hh"

BDSRoundTunnel::BDSRoundTunnel(){
}

BDSRoundTunnel::~BDSRoundTunnel(){
}

void BDSRoundTunnel::CalculateDimensions(){
  _offsetY = _radius - _beamlineCeilingHeight;
  tunnelTrans.setX(_offsetX);
  tunnelTrans.setY(_offsetY);
  tunnelTrans.setZ(0);

  //Size of a block used in geometry construction.
  _blockSize = 4*_radius;
  //A vector used for construction the tunnel floor.
  _floorOffset = G4ThreeVector(0,-blockSize-_floorBeamlineHeight + _offsetY,0);
  
}

void BDSRoundTunnel::BuildStraightSolids(){
  _tunnelSizedBlock = new G4Box(
				_motherName+"_bnd_sized_block_solid", 
				_blockSize,
				_blockSize,
				(_length-BDSGlobalConstants::Instance()->GetLengthSafety())
				);
  
  _tunnelSolid=new G4Tubs(_motherName+"_tun_solid",
			  0,
			  _radius+BDSGlobalConstants::Instance()->GetTunnelThickness(),
			  (_length-BDSGlobalConstants::Instance()->GetLengthSafety())/2.0,
			  0,CLHEP::twopi*CLHEP::radian);
  
  _soilSolid=new G4Tubs(_motherName+"_tun_soil_solid",
			_radius+BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetLengthSafety(),
			_radius+BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetLengthSafety()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness(),	
			(_length-BDSGlobalConstants::Instance()->GetLengthSafety())/2.0,
			0,CLHEP::twopi*CLHEP::radian);
  
  _innerTunnelSolid=new G4Tubs(_motherName+"_inner_tun_solid",
			       0.,
			       _radius,
			       (_length-BDSGlobalConstants::Instance()->GetLengthSafety())/2,
			       0,CLHEP::twopi*CLHEP::radian);
  
  _largerInnerTunnelSolid=new G4Tubs(_motherName+"_inner_tun_solid",
				     0.,
				     _radius+BDSGlobalConstants::Instance()->GetLengthSafety(),
				     (_length-BDSGlobalConstants::Instance()->GetLengthSafety()),
				     0,CLHEP::twopi*CLHEP::radian);
  
  
  
  
  _tunnelCavity = new G4SubtractionSolid(
                                         _motherName +"_tun_cavity_solid", 
                                         _innerTunnelSolid,
                                         _tunnelSizedBlock,
                                         nullRotationMatrix,
                                         _floorOffset
                                         );
  
  _largerTunnelCavity = new G4SubtractionSolid(
					       _motherName +"_tun_cavity_solid", 
					       _largerInnerTunnelSolid,
					       _tunnelSizedBlock,
					       nullRotationMatrix,
					       _floorOffset
					       );
  
}

void BDSRoundTunnel::BuildAngleSolids(){
  G4double pi_ov_2 = asin(1.);
  tunnelRot->rotateY(pi_ov_2);
  
  G4double xHalfLengthPlus, xHalfLengthMinus, tunHalfLen;
  xHalfLengthMinus = (_length/_motherComponent->GetAngle())*sin(_motherComponent->GetAngle()/2)
    - fabs(cos(_motherComponent->GetAngle()/2)) * (_radius + BDSGlobalConstants::Instance()->GetTunnelThickness()) * tan(_motherComponent->GetAngle()/2)/2;
  
  xHalfLengthPlus = (_length/_motherComponent->GetAngle())*sin(_motherComponent->GetAngle()/2)
    + fabs(cos(_motherComponent->GetAngle()/2)) * (_radius + BDSGlobalConstants::Instance()->GetTunnelThickness()) * tan(_motherComponent->GetAngle()/2)/2;
  
  tunHalfLen = std::max(xHalfLengthPlus,xHalfLengthMinus);
  tunnelTrans.setZ(0);
  tunnelTrans.setX(0);
  tunnelTrans.setY(_offsetY);
  
  
  
#ifdef BDSDEBUG
    G4cout << "Building a block" << G4endl;
#endif
    _tunnelSizedBlock = new G4Box(
                                    _motherName+"_bnd_sized_block_solid", 
                                    _blockSize,
                                    _blockSize,
                                    tunHalfLen
				    );

    
#ifdef BDSDEBUG
    G4cout << "Building a tunnel solid" << G4endl;
#endif

    _tunnelSolid = new G4IntersectionSolid(
                                             _motherName+"_tun_solid",
                                             new G4Tubs(
                                                        _motherName+"_temp_tun_solid",
                                                        0,
                                                        _radius+BDSGlobalConstants::Instance()->GetTunnelThickness(),
                                                        tunHalfLen,
                                                        0,CLHEP::twopi*CLHEP::radian),			    
                                             _motherVolume->GetSolid(),
					     BDSGlobalConstants::Instance()->RotYM90(),
                                             nullThreeVector
                                             ); 
#ifdef BDSDEBUG
    G4cout << "Building a soil tunnel solid" << G4endl;
#endif
    _soilSolid = new G4IntersectionSolid(
                                           _motherName+"_soil_solid",
                                           new G4Tubs(
                                                      _motherName+"_temp_soil_solid",
                                                      _radius+BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetLengthSafety(),
                                                      _radius+BDSGlobalConstants::Instance()->GetTunnelThickness()+BDSGlobalConstants::Instance()->GetLengthSafety()+BDSGlobalConstants::Instance()->GetTunnelSoilThickness(),
                                                      tunHalfLen,
                                                      0,CLHEP::twopi*CLHEP::radian),
                                           _motherVolume->GetSolid(),
                                           BDSGlobalConstants::Instance()->RotYM90(),
                                           nullThreeVector
                                           ); 
#ifdef BDSDEBUG   
    G4cout << "Building inner tunnel solid" << G4endl;
#endif
    _innerTunnelSolid=new G4IntersectionSolid(
                                                _motherName+"_inner_tun_solid",
                                                new G4Tubs(_motherName+"_temp_inner_tun_solid",
                                                           0.,
                                                           _radius,
                                                           tunHalfLen,
                                                           0,CLHEP::twopi*CLHEP::radian),
                                                _motherVolume->GetSolid(),
                                                BDSGlobalConstants::Instance()->RotYM90(),
                                                nullThreeVector
                                                ); 
#ifdef BDSDEBUG
    G4cout << "Building larger inner tunnel solid" << G4endl;
#endif
    _largerInnerTunnelSolid= new G4Tubs(_motherName+"_temp_inner_tun_solid",
                                          0.,
                                          _radius+BDSGlobalConstants::Instance()->GetLengthSafety(),
                                          2*tunHalfLen,
                                          0,CLHEP::twopi*CLHEP::radian);
                                                       
    
    
#ifdef BDSDEBUG
    G4cout << "Building tunnel cavity" << G4endl;
#endif
    _tunnelCavity = new G4SubtractionSolid(
                                         _motherName +"_tun_cavity_solid", 
                                         _innerTunnelSolid,
                                         _tunnelSizedBlock,
                                         nullRotationMatrix,
                                         _floorOffset
                                         );
#ifdef BDSDEBUG
    G4cout << "Building larger tunnel cavity" << G4endl;
#endif
   _largerTunnelCavity = new G4SubtractionSolid(
                                         _motherName +"_tun_cavity_solid", 
                                         _largerInnerTunnelSolid,
                                         _tunnelSizedBlock,
                                         nullRotationMatrix,
                                         _floorOffset
                                         );
}

void BDSRoundTunnel::BuildSolidVolumes(){
  
  tunnelRot=new G4RotationMatrix();
  
  
  if ( _motherComponent->GetAngle()==0 ){ //Build a straight tunnel using tubes
    BuildStraightSolids();
  } else {
    BuildAngleSolids();
  }
  
#ifdef BDSDEBUG
  G4cout << "Building tunnel walls" << G4endl;
#endif
  _tunnelWalls = new G4SubtractionSolid(
					_motherName+"_tun_walls",
					_tunnelSolid,
					_largerTunnelCavity
					);
}

