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
  bGrad(bGradIn)
{
  angle       = angleIn;
  outerRadius = magnetOuterInfo->outerDiameter*0.5; 
  CalculateLengths(length);
}

void BDSRBend::CalculateLengths(G4double aLength)
{
  //full length along chord - just its length in case of rbend
  chordLength = aLength;

  // orientation of shifts - depends on angle - calculations use absolute value of angle for safety
  G4int orientation = BDS::CalculateOrientation(angle);
  
  // straightSectionChord is the distance along the chord required to be used by a drift pipe so that
  // the outer logical volume (magnet cylinder - defined by outRadius) doesn't protrude
  // into the previous volume / outside the marker volume.  for zero angle, this is 0

  straightSectionChord        = outerRadius / (tan(0.5*fabs(angle)) + tan((0.5*CLHEP::pi) - (0.5*fabs(angle))) );
  magFieldLength              = chordLength;
  magnetXShift                = orientation*straightSectionChord*tan(0.5*fabs(angle));
  magnetOuterOffset           = G4ThreeVector(magnetXShift, 0, 0);
  G4double      dz            = (magFieldLength*0.5)+(straightSectionChord*0.5);
  G4ThreeVector driftposstart = G4ThreeVector(0.5*magnetXShift, 0, -1*dz);
  G4ThreeVector driftposend   = G4ThreeVector(0.5*magnetXShift, 0, dz);
  straightSectionLength       = straightSectionChord / (cos(0.5*fabs(angle)));
  // increase container radius to account for magnet outer geometry offset
  // container axis is chord axis between entry and exit points
  containerRadius             += fabs(magnetXShift)*1.001; // 1% margin due to calculations

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
    {
      arclength = fabs(angle) * ((magFieldLength*0.5) / sin(0.5*fabs(angle)));
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
  else
    {arclength = magFieldLength;}
}

