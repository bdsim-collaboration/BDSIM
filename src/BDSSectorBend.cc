#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"

#include "BDSSectorBend.hh"

#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSDipoleStepper.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetType.hh"
#include "BDSSbendMagField.hh"
#include "BDSUtilities.hh"        // for calculateorientation

#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4VPhysicalVolume.hh"

#include "globals.hh"             // geant4 types / globals

BDSSectorBend::BDSSectorBend(G4String            name,
			     G4double            arcLength,
			     G4double            angleIn,
			     G4double            bField,
			     G4double            bGrad,
			     G4double            e1in,
			     G4double            e2in,
			     BDSBeamPipeInfo*    beamPipeInfo,
			     BDSMagnetOuterInfo* magnetOuterInfo):
  BDSMagnet(BDSMagnetType::sectorbend, name, arcLength,
	    beamPipeInfo, magnetOuterInfo),
  itsBField(bField),itsBGrad(bGrad),e1(e1in),e2(e2in)
{
  //G4cout << "e1 " << e1 << G4endl;
  //G4cout << "e2 " << e2 << G4endl;
  
  /// BDSMagnet doesn't provide the ability to pass down angle to BDSAcceleratorComponent
  /// - this results in a wrongly chord length
  angle       = angleIn;
  if (BDS::IsFinite(angle))
    {
      chordLength = 2.0 * arcLength * sin(0.5*angleIn) / angleIn;
      // prepare normal vectors for input and output planes
      // calculate components of normal vectors (in the end mag(normal) = 1)
      orientation   = BDS::CalculateOrientation(angleIn);
      G4double in_z = cos(0.5*fabs(angleIn)); 
      G4double in_x = sin(0.5*fabs(angleIn));
      inputface     = G4ThreeVector(-orientation*in_x, 0.0, -1.0*in_z);
      //-1 as pointing down in z for normal
      outputface    = G4ThreeVector(-orientation*in_x, 0.0, in_z);
    }
  else
    {chordLength = arcLength;}

#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "angle:        " << angle     << G4endl;
  G4cout << __METHOD_NAME__ << "arc length:   " << arcLength << G4endl;
  G4cout << __METHOD_NAME__ << "chord length: " << chordLength << G4endl;
#endif
}

void BDSSectorBend::Build()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  BDSMagnet::Build();
  
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

void BDSSectorBend::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  G4ThreeVector Bfield(0.,itsBField,0.);
  // B-Field constructed with arc length for radius of curvature
  itsMagField = new BDSSbendMagField(Bfield,arcLength,angle);
  itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);  
  BDSDipoleStepper* dipoleStepper = new BDSDipoleStepper(itsEqRhs);
  dipoleStepper->SetBField(itsBField);
  dipoleStepper->SetBGrad(itsBGrad);
  itsStepper = dipoleStepper;
}

void BDSSectorBend::BuildOuter()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4Material* outerMaterial          = magnetOuterInfo->outerMaterial;
  BDSMagnetGeometryType geometryType = magnetOuterInfo->geometryType; 
  BDSMagnetOuterFactory* theFactory  = BDSMagnetOuterFactory::Instance();
  G4double containerDiameter = 2*containerRadius;
  
  outer = theFactory->CreateSectorBend(geometryType, name, chordLength, beampipe,
					    outerDiameter, chordLength,
					    angle, e1, e2, outerMaterial);

  if (outer)
    {
      BDSGeometryComponent* container = outer->GetMagnetContainer();
      containerSolid    = container->GetContainerSolid()->Clone();
      G4ThreeVector contOffset = container->GetPlacementOffset();
      // set the main offset of the whole magnet which is placed w.r.t. the
      // zero coordinate of the container solid
      SetPlacementOffset(contOffset);

      InheritExtents(container); // update extents
      outer->ClearMagnetContainer(); // delete the magnet container as done with
    }
}


void BDSSectorBend::BuildBeampipe()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "sector bend version " << G4endl;
#endif
  BDSBeamPipeFactory* factory = BDSBeamPipeFactory::Instance();

  if (BDS::IsFinite(angle))
    {
      beampipe = factory->CreateBeamPipeAngledInOut(beamPipeInfo->beamPipeType,
						    name,
						    chordLength - lengthSafety,
						    e1,
						    e2,
						    beamPipeInfo->aper1,
						    beamPipeInfo->aper2,
						    beamPipeInfo->aper3,
						    beamPipeInfo->aper4,
						    beamPipeInfo->vacuumMaterial,
						    beamPipeInfo->beamPipeThickness,
						    beamPipeInfo->beamPipeMaterial);
    }
  else
    {
      beampipe = factory->CreateBeamPipe(name,
					 chordLength - lengthSafety,
					 beamPipeInfo);
    }

  RegisterDaughter(beampipe);

  SetAcceleratorVacuumLogicalVolume(beampipe->GetVacuumLogicalVolume());
}
