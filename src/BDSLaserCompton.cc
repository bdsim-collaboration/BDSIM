#include "BDSComptonEngine.hh"
#include "BDSGlobalConstants.hh" 
#include "BDSLaserCompton.hh"

#include "globals.hh"
#include "G4Gamma.hh"

#include "CLHEP/Units/PhysicalConstants.h"

BDSLaserCompton::BDSLaserCompton(const G4String& processName):
  G4VDiscreteProcess(processName),
  laserEnergy(0.0)
{
  globals = BDSGlobalConstants::Instance();
  laserWavelength = globals->GetLaserwireWavelength();
  laserDirection  = globals->GetLaserwireDir();
  comptonEngine   = new BDSComptonEngine();
} 

BDSLaserCompton::~BDSLaserCompton()
{
  delete comptonEngine;
}

G4VParticleChange* BDSLaserCompton::PostStepDoIt(const G4Track& trackData,
						 const G4Step& stepData)
{
 aParticleChange.Initialize(trackData);
 
 // ensure that Laserwire can only occur once in an event
 G4cout << "FireLaserCompton == " << FireLaserCompton << G4endl;
 
 if(!FireLaserCompton)
   {return G4VDiscreteProcess::PostStepDoIt(trackData,stepData);}
 
 G4Material* aMaterial = trackData.GetMaterial();
 
 if (aMaterial == BDSMaterials::Instance()->GetMaterial("LaserVac"))
   {
     laserWavelength = globals->GetLaserwireWavelength();
     laserDirection  = globals->GetLaserwireDir();
     
     laserEnergy = CLHEP::twopi*CLHEP::hbarc/laserWavelength;
     
     // point laserwire in x:     P_x        Py Pz   E
     G4LorentzVector Laser4mom(laserEnergy*laserDirection.unit(),laserEnergy);
     
     const G4DynamicParticle* aDynamicParticle = trackData.GetDynamicParticle();
     
     comptonEngine->SetIncomingElectron4Vec(aDynamicParticle->Get4Momentum());
     comptonEngine->SetIncomingPhoton4Vec(Laser4mom);
     comptonEngine->PerformCompton();
     
     if(globals->GetLaserwireTrackPhotons())
       {
	 // create G4DynamicParticle object for the Gamma 
	 G4LorentzVector ScatGam = comptonEngine->GetScatteredGamma();
	 G4DynamicParticle* aGamma = new G4DynamicParticle (G4Gamma::Gamma(), 
							    ScatGam.vect().unit(),// direction 
							    ScatGam.e());
	 
	 aParticleChange.SetNumberOfSecondaries(1);
	 aParticleChange.AddSecondary(aGamma); 
	 if(!globals->GetLaserwireTrackElectrons())
	   {
	     aParticleChange.ProposeEnergy(0.);
	     aParticleChange.ProposeLocalEnergyDeposit (0.);
	     aParticleChange.ProposeTrackStatus(fStopAndKill);
	   }
       }
     else
       {
	 aParticleChange.SetNumberOfSecondaries(0);
     	 aParticleChange.ProposeLocalEnergyDeposit (0.);
       }
    
     G4double NewKinEnergy = comptonEngine->GetScatteredElectron().e()-CLHEP::electron_mass_c2;
     
     G4LorentzVector ScatEl = comptonEngine->GetScatteredElectron();
     
     if (NewKinEnergy > 0.)
       {
	 aParticleChange.ProposeMomentumDirection(ScatEl.vect().unit());
	 aParticleChange.ProposeEnergy(NewKinEnergy);
	 aParticleChange.ProposeLocalEnergyDeposit (0.); 
       } 
     else
       { 
	 aParticleChange.ProposeEnergy( 0. );
	 aParticleChange.ProposeLocalEnergyDeposit (0.);
	 G4double charge= aDynamicParticle->GetCharge();
	 if (charge<0.) aParticleChange.ProposeTrackStatus(fStopAndKill);
	 else       aParticleChange.ProposeTrackStatus(fStopButAlive);
       }    
   }
 
 FireLaserCompton = false;
 
 return G4VDiscreteProcess::PostStepDoIt(trackData,stepData);
}
