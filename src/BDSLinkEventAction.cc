/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#include "BDSAuxiliaryNavigator.hh"
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSHitSamplerLink.hh"
#include "BDSLinkEventAction.hh"
#include "BDSLinkRunAction.hh"
#include "BDSOutput.hh"
#include "BDSSDSamplerLink.hh"
#include "BDSSDManager.hh"
#include "BDSUtilities.hh"

#include "globals.hh"
#include "G4Event.hh"
#include "G4EventManager.hh"
#include "G4HCofThisEvent.hh"
#include "G4PropagatorInField.hh"
#include "G4Run.hh"
#include "G4SDManager.hh"
#include "G4TransportationManager.hh"

#include <algorithm>
#include <map>
#include <string>
#include <vector>

BDSLinkEventAction::BDSLinkEventAction(BDSOutput* outputIn,
				       BDSLinkRunAction* runActionIn):
  output(outputIn),
  runAction(runActionIn),
  collIDSamplerLink(-1),
  currentEventIndex(0)
{
  BDSGlobalConstants* globals = BDSGlobalConstants::Instance();
  verboseEventBDSIM         = globals->VerboseEventBDSIM();
  verboseEventStart         = globals->VerboseEventStart();
  verboseEventStop          = BDS::VerboseEventStop(verboseEventStart, globals->VerboseEventContinueFor());
  printModulo               = globals->PrintModuloEvents();
}

BDSLinkEventAction::~BDSLinkEventAction()
{;}

void BDSLinkEventAction::BeginOfEventAction(const G4Event* evt)
{
  currentEventIndex = evt->GetEventID();
  
  // reset navigators to ensure no mis-navigating and that events are truly independent
  BDSAuxiliaryNavigator::ResetNavigatorStates();
  // make further attempts to clear Geant4's tracking history between events to make them
  // truly independent.
  G4Navigator* trackingNavigator = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking();
  trackingNavigator->ResetStackAndState();
  G4TransportationManager* tm = G4TransportationManager::GetTransportationManager();
  int i = 0;
  for (auto it = tm->GetActiveNavigatorsIterator(); i < (int)tm->GetNoActiveNavigators(); it++)
    {(*it)->ResetStackAndState(); i++;}
  tm->GetPropagatorInField()->ClearPropagatorState(); // <- this one really makes a difference

  // number feedback
  if (currentEventIndex % printModulo == 0)
    {G4cout << "---> Begin of event: " << currentEventIndex << G4endl;}
  if (verboseEventBDSIM) // always print this out
    {G4cout << __METHOD_NAME__ << "event #" << currentEventIndex << G4endl;}

  // cache hit collection IDs for quicker access
  if (collIDSamplerLink < 0)
    {// if one is -1 then all need initialised.
      G4SDManager*  g4SDMan  = G4SDManager::GetSDMpointer();
      BDSSDManager* bdsSDMan = BDSSDManager::Instance();
      collIDSamplerLink = g4SDMan->GetCollectionID(bdsSDMan->SamplerLink()->GetName());
    }
}

void BDSLinkEventAction::EndOfEventAction(const G4Event* evt)
{
  // Get the hits collection of this event - all hits from different SDs.
  G4HCofThisEvent* HCE = evt->GetHCofThisEvent();
  
  typedef BDSHitsCollectionSamplerLink slhc;
  slhc* samplerLink = HCE ? dynamic_cast<slhc*>(HCE->GetHC(collIDSamplerLink)) : nullptr;

  if (!samplerLink)
    {return;}
  if (samplerLink->entries() <= 0)
    {return;}
  else
    {
      runAction->AppendHits(currentEventIndex, samplerLink);
    }

  /*
  output->FillEvent(eventInfo,
		    evt->GetPrimaryVertex(),
		    SampHC,
		    hitsCylinder,
		    eCounterHits,
		    eCounterFullHits,
		    eCounterVacuumHits,
		    eCounterTunnelHits,
		    eCounterWorldHits,
		    eCounterWorldContentsHits,
		    worldExitHits,
		    primaryHit,
		    primaryLoss,
		    interestingTrajectories,
		    collimatorHits,
		    apertureImpactHits,
		    BDSGlobalConstants::Instance()->TurnsTaken());
  */
}
