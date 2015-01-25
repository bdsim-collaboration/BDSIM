#include "BDSGlobalConstants.hh"
#include "BDSDebug.hh"

#include "BDSRBend.hh"

#include "BDSDipoleStepper.hh"
#include "BDSMaterials.hh"
#include "BDSSbendMagField.hh"

#include "G4FieldManager.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_EqRhs.hh"
#include "G4PVPlacement.hh"               
#include "G4SubtractionSolid.hh"
#include "G4Box.hh"
#include "G4Trd.hh"
#include "G4Tubs.hh"
#include "G4CutTubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"


BDSRBend::BDSRBend(G4String aName, G4double aLength, 
                   G4double bpRad, G4double FeRad,
                   G4double bField, G4double angle, G4double outR,
                   std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
                   G4double tilt, G4double bGrad, 
                   G4String aTunnelMaterial, G4String aMaterial):
  BDSMultipole(aName, aLength, bpRad, FeRad, blmLocZ, blmLocTheta, aTunnelMaterial, aMaterial,
	       0, 0, angle)
{
  SetOuterRadius(outR);
  itsTilt   = tilt;
  itsBField = bField;
  itsBGrad  = bGrad;
  
  //full length along chord - just its length in case of rbend
  itsChordLength = aLength;

  if (itsAngle < 0)
    {orientation = -1;}
  else
    {orientation = 1;}

  // itsStraightSectionChord is the distance along the chord required to be used by a drift pipe so that
  // the outer logical volume (magnet cylinder - defined by outR) doesn't protrude
  // into the previous volume / outside the marker volume.  for zero angle, this is 0

  itsStraightSectionChord     = outR / (tan(0.5*fabs(itsAngle)) + tan((0.5*CLHEP::pi) - (0.5*fabs(itsAngle))) );
  itsMagFieldLength           = itsChordLength - (2.0*itsStraightSectionChord);
  magnetXShift                = orientation*itsStraightSectionChord*tan(0.5*fabs(itsAngle));
  G4ThreeVector magpos        = G4ThreeVector(magnetXShift, 0, 0);
  G4double      dz            = (itsMagFieldLength*0.5)+(itsStraightSectionChord*0.5);
  G4ThreeVector driftposstart = G4ThreeVector(0.5*magnetXShift, 0, -1*dz);
  G4ThreeVector driftposend   = G4ThreeVector(0.5*magnetXShift, 0, dz);
  itsStraightSectionLength    = itsStraightSectionChord / (cos(0.5*fabs(itsAngle)));

  G4double in_z = cos(0.5*fabs(itsAngle)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double in_x = sin(0.5*fabs(itsAngle));
  inputface  = G4ThreeVector(orientation*in_x, 0.0, -1.0*in_z); //-1 as pointing down in z for normal
  outputface = G4ThreeVector(orientation*in_x, 0.0, in_z);
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Angle                   = " << itsAngle                 << G4endl;
  G4cout << __METHOD_NAME__ << "Straight section chord  = " << itsStraightSectionChord  << G4endl;
  G4cout << __METHOD_NAME__ << "Magnetic field length   = " << itsMagFieldLength        << G4endl;
  G4cout << __METHOD_NAME__ << "Straight section length = " << itsStraightSectionLength << G4endl;
  G4cout << __METHOD_NAME__ << "Straight section chord  = " << itsStraightSectionChord  << G4endl;
  G4cout << __METHOD_NAME__ << "Magnet shift in X       = " << magnetXShift             << G4endl;
#endif
}

void BDSRBend::Build()
{
  BDSMultipole::Build();
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
      
      BuildOuterFieldManager(2, BFldIron,CLHEP::pi/2);
    }
}

void BDSRBend::SetVisAttributes()
{
  itsVisAttributes = new G4VisAttributes(G4Colour(0,0,1)); //blue
  itsVisAttributes->SetForceSolid(true);
}

void BDSRBend::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  G4ThreeVector Bfield(0.,-itsBField,0.);
  G4double arclength = fabs(itsAngle) * ((itsMagFieldLength*0.5) / sin(0.5*fabs(itsAngle)));
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " calculated arclength in dipole field: " << arclength << G4endl;
#endif
  itsMagField = new BDSSbendMagField(Bfield,arclength,itsAngle);
  itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);  
  
  BDSDipoleStepper* dipoleStepper = new BDSDipoleStepper(itsEqRhs);
  dipoleStepper->SetBField(-itsBField); // note the - sign...
  dipoleStepper->SetBGrad(itsBGrad);
  itsStepper = dipoleStepper;
}

void BDSRBend::BuildMarkerLogicalVolume()
{
  G4String LocalLogicalName=itsName;
  G4double boxSize=BDSGlobalConstants::Instance()->GetComponentBoxSize()+BDSGlobalConstants::Instance()->GetTunnelRadius();
  
  // make marker volume from G4cuttubs - a cylinder along the chord line with angled faces defined by normal vectors
  // this is done first so other solids (beam pipe) can be trimmed with the angled face
  // make solid
  G4CutTubs* itsMarkerSolidVolume = new G4CutTubs( itsName+"_marker",  // name
						   0.0,                // minimum radius = 0 for solid cylinder
						   boxSize/2.0,        // radius - determined above
						   itsChordLength/2.0, // length about centre point
						   0.0,                // starting angle
						   2.0*CLHEP::pi,      // finishing angle - full
						   inputface,          // input face normal vector
						   outputface );       // output face normal vector

  // make logical volume
  G4Material* vacuumMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial());
  itsMarkerLogicalVolume = new G4LogicalVolume(itsMarkerSolidVolume,
					       vacuumMaterial,
					       LocalLogicalName+"_marker");

#ifndef NOUSERLIMITS
  itsMarkerUserLimits = new G4UserLimits(DBL_MAX,DBL_MAX,DBL_MAX);
  itsMarkerUserLimits->SetMaxAllowedStep(itsMagFieldLength*0.5);
  itsMarkerLogicalVolume->SetUserLimits(itsMarkerUserLimits);
#endif
  // zero field in the marker volume
  itsMarkerLogicalVolume->
    SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
}

// construct a beampipe for r bend
void BDSRBend::BuildBeampipe(G4String materialName)
{
  G4Material* bpmaterial;
  if(materialName == "")
    { bpmaterial = BDSMaterials::Instance()->GetMaterial( BDSGlobalConstants::Instance()->GetPipeMaterialName() );}
  else
    { bpmaterial = BDSMaterials::Instance()->GetMaterial(materialName); }
  
  // compute some geometrical parameters
  G4double bpThickness = BDSGlobalConstants::Instance()->GetBeampipeThickness();

  //start section beam pipe solid
  G4CutTubs* straightBeampipeOuter = new G4CutTubs(+"_pipe_start_outer_env",      // name
						   itsBpRadius,                   // inner R
						   itsBpRadius+bpThickness+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0, // outer R
						   itsStraightSectionLength*0.5,  // half length in z
						   0.0,                           // starting angle
						   2.0*CLHEP::pi,                 // finishing angle - full
						   G4ThreeVector(0,0,-1),         // will be normal to previous beam pipe
						   outputface );                  // will be angled to match rbend magnet volume
  
  //start section vacuum solid
  G4CutTubs* straightBeampipeVacuum = new G4CutTubs(+"_pipe_start_outer_env",      // name
						    0,                             // inner R
						    itsBpRadius,                   // outer R
						    itsStraightSectionLength*0.5,  // half length
						    0.0,                           // starting angle
						    2.0*CLHEP::pi,                 // finishing angle - full
						    G4ThreeVector(0,0,-1),         // will be normal to previous beam pipe
						    outputface );                  // will be angled to match rbend magnet volume

  //middle section beam pipe solid
  G4Tubs* magnetBeampipeOuter = new G4Tubs(itsName+"_pipe_middle_outer", // name
					   itsBpRadius,                  // inner R
					   itsBpRadius+bpThickness+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0, // outer R
					   itsMagFieldLength/2.0,        // half length in z
					   0.0,                          // starting angle
					   2.0*CLHEP::pi );              // finishing angle - full
  
  //middle section vacuum solid
  G4Tubs* magnetBeampipeVacuum = new G4Tubs(itsName+"_pipe_middle_inner",  // name
					    0,                             // inner R
					    itsBpRadius,                   // outer R
					    itsMagFieldLength/2.0,         // half length in z
					    0.0,                           // starting angle
					    2.0*CLHEP::pi );               // finishing angle - full

  //end section beam pipe solid - can just use start section and rotate it
  
  // create logical volumes from solids
  G4Material* vacuumMaterial = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial());
  G4LogicalVolume* straightBeampipeLogVolume = new G4LogicalVolume(straightBeampipeOuter,
								   bpmaterial,
								   itsName+"_bmp_logical");
  
  G4LogicalVolume* straightVacuumLogVolume = new G4LogicalVolume(straightBeampipeVacuum,
								 vacuumMaterial,
								 itsName+"_bmp_Inner_log");
  
  G4LogicalVolume* magnetBeampipeLogVolume = new G4LogicalVolume(magnetBeampipeOuter,
								 bpmaterial,
								 itsName+"_bmp_logical_middle");
  
  G4LogicalVolume* magnetVacuumLogVolume   = new G4LogicalVolume(magnetBeampipeVacuum,
								 vacuumMaterial,
								 itsName+"_bmp_Inner_log_middle");
  
  // set magnetic field inside magnet beam pipe
  magnetBeampipeLogVolume->SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
  magnetVacuumLogVolume->SetFieldManager(itsBPFieldMgr,false);
  
  // place logical volumes inside marker (container) volume
  // calculate offsets and rotations
  G4double straightSectionCentralZ = (itsMagFieldLength*0.5) + (itsStraightSectionChord*0.5);
  G4RotationMatrix* straightStartRM = new G4RotationMatrix();
  straightStartRM->rotateY(itsAngle*0.5);
  G4RotationMatrix* straightEndRM = new G4RotationMatrix();
  straightEndRM->rotateY(CLHEP::pi-itsAngle*0.5);
  straightEndRM->rotateZ(CLHEP::pi);
  G4ThreeVector straightStartPos = G4ThreeVector(orientation*magnetXShift*0.5,0,-straightSectionCentralZ);
  G4ThreeVector straightEndPos   = G4ThreeVector(orientation*magnetXShift*0.5,0,straightSectionCentralZ);
  G4ThreeVector magnetPos        = G4ThreeVector(orientation*magnetXShift,0,0);

  // placement of first straight section beam pipe
  G4VPhysicalVolume* straightStartBPPhysVol = new G4PVPlacement(straightStartRM,           // rotation			
								straightStartPos,          // position
								straightBeampipeLogVolume, // logical volume
								itsName+"_Bmp",            // name
								itsMarkerLogicalVolume,    // mother volume
								false,		           // no booleanm operation
								0,                         // copy number
								BDSGlobalConstants::Instance()->GetCheckOverlaps() );
  
  // placement of first straight section vacuum								 
  G4VPhysicalVolume* straightStartVacPhysVol = new G4PVPlacement(straightStartRM,          // rotation 	
								 straightStartPos,         // position
								 straightVacuumLogVolume,  // logical volume
								 itsName+"_InnerBmp",      // name
								 itsMarkerLogicalVolume,   // mother volume
								 false,		           // no booleanm operation
								 0,                        // copy number
								 BDSGlobalConstants::Instance()->GetCheckOverlaps() );

  // placement of central beam pipe
  G4VPhysicalVolume* magnetBPPhysVol = new G4PVPlacement(0,                           // rotation
							 magnetPos,	              // position
							 magnetBeampipeLogVolume,     // logical volume
							 itsName+"_bmp",	      // name
							 itsMarkerLogicalVolume,      // mother volume
							 false,	                      // no boolean operation
						         0,                           // copy number
							 BDSGlobalConstants::Instance()->GetCheckOverlaps() );
  
  // placement of central vacuum
  G4VPhysicalVolume* magnetVacPhysVol = new G4PVPlacement(0,                          // rotation
							  magnetPos,	              // position
							  magnetVacuumLogVolume,      // logical volume
							  itsName+"_InnerBmp",        // name
							  itsMarkerLogicalVolume,     // mother volume
							  false,		      // no booleanm operation
							  0,                          // copy number
							  BDSGlobalConstants::Instance()->GetCheckOverlaps() );
  
  // placement of end straight section beam pipe - use start logical volume but rotate it
  G4VPhysicalVolume* straightEndBPPhysVol = new G4PVPlacement(straightEndRM,             // rotation
							      straightEndPos,            // position
							      straightBeampipeLogVolume, // logical volume
							      itsName+"_Bmp",            // name
							      itsMarkerLogicalVolume,    // mother volume
							      false,		         // no booleanm operation
							      0,                         // copy number
							      BDSGlobalConstants::Instance()->GetCheckOverlaps() );
  
  // placement of end straight section vacuum - use start logical volume but rotate it
  G4VPhysicalVolume* straightEndVacPhysVol = new G4PVPlacement(straightEndRM,           // rotation 
							      straightEndPos,          // position
							      straightVacuumLogVolume, // logical volume
							      itsName+"_InnerBmp",     // name
							      itsMarkerLogicalVolume,  // mother volume
							      false,		       // no booleanm operation
							      0,                       // copy number
							      BDSGlobalConstants::Instance()->GetCheckOverlaps() );
  
  
  SetMultiplePhysicalVolumes(straightStartBPPhysVol);
  SetMultiplePhysicalVolumes(straightStartVacPhysVol);
  SetMultiplePhysicalVolumes(magnetBPPhysVol);
  SetMultiplePhysicalVolumes(magnetVacPhysVol);
  SetMultiplePhysicalVolumes(straightEndBPPhysVol);
  SetMultiplePhysicalVolumes(straightEndVacPhysVol);

  // define sensitive volumes for hit generation
  if(BDSGlobalConstants::Instance()->GetSensitiveBeamPipe()){
    AddSensitiveVolume(straightBeampipeLogVolume); //start and end sections as both same logical volume
    AddSensitiveVolume(magnetBeampipeLogVolume);   //middle section - previously not included
  }

#ifndef NOUSERLIMITS
  // set user limits for stepping, tracking and propagation in B field
  G4double thresholdCutCharged = BDSGlobalConstants::Instance()->GetThresholdCutCharged();
  G4double maxTime             = BDSGlobalConstants::Instance()->GetMaxTime();
  // central beam pipe
  G4UserLimits* magnetUserLimits = new G4UserLimits("rbend central cuts", DBL_MAX, DBL_MAX, DBL_MAX, thresholdCutCharged);
  double nsteps = ceil(fabs(itsAngle)/0.01);
  double maxstepsize = itsMagFieldLength/nsteps/10.; //fairly abritrary fraction but ensures closure at this level
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " maximum step size set to: " << maxstepsize/CLHEP::mm << " mm" << G4endl;
#endif
  magnetUserLimits->SetMaxAllowedStep(maxstepsize);
  magnetUserLimits->SetUserMaxTime(maxTime);
  
  // end sections beam pipe
  G4UserLimits* straightUserLimits = new G4UserLimits("rbend ends cuts", DBL_MAX, DBL_MAX, DBL_MAX, thresholdCutCharged);
  straightUserLimits->SetMaxAllowedStep(itsStraightSectionLength);
  straightUserLimits->SetUserMaxTime(maxTime);

  straightBeampipeLogVolume->SetUserLimits(straightUserLimits);
  straightVacuumLogVolume->SetUserLimits(straightUserLimits);
  magnetBeampipeLogVolume->SetUserLimits(magnetUserLimits);
  magnetVacuumLogVolume->SetUserLimits(magnetUserLimits);
#endif
  
  // set visualization attributes
  G4VisAttributes* vacuumVis = new G4VisAttributes(G4Colour(0., 0., 0., 0.2));
  vacuumVis->SetForceSolid(true);
  G4VisAttributes* beampipeVis = new G4VisAttributes(G4Colour(0.4, 0.4, 0.4));
  beampipeVis->SetForceSolid(true);
  
  straightBeampipeLogVolume->SetVisAttributes(beampipeVis);
  straightVacuumLogVolume->SetVisAttributes(vacuumVis);
  magnetBeampipeLogVolume->SetVisAttributes(beampipeVis);
  magnetVacuumLogVolume->SetVisAttributes(vacuumVis);
}

void BDSRBend::BuildOuterLogicalVolume(G4bool outerMaterialIsVacuum){
  G4Material* material;
  if(itsMaterial != "")
    material = BDSMaterials::Instance()->GetMaterial(itsMaterial);
  else
    material = BDSMaterials::Instance()->GetMaterial("Iron");

  // build solid
  G4Tubs *magTubs = new G4Tubs(itsName+"_solid_env",   // name
			       itsInnerIronRadius,     // inner R + overlap safety
			       itsOuterR+BDSGlobalConstants::Instance()->GetLengthSafety()/2.0, // outer R
			       itsMagFieldLength/2.0,  // half length
			       0,                      // starting angle
			       2.0*CLHEP::pi );        // finishing angle
  
  // build logical volume
  G4Material* materialToUse = NULL;
  if (outerMaterialIsVacuum)
    { materialToUse = BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetVacuumMaterial());}
  else
    { materialToUse = material;}
  itsOuterLogicalVolume = new G4LogicalVolume(magTubs,
					      materialToUse,
					      itsName+"_outer");

  
  //placement
  G4ThreeVector magnetPos        = G4ThreeVector(magnetXShift,0,0);
  itsPhysiComp = new G4PVPlacement(0,                      // rotation
				   magnetPos,              // position
				   itsOuterLogicalVolume,  // logical volume
				   itsName+"_solid",       // name
				   itsMarkerLogicalVolume, // mother  volume
				   false,                  // no boolean operation
				   0,                      // copy number
				   BDSGlobalConstants::Instance()->GetCheckOverlaps());

  SetMultiplePhysicalVolumes(itsPhysiComp);
  
  // define sensitive volumes for hit generation
  if(BDSGlobalConstants::Instance()->GetSensitiveComponents()){
    AddSensitiveVolume(itsOuterLogicalVolume);
  }
  
  // set visualization attributes
  itsOuterLogicalVolume->SetVisAttributes(itsVisAttributes);

#ifndef NOUSERLIMITS
  itsOuterUserLimits =
    new G4UserLimits("multipole cut",DBL_MAX,DBL_MAX,BDSGlobalConstants::Instance()->GetMaxTime(),
                     BDSGlobalConstants::Instance()->GetThresholdCutCharged());
  itsOuterUserLimits->SetMaxAllowedStep(itsMagFieldLength);
  itsOuterLogicalVolume->SetUserLimits(itsOuterUserLimits);
#endif
}

BDSRBend::~BDSRBend()
{
}
