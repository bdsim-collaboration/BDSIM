/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/

// This code implementation is the intellectual property of
// the GEANT4 collaboration.
//
// By copying, distributing or modifying the Program (or any work
// based on the Program) you indicate your acceptance of this statement,
// and all its terms.
//
// $Id: BDSUserSpecialCuts.cc,v 1.2 2005/01/27 11:35:32 agapov Exp $
// GEANT4 tag $Name:  $
// 
// --------------------------------------------------------------
// History
//
// 15-04-98 first implementation, mma                   
// --------------------------------------------------------------

#include "BDSGlobalConstants.hh"
#include "BDSUserSpecialCuts.hh"


#include "G4Step.hh"
#include "G4UserLimits.hh"
#include "G4VParticleChange.hh"
#include "G4EnergyLossTables.hh"
#include "G4Version.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BDSUserSpecialCuts::BDSUserSpecialCuts(const G4String& aName)
  : G4VProcess(aName)
{
   if (verboseLevel>0) {
     G4cout << GetProcessName() << " is created "<< G4endl;
   }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BDSUserSpecialCuts::~BDSUserSpecialCuts()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

BDSUserSpecialCuts::BDSUserSpecialCuts(BDSUserSpecialCuts& right)
  : G4VProcess(right)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
G4double BDSUserSpecialCuts::PostStepGetPhysicalInteractionLength(
                             const G4Track& aTrack,
			     G4double,
			     G4ForceCondition* condition
			    )
{
  // condition is set to "Not Forced"
  *condition = NotForced;

   G4double ProposedStep = DBL_MAX;
   G4UserLimits* pUserLimits = aTrack.GetVolume()->GetLogicalVolume()->GetUserLimits();
   if (pUserLimits)
     { //max track length
       ProposedStep = (pUserLimits->GetUserMaxTrackLength(aTrack) - aTrack.GetTrackLength());
       if (ProposedStep < 0.) return 0.;
       //max time limit
       G4double beta = (aTrack.GetDynamicParticle()->GetTotalMomentum())/(aTrack.GetTotalEnergy());
       G4double dTime=(pUserLimits->GetUserMaxTime(aTrack) - aTrack.GetGlobalTime());
       G4double temp = beta*c_light*dTime;
       if (temp < 0.) return 0.;
       if (ProposedStep > temp) ProposedStep = temp;                  
       //min remaining range (only for charged particle)
       G4ParticleDefinition* Particle = aTrack.GetDefinition();
           if (Particle->GetPDGCharge() != 0.)
          {
          G4double              Ekine    = aTrack.GetKineticEnergy();
          G4Material*           Material = aTrack.GetMaterial();
          G4double RangeNow = G4EnergyLossTables::GetRange(Particle,Ekine,Material);
          temp = (RangeNow - pUserLimits->GetUserMinRange(aTrack));
          if (temp < 0.) return 0.;
          if (ProposedStep > temp) ProposedStep = temp;
          //min kinetic energy (only for charged particle)
          G4double Emin = pUserLimits->GetUserMinEkine(aTrack);

	  //          G4cout<<"emin="<<Emin<<" ekine="<<Ekine<<G4endl;
	  // // gab:
	  // if(Ekine<Emin)return 0.;

          G4double Rmin = G4EnergyLossTables::GetRange(Particle,Emin,Material);
          temp = RangeNow - Rmin;
          if (temp < 0.) return 0.;
          if (ProposedStep > temp) ProposedStep = temp;
	  }         
     }   
   return ProposedStep;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VParticleChange* BDSUserSpecialCuts::PostStepDoIt(
			     const G4Track& aTrack,
			     const G4Step& 
			    )
//
// Kill the current particle, if requested by BDSUserLimits 
// 			    			    			    
{
   aParticleChange.Initialize(aTrack);

#if G4VERSION_NUMBER > 699
   aParticleChange.ProposeEnergy(0.) ;
   aParticleChange.ProposeLocalEnergyDeposit (aTrack.GetKineticEnergy()) ;
#else
   aParticleChange.SetEnergyChange(0.) ;
   aParticleChange.SetLocalEnergyDeposit (aTrack.GetKineticEnergy()) ;
#endif

   // gab: killing now would remove positron rest mass from the total energy...
   //   aParticleChange.SetStatusChange(fStopAndKill);
   return &aParticleChange;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
