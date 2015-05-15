/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
//      ------------ BDSLaserCompton physics process --------
//                     by Grahame Blair, 18 October 2001
#include "BDSGlobalConstants.hh" 

#include "BDSLaserCompton.hh"
#include "G4ios.hh"
#include "G4Gamma.hh"
#include "CLHEP/Units/PhysicalConstants.h"

BDSLaserCompton::BDSLaserCompton(const G4String& processName)
  :  G4VDiscreteProcess(processName),//isInitialised(false),
     itsLaserEnergy(0.0)
{
  itsLaserWavelength=BDSGlobalConstants::Instance()->GetLaserwireWavelength();
  itsLaserDirection=BDSGlobalConstants::Instance()->GetLaserwireDir();


  //	if(itsLaserWavelength<=0.)
  //	 {G4Exception("BDSLaserCompton: Invalid Wavelength");}
  // itsLaserEnergy=CLHEP::twopi*CLHEP::hbarc/itsLaserWavelength;
 // point laserwire in x:     P_x        Py Pz   E
 //G4LorentzVector Laser4mom(itsLaserEnergy,0,0,itsLaserEnergy);
 //itsComptonEngine=new BDSComptonEngine(Laser4mom);
  itsComptonEngine=new BDSComptonEngine();
} 

 
BDSLaserCompton::~BDSLaserCompton()
{
  delete itsComptonEngine;
}


G4VParticleChange* BDSLaserCompton::PostStepDoIt(const G4Track& trackData,
						 const G4Step& stepData)
{
 
 
 aParticleChange.Initialize(trackData);
 
 // ensure that Laserwire can only occur once in an event
 G4cout << "FireLaserCompton == " << FireLaserCompton << G4endl;
 if(!FireLaserCompton){
   return G4VDiscreteProcess::PostStepDoIt(trackData,stepData);
 }
 G4Material* aMaterial=trackData.GetMaterial() ;
 
 if(aMaterial==BDSMaterials::Instance()->GetMaterial("LaserVac"))
   {
     itsLaserWavelength=BDSGlobalConstants::Instance()->GetLaserwireWavelength();
     itsLaserDirection=BDSGlobalConstants::Instance()->GetLaserwireDir();
     
     //G4cout << "&&&&&" << itsLaserDirection << "&&&&&\n";
     if(itsLaserWavelength<=0.)
       {G4Exception("BDSLaserCompton::PostStepDoIt - Invalid Wavelength", "-1", FatalException, "");}
     itsLaserEnergy=CLHEP::twopi*CLHEP::hbarc/itsLaserWavelength;
     // point laserwire in x:     P_x        Py Pz   E
     G4LorentzVector Laser4mom(itsLaserEnergy*itsLaserDirection.unit(),itsLaserEnergy);
     
     const G4DynamicParticle* aDynamicParticle=trackData.GetDynamicParticle();
     
     itsComptonEngine->
       SetIncomingElectron4Vec(aDynamicParticle->Get4Momentum());
     itsComptonEngine->SetIncomingPhoton4Vec(Laser4mom);
     
     itsComptonEngine->PerformCompton();
     
     if(BDSGlobalConstants::Instance()->GetLaserwireTrackPhotons())
       {

	 // create G4DynamicParticle object for the Gamma 
	 G4LorentzVector ScatGam=itsComptonEngine->GetScatteredGamma();
	 //      G4cout<<" Gamma Energy="<<ScatGam.e()/GeV<<" GeV"<<G4endl;
	 G4DynamicParticle* aGamma= 
	   new G4DynamicParticle (G4Gamma::Gamma(), 
				  ScatGam.vect().unit(),// direction 
				  ScatGam.e());
	 
	 aParticleChange.SetNumberOfSecondaries(1);
	 aParticleChange.AddSecondary(aGamma); 
	 if(!BDSGlobalConstants::Instance()->GetLaserwireTrackElectrons())
	   {
	     aParticleChange.ProposeEnergy( 0. );
	     aParticleChange.ProposeLocalEnergyDeposit (0.);
	     aParticleChange.ProposeTrackStatus(fStopAndKill);
	   }
       }
     else
       {
	 aParticleChange.SetNumberOfSecondaries(0);
     	 aParticleChange.ProposeLocalEnergyDeposit (0.);
       }
     //
     // Update the incident particle 
     //

    
     G4double NewKinEnergy=
       itsComptonEngine->GetScatteredElectron().e()-CLHEP::electron_mass_c2;
     
     //  G4double NewKinEnergy=0; // tmp to track photon only
     
     G4LorentzVector ScatEl=itsComptonEngine->GetScatteredElectron();
     
     
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
 
 FireLaserCompton=false;
 
 return G4VDiscreteProcess::PostStepDoIt(trackData,stepData);
}
