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
#ifndef BDSLINKEVENTACTION_H
#define BDSLINKEVENTACTION_H

#include "G4Types.hh"
#include "G4UserEventAction.hh"

class BDSOutput;
class G4Event;
class G4PrimaryVertex;

/**
 * @brief Process information at the event level for Link to trackers.
 * 
 * @author Laurie Nevay
 */

class BDSLinkEventAction: public G4UserEventAction
{
public:
  explicit BDSLinkEventAction(BDSOutput* outputIn);
  virtual ~BDSLinkEventAction();
  
  virtual void BeginOfEventAction(const G4Event*);
  virtual void EndOfEventAction(const G4Event*);

private:
  BDSOutput* output;         ///< Cache of output instance. Not owned by this class.
  G4bool verboseEventBDSIM;
  G4int  verboseEventStart;
  G4int  verboseEventStop;
  G4int  printModulo;

  G4int collIDSamplerLink;
  G4int currentEventIndex;
};

#endif

