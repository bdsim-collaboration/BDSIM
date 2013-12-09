#include "BDSStraightTunnelCavitySectionBuilder.hh"
#include "BDSGlobalConstants.hh"

BDSStraightTunnelCavitySectionBuilder::BDSStraightTunnelCavitySectionBuilder(){
}

BDSStraightTunnelCavitySectionBuilder::~BDSStraightTunnelCavitySectionBuilder(){
}

void BDSStraightTunnelCavitySectionBuilder::build(){
  _tunnelCavitySection->solid(new G4Tubs(  (G4String)_tunnelCavitySection->name() + "_solid",
					   0,
					   _tunnelCavitySection->radius()+BDSGlobalConstants::Instance()->GetTunnelThickness(),
					   _tunnelCavitySection->length(),
					   0,CLHEP::twopi*CLHEP::radian)
			      );
  _tunnelCavitySection->logicalVolume(new G4LogicalVolume(_tunnelCavitySection->solid(),_tunnelCavitySection->material(),(G4String)_tunnelCavitySection->name()+"_logical_volume"));
}

