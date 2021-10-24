/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#include "BDSPhysicsRadioactivation.hh"

#include "globals.hh"
#include "G4DeexPrecoParameters.hh"
#include "G4GenericIon.hh"
#include "G4EmParameters.hh"
#include "G4LossTableManager.hh"
#include "G4NuclideTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4PhysicsListHelper.hh"
#include "G4Radioactivation.hh"
#include "G4UAtomicDeexcitation.hh"
#include "G4VAtomDeexcitation.hh"

BDSPhysicsRadioactivation::BDSPhysicsRadioactivation():
  G4VPhysicsConstructor("BDSPhysicsRadioactivation")
{
  ra = new G4Radioactivation();
}

BDSPhysicsRadioactivation::~BDSPhysicsRadioactivation()
{;}

void BDSPhysicsRadioactivation::ConstructParticle()
{ 
  G4GenericIon::GenericIon();
}

void BDSPhysicsRadioactivation::ConstructProcess()
{
  if (Activated())
    {return;}

  // atomic rearrangement
  G4bool ARMflag = false;
  ra->SetARM(ARMflag);

  // initialise atomic deexcitation
  G4LossTableManager* man = G4LossTableManager::Instance();
  G4VAtomDeexcitation* ad = man->AtomDeexcitation();
  if(!ad)
    {
      G4EmParameters::Instance()->SetAuger(true);
      ad = new G4UAtomicDeexcitation();
      ad->InitialiseAtomicDeexcitation();
      man->SetAtomDeexcitation(ad);
    }
  
  G4PhysicsListHelper::GetPhysicsListHelper()->RegisterProcess(ra, G4GenericIon::GenericIon());

  SetActivated();
}
