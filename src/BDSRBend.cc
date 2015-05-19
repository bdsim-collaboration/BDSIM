#include "BDSGlobalConstants.hh"
#include "BDSDebug.hh"

#include "BDSRBend.hh"

#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSDipoleStepper.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetType.hh"
#include "BDSMaterials.hh"
#include "BDSSbendMagField.hh"
#include "BDSTunnelInfo.hh"
#include "BDSUtilities.hh"

#include "G4FieldManager.hh"
#include "G4IntersectionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_EqRhs.hh"
#include "G4PVPlacement.hh"
#include "G4Tubs.hh"
#include "G4CutTubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

BDSRBend::BDSRBend(G4String           name,
		   G4double           length,
		   G4double           bField,
		   G4double           bGrad,
		   G4double           angle,
		   BDSBeamPipeInfo    beamPipeInfo,
		   BDSMagnetOuterInfo magnetOuterInfo,
		   BDSTunnelInfo      tunnelInfo):
  BDSMultipole(BDSMagnetType::rectangularbend,name,length,beamPipeInfo,magnetOuterInfo,tunnelInfo),
  itsBField(bField),itsBGrad(bGrad)
{
  itsAngle    = angle;
  outerRadius = magnetOuterInfo.outerDiameter*0.5; 
  CommonConstructor(length);
}


void BDSRBend::CommonConstructor(G4double aLength)
{
  //full length along chord - just its length in case of rbend
  itsChordLength = aLength;

  orientation = BDS::CalculateOrientation(itsAngle);

  // itsStraightSectionChord is the distance along the chord required to be used by a drift pipe so that
  // the outer logical volume (magnet cylinder - defined by outRadius) doesn't protrude
  // into the previous volume / outside the marker volume.  for zero angle, this is 0

  itsStraightSectionChord     = outerRadius / (tan(0.5*fabs(itsAngle)) + tan((0.5*CLHEP::pi) - (0.5*fabs(itsAngle))) );
  itsMagFieldLength           = itsChordLength - (2.0*itsStraightSectionChord);
  magnetXShift                = orientation*itsStraightSectionChord*tan(0.5*fabs(itsAngle));
  G4ThreeVector magpos        = G4ThreeVector(magnetXShift, 0, 0);
  G4double      dz            = (itsMagFieldLength*0.5)+(itsStraightSectionChord*0.5);
  G4ThreeVector driftposstart = G4ThreeVector(0.5*magnetXShift, 0, -1*dz);
  G4ThreeVector driftposend   = G4ThreeVector(0.5*magnetXShift, 0, dz);
  itsStraightSectionLength    = itsStraightSectionChord / (cos(0.5*fabs(itsAngle)));

  G4double in_z = cos(0.5*fabs(itsAngle)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double in_x = sin(0.5*fabs(itsAngle));
  inputface  = G4ThreeVector(-orientation*in_x, 0.0, -1.0*in_z); //-1 as pointing down in z for normal
  outputface = G4ThreeVector(-orientation*in_x, 0.0, in_z);
  
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
  itsMarkerLogicalVolume = new G4LogicalVolume(itsMarkerSolidVolume,
					       BDSMaterials::Instance()->GetMaterial(BDSGlobalConstants::Instance()->GetEmptyMaterial()),
					       LocalLogicalName+"_marker");

#ifndef NOUSERLIMITS
  itsMarkerUserLimits = new G4UserLimits(*(BDSGlobalConstants::Instance()->GetDefaultUserLimits()));
  itsMarkerUserLimits->SetMaxAllowedStep(itsLength*0.5);
  itsMarkerLogicalVolume->SetUserLimits(itsMarkerUserLimits);
#endif
  
  // zero field in the marker volume
  itsMarkerLogicalVolume->
    SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);

  SetExtentX(-boxSize*0.5,boxSize*0.5);
  SetExtentY(-boxSize*0.5,boxSize*0.5);
  SetExtentZ(-itsChordLength*0.5,itsChordLength*0.5);
}

void BDSRBend::BuildOuterVolume()
{
  //need to make a shorter outer volume for rbend geometry
  //let's cheat and use the base class method by fiddling the
  //component length then setting it back - reduces code duplication
  G4double originalLength = itsLength;
  itsLength = itsMagFieldLength;
  BDSMultipole::BuildOuterVolume();
  itsLength = originalLength;
}

// construct a beampipe for r bend
void BDSRBend::BuildBeampipe()
{
  BDSBeamPipe* bpFirstBit =
    BDSBeamPipeFactory::Instance()->CreateBeamPipeAngledOut(beamPipeType,
							    itsName,
							    itsStraightSectionLength,
							    -itsAngle*0.5,
							    aper1,
							    aper2,
							    aper3,
							    aper4,
							    vacuumMaterial,
							    beamPipeThickness,
							    beamPipeMaterial);
  
  beampipe =
    BDSBeamPipeFactory::Instance()->CreateBeamPipe(beamPipeType,
						   itsName,
						   itsMagFieldLength,
						   aper1,
						   aper2,
						   aper3,
						   aper4,
						   vacuumMaterial,
						   beamPipeThickness,
						   beamPipeMaterial);

  BDSBeamPipe* bpLastBit =
    BDSBeamPipeFactory::Instance()->CreateBeamPipeAngledIn(beamPipeType,
							   itsName,
							   itsStraightSectionLength,
							   itsAngle*0.5,
							   aper1,
							   aper2,
							   aper3,
							   aper4,
							   vacuumMaterial,
							   beamPipeThickness,
							   beamPipeMaterial);

  // place logical volumes inside marker (container) volume
  // calculate offsets and rotations
  G4double straightSectionCentralZ = (itsMagFieldLength*0.5) + (itsStraightSectionChord*0.5);
  G4RotationMatrix* straightStartRM = new G4RotationMatrix();
  straightStartRM->rotateY(itsAngle*0.5);
  G4RotationMatrix* straightEndRM = new G4RotationMatrix();
  straightEndRM->rotateY(-itsAngle*0.5);
  straightEndRM->rotateZ(CLHEP::pi);
  G4ThreeVector straightStartPos = G4ThreeVector(orientation*magnetXShift*0.5,0,-straightSectionCentralZ);
  G4ThreeVector straightEndPos   = G4ThreeVector(orientation*magnetXShift*0.5,0,straightSectionCentralZ);
  G4ThreeVector magnetPos        = G4ThreeVector(orientation*magnetXShift,0,0);
  
  new G4PVPlacement(straightStartRM,                         // rotation
		    straightStartPos,                        // position
		    bpFirstBit->GetContainerLogicalVolume(), // logical volume
		    itsName+"_bp_start_phys",                // name
		    itsMarkerLogicalVolume,                  // mother volume
		    false,		                     // no booleanm operation
		    0,                                       // copy number
		    BDSGlobalConstants::Instance()->GetCheckOverlaps() );

   new G4PVPlacement(0,                                      // rotation
		     magnetPos,	                             // position
		     beampipe->GetContainerLogicalVolume(),  // logical volume
		     itsName+"_bp_phys",                     // name
		     itsMarkerLogicalVolume,                 // mother volume
		     false,	                             // no boolean operation
		     0,                                      // copy number
		     BDSGlobalConstants::Instance()->GetCheckOverlaps() );

   new G4PVPlacement(straightEndRM,                          // rotation
		     straightEndPos,	                     // position
		     bpLastBit->GetContainerLogicalVolume(), // logical volume
		     itsName+"_bp_end_phys",	             // name
		     itsMarkerLogicalVolume,                 // mother volume
		     false,	                             // no boolean operation
		     0,                                      // copy number
		     BDSGlobalConstants::Instance()->GetCheckOverlaps() );

   // can't use BeamPipeCommonTasks() as it places the beampipe volume and that'll be wrong in this case
   // have to do everything manually here
   beampipe->GetVacuumLogicalVolume()->SetFieldManager(itsBPFieldMgr,false);
   itsMarkerLogicalVolume->
    SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
   
   RegisterLogicalVolumes(bpFirstBit->GetAllLogicalVolumes());
   RegisterLogicalVolumes(beampipe->GetAllLogicalVolumes());
   RegisterLogicalVolumes(bpLastBit->GetAllLogicalVolumes());

   // register components as sensitive if required
   if(BDSGlobalConstants::Instance()->GetSensitiveBeamPipe())
     {
       RegisterSensitiveVolumes(bpFirstBit->GetAllSensitiveVolumes());
       RegisterSensitiveVolumes(beampipe->GetAllSensitiveVolumes());
       RegisterSensitiveVolumes(bpLastBit->GetAllSensitiveVolumes());
     }
   
   SetExtentX(beampipe->GetExtentX()); //not exact but only central porition will use this
   SetExtentY(beampipe->GetExtentY());
   SetExtentZ(-itsChordLength*0.5,itsChordLength*0.5);
}
