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

#ifndef BDSSCORERTIMEFILTER_H
#define BDSSCORERTIMEFILTER_H 1

#include "globals.hh"
#include "G4VSDFilter.hh"

class BDSScorerTimeFilter : public G4VSDFilter
{

//-------
  public: // with description
      BDSScorerTimeFilter(G4String name,
			      G4double tlow=0.0,
			      G4double thigh=1e8);


     virtual ~BDSScorerTimeFilter();

  public: // with description
     virtual G4bool Accept(const G4Step*) const;

  private:
     G4double fLowTime;
     G4double fHighTime;
};

#endif
