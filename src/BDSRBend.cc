#include "BDSGlobalConstants.hh"
#include "BDSDebug.hh"

#include "BDSRBend.hh"

#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSDipoleStepper.hh"
#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetType.hh"
#include "BDSSbendMagField.hh"
#include "BDSUtilities.hh"

#include "G4CutTubs.hh"
#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Mag_EqRhs.hh"
#include "G4PVPlacement.hh"
#include "G4VPhysicalVolume.hh"

BDSRBend::BDSRBend(G4String            name,
		   G4double            length,
		   G4double            bFieldIn,
		   G4double            bGradIn,
		   G4double            angleIn,
		   BDSBeamPipeInfo*    beamPipeInfo,
		   BDSMagnetOuterInfo* magnetOuterInfo):
  BDSMagnet(BDSMagnetType::rectangularbend, name, length,
	    beamPipeInfo, magnetOuterInfo),
  bField(bFieldIn),
  bGrad(bGradIn),
  bpFirstBit(nullptr),
  bpLastBit(nullptr)
{
  angle       = angleIn;
  outerRadius = magnetOuterInfo->outerDiameter*0.5; 
  CalculateLengths(length);
}

BDSRBend::~BDSRBend()
{
  if (bpFirstBit) // may not have been constructed
    {delete bpFirstBit;}
  if (bpLastBit) // may not have been constructed
    {delete bpLastBit;}
}

void BDSRBend::CalculateLengths(G4double aLength)
{
  //full length along chord - just its length in case of rbend
  chordLength = aLength;

  orientation = BDS::CalculateOrientation(angle);

  // straightSectionChord is the distance along the chord required to be used by a drift pipe so that
  // the outer logical volume (magnet cylinder - defined by outRadius) doesn't protrude
  // into the previous volume / outside the marker volume.  for zero angle, this is 0

  straightSectionChord        = outerRadius / (tan(0.5*fabs(angle)) + tan((0.5*CLHEP::pi) - (0.5*fabs(angle))) );
  magFieldLength              = chordLength - (2.0*straightSectionChord);
  magnetXShift                = orientation*straightSectionChord*tan(0.5*fabs(angle));
  magnetOuterOffset           = G4ThreeVector(magnetXShift, 0, 0);
  G4double      dz            = (magFieldLength*0.5)+(straightSectionChord*0.5);
  G4ThreeVector driftposstart = G4ThreeVector(0.5*magnetXShift, 0, -1*dz);
  G4ThreeVector driftposend   = G4ThreeVector(0.5*magnetXShift, 0, dz);
  straightSectionLength       = straightSectionChord / (cos(0.5*fabs(angle)));
  // increase container radius to account for magnet outer geometry offset
  // container axis is chord axis between entry and exit points
  containerRadius             += fabs(magnetXShift);

  G4double in_z = cos(0.5*fabs(angle)); // calculate components of normal vectors (in the end mag(normal) = 1)
  G4double in_x = sin(0.5*fabs(angle));
  inputface  = G4ThreeVector(-orientation*in_x, 0.0, -1.0*in_z); //-1 as pointing down in z for normal
  outputface = G4ThreeVector(-orientation*in_x, 0.0, in_z);
  
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "Angle                   = " << angle                    << G4endl;
  G4cout << __METHOD_NAME__ << "Straight section chord  = " << straightSectionChord  << G4endl;
  G4cout << __METHOD_NAME__ << "Magnetic field length   = " << magFieldLength        << G4endl;
  G4cout << __METHOD_NAME__ << "Straight section length = " << straightSectionLength << G4endl;
  G4cout << __METHOD_NAME__ << "Straight section chord  = " << straightSectionChord  << G4endl;
  G4cout << __METHOD_NAME__ << "Magnet shift in X       = " << magnetXShift             << G4endl;
#endif
}

void BDSRBend::Build()
{
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
      
      BuildOuterFieldManager(2, BFldIron,CLHEP::pi/2);
    }
}

void BDSRBend::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  G4ThreeVector Bfield(0.,bField,0.);
  G4double arclength;
  if (BDS::IsFinite(angle))
    {arclength = fabs(angle) * ((magFieldLength*0.5) / sin(0.5*fabs(angle)));}
  else
    {arclength = magFieldLength;}
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "calculated arclength in dipole field: " << arclength << G4endl;
#endif
  itsMagField = new BDSSbendMagField(Bfield,arclength,angle);
  itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);  
  
  BDSDipoleStepper* dipoleStepper = new BDSDipoleStepper(itsEqRhs);
  dipoleStepper->SetBField(bField);
  dipoleStepper->SetBGrad(bGrad);
  itsStepper = dipoleStepper;
}

void BDSRBend::BuildOuter()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  G4Material* outerMaterial          = magnetOuterInfo->outerMaterial;
  BDSMagnetGeometryType geometryType = magnetOuterInfo->geometryType; 
  BDSMagnetOuterFactory* theFactory  = BDSMagnetOuterFactory::Instance();
  G4double containerDiameter = 2*containerRadius;
  
  outer = theFactory->CreateRectangularBend(geometryType, name, magFieldLength, beampipe,
					    outerDiameter, containerDiameter, chordLength,
					    angle, outerMaterial);

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

void BDSRBend::BuildBeampipe()
{
  // check for finite length (can be negative if angle is zero or very small)
  G4double safeStraightSectionLength = straightSectionLength - lengthSafetyLarge;
  if (safeStraightSectionLength > 0)
    {
      bpFirstBit = BDSBeamPipeFactory::Instance()->CreateBeamPipeAngledOut(beamPipeInfo->beamPipeType,
									   name,
									   safeStraightSectionLength,
									   -angle*0.5,
									   beamPipeInfo->aper1,
									   beamPipeInfo->aper2,
									   beamPipeInfo->aper3,
									   beamPipeInfo->aper4,
									   beamPipeInfo->vacuumMaterial,
									   beamPipeInfo->beamPipeThickness,
									   beamPipeInfo->beamPipeMaterial);
      
      bpLastBit = BDSBeamPipeFactory::Instance()->CreateBeamPipeAngledIn(beamPipeInfo->beamPipeType,
									 name,
									 safeStraightSectionLength,
									 angle*0.5,
									 beamPipeInfo->aper1,
									 beamPipeInfo->aper2,
									 beamPipeInfo->aper3,
									 beamPipeInfo->aper4,
									 beamPipeInfo->vacuumMaterial,
									 beamPipeInfo->beamPipeThickness,
									 beamPipeInfo->beamPipeMaterial);
    }
  
  beampipe = BDSBeamPipeFactory::Instance()->CreateBeamPipe(beamPipeInfo->beamPipeType,
							    name,
							    magFieldLength - lengthSafety,
							    beamPipeInfo->aper1,
							    beamPipeInfo->aper2,
							    beamPipeInfo->aper3,
							    beamPipeInfo->aper4,
							    beamPipeInfo->vacuumMaterial,
							    beamPipeInfo->beamPipeThickness,
							    beamPipeInfo->beamPipeMaterial);

  G4double extentX = beampipe->GetExtentX().second + fabs(magnetOuterOffset.x());
  SetExtentX(-extentX, extentX);
  SetExtentY(beampipe->GetExtentY());
  SetExtentZ(-chordLength*0.5,chordLength*0.5);
}

void BDSRBend::BuildContainerLogicalVolume()
{
  if (!outer)
    {
      // update container solid to hold all the beampipe segments as there's no outer
      // and the default way won't suffice for rbend's unique geometry
      G4double smallContainerRadius = extentX.second; // +ve extent - updated by build beam pipe
      containerSolid = new G4CutTubs(name + "_container_solid", // name
				     0,                         // inner radius
				     smallContainerRadius,      // outer radius
				     chordLength*0.5,           // half length
				     0,                         // start angle
				     CLHEP::twopi,              // sweep angle
				     inputface,                 // input normal vector
				     outputface);               // output normal vector

    }

  containerLogicalVolume = new G4LogicalVolume(containerSolid,
					       emptyMaterial,
					       name + "_container_lv");
  
  // supposed to protect against fields being overridden
  containerLogicalVolume->SetFieldManager(BDSGlobalConstants::Instance()->GetZeroFieldManager(),false);
}

void BDSRBend::PlaceComponents()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif
  // place logical volumes inside marker (container) volume
  // calculate offsets and rotations
  G4double straightSectionCentralZ = (magFieldLength*0.5) + (straightSectionChord*0.5);
  G4RotationMatrix* straightStartRM = new G4RotationMatrix();
  straightStartRM->rotateY(-angle*0.5);
  G4RotationMatrix* straightEndRM = new G4RotationMatrix();
  straightEndRM->rotateY(-angle*0.5);
  straightEndRM->rotateZ(CLHEP::pi);
  G4ThreeVector straightStartPos = G4ThreeVector(magnetXShift*0.5,0,-straightSectionCentralZ);
  straightStartPos += -1*GetPlacementOffset();
  G4ThreeVector straightEndPos   = G4ThreeVector(magnetXShift*0.5,0,straightSectionCentralZ);
  straightEndPos += -1*GetPlacementOffset();

  G4Transform3D straightStartTF(*straightStartRM, straightStartPos);
  G4Transform3D straightEndTF  (*straightEndRM,   straightEndPos);

  RegisterRotationMatrix(straightStartRM);
  RegisterRotationMatrix(straightEndRM);

  if (bpFirstBit)
    {
      G4PVPlacement* pipeStartPV = new G4PVPlacement(straightStartTF,
						     bpFirstBit->GetContainerLogicalVolume(), // logical volume
						     name+"_beampipe_start_pv",               // name
						     containerLogicalVolume,                  // mother volume
						     false,		                      // no booleanm operation
						     0,                                       // copy number
						     checkOverlaps);
      RegisterPhysicalVolume(pipeStartPV);
    }

  // no if(placeBeamPipe) here as custom procedure and rbend has different construction
  if (outer)
    {
      G4ThreeVector outerOffset    = outer->GetPlacementOffset();
      G4ThreeVector beamPipeOffset = GetPlacementOffset() + magnetOuterOffset + outerOffset;
      G4PVPlacement* pipePV = new G4PVPlacement(0,
						beamPipeOffset,
						beampipe->GetContainerLogicalVolume(),   // logical volume
						name+"_beampipe_pv",                     // name
						containerLogicalVolume,                  // mother volume
						false,	                                 // no boolean operation
						0,                                       // copy number
						checkOverlaps);
      RegisterPhysicalVolume(pipePV);
    }

  if (bpLastBit)
    {
      G4PVPlacement* pipeEndRM   = new G4PVPlacement(straightEndTF,
						     bpLastBit->GetContainerLogicalVolume(),  // logical volume
						     name+"_beampipe_end_pv",	              // name
						     containerLogicalVolume,                  // mother volume
						     false,	                              // no boolean operation
						     0,                                       // copy number
						     checkOverlaps);
      RegisterPhysicalVolume(pipeEndRM);
    }

  if (outer)
    {
      G4ThreeVector placementOffset = magnetOuterOffset + outer->GetPlacementOffset();
      
      // place outer volume
      G4PVPlacement* magnetOuterPV = new G4PVPlacement(0,                           // rotation
						       placementOffset,             // at normally (0,0,0)
						       outer->GetContainerLogicalVolume(), // its logical volume
						       name+"_outer_pv",            // its name
						       containerLogicalVolume,      // its mother  volume
						       false,                       // no boolean operation
						       0,                           // copy number
						       BDSGlobalConstants::Instance()->GetCheckOverlaps());

      RegisterPhysicalVolume(magnetOuterPV);
    }
}


std::vector<G4LogicalVolume*> BDSRBend::GetAllSensitiveVolumes() const
{
  std::vector<G4LogicalVolume*> result;
  for (auto it : allSensitiveVolumes)
    {result.push_back(it);}

  if (beampipe)
    {
      for (auto it : beampipe->GetAllSensitiveVolumes())
	{result.push_back(it);}
    }

  if (bpFirstBit)
    {
      for (auto it : bpFirstBit->GetAllSensitiveVolumes())
	{result.push_back(it);}
    }

  if (bpLastBit)
    {
      for (auto it : bpLastBit->GetAllSensitiveVolumes())
	{result.push_back(it);}
    }
  return result;
}
