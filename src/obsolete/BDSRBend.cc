#include "BDSBeamPipe.hh"
#include "BDSBeamPipeFactory.hh"
#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSFieldInfo.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnetOuterFactory.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMagnetType.hh"
#include "BDSRBend.hh"
#include "BDSUtilities.hh"

#include "globals.hh" // geant4 types / globals
#include "G4CutTubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4VPhysicalVolume.hh"

BDSRBend::BDSRBend(G4String            name,
		   G4double            length,
		   G4double            angleIn,
		   BDSBeamPipeInfo*    beamPipeInfo,
		   BDSMagnetOuterInfo* magnetOuterInfo,
		   BDSFieldInfo*       vacuumFieldInfo,
		   BDSFieldInfo*       outerFieldInfo):
  BDSMagnet(BDSMagnetType::rectangularbend, name, length,
	    beamPipeInfo, magnetOuterInfo, vacuumFieldInfo, outerFieldInfo)
{
  angle       = angleIn;
  outerRadius = magnetOuterInfo->outerDiameter*0.5; 
}

void BDSRBend::Build()
{
  BDSMagnet::Build();
  /*
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
      }*/
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

