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
#include "BDSHitSamplerLink.hh"
#include "BDSLinkEventAction.hh"
#include "BDSLinkRunAction.hh"

BDSLinkRunAction::BDSLinkRunAction():
  allHits(nullptr)
{;}

BDSLinkRunAction::~BDSLinkRunAction()
{
  delete allHits;
}

void BDSLinkRunAction::BeginOfRunAction(const G4Run* /*aRun*/)
{
  nSecondariesToReturn = 0;
  nPrimariesToReturn   = 0;
  BDSAuxiliaryNavigator::ResetNavigatorStates();
  allHits = new BDSHitsCollectionSamplerLink();
  //output->InitialiseGeometryDependent();
  //output->NewFile();
  // Write options now file open.
  //const GMAD::OptionsBase* ob = BDSParser::Instance()->GetOptionsBase();
  //output->FillOptions(ob);
  // Write beam
  //const GMAD::BeamBase* bb = BDSParser::Instance()->GetBeamBase();
  //output->FillBeam(bb);
  // Write model now file open.
  //output->FillModel();
  // Write out geant4 data including particle tables.
  //output->FillGeant4Data(usingIons);
}

void BDSLinkRunAction::EndOfRunAction(const G4Run* /*aRun*/)
{;}

void BDSLinkRunAction::AppendHits(G4int currentEventIndex,
				  const BDSHitsCollectionSamplerLink* hits)
{
  if (!hits)
    {return;}
  for (G4int i = 0; i < (G4int)hits->entries(); i++)
    {
      auto hit = new BDSHitSamplerLink(*(*hits)[i]);
      hit->eventID = currentEventIndex;
      hit->parentID == 0 ? nPrimariesToReturn++ : nSecondariesToReturn++;
      allHits->insert(hit);
    }
}