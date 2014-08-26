/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
//      ------------ BDSPlanckScatter physics process --------
//                     by Grahame Blair, 18 October 2001
#include "BDSGlobalConstants.hh" 

#include "BDSPlanckScatter.hh"
#include "G4ios.hh"
#include "G4Gamma.hh"
#include "G4UnitsTable.hh"

BDSPlanckScatter::BDSPlanckScatter():G4VEnergyLossProcess("PlanckScatt")
{

  // TODO: change to appropriate definition!!!
  itsTemperature = 300 * CLHEP::kelvin;

  if(itsTemperature<=0.){G4Exception("BDSPlanckScatter: Invalid Temperature", "-1", FatalException, "");}
  itsPlanckEngine=new BDSPlanckEngine(itsTemperature);
  itsComptonEngine=new BDSComptonEngine();

  // Thomspson cross sec (to be replaced below with Compton)
  G4double sigma_T=0.6652*CLHEP::barn;
  
  G4double AvPhotonEnergy=2.7*CLHEP::k_Boltzmann*itsTemperature;
  
  G4double w= BDSGlobalConstants::Instance()->GetBeamTotalEnergy()*AvPhotonEnergy/
    pow( CLHEP::electron_mass_c2,2);
  
  G4double sigma=sigma_T*3/4*(
			      (1+w)/pow(w,3)*( 2*w*(1+w)/(1+2*w) -log(1+2*w))
			      + log(1+2*w)/(2*w)
			      - (1+3*w)/pow((1+2*w),2) );
  
  G4double photon_density = pow((itsTemperature/295.15),3)*5.329e14*pow(CLHEP::m,-3);
  itsPlanckMeanFreePath=1/(photon_density*sigma);
  
  // include scaling so that statistics are more reasonable:
  itsPlanckMeanFreePath /= BDSGlobalConstants::Instance()->GetPlanckScatterFe(); 
} 


BDSPlanckScatter::~BDSPlanckScatter()
{
  delete itsComptonEngine;
  delete itsPlanckEngine;
}


G4VParticleChange* BDSPlanckScatter::PostStepDoIt(const G4Track& trackData,
						  const G4Step& stepData)
{
  
  aParticleChange.Initialize(trackData);
  
  const G4DynamicParticle* aDynamicParticle=trackData.GetDynamicParticle(); 
  itsComptonEngine->SetIncomingElectron4Vec(aDynamicParticle->Get4Momentum());
  
  itsComptonEngine->SetIncomingPhoton4Vec(itsPlanckEngine->PerformPlanck());
  
  itsComptonEngine->PerformCompton();
  
  // create G4DynamicParticle object for the Gamma 
  G4LorentzVector ScatGam=itsComptonEngine->GetScatteredGamma();
  
  G4DynamicParticle* aGamma= 
    new G4DynamicParticle (G4Gamma::Gamma(), 
			   ScatGam.vect().unit(),// direction 
			   ScatGam.e());
  
  aParticleChange.SetNumberOfSecondaries(1);
  aParticleChange.AddSecondary(aGamma); 
  
  //
  // Update the incident particle 
  //
  G4double NewKinEnergy=
    itsComptonEngine->GetScatteredElectron().e()-CLHEP::electron_mass_c2;
  
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
  
  return G4VContinuousDiscreteProcess::PostStepDoIt(trackData,stepData);
}

void BDSPlanckScatter::InitialiseEnergyLossProcess(const G4ParticleDefinition*, const G4ParticleDefinition*)
{
}

void BDSPlanckScatter::PrintInfo()
{
}

