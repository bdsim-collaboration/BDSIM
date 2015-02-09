#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"

#include "BDSSolenoid.hh"
#include "BDSSolenoidMagField.hh"
#include "BDSSolenoidStepper.hh"
#include "G4Mag_UsualEqRhs.hh"

#include "G4MagneticField.hh"
#include "G4HelixImplicitEuler.hh"
#include "G4SimpleRunge.hh"
#include "G4ClassicalRK4.hh"

#include "G4UniformMagField.hh"
#include "G4Tubs.hh"
#include "G4VisAttributes.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4UserLimits.hh"


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

void BDSSolenoid::BuildBeampipe(G4String materialName)
{
  BDSMultipole::BuildBeampipe(materialName);
  itsInnerBeampipeUserLimits->SetMaxAllowedStep(itsLength*0.5);
  itsInnerBPLogicalVolume->SetUserLimits(itsInnerBeampipeUserLimits);
}

void BDSSolenoid::SetVisAttributes()
{
  itsVisAttributes=new G4VisAttributes(G4Colour(1.,0.,0.)); //red
  itsVisAttributes->SetForceSolid(true);
}

void BDSSolenoid::BuildBPFieldAndStepper()
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << G4endl;
#endif

  //#ifdef _USE_GEANT4_STEPPER_
  // using Geant4
//G4ThreeVector Bfield(0.,0.,itsBField);
//itsMagField = new G4UniformMagField(Bfield);
  //itsMagField = new BDSSolenoidMagField(itsBField);
  itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);
//itsStepper  = new G4ClassicalRK4(itsEqRhs,6);
  //itsStepper  = new G4SimpleRunge(itsEqRhs);
  //itsStepper  = new G4HelixImplicitEuler(itsEqRhs);
  //#endif

//#else
  // using BDSIM
  G4ThreeVector Bfield(0.,0.,itsBField);
  itsMagField = new G4UniformMagField(Bfield);
  itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);
  BDSSolenoidStepper* solenoidStepper = new BDSSolenoidStepper(itsEqRhs);
  solenoidStepper->SetBField(itsBField);
  itsStepper = solenoidStepper;
//#endif
//  */
}

BDSSolenoid::~BDSSolenoid()
{
}
