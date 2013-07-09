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
#include "G4UnitsTable.hh"

#if G4VERSION_NUMBER > 899
BDSLaserCompton::BDSLaserCompton(const G4String& processName)
  :  G4VDiscreteProcess(processName),//isInitialised(false),
     itsLaserEnergy(0.0)
#else
BDSLaserCompton::BDSLaserCompton(const G4String& processName)
     :  G4VeEnergyLoss(processName), itsLaserEnergy(0.0)
#endif
{
  itsLaserWavelength=BDSGlobalConstants::Instance()->GetLaserwireWavelength();
  itsLaserDirection=BDSGlobalConstants::Instance()->GetLaserwireDir();


  //	if(itsLaserWavelength<=0.)
  //	 {G4Exception("BDSLaserCompton: Invalid Wavelength");}
  // itsLaserEnergy=twopi*hbarc/itsLaserWavelength;
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
	 #if G4VERSION_NUMBER > 899
   return G4VDiscreteProcess::PostStepDoIt(trackData,stepData);
   #else
	 return G4VContinuousDiscreteProcess::PostStepDoIt(trackData,stepData);
	 #endif
 }
 G4Material* aMaterial=trackData.GetMaterial() ;
 
 if(aMaterial==BDSMaterials::Instance()->GetMaterial("LaserVac"))
   {
     itsLaserWavelength=BDSGlobalConstants::Instance()->GetLaserwireWavelength();
     itsLaserDirection=BDSGlobalConstants::Instance()->GetLaserwireDir();
     
     //G4cout << "&&&&&" << itsLaserDirection << "&&&&&\n";
     if(itsLaserWavelength<=0.)
       {G4Exception("BDSLaserCompton::PostStepDoIt - Invalid Wavelength", "-1", FatalException, "");}
     itsLaserEnergy=twopi*hbarc/itsLaserWavelength;
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
#if G4VERSION_NUMBER > 699
	     aParticleChange.ProposeEnergy( 0. );
	     aParticleChange.ProposeLocalEnergyDeposit (0.);
	     aParticleChange.ProposeTrackStatus(fStopAndKill);
#else
	     aParticleChange.SetEnergyChange( 0. );
	     aParticleChange.SetLocalEnergyDeposit (0.);
	     aParticleChange.SetStatusChange(fStopAndKill);
#endif
	   }
       }
     else
       {
#if G4VERSION_NUMBER > 699
	 aParticleChange.SetNumberOfSecondaries(0);
     	 aParticleChange.ProposeLocalEnergyDeposit (0.);
#else
	 aParticleChange.SetNumberOfSecondaries(0);
     	 aParticleChange.SetLocalEnergyDeposit (0.);
#endif

       }
     //
     // Update the incident particle 
     //

    
     G4double NewKinEnergy=
       itsComptonEngine->GetScatteredElectron().e()-electron_mass_c2;
     
     //  G4double NewKinEnergy=0; // tmp to track photon only
     
     G4LorentzVector ScatEl=itsComptonEngine->GetScatteredElectron();
     
     
     if (NewKinEnergy > 0.)
       {
#if G4VERSION_NUMBER > 699
	 aParticleChange.ProposeMomentumDirection(ScatEl.vect().unit());
	 aParticleChange.ProposeEnergy(NewKinEnergy);
	 aParticleChange.ProposeLocalEnergyDeposit (0.); 
#else
	 aParticleChange.SetMomentumChange(ScatEl.vect().unit());
	 aParticleChange.SetEnergyChange(NewKinEnergy);
	 aParticleChange.SetLocalEnergyDeposit (0.); 
#endif
       } 
     else
       { 

#if G4VERSION_NUMBER > 699
	 aParticleChange.ProposeEnergy( 0. );
	 aParticleChange.ProposeLocalEnergyDeposit (0.);
	 G4double charge= aDynamicParticle->GetCharge();
	 if (charge<0.) aParticleChange.ProposeTrackStatus(fStopAndKill);
	 else       aParticleChange.ProposeTrackStatus(fStopButAlive);
#else
	 aParticleChange.SetEnergyChange( 0. );
	 aParticleChange.SetLocalEnergyDeposit (0.);
	 G4double charge= aDynamicParticle->GetCharge();
	 if (charge<0.) aParticleChange.SetStatusChange(fStopAndKill);
	 else       aParticleChange.SetStatusChange(fStopButAlive);
#endif

       }    
     
   }
 
 FireLaserCompton=false;
 
 #if G4VERSION_NUMBER > 899
 return G4VDiscreteProcess::PostStepDoIt(trackData,stepData);
 #else
 return G4VContinuousDiscreteProcess::PostStepDoIt(trackData,stepData);
 #endif
}

#if G4VERSION_NUMBER > 899
/*
#include "G4LossTableManager.hh"
#include "G4eBremsstrahlungModel.hh"
#include "G4UniversalFluctuation.hh"

void BDSLaserCompton::InitialiseEnergyLossProcess(const G4ParticleDefinition* p, const G4ParticleDefinition*)
{
  if(!isInitialised) {
    particle = p;
    SetSecondaryParticle(G4Gamma::Gamma());
    SetIonisation(false);
    if (!EmModel()) SetEmModel(new G4eBremsstrahlungModel());
    EmModel()->SetLowEnergyLimit (100*eV);
    EmModel()->SetHighEnergyLimit(100*TeV);
    if (!FluctModel()) SetFluctModel(new G4UniversalFluctuation());

    AddEmModel(1, EmModel(), FluctModel());
    isInitialised = true;
  }
  G4LossTableManager* man = G4LossTableManager::Instance();
  dynamic_cast<G4eBremsstrahlungModel*>(EmModel())
    ->SetEnergyThreshold(man->BremsstrahlungTh());
  dynamic_cast<G4eBremsstrahlungModel*>(EmModel())
    ->SetLPMflag(man->LPMFlag());
}

void BDSLaserCompton::PrintInfo()
{

  if(EmModel()) {
    G4cout << "      Total cross sections and sampling from "
           << EmModel()->GetName() << " model"
           << " (based on the EEDL data library) "
           << "\n      Good description from 1 KeV to 100 GeV, "
           << "log scale extrapolation above 100 GeV."
           << " LPM flag "
           << dynamic_cast<G4eBremsstrahlungModel*>(EmModel())->LPMflag()
           << G4endl;
    G4double eth = dynamic_cast<G4eBremsstrahlungModel*>(EmModel())->EnergyThreshold();
    if(eth < DBL_MIN)
      G4cout << "      HighEnergyThreshold(GeV)= " << eth/GeV
             << G4endl;
  }

}
*/
#endif
