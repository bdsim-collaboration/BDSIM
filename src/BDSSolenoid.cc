
#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSSolenoid.hh"
#include "BDSSolenoidMagField.hh"
#include "BDSSolenoidStepper.hh"

#include "G4LogicalVolume.hh"
#include "G4Mag_UsualEqRhs.hh"
#include "G4MagneticField.hh"
#include "G4Tubs.hh"
#include "G4UniformMagField.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "G4VPhysicalVolume.hh"

BDSSolenoid::BDSSolenoid(G4String        name,
			 G4double        length,
			 G4double        bField,
			 BDSBeamPipeInfo beamPipeInfoIn,
			 G4double        boxSize,
			 G4String        outerMaterial,
			 G4String        tunnelMaterial,
			 G4double        tunnelRadius,
			 G4double        tunnelOffsetX):
  BDSMultipole(name,length,beamPipeInfoIn,boxSize,outerMaterial,tunnelMaterial,tunnelRadius,tunnelOffsetX),
  itsBField(bField)
{;}

void BDSSolenoid::Build()
{
  BDSMultipole::Build();
  if(BDSGlobalConstants::Instance()->GetIncludeIronMagFields())
    {
      G4cout << __METHOD_NAME__ << "IncludeIronMagFields option not implemented for solenoid class"<<G4endl;
    }
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
  
  G4ThreeVector Bfield(0.,0.,itsBField);
  itsMagField = new G4UniformMagField(Bfield);
  itsEqRhs    = new G4Mag_UsualEqRhs(itsMagField);
  BDSSolenoidStepper* solenoidStepper = new BDSSolenoidStepper(itsEqRhs);
  solenoidStepper->SetBField(itsBField);
  itsStepper = solenoidStepper;
}
