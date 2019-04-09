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

#include "BDSScorerTimeFilter.hh"
#include "G4Step.hh"
#include "G4UnitsTable.hh"


BDSScorerTimeFilter::BDSScorerTimeFilter(G4String name,
						 G4double tlow,
						 G4double thigh)
  :G4VSDFilter(name),fLowTime(tlow),fHighTime(thigh)
{;}

BDSScorerTimeFilter::~BDSScorerTimeFilter()
{;}

G4bool BDSScorerTimeFilter::Accept(const G4Step* aStep) const
{
  G4double time = aStep->GetTrack()->GetGlobalTime();
  if ( time < fLowTime  ) return FALSE;
  if ( time >= fHighTime ) return FALSE;
  return TRUE;
}