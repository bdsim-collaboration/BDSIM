//  
//   BDSIM, (C) 2001-2007
//   
//   version 0.4
//  
//
//
//   Solenoid class
//
//
//   History
//
//     21 Oct 2007 by Marchiori,  v.0.4
//
//


#include "BDSGlobalConstants.hh" 

#include "BDSSolenoid.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "BDSSolenoidMagField.hh"
#include "G4MagneticField.hh"
#include "BDSSolenoidStepper.hh"
#include "G4HelixImplicitEuler.hh"

#include "G4UniformMagField.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"
#include "G4TransportationManager.hh"

//============================================================

BDSSolenoid::BDSSolenoid(G4String aName, G4double aLength, 
			 G4double bpRad, G4double FeRad,
			 G4double bField, G4double outR,
                         std::list<G4double> blmLocZ, std::list<G4double> blmLocTheta,
			 G4String aTunnelMaterial, G4String aMaterial):
  BDSMultipole(aName, aLength, bpRad, FeRad, blmLocZ, blmLocTheta, aTunnelMaterial, aMaterial),
  itsBField(bField)
{
  SetOuterRadius(outR);
}

void BDSSolenoid::Build()
{
  BDSMultipole::Build();
  if(BDSGlobalConstants::Instance()->GetIncludeIronMagFields())
    {
      G4cerr<<"IncludeIronMagFields option not implemented for solenoid class"<<G4endl;
    }
}
  
void BDSSolenoid::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(1.,0.,0.)); //red
  itsVisAttributes->SetForceSolid(true);
}

void BDSSolenoid::BuildBPFieldAndStepper()
{
  // set up the magnetic field and stepper

#ifdef _USE_GEANT4_STEPPER_
  // using Geant4
  itsMagField = new BDSSolenoidMagField(itsBField);
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);
  itsStepper=new G4HelixImplicitEuler(itsEqRhs);
#else
  // using BDSIM
  G4ThreeVector Bfield(0.,0.,itsBField);
  itsMagField=new G4UniformMagField(Bfield);
  itsEqRhs=new G4Mag_UsualEqRhs(itsMagField);
  itsStepper=new BDSSolenoidStepper(itsEqRhs);
  BDSSolenoidStepper* solenoidStepper = dynamic_cast<BDSSolenoidStepper*>(itsStepper);
  solenoidStepper->SetBField(itsBField);
#endif
}

BDSSolenoid::~BDSSolenoid()
{
}
