
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

BDSSolenoid::BDSSolenoid(G4String           name,
			 G4double           length,
			 G4double           bField,
			 BDSBeamPipeInfo    beamPipeInfo,
			 BDSMagnetOuterInfo magnetOuterInfo,
			 BDSTunnelInfo      tunnelInfo):
  BDSMultipole(BDSMagnetType::solenoid,name,length,beamPipeInfo,magnetOuterInfo,tunnelInfo),
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
