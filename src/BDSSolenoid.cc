#include "BDSGlobalConstants.hh" 
#include "BDSDebug.hh"

#include "BDSSolenoid.hh"
#include "BDSSolenoidMagField.hh"
#include "BDSSolenoidStepper.hh"
#include "G4Mag_UsualEqRhs.hh"

#include "G4MagneticField.hh"

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

BDSSolenoid::BDSSolenoid(G4String        name,
			 G4double        length,
			 G4double        bField,
			 BDSBeamPipeType beamPipeType,
			 G4double        aper1,
			 G4double        aper2,
			 G4double        aper3,
			 G4double        aper4,
			 G4Material*     vacuumMaterial,
			 G4double        beamPipeThickness,
			 G4Material*     beamPipeMaterial,
			 G4double        boxSize,
			 G4String        outerMaterial,
			 G4String        tunnelMaterial,
			 G4double        tunnelRadius,
			 G4double        tunnelOffsetX):
  BDSMultipole(name,length,beamPipeType,aper1,aper2,aper3,aper4,vacuumMaterial,beamPipeThickness,
	       beamPipeMaterial,boxSize,outerMaterial,tunnelMaterial,tunnelRadius,tunnelOffsetX),
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
