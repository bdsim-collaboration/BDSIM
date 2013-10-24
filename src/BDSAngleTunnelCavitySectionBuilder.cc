#include "BDSAngleTunnelCavitySectionBuilder.hh"
#include "G4IntersectionSolid.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"



BDSAngleTunnelCavitySectionBuilder::BDSAngleTunnelCavitySectionBuilder(){
}

BDSAngleTunnelCavitySectionBuilder::~BDSAngleTunnelCavitySectionBuilder(){
}

void BDSAngleTunnelCavitySectionBuilder::build(){
  G4double xHalfLengthMinus=(_tunnelCavitySection->length()/_tunnelCavitySection->angle())*sin(_tunnelCavitySection->angle()/2)
    - fabs(cos(_tunnelCavitySection->angle()/2)) * (_tunnelCavitySection->radius() + BDSGlobalConstants::Instance()->GetTunnelThickness()) * tan(_tunnelCavitySection->angle()/2)/2;
  G4double xHalfLengthPlus = (_tunnelCavitySection->length()/_tunnelCavitySection->angle())*sin(_tunnelCavitySection->angle()/2)
    + fabs(cos(_tunnelCavitySection->angle()/2)) * (_tunnelCavitySection->radius() + BDSGlobalConstants::Instance()->GetTunnelThickness()) * tan(_tunnelCavitySection->angle()/2)/2; 
  
  G4double halfLen=std::max(xHalfLengthPlus, xHalfLengthMinus);
  G4double pi_ov_2= asin(1.);
  _tunnelCavitySection->rotation()->rotateY(pi_ov_2);

  _tunnelCavitySection->solid(new G4IntersectionSolid(
						      (G4String)_tunnelCavitySection->name()+"_solid",
						       new G4Tubs(
								  (G4String)_tunnelCavitySection->name()+"_temp_solid",
								  0,
								  _tunnelCavitySection->radius()+BDSGlobalConstants::Instance()->GetTunnelThickness(),
								  halfLen,
								  0,CLHEP::twopi*CLHEP::radian),			    
						       _tunnelCavitySection->acceleratorComponent()->GetMarkerLogicalVolume()->GetSolid(),
						       BDSGlobalConstants::Instance()->RotYM90(),
						      (G4ThreeVector)0
						       )
			       );
  _tunnelCavitySection->logicalVolume(new G4LogicalVolume(_tunnelCavitySection->solid(),_tunnelCavitySection->material(),(G4String)_tunnelCavitySection->name()+"_logical_volume"));
}

