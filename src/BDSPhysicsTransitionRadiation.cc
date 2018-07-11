/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "BDSPhysicsTransitionRadiation.hh"

#include "globals.hh" // geant4 types / globals
#include "G4AutoDelete.hh"
#include "G4TransitionRadiation.hh"
#include "G4Gamma.hh"
#include "G4LeptonConstructor.hh"
#include "G4OpticalPhoton.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicsListHelper.hh"
#include "G4Version.hh"

BDSPhysicsTransitionRadiation::BDSPhysicsTransitionRadiation(G4double maxPhotonsPerStepIn,
					 G4double maxBetaChangePerStepIn):
  G4VPhysicsConstructor("BDSPhysicsTransitionRadiation"),
  maxPhotonsPerStep(maxPhotonsPerStepIn),
  maxBetaChangePerStep(maxBetaChangePerStepIn)
{;}

BDSPhysicsTransitionRadiation::~BDSPhysicsTransitionRadiation()
{;}

void BDSPhysicsTransitionRadiation::ConstructParticle()
{
  G4LeptonConstructor leptons;
  leptons.ConstructParticle();

  G4Gamma::Gamma();
  G4OpticalPhoton::OpticalPhotonDefinition();
}

void BDSPhysicsTransitionRadiation::ConstructProcess()
{
  if (Activated())
    {return;}
  
  G4TransitionRadiation* transitionRadiation = new G4TransitionRadiation();
  G4AutoDelete::Register(transitionRadiation);

  // reduce memory profile
  transitionRadiation->SetTrackSecondariesFirst(true);

  // common settings (similar to optical physics)
  if (maxPhotonsPerStep > 0)
    {transitionRadiation->SetMaxNumPhotonsPerStep(maxPhotonsPerStep);}
  transitionRadiation->SetMaxBetaChangePerStep(maxBetaChangePerStep);

  G4PhysicsListHelper* ph = G4PhysicsListHelper::GetPhysicsListHelper();
    
#if G4VERSION_NUMBER > 1029
  auto aParticleIterator = GetParticleIterator();
#endif
  aParticleIterator->reset();
  while( (*aParticleIterator)() )
    {
      G4ParticleDefinition* particle = aParticleIterator->value();
      
      if (transitionRadiation->IsApplicable(*particle))
	{ph->RegisterProcess(transitionRadiation, particle);}
    }

  SetActivated();
}
