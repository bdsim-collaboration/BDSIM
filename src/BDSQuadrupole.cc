//  
//   BDSIM, (C) 2001-2006 
//   
//   version 0.3
//  
//
//
//   Quadrupole class
//
//   History
//
//     21 Nov 2006 by Agapov,  v.0.3
//     22 Mar 2005 by Agapov, Carter,  v.0.2
//     x  x   2002 by Blair
//
//

#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSMagnetType.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSQuadrupole.hh"
#include "BDSQuadMagField.hh"
#include "BDSQuadStepper.hh"
#include "BDSEnergyCounterSD.hh"

#include "G4FieldManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Tubs.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

class BDSTiltOffset;

BDSQuadrupole::BDSQuadrupole(G4String           name,
			     G4double           length,
			     G4double           bGrad,
			     BDSBeamPipeInfo*   beamPipeInfo,
			     BDSMagnetOuterInfo magnetOuterInfo,
			     BDSTiltOffset      tiltOffset):
  BDSMagnet(BDSMagnetType::quadrupole, name, length,
	    beamPipeInfo, magnetOuterInfo, tiltOffset),
  itsBGrad(bGrad)
{;}

void BDSQuadrupole::Build() 
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
      polePos[0]=-BDSGlobalConstants::Instance()->GetMagnetPoleRadius()*sin(CLHEP::pi/4);
      polePos[1]=BDSGlobalConstants::Instance()->GetMagnetPoleRadius()*cos(CLHEP::pi/4);
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
      
      BuildOuterFieldManager(4, BFldIron,CLHEP::pi/4);
    }
}

void BDSQuadrupole::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper
  itsMagField = new BDSQuadMagField(1*itsBGrad); //L Deacon checking sign of field 4/7/12
  itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);
  BDSQuadStepper* quadStepper = new BDSQuadStepper(itsEqRhs);
  quadStepper->SetBGrad(itsBGrad);
  itsStepper = quadStepper;
}
