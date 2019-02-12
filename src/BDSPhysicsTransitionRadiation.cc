/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
#include "G4ForwardXrayTR.hh"
#include "G4Gamma.hh"
#include "G4LeptonConstructor.hh"
#include "G4OpticalPhoton.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicsListHelper.hh"
#include "G4TransitionRadiation.hh"
#include "G4Version.hh"
#include "G4VTransitionRadiation.hh"


BDSPhysicsTransitionRadiation::BDSPhysicsTransitionRadiation():
  G4VPhysicsConstructor("BDSPhysicsTransitionRadiation")
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
  
  // G4VTransitionRadiation* transitionRadiation = new G4VTransitionRadiation();
  // G4VTransitionRadiation* transitionRadiation = new G4VTransitionRadiation();
  G4ForwardXrayTR* transitionRadiation = new G4ForwardXrayTR();
  G4AutoDelete::Register(transitionRadiation);
  
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

