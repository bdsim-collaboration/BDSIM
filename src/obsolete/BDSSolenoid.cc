#include "BDSBeamPipeInfo.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetOuterInfo.hh"
#include "BDSSolenoid.hh"

BDSSolenoid::BDSSolenoid(G4String            name,
			 G4double            length,
			 G4double            bField,
			 BDSBeamPipeInfo*    beamPipeInfo,
			 BDSMagnetOuterInfo* magnetOuterInfo):
  BDSMagnet(BDSMagnetType::solenoid, name, length,
	    beamPipeInfo, magnetOuterInfo),
  itsBField(bField)
{;}

void BDSSolenoid::Build()
{
  BDSMagnet::Build();
  if(BDSGlobalConstants::Instance()->IncludeIronMagFields())
    {G4cout << __METHOD_NAME__ << "IncludeIronMagFields option not implemented for solenoid class"<<G4endl;}
}
