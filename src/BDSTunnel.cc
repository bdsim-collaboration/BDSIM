#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "BDSTunnel.hh"
#include "BDSMaterials.hh"
#include "BDSGlobalConstants.hh"
#include "BDSDebug.hh"
#include <sstream>
#include <float.h>

#define BDSDEBUG 1

BDSTunnel::BDSTunnel(Element element){
  G4cout << __METHOD_NAME__ << G4endl;
  G4cout << __METHOD_NAME__<< " - element const. - initially radius = " << radius() << G4endl;
  
  _angle=0;
  _length=0;//Length and angle will be set according to the accelerator component.
  _material = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetTunnelMaterialName());
  _soilMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetSoilMaterialName());
  G4cout << __METHOD_NAME__ << " - def. const., global constants radius  = " << BDSGlobalConstants::Instance()->GetTunnelRadius() << G4endl;
  radius(BDSGlobalConstants::Instance()->GetTunnelRadius());
  G4cout << __METHOD_NAME__ << " - def. const., _radius set to " << radius() << G4endl;
  G4cout << __METHOD_NAME__ << " - def. const., _radius set to " << _radius << G4endl;
  _floorBeamlineHeight = BDSGlobalConstants::Instance()->GetFloorBeamlineHeight();
  _beamlineCeilingHeight = BDSGlobalConstants::Instance()->GetBeamlineCeilingHeight();
  _offsetX = BDSGlobalConstants::Instance()->GetTunnelOffsetX();
  _thickness = BDSGlobalConstants::Instance()->GetTunnelThickness();
  _soilThickness = BDSGlobalConstants::Instance()->GetTunnelSoilThickness();
  _rot = new G4RotationMatrix(0,0,0);
  _trans.set(0,0,0);
  _nullThreeVector.set(0,0,0);
  _nullRotationMatrix = new G4RotationMatrix(0,0,0);
  _bBuildTunnel = true;
  G4cout << __METHOD_NAME__<< " - element const. - end radius = " << radius() << G4endl;
  G4cout << __METHOD_END__ << " - def. const." << G4endl;


  G4cout << __METHOD_NAME__<< " - element const. - dc - radius = " << radius() << G4endl;
  //Set tunnel parameters to parser parameters if they have been set by the parser (i.e. not DBL_MAX).
  length(element.l);
  angle(element.angle);
  if (element.tunnelRadius != DBL_MAX){
    radius(element.tunnelRadius);
    G4cout << __METHOD_NAME__<< " - element const. - setting radius to " << radius() << G4endl;
  }
  if (element.floorBeamlineHeight != DBL_MAX){
    floorBeamlineHeight(element.floorBeamlineHeight);
  }
  G4cout << __METHOD_NAME__<< " - fbh - radius = " << radius() << G4endl;
  if (element.beamlineCeilingHeight != DBL_MAX){
    beamlineCeilingHeight(element.beamlineCeilingHeight);
  }
  G4cout << __METHOD_NAME__<< " - bch - radius = " << radius() << G4endl;
  if (element.tunnelOffsetX != DBL_MAX){
    offsetX(element.tunnelOffsetX);
  }
  G4cout << __METHOD_NAME__<< " - ox - radius = " << radius() << G4endl;
  if (element.tunnelThickness != DBL_MAX){
    thickness(element.tunnelThickness);
  }
  G4cout << __METHOD_NAME__<< " - thi - radius = " << radius() << G4endl;
  if (element.tunnelSoilThickness != DBL_MAX){
    soilThickness(element.tunnelSoilThickness);
  }
  G4cout << __METHOD_NAME__<< " - element const. - radius = " << radius() << G4endl;
  G4cout << __METHOD_END__ << G4endl;
}

double BDSTunnel::radius() const{
  return _radius;
}

void BDSTunnel::radius(G4double val){
  _radius = val;
  G4cout << __METHOD_NAME__ << " - radius set to " << _radius << G4endl;
}


BDSTunnel::BDSTunnel(){
}

BDSTunnel::~BDSTunnel(){
}

/*
void BDSTunnel::motherComponent(BDSAcceleratorComponent* val){
  _motherComponent = val;
  _motherVolume=_motherComponent->GetLogicalVolume();
  _motherName=_motherComponent->GetName();;
}
*/

void BDSTunnel::motherVolume(G4LogicalVolume* val){
  //  _motherComponent = val;
#ifdef BDSDEBUG 
  G4cout << __METHOD_NAME__ << " - setting mother volume to " << val->GetName() << G4endl;
#endif
  _motherVolume=val;
  _motherName=_motherVolume->GetName();
#ifdef BDSDEBUG 
  G4cout << __METHOD_END__ << G4endl;
#endif
}


//G4Exception("Error: length of component less than safety length", "-1", FatalErrorInArgument, "");



//Check for exceptions (use before building).
void BDSTunnel::CheckExceptions(){
  //Print debug statements.
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
  /*
  G4cout << "Soil :"
         << " r= " << (_radius+BDSGlobalConstants::Instance()->GetTunnelThickness())/CLHEP::m + BDSGlobalConstants::Instance()->GetTunnelSoilThickness()/CLHEP::m<< " m"
         << " l= " << _length/CLHEP::m << " m"
         << " material = " << _soilMaterial->GetName() << " m"
         << G4endl;
  G4cout << "Outer tunnel :"
         << " r= " << (_radius+BDSGlobalConstants::Instance()->GetTunnelThickness())/CLHEP::m << " m"
         << " l= " << _length/CLHEP::m << " m"
         << " material = " << _material->GetName() << " m"
         << G4endl;
  G4cout << "Inner tunnel :"
         << " r= " << _radius/CLHEP::m << " m"
         << " l= " << _length/CLHEP::m << " m"
         << G4endl;
  */
#endif
  
  std::stringstream ss;
  //Make sure length is not too short....
  if(_length <= BDSGlobalConstants::Instance()->GetLengthSafety()){
    ss.str("");
    ss << __METHOD_NAME__ << " - length, " << _length << ", less than or equal to safety length, " << BDSGlobalConstants::Instance()->GetLengthSafety() << ". Not building.";
    G4Exception(ss.str().c_str(), "-1", JustWarning, "");
    _bBuildTunnel=false;
  } else { //...if not, check for other exceptions...
    //Make sure mother volume is set.
    if(_motherVolume==NULL){
      ss.str("");
      ss << "BDSTunnel - mother volume not set.";
      G4Exception(ss.str().c_str(), "-1", FatalException, "");
    }
    /*
    if(_motherComponent==NULL){
      ss.str("");
      ss << "BDSTunnel - mother accelerator component not set.";
      G4Exception(ss.str().c_str(), "-1", FatalException, "");
    }
    */
    //Make sure tunnel radius has been set.
    if(_radius == DBL_MAX){
      ss.str("");
      ss << "BDSTunnel - radius, " << _radius << ", has not been set.";
      G4Exception(ss.str().c_str(), "-1", FatalErrorInArgument, "");
    }
    
    //Make sure values are sensible.
    if((_floorBeamlineHeight == DBL_MAX) && (_beamlineCeilingHeight == DBL_MAX)){ //If both unset...
      ss.str("");
      ss << "BDSTunnel - floorBeamlineHeight and beamlineCeilingHeight not set. Setting defaults.";
      G4Exception(ss.str().c_str(), "-1", JustWarning, "");
      _floorBeamlineHeight = _radius;  //...set them to radius....
      _beamlineCeilingHeight = _radius;
    } else if((_floorBeamlineHeight == DBL_MAX) || (_beamlineCeilingHeight == DBL_MAX)){ //...else if one is unset....
      if(_floorBeamlineHeight == DBL_MAX){
	ss.str("");
	ss << "BDSTunnel - floorBeamlineHeight not set. Setting default.";
	G4Exception(ss.str().c_str(), "-1", JustWarning, "");
	_floorBeamlineHeight = 2*_radius - _beamlineCeilingHeight;  //...set the appropriate one.
      } 
      if(_beamlineCeilingHeight == DBL_MAX){
	ss.str("");
	ss << "BDSTunnel - beamlineCeilingHeight not set. Setting default.";
	G4Exception(ss.str().c_str(), "-1", JustWarning, "");
	_beamlineCeilingHeight = 2*_radius - _floorBeamlineHeight;
      }
    }
    
    if(_offsetX == DBL_MAX){
      _offsetX = 0;
      ss.str("");
      ss << "BDSTunnel - offsetX not set. Setting default.";
      G4Exception(ss.str().c_str(), "-1", JustWarning, "");
    }
    if(_thickness == DBL_MAX){
      ss.str("");
      ss << "BDSTunnel - thickness not set. Setting default.";
      G4Exception(ss.str().c_str(), "-1", JustWarning, "");
      _thickness = 0.5*CLHEP::m;
    }
    if(_soilThickness == DBL_MAX){
      ss.str("");
      ss << "BDSTunnel - soilThickness not set. Setting default.";
      G4Exception(ss.str().c_str(), "-1", JustWarning, "");
      _soilThickness = 5*CLHEP::m;
    }
  }
#ifdef BDSDEBUG
  G4cout << __METHOD_END__ << G4endl;
#endif
}

void BDSTunnel::BuildSolidVolumes(){
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
}

void BDSTunnel::CalculateDimensions(){
}

void BDSTunnel::Build(){
  G4cout << __METHOD_NAME__ << G4endl;
  CheckExceptions();
  if(_bBuildTunnel){
  G4cout << __METHOD_NAME__ << " - building solid volumes..." << G4endl;
  BuildSolidVolumes();
  BuildLogicalVolumes(); 
  PlaceLogicalVolumes();
  SetUserLimits();
  SetVisAttributes();
  }
  G4cout << __METHOD_END__ << G4endl;
}

void BDSTunnel::BuildLogicalVolumes(){
#ifdef BDSDEBUG
  G4cout << "Building tunnel walls logical volume" << G4endl;
#endif
  
  _wallsLogicalVolume=	
    new G4LogicalVolume(_wallsSolid,
                        _material,
                        _motherName+"_tun_walls_log");
  
#ifdef BDSDEBUG
  G4cout << "Building soil tunnel logical volume" << G4endl;
#endif
  _soilLogicalVolume=	
    new G4LogicalVolume(_soilSolid,
                        _soilMaterial,
                        _motherName+"_tun_soil_log");
}

void BDSTunnel::PlaceLogicalVolumes(){

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " - placing tunnel walls" << G4endl;
#endif
  _physiComp = new G4PVPlacement(
                                         _rot,		     // no rotation
                                         _trans,	                     // at (0,0,0)
                                         _wallsLogicalVolume,  // its logical volume
                                         _motherName+"_tun_phys",	     // its name
                                         _motherVolume,    // its mother  volume
                                         true,		     // no boolean operation
                                         0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		             // copy number
  
  
  multiplePhysicalVolumes(_physiComp);
  
  /*
#ifdef BDSDEBUG
  G4cout << "Placing soil around tunnel" << G4endl;
#endif
  _physiCompSoil = new G4PVPlacement(
                                             _rot,		     // no rotation
                                             _trans,	                     // at (0,0,0)
                                             _soilLogicalVolume,  // its logical volume
                                             _motherName+"_tun_phys_soil",	     // its name
                                             _motherVolume,    // its mother  volume
                                             true,		     // no boolean operation
                                             0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		             // copy number
  
  multiplePhysicalVolumes(_physiCompSoil);
  */
}

void BDSTunnel::SetUserLimits(){
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
#ifndef NOUSERLIMITS
  _userLimits = new G4UserLimits("tunnel cuts");
  _soilUserLimits = new G4UserLimits("tunnel soil cuts");
  G4double tcut = BDSGlobalConstants::Instance()->GetThresholdCutCharged();
  if(tcut>0){
    _userLimits->SetUserMinEkine(tcut);
    _soilUserLimits->SetUserMinEkine(tcut);
  }
  _userLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  _soilUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());


  _userLimits->SetMaxAllowedStep(_length);
  _soilUserLimits->SetMaxAllowedStep(_length);

  
  _wallsLogicalVolume->SetUserLimits(_userLimits);
  _soilLogicalVolume->SetUserLimits(_soilUserLimits);

#endif
#ifdef BDSDEBUG
  G4cout << __METHOD_END__ << G4endl;
#endif
}

void BDSTunnel::SetVisAttributes(){
#ifdef BDSDEBUG
  G4cout << "Setting vis attributes" << G4endl;
#endif
  //
  // set visualization attributes
  //
  _visAtt = new G4VisAttributes(G4Colour(1.0, 0.0, 0.0));
  _visAtt->SetVisibility(false);
  _visAtt->SetForceWireframe(true);  
  _soilLogicalVolume->SetVisAttributes(_visAtt);
  _visAtt1 = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));
  _visAtt1->SetVisibility(BDSGlobalConstants::Instance()->GetShowTunnel());
  _visAtt1->SetForceSolid(true);
  _wallsLogicalVolume->SetVisAttributes(_visAtt1);
  /*
  _visAtt2 = new G4VisAttributes(G4Colour(0.0, 0.5, 0.5));
  _visAtt2->SetVisibility(false);
  _visAtt2->SetForceSolid(true);
  */

}



