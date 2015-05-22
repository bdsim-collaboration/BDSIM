#include "BDSGlobalConstants.hh" 

#include "BDSSectorBend.hh"

#include "BDSDipoleStepper.hh"
#include "BDSMaterials.hh"
#include "BDSSbendMagField.hh"

#include "G4Polyhedra.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4EllipticalTube.hh"
#include "G4SubtractionSolid.hh"
#include "G4IntersectionSolid.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4PVPlacement.hh"               
#include "G4UserLimits.hh"
#include "G4VPhysicalVolume.hh"

//============================================================

BDSSectorBend::BDSSectorBend(G4String aName, G4double aLength, 
			     G4double bpRad, G4double FeRad,
			     G4double bField, G4double angle, G4double outR,
                             std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
			     G4double tilt, G4double bGrad, 
			     G4String aTunnelMaterial, G4String aMaterial, G4double aXAper, G4double aYAper):
  BDSMultipole(aName, aLength, bpRad, FeRad, blmLocZ, blmLocTheta, aTunnelMaterial, aMaterial,
	       aXAper, aYAper, angle)
{
  SetOuterRadius(outR);
  itsTilt=tilt;
  itsBField=bField;
  itsBGrad=bGrad;
}

void BDSSectorBend::Build()
{
  BDSMultipole::Build();
  //Build IP8Gate
  //      BuildGate();
  
  if(BDSGlobalConstants::Instance()->GetIncludeIronMagFields())
    {
      G4double polePos[4];
      G4double Bfield[3];
      
      //coordinate in GetFieldValue
      polePos[0]=0.;
      polePos[1]=BDSGlobalConstants::Instance()->GetMagnetPoleRadius();
      polePos[2]=0.;
      polePos[3]=-999.;//flag to use polePos rather than local track
      
      itsMagField->GetFieldValue(polePos,Bfield);
      G4double BFldIron=
	sqrt(Bfield[0]*Bfield[0]+Bfield[1]*Bfield[1])*
	BDSGlobalConstants::Instance()->GetMagnetPoleSize()/
	(BDSGlobalConstants::Instance()->GetComponentBoxSize()/2-
	 BDSGlobalConstants::Instance()->GetMagnetPoleRadius());
      
      // Magnetic flux from a pole is divided in two directions
      BFldIron/=2.;
      
      BuildOuterFieldManager(2, BFldIron,CLHEP::halfpi);
    }
}

void BDSSectorBend::SetVisAttributes()
{
  itsVisAttributes = new G4VisAttributes(G4Colour(0,0,1)); //blue
  itsVisAttributes->SetForceSolid(true);
  itsVisAttributes->SetVisibility(true);
}

void BDSSectorBend::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  G4ThreeVector Bfield(0.,-itsBField,0.);
  itsMagField=new BDSSbendMagField(Bfield,itsLength,itsAngle);

  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);  
  
  itsStepper = new BDSDipoleStepper(itsEqRhs);
  BDSDipoleStepper* dipoleStepper = dynamic_cast<BDSDipoleStepper*>(itsStepper);
  dipoleStepper->SetBField(-itsBField); // note the - sign...
  dipoleStepper->SetBGrad(itsBGrad);
}

void BDSSectorBend::BuildOuterLogicalVolume(G4bool OuterMaterialIsVacuum)
{
  //
  // build magnet (geometry + magnetic field)
  //
      
  G4String geometry = BDSGlobalConstants::Instance()->GetMagnetGeometry();
 
  if(geometry =="standard") 
    BuildStandardOuterLogicalVolume(OuterMaterialIsVacuum); // standard - sbend with poles and pockets
  else if(geometry =="cylinder")  
    BuildCylindricalOuterLogicalVolume(OuterMaterialIsVacuum); // cylinder outer volume
  else //default - cylinder - standard
    BuildCylindricalOuterLogicalVolume(OuterMaterialIsVacuum); // cylinder outer volume
  
  //
  // define sensitive volumes for hit generation
  //
  if(BDSGlobalConstants::Instance()->GetSensitiveComponents()){
    AddSensitiveVolume(itsOuterLogicalVolume);
  }
}

// construct a beampipe for sector bend
void BDSSectorBend::BuildBeampipe(G4String materialName)
{
  G4Material* material;
  if(materialName == "")
    {
      material = BDSMaterials::Instance()->GetMaterial( BDSGlobalConstants::Instance()->GetPipeMaterialName() );
    }
  else
    {
      material = BDSMaterials::Instance()->GetMaterial(materialName);
    }

  //
  // compute some geometrical parameters
  //
  G4double boxSize = BDSGlobalConstants::Instance()->GetComponentBoxSize();

  G4double xHalfLengthPlus, xHalfLengthMinus, tubLen;
  if(itsAngle != 0){
    xHalfLengthMinus =
      (itsLength/itsAngle)*sin(itsAngle/2)
      - fabs(cos(itsAngle/2)) * boxSize * tan(itsAngle/2)/2;
    
    xHalfLengthPlus =
      (itsLength/itsAngle)*sin(itsAngle/2)
      + fabs(cos(itsAngle/2)) * boxSize * tan(itsAngle/2)/2;
    tubLen = std::max(xHalfLengthPlus,xHalfLengthMinus);
  } else {
    tubLen=(itsLength)/2.0;
  }

  //
  // build beampipe
  //
  G4VSolid *pipeTubsEnv = new G4SubtractionSolid("_pipe_outer_env",
                                               new G4EllipticalTube(itsName+"_pipe_outer_tmp_1",
                                                                    this->GetAperX()+BDSGlobalConstants::Instance()->GetBeampipeThickness(), 
                                                                    this->GetAperY()+BDSGlobalConstants::Instance()->GetBeampipeThickness(),          
                                                                    tubLen),
                                               new G4EllipticalTube(itsName+"_pipe_outer_tmp_2",
                                                                    this->GetAperX()+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0, 
                                                                    this->GetAperY()+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0,          
                                                                    tubLen*2)
                                               );
  
  G4VSolid *pipeInnerEnv = new G4EllipticalTube(itsName+"_pipe_outer_tmp_2",
                                                this->GetAperX(), 
                                                this->GetAperY(),          
                                                tubLen);

  G4IntersectionSolid *pipeTubs =
    new G4IntersectionSolid(itsName+"_pipe_outer",
			    pipeTubsEnv,
 			    itsMarkerSolidVolume,
			    BDSGlobalConstants::Instance()->RotYM90(),
			    (G4ThreeVector)0);
  
  G4IntersectionSolid *pipeInner =
    new G4IntersectionSolid(itsName+"_pipe_inner",
			    pipeInnerEnv, 
 			    itsMarkerSolidVolume,
			    BDSGlobalConstants::Instance()->RotYM90(),
			    (G4ThreeVector)0);

  itsBeampipeLogicalVolume=	
    new G4LogicalVolume(pipeTubs,
			material,
			itsName+"_bmp_logical");
  
  itsInnerBPLogicalVolume=	
    new G4LogicalVolume(pipeInner,
			BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			itsName+"_bmp_Inner_log");

  G4VPhysicalVolume* PhysiInner;
  PhysiInner = 
    new G4PVPlacement(
		      BDSGlobalConstants::Instance()->RotY90(),		       // rotation
		      (G4ThreeVector)0,	               // at (0,0,0)
		      itsInnerBPLogicalVolume, // its logical volume
		      itsName+"_InnerBmp",     // its name
		      itsMarkerLogicalVolume,  // its mother volume
		      false,		       // no booleanm operation
		      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		       // copy number
  
  SetMultiplePhysicalVolumes(PhysiInner);

  G4VPhysicalVolume* PhysiComp;
  PhysiComp =
    new G4PVPlacement(
		      BDSGlobalConstants::Instance()->RotY90(),		        // rotation
		      (G4ThreeVector)0,	                // at (0,0,0)
		      itsBeampipeLogicalVolume, // its logical volume
		      itsName+"_bmp",	        // its name
		      itsMarkerLogicalVolume,   // its mother volume
		      false,		        // no boolean operation
		      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());		        // copy number
  
  SetMultiplePhysicalVolumes(PhysiComp);
  //
  // define sensitive volumes for hit generation
  //
  if(BDSGlobalConstants::Instance()->GetSensitiveBeamPipe()){
    AddSensitiveVolume(itsBeampipeLogicalVolume);
  }

  //
  // set user limits for stepping, tracking and propagation in B field
  //
#ifndef NOUSERLIMITS
  itsBeampipeUserLimits =
    new G4UserLimits("beampipe cuts",DBL_MAX,DBL_MAX,DBL_MAX,
  		     BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  G4double maxStepFactor=0.5;
  itsBeampipeUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsBeampipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  itsBeampipeLogicalVolume->SetUserLimits(itsBeampipeUserLimits);
  
  G4double maxStepFactorIn=0.5;
  itsInnerBeampipeUserLimits =
    new G4UserLimits("inner beampipe cuts",DBL_MAX,DBL_MAX,DBL_MAX,
  		     BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsInnerBeampipeUserLimits->SetMaxAllowedStep(itsLength*maxStepFactorIn);
  itsInnerBeampipeUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  itsInnerBPLogicalVolume->SetUserLimits(itsInnerBeampipeUserLimits);
#endif

  //
  // set magnetic field inside beampipe
  //
  itsBeampipeLogicalVolume->SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
  itsInnerBPLogicalVolume->SetFieldManager(itsFieldMgr,false);

  //
  // set visualization attributes
  //
  G4VisAttributes* VisAtt =  new G4VisAttributes(G4Colour(0., 0., 0));
  VisAtt->SetForceSolid(true);
  itsInnerBPLogicalVolume->SetVisAttributes(VisAtt);

  G4VisAttributes* VisAtt1 = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));
  VisAtt1->SetVisibility(true);
  VisAtt1->SetForceSolid(true);
  itsBeampipeLogicalVolume->SetVisAttributes(VisAtt1);
}


G4double BDSSectorBend::GetArcLength()
{
  // arc length = radius*angle
  //            = (geometrical length/(2.0*sin(angle/2))*angle
  if (itsAngle == 0.0)
    return itsLength;
  else
    return (itsLength * (0.5*itsAngle) / sin(0.5*itsAngle));
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//					Cylindrical geometry						//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void BDSSectorBend::BuildCylindricalOuterLogicalVolume(G4bool OuterMaterialIsVacuum){
  
  G4Material* material;
  
  if(itsMaterial != "")
    material = BDSMaterials::Instance()->GetMaterial(itsMaterial);
  else
    material = BDSMaterials::Instance()->GetMaterial("Iron");
  
  G4double xHalfLengthPlus, xHalfLengthMinus, tubLen;
  if(itsAngle != 0){
    xHalfLengthMinus = (itsLength/itsAngle)*sin(itsAngle/2)
      - fabs(cos(itsAngle/2)) * BDSGlobalConstants::Instance()->GetComponentBoxSize() * tan(itsAngle/2)/2
      - BDSGlobalConstants::Instance()->GetLengthSafety()/2;

    xHalfLengthPlus = (itsLength/itsAngle)*sin(itsAngle/2)
    + fabs(cos(itsAngle/2)) * BDSGlobalConstants::Instance()->GetComponentBoxSize() * tan(itsAngle/2)/2
      - BDSGlobalConstants::Instance()->GetLengthSafety()/2;
    
    tubLen = std::max(xHalfLengthPlus,xHalfLengthMinus);
  } else {
    tubLen = (itsLength-BDSGlobalConstants::Instance()->GetLengthSafety())/2.0;
  }
    
  G4VSolid *magTubsEnv = new G4SubtractionSolid(itsName+"_solid_env",
                                                new G4Tubs(itsName+"_solid_tmp_1",
                                                           itsInnerIronRadius + BDSGlobalConstants::Instance()->GetLengthSafety()/2.0, // inner R + overlap safety
                                                           itsOuterR,          // outer R
                                                           tubLen,             // length
                                                           0,                  // starting phi
                                                           CLHEP::twopi * CLHEP::rad ),      // delta phi
                                                new G4EllipticalTube(itsName+"_pipe_outer_tmp_2",
                                                                     this->GetAperX()+BDSGlobalConstants::Instance()->GetBeampipeThickness()+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0, 
                                                                     this->GetAperY()+BDSGlobalConstants::Instance()->GetBeampipeThickness()+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0,          
                                                                     tubLen*2)
                                                );
  
  G4IntersectionSolid *magTubs =
    new G4IntersectionSolid(itsName+"_solid",
			    magTubsEnv,
			    itsMarkerSolidVolume,
			    BDSGlobalConstants::Instance()->RotYM90(),
			    (G4ThreeVector)0); 

  if(OuterMaterialIsVacuum)
    {
      itsOuterLogicalVolume = 
	new G4LogicalVolume(magTubs,
			    BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			    itsName+"_outer");
    }
  else
    {
      itsOuterLogicalVolume=
	new G4LogicalVolume(magTubs,
			    material,
			    itsName+"_outer");
    }

  itsPhysiComp =
    new G4PVPlacement(
                      BDSGlobalConstants::Instance()->RotY90(),                 // rotation
                      (G4ThreeVector)0,                      // at (0,0,0)
                      itsOuterLogicalVolume,  // its logical volume
                      itsName+"_solid",       // its name
                      itsMarkerLogicalVolume, // its mother  volume
                      false,                  // no boolean operation
                      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());                     // copy number

  SetMultiplePhysicalVolumes(itsPhysiComp);
  G4double  maxStepFactor=0.5;

  //
  // set visualization attributes
  //
  itsOuterLogicalVolume->SetVisAttributes(itsVisAttributes);
  
#ifndef NOUSERLIMITS
  itsOuterUserLimits =
    new G4UserLimits("multipole cut",DBL_MAX,DBL_MAX,DBL_MAX,
                     BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsOuterUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsOuterUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  itsOuterLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////
//					Detailed geometry						//
//////////////////////////////////////////////////////////////////////////////////////////////////////////

void BDSSectorBend::BuildStandardOuterLogicalVolume(G4bool OuterMaterialIsVacuum){

  G4Material* material;
  
  if(itsMaterial != "")
    material = BDSMaterials::Instance()->GetMaterial(itsMaterial);
  else
    material = BDSMaterials::Instance()->GetMaterial("Iron");
  
  G4double xHalfLengthPlus, xHalfLengthMinus, tubLen;
  if(itsAngle != 0){
    xHalfLengthMinus = (itsLength/itsAngle)*sin(itsAngle/2)
      - fabs(cos(itsAngle/2)) * BDSGlobalConstants::Instance()->GetComponentBoxSize() * tan(itsAngle/2)/2
      - BDSGlobalConstants::Instance()->GetLengthSafety()/2;

    xHalfLengthPlus = (itsLength/itsAngle)*sin(itsAngle/2)
    + fabs(cos(itsAngle/2)) * BDSGlobalConstants::Instance()->GetComponentBoxSize() * tan(itsAngle/2)/2
      - BDSGlobalConstants::Instance()->GetLengthSafety()/2;
    
    tubLen = std::max(xHalfLengthPlus,xHalfLengthMinus);
  } else {
    tubLen = (itsLength-BDSGlobalConstants::Instance()->GetLengthSafety())/2.0;
  }
  
  ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  G4VSolid *magTubsEnv = new G4SubtractionSolid(itsName+"_solid_env",
                                                new G4Tubs(itsName+"_solid_tmp_1",
                                                           itsInnerIronRadius + BDSGlobalConstants::Instance()->GetLengthSafety()/2.0, // inner R + overlap safety
                                                           itsOuterR,          // outer R
                                                           tubLen,             // length
                                                           0,                  // starting phi
                                                           CLHEP::twopi * CLHEP::rad ),      // delta phi
                                                new G4EllipticalTube(itsName+"_pipe_outer_tmp_2",
                                                                     this->GetAperX()+BDSGlobalConstants::Instance()->GetBeampipeThickness()+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0, 
                                                                     this->GetAperY()+BDSGlobalConstants::Instance()->GetBeampipeThickness()+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0,          
                                                                     tubLen*2)
                                                );
  
  ///////////////////////////////////////////////////

  G4Material* air  = G4Material::GetMaterial("G4_AIR");
  G4double worldLength = 2*CLHEP::m;

  G4cout << "worldLength " << worldLength;

  // Definitions of Solids, Logical Volumes, Physical Volumes
  // World

  G4Box* worldS
    = new G4Box("world",                                    //its name
                worldLength/2,worldLength/2,worldLength/2); //its size
  G4LogicalVolume* worldLV
    = new G4LogicalVolume(
                 worldS,   //its solid
                 air,      //its material
                 "World"); //its name
  
  //  Must place the World Physical volume unrotated at (0,0,0).
  // 

/*
  G4VPhysicalVolume* worldPV
    = new G4PVPlacement(
                 0,               // no rotation
                 G4ThreeVector(), // at (0,0,0)
                 worldLV,         // its logical volume
                 "World",         // its name
                 0,               // its mother  volume
                 false,           // no boolean operations
                 0,               // copy number
                 BDSGlobalConstants::Instance()->GetCheckOverlaps()); // checking overlaps 
*/
 

// =========== Inputs ================= //

  // magnet parameters

  double mag_inradius = 192*CLHEP::mm; // inner radius
  double mag_extradius = 400*CLHEP::mm; // external radius

  // ==================================== //


  // Defining external shape using polyhedra

  double zplanepos [2] = {0,tubLen};
  double pipelength [2] = {-2.0*CLHEP::cm,tubLen+2.0*CLHEP::cm};
  double rinner [2] = {mag_inradius, mag_inradius};
  double router [2] = {mag_extradius, mag_extradius};

  G4Polyhedra* polyShape = new G4Polyhedra("polyShape", 0.*CLHEP::deg, 360.*CLHEP::deg, 4, 2, zplanepos, rinner, router);

  ///////////////////////////////////////////////////

  // Coils

  double coil_size_x = 8.0*CLHEP::cm;
  double coil_size_y = 4.0*CLHEP::cm;

  G4VSolid* Coil = new G4Box("Coil",coil_size_x,coil_size_y,tubLen/2.0);

  G4LogicalVolume* CoilLV = 
    new G4LogicalVolume(Coil,             //its solid
                        material,   //its material
                        "CoilLV");        //its name
   
    /////////////////////////////////////////////////////
    
    // Beampipe
  
  double beampipe_rinner [2] = {0.0, 0.0};
  double beampipe_router [2] = {this->GetAperY()+BDSGlobalConstants::Instance()->GetBeampipeThickness(), this->GetAperY()+BDSGlobalConstants::Instance()->GetBeampipeThickness()};

  G4Polyhedra* Beampipe = new G4Polyhedra("Beampipe", 0.*CLHEP::deg, 360.*CLHEP::deg, 4, 2, pipelength, beampipe_rinner, beampipe_router);
  G4LogicalVolume* BeampipeLV = 
    new G4LogicalVolume(Beampipe,             //its solid
                        material,   //its material
                        "BeampipeLV");        //its name

  // Iron yoke subtracting beampipe

  G4LogicalVolume* shieldLV = 
    new G4LogicalVolume(   new G4SubtractionSolid(
			   "Shield", polyShape, Beampipe, 0, 
			   G4ThreeVector((mag_extradius+mag_inradius)/2.0*cos(CLHEP::pi/4.0),
					 (mag_extradius+mag_inradius)/2.0*sin(CLHEP::pi/4.0),0.)),             //its solid
                        material,   //its material
                        "shieldLV");        //its name

   ////////////////////////////////////////////////////

  G4ThreeVector positionQuad = G4ThreeVector(0,0,0);
  
  // Place the external shape in the mother volume

  G4RotationMatrix* rm = new G4RotationMatrix();
  rm->rotateZ(360.0/2.0/4.0*CLHEP::deg);
  G4ThreeVector uz = G4ThreeVector(-35.*CLHEP::cm,0.,0.); 

  new G4PVPlacement(rm,             //rotation,
		    uz,             //position
		    shieldLV,            //its logical volume
		    "poleShield",               //its name
		    worldLV,             //its mother  volume
		    false,                 //no boolean operation
		    0,                     //copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());       // checking ove
  /*
  G4ThreeVector uz2 = G4ThreeVector(mag_extradius+mag_inradius,0.,0.); 
  
  G4RotationMatrix* rm2 = new G4RotationMatrix();
  rm2->rotateZ(225.0*CLHEP::deg);
  new G4PVPlacement(rm2,             //rotation,
		    uz2,             //position
		    shieldLV,            //its logical volume
		    "poleShield",               //its name
		    worldLV,             //its mother  volume
		    false,                 //no boolean operation
		    0,                     //copy number
		    fCheckOverlaps);       // checking ove

  */ 
  // Coil placement
  //rm->rotateX(0.0*CLHEP::deg);


   ////////////////////////////////////////////////////////////////////////

  double magnet_shift = -35.*CLHEP::cm; // displace magnet to center ir with respect to the beampipe

  double coil_pos_x = mag_inradius-coil_size_x;
  double coil_pos_y = mag_inradius-coil_size_y;

  G4ThreeVector positionCoil1 = G4ThreeVector(coil_pos_x + magnet_shift,coil_pos_y,tubLen/2.0);
  
  new G4PVPlacement(0,             //rotation,
		    positionCoil1,             //position
		    CoilLV,            //its logical volume
		    "Coil",               //its name
		    worldLV,             //its mother  volume
		    false,                 //no boolean operation
		    0,                     //copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());       // checking ove

   G4ThreeVector positionCoil2 = G4ThreeVector(coil_pos_x + magnet_shift,-coil_pos_y,tubLen/2.0);
  
  new G4PVPlacement(0,             //rotation,
		    positionCoil2,             //position
		    CoilLV,            //its logical volume
		    "Coil",               //its name
		    worldLV,             //its mother  volume
		    false,                 //no boolean operation
		    0,                     //copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());       // checking ove


  double coil_pos_x2 = mag_extradius+coil_size_x;
  double coil_pos_y2 = mag_inradius-coil_size_y;

  G4ThreeVector positionCoil3 = G4ThreeVector(coil_pos_x2 + magnet_shift,coil_pos_y2,tubLen/2.0);
  
  new G4PVPlacement(0,             //rotation,
		    positionCoil3,             //position
		    CoilLV,            //its logical volume
		    "Coil",               //its name
		    worldLV,             //its mother  volume
		    false,                 //no boolean operation
		    0,                     //copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());       // checking ove

 G4ThreeVector positionCoil4 = G4ThreeVector(coil_pos_x2 + magnet_shift,-coil_pos_y2,tubLen/2.0);
  
  new G4PVPlacement(0,             //rotation,
		    positionCoil4,             //position
		    CoilLV,            //its logical volume
		    "Coil",               //its name
		    worldLV,             //its mother  volume
		    false,                 //no boolean operation
		    0,                     //copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps());       // checking ove

  ///////////////////////////////////////////////////////////////////////////////////////////

  G4VisAttributes* magnetVisAtt = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));

  BeampipeLV ->SetVisAttributes(magnetVisAtt);
  
  // color-coding for the pole
  G4VisAttributes* VisAtt = 
    new G4VisAttributes(G4Colour(0.,0.,1.));
  VisAtt->SetForceSolid(true);
  CoilLV->SetVisAttributes(VisAtt);
  shieldLV->SetVisAttributes(VisAtt);

  //////////////////////////////////////////////////////////////////////////////////////////////////////////////////


  G4RotationMatrix* rm2 = new G4RotationMatrix();
  rm2->rotateZ(45.0*CLHEP::deg);

  G4IntersectionSolid *magTubs =
    new G4IntersectionSolid(itsName+"_solid",
			    magTubsEnv,
			    itsMarkerSolidVolume,
			    BDSGlobalConstants::Instance()->RotYM90(),
			    (G4ThreeVector)0); 

  if(OuterMaterialIsVacuum)
    {
      itsOuterLogicalVolume = 
	new G4LogicalVolume(magTubs,
			    BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial()),
			    itsName+"_outer");
    }
  else
    {
      itsOuterLogicalVolume=
	new G4LogicalVolume(magTubs,
			    material,
			    itsName+"_outer");
    }

  G4ThreeVector worldposition = G4ThreeVector(0.,0.,0.);


  itsPhysiComp =
    new G4PVPlacement(
                      BDSGlobalConstants::Instance()->RotY90(),                 // rotation
                      worldposition,                      // at (0,0,0)
                      //itsOuterLogicalVolume,  // its logical volume
		      worldLV,
                      itsName+"_solid",       // its name
                      itsMarkerLogicalVolume, // its mother  volume
                      false,                  // no boolean operation
                      0, BDSGlobalConstants::Instance()->GetCheckOverlaps());                     // copy number

  SetMultiplePhysicalVolumes(itsPhysiComp);
  G4double  maxStepFactor=0.5;

#ifndef NOUSERLIMITS
  itsOuterUserLimits =
    new G4UserLimits("multipole cut",DBL_MAX,DBL_MAX,DBL_MAX,
                     BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsOuterUserLimits->SetMaxAllowedStep(itsLength*maxStepFactor);
  itsOuterUserLimits->SetUserMaxTime(BDSGlobalConstants::Instance()->GetMaxTime());
  itsOuterLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BDSSectorBend::~BDSSectorBend()
{
}
