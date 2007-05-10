/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 1.11.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
//
// ********************************************************************
// * DISCLAIMER                                                       *
// *                                                                  *
// * The following disclaimer summarizes all the specific disclaimers *
// * of contributors to this software. The specific disclaimers,which *
// * govern, are listed with their locations in:                      *
// *   http://cern.ch/geant4/license                                  *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.                                                             *
// *                                                                  *
// * This  code  implementation is the  intellectual property  of the *
// * GEANT4 collaboration.                                            *
// * By copying,  distributing  or modifying the Program (or any work *
// * based  on  the Program)  you indicate  your  acceptance of  this *
// * statement, and all its terms.                                    *
// ********************************************************************
//
//
// $Id: BDSGammaConversion.hh,v 1.1 2005/01/22 17:05:30 agapov Exp $
// GEANT4 tag $Name:  $
//
//------------------ BDSGammaConversion physics process -------------------------
//                   by Michel Maire, 24 May 1996
//
// 11-06-96, Added GetRandomAtom() method and new data member
//           for cumulative total cross section, by M.Maire
// 21-06-96, SetCuts inplementation, M.Maire
// 16-09-96, Dynamical array PartialSumSigma, M.Maire
// 14-01-97, crossection table + meanfreepath table.
//           PartialSumSigma removed, M.Maire
// 14-03-97, new physics scheme for geant4alpha, M.Maire
// 13-08-98, new methods SetBining() PrintInfo()
// 03-08-01, new methods Store/Retrieve PhysicsTable (mma)
// 06-08-01, BuildThePhysicsTable() called from constructor (mma)
// 19-09-01, come back to previous ProcessName: "conv"
// 20-09-01, DoIt: fminimalEnergy = 1*eV (mma) 
// 01-10-01, come back to BuildPhysicsTable(const G4ParticleDefinition&)    
// -----------------------------------------------------------------------------

// class description
//
// gamma ---> e+ e- 
// inherit from G4VDiscreteProcess
//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#ifndef BDSGammaConversion_h
#define BDSGammaConversion_h 1

#include "BDSGlobalConstants.hh"

#include "G4ios.hh" 
#include "globals.hh"
#include "Randomize.hh" 
#include "G4VDiscreteProcess.hh"
#include "G4PhysicsTable.hh"
#include "G4PhysicsLogVector.hh"
#include "G4Element.hh"
#include "G4Gamma.hh" 
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Step.hh"

#include "BDSConvParticleChange.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
class BDSGammaConversion : public G4VDiscreteProcess
 
{  
public:  // with description
 
  BDSGammaConversion(const G4String& processName ="BDSconv");
  
  ~BDSGammaConversion();
  
  G4bool IsApplicable(const G4ParticleDefinition&);
  // true for Gamma only.
  
  void SetPhysicsTableBining(G4double lowE, G4double highE, G4int nBins);
  // Allows to define the binning of the PhysicsTables, 
  // before to build them.
  
  void BuildPhysicsTable(const G4ParticleDefinition&);
  // It builds the total CrossSectionPerAtom table, for Gamma,
  // and for every element contained in the elementTable.
  // It builds the MeanFreePath table, for Gamma,
  // and for every material contained in the materialTable.       
  
  virtual G4bool StorePhysicsTable(const G4ParticleDefinition* ,
			   const G4String& directory, G4bool);
  // store CrossSection and MeanFreePath tables into an external file
  // specified by 'directory' (must exist before invokation)
  
  G4bool RetrievePhysicsTable(const G4ParticleDefinition* ,
			      const G4String& directory, G4bool);
  // retrieve CrossSection and MeanFreePath tables from an external file
  // specified by 'directory' 
  
  void PrintInfoDefinition();
  // Print few lines of informations about the process: validity range,
  // origine ..etc..
  // Invoked by BuildThePhysicsTable(). 
  
  G4double GetMeanFreePath(const G4Track& aTrack,
			   G4double previousStepSize,
			   G4ForceCondition* condition);
  // It returns the MeanFreePath of the process for the current track :
  // (energy, material)
  // The previousStepSize and G4ForceCondition* are not used.
  // This function overloads a virtual function of the base class.	
  // It is invoked by the ProcessManager of the Particle.
  
  G4double GetCrossSectionPerAtom(const G4DynamicParticle* aDynamicGamma,
				  G4Element*         anElement);
  // It returns the total CrossSectionPerAtom of the process, 
  // for the current DynamicGamma (energy), in anElement.	
  
  G4VParticleChange* PostStepDoIt(const G4Track& aTrack,
				  const G4Step& aStep);
  // It computes the final state of the process (at end of step),
  // returned as a ParticleChange object.			    
  // This function overloads a virtual function of the base class.
  // It is invoked by the ProcessManager of the Particle.
  
    
    protected:
  
  virtual G4double ComputeCrossSectionPerAtom(G4double GammaEnergy, 
					      G4double AtomicNumber);
  
  virtual G4double ComputeMeanFreePath (G4double GammaEnergy, 
					G4Material* aMaterial);
  
  virtual G4double ComputeMuonMeanFreePath(G4double GammaEnergy, 
					   G4Material* aMaterial);

  G4double GetMuonConversionCrossSection();
  G4double ComputeMuonCrossSectionPerAtom(
					  G4double Egam, 
					  G4double Z, G4double A);

private:
  
  G4Element* SelectRandomAtom(const G4DynamicParticle* aDynamicGamma,
			      G4Material* aMaterial);
  
  G4double ScreenFunction1(G4double ScreenVariable);
  
  G4double ScreenFunction2(G4double ScreenVariable);
  
private:
  
     // hide assignment operator as private 
  BDSGammaConversion& operator=(const BDSGammaConversion &right);
  BDSGammaConversion(const BDSGammaConversion& );
  
private:
  
  G4PhysicsTable* theCrossSectionTable;    // table for crossection
  G4PhysicsTable* theMeanFreePathTable;
  
  G4double LowestEnergyLimit ;     // low  energy limit of the tables
  G4double HighestEnergyLimit ;    // high energy limit of the tables 
  G4int NumbBinTable ;             // number of bins in the tables
  
  G4double fminimalEnergy;         // minimalEnergy of produced particles
  
  G4double MeanFreePath;           // actual MeanFreePath (current medium)
  G4double muonWeight;
  

  BDSConvParticleChange fParticleChange;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "BDSGammaConversion.icc"
  
#endif
 
