#include "BDSTunnel.hh"
#include <sstream>

//Must be initialised with a mother component.
BDSTunnel::BDSTunnel(BDSAcceleratorComponent* val){
  _motherComponent = val;
  _length=_motherComponent->GetLength();
  _radius=_motherComponent->GetTunnelRadius();
  _beamlineCeilingHeight=_motherComponent->GetBeamlineCeilingHeight();
  _floorBeamlineHeight=_motherComponent->GetFloorBeamlineHeight();
  _offsetX=_motherComponent->Get;
  _offsetY=_radius - _beamlineCeilingHeight;
  _material = BDSMaterials::Instance()->GetMaterial(_motherComponent->GetTunnelMaterialName());
  _soilMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetSoilMaterialName());
  _rot = new G4RotationMatrix(0,0,0);
  _trans.set(0,0,0);
  _floorOffset.set(0,0,0);
  _nullThreeVector.set(0,0,0);
  _motherVolume=_motherComponent->GetLogicalVolume();
  _motherName=_motherComponent->GetName();;
}

BDSTunnel::~BDSTunnel(){
}

//Check for exceptions (use before building).
void BDSTunnel::CheckExceptions(){
}

//G4Exception("Error: length of component less than safety length", "-1", FatalErrorInArgument, "");

void motherComponent(BDSAcceleratorComponent* val){
  _motherComponent=val;
  _motherVolume=_motherComponent->GetMarkerLogicalVolume();
  _motherName=_motherComponent->GetName();
}


void BDSTunnel::Build(){
  Calculate();

  //Make sure length is not zero
  if(_length <= BDSGlobalConstants::Instance()->GetLengthSafety()){
    std::stringstream ss;
    ss << "BDSTunnel - length, " << _length << ", less than or equal to safety length, " << BDSGlobalConstants::Instance()->GetLengthSafety() << ". Not building.";
    G4Exception(ss.str().c_str(), "-1", JustWarning, "");
    return;
  }
    //Make sure mother volume is set.
  if(_motherVolume==NULL){
    std::stringstream ss;
    ss << "BDSTunnel - mother volume not set.";
    G4Exception(ss.str().c_str(), "-1", FatalException, "");
    return;
  }
  if(_motherComponent==NULL){
    std::stringstream ss;
    ss << "BDSTunnel - mother accelerator component not set.";
    G4Exception(ss.str().c_str(), "-1", FatalException, "");
    return;
  }

#ifdef BDSDEBUG
  G4cout << "Soil :"
         << " r= " << (_radius+BDSGlobalConstants::Instance()->GetTunnelThickness())/CLHEP::m + BDSGlobalConstants::Instance()->GetTunnelSoilThickness()/CLHEP::m<< " m"
         << " l= " << _length/CLHEP::m << " m"
         << " material = " << soilMaterialName << " m"
         << G4endl;
  G4cout << "Outer tunnel :"
         << " r= " << (_radius+BDSGlobalConstants::Instance()->GetTunnelThickness())/CLHEP::m << " m"
         << " l= " << _length/CLHEP::m << " m"
         << " material = " << tunnelMaterialName << " m"
         << G4endl;
  G4cout << "Inner tunnel :"
         << " r= " << _radius/CLHEP::m << " m"
         << " l= " << _length/CLHEP::m << " m"
         << G4endl;
#endif

  BuildSolids(); //Will call the method of the derived class because Build() is virtual.
  BuildLogicalVolumes(); 
  PlaceLogicalVolumes();
  SetUserLimits();
  SetVisAttributes();
}

void BDSTunnel::BuildLogicalVolumes(){
#ifdef BDSDEBUG
  G4cout << "Building tunnel walls logical volume" << G4endl;
#endif
  
  _tunnelWallsLogicalVolume=	
    new G4LogicalVolume(_tunnelWallsSolid,
                        _tunnelMaterial,
                        _motherName+"_tun_walls_log");
  
#ifdef BDSDEBUG
  G4cout << "Building soil tunnel logical volume" << G4endl;
#endif
  _soilTunnelLogicalVolume=	
    new G4LogicalVolume(_soilSolid,
                        _soilMaterial,
                        _motherName+"_tun_soil_log");
}

void BDSTunnel::PlaceLogicalVolumes(){

#ifdef BDSDEBUG
  G4cout << "Placing tunnel walls" << G4endl;
#endif
  _tunnelPhysiComp = new G4PVPlacement(
                                         tunnelRot,		     // no rotation
                                         tunnelTrans,	                     // at (0,0,0)
                                         _tunnelWallsLogicalVolume,  // its logical volume
                                         _motherName+"_tun_phys",	     // its name
                                         _motherVolume,    // its mother  volume
                                         true,		     // no boolean operation
                                         0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		             // copy number
  
  
  SetMultiplePhysicalVolumes(_tunnelPhysiComp);

#ifdef BDSDEBUG
  G4cout << "Placing soil around tunnel" << G4endl;
#endif
  _tunnelPhysiCompSoil = new G4PVPlacement(
                                             tunnelRot,		     // no rotation
                                             tunnelTrans,	                     // at (0,0,0)
                                             _soilTunnelLogicalVolume,  // its logical volume
                                             _motherName+"_tun_phys_soil",	     // its name
                                             _motherVolume,    // its mother  volume
                                             true,		     // no boolean operation
                                             0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		             // copy number
  
  SetMultiplePhysicalVolumes(_tunnelPhysiCompSoil);


}

void BDSTunnel::SetUserLimits(){
#ifndef NOUSERLIMITS
  _tunnelUserLimits = new G4UserLimits("tunnel cuts");
  _soilTunnelUserLimits = new G4UserLimits("tunnel soil cuts");
  _innerTunnelUserLimits = new G4UserLimits("inner tunnel cuts");
  G4double tcut = BDSGlobalConstants::Instance()->GetThresholdCutCharged();
  if(tcut>0){
    _tunnelUserLimits->SetUserMinEkine(tcut);
    _soilTunnelUserLimits->SetUserMinEkine(tcut);
    _innerTunnelUserLimits->SetUserMinEkine(tcut);
  }
  _tunnelUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  _soilTunnelUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  _innerTunnelUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());

  _tunnelUserLimits->SetMaxAllowedStep(_length);
  _soilTunnelUserLimits->SetMaxAllowedStep(_length);
  _innerTunnelUserLimits->SetMaxAllowedStep(_length);
  
  _tunnelWallsLogicalVolume->SetUserLimits(_tunnelUserLimits);
  _soilTunnelLogicalVolume->SetUserLimits(_soilTunnelUserLimits);
  _tunnelCavityLogicalVolume->SetUserLimits(_innerTunnelUserLimits);
#endif
}

void BDSTunnel::SetVisAttributes(){
#ifdef BDSDEBUG
  G4cout << "Setting vis attributes" << G4endl;
#endif
  //
  // set visualization attributes
  //
  VisAtt = new G4VisAttributes(G4Colour(0.0, 1.0, 0.0));
  VisAtt->SetVisibility(true);
  VisAtt->SetForceWireframe(true);  
  
  _soilTunnelLogicalVolume->SetVisAttributes(VisAtt);
  VisAtt1 = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));
  VisAtt1->SetVisibility(BDSGlobalConstants::Instance()->GetShowTunnel());
  VisAtt1->SetForceSolid(true);
  VisAtt2 = new G4VisAttributes(G4Colour(0.0, 0.5, 0.5));
  VisAtt2->SetVisibility(false);
  VisAtt2->SetForceSolid(true);
  _tunnelWallsLogicalVolume->SetVisAttributes(VisAtt1);
  _motherVolume->SetVisAttributes(VisAtt);
  _tunnelCavityLogicalVolume->SetVisAttributes(VisAtt2);
}



