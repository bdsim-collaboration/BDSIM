/* BDSIM code.    Version 1.0
   Author: Grahame A. Blair, Royal Holloway, Univ. of London.
   Last modified 24.7.2002
   Copyright (c) 2002 by G.A.Blair.  ALL RIGHTS RESERVED. 
*/
#ifndef BDSLaserCompton_h
#define BDSLaserCompton_h 1

#include "G4ios.hh" 
#include "globals.hh"
#include "Randomize.hh" 
#include "G4Version.hh"
#if G4VERSION_NUMBER > 899
#include "G4VDiscreteProcess.hh"
#else
#include "G4VeEnergyLoss.hh"
#endif
#include "G4VPhysicsConstructor.hh"
#include "G4Track.hh"
#include "G4Step.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4OrderedTable.hh" 
#include "G4PhysicsTable.hh"
#include "G4PhysicsLogVector.hh"
#include "BDSComptonEngine.hh"
#include "BDSMaterials.hh"


// flag initiated in BDSEventAction
extern G4bool FireLaserCompton;

#if G4VERSION_NUMBER > 899
class BDSLaserCompton : public G4VDiscreteProcess
#else
class BDSLaserCompton : public G4VeEnergyLoss
#endif
{ 
  public:
   
  BDSLaserCompton(const G4String& processName = "eLaser");
  
  ~BDSLaserCompton();

#if G4VERSION > 8  
//  virtual void PrintInfo();
#endif

  G4bool IsApplicable(const G4ParticleDefinition&);
  
  G4double GetMeanFreePath(const G4Track& track,
			   G4double previousStepSize,
			   G4ForceCondition* condition );
  
  G4VParticleChange *PostStepDoIt(const G4Track& track,         
				  const G4Step&  step);                 
  
  inline void SetLaserDirection(G4ThreeVector aDirection);
  inline G4ThreeVector GetLaserDirection();
  
  inline void SetLaserWavelength(G4double aWavelength);
  inline G4double GetLaserWavelength();
  
protected:
  
  G4double ComputeMeanFreePath( const G4ParticleDefinition* ParticleType,
				G4double KineticEnergy, 
				const G4Material* aMaterial);

  //  virtual G4double SecondaryEnergyThreshold(size_t index);

protected:
  //#if G4VERSION_NUMBER > 899
  //  G4bool isInitialised;  
  //  const G4ParticleDefinition* particle;

//  virtual void InitialiseEnergyLossProcess(const G4ParticleDefinition*, const G4ParticleDefinition*);
//#endif
  
private:
  
  BDSLaserCompton & operator=(const BDSLaserCompton &right);
  
  BDSLaserCompton(const BDSLaserCompton&);

  //  const std::vector<G4double>* secondaryEnergyCuts;
  
private:
  G4double itsLaserWavelength;
  G4ThreeVector itsLaserDirection;
  G4double itsLaserEnergy;
  BDSComptonEngine* itsComptonEngine;
  //  G4Material* itsLastMaterial;

};
inline G4bool BDSLaserCompton::IsApplicable(
					    const G4ParticleDefinition& particle)
{
  return(  (&particle == G4Electron::Electron())
	   ||(&particle == G4Positron::Positron()) );
}

inline G4double BDSLaserCompton::GetMeanFreePath(const G4Track& track,
						 G4double /*PreviousStepSize*/,
						 G4ForceCondition* ForceCondition)
{
  if( track.GetMaterial()==BDSMaterials::Instance()->GetMaterial("LaserVac") &&
      FireLaserCompton ) {
    *ForceCondition=Forced;
  }
  return DBL_MAX;
}


inline void BDSLaserCompton::SetLaserDirection(G4ThreeVector aDirection)
{itsLaserDirection=aDirection;}

inline G4ThreeVector BDSLaserCompton::GetLaserDirection()
{return itsLaserDirection;}

inline void BDSLaserCompton::SetLaserWavelength(G4double aWavelength)
{itsLaserWavelength=aWavelength;}

inline G4double BDSLaserCompton::GetLaserWavelength()
{return itsLaserWavelength;}

// inline G4double BDSLaserCompton::SecondaryEnergyThreshold(size_t index)
// {
//   return (*secondaryEnergyCuts)[index];
// }

#endif
