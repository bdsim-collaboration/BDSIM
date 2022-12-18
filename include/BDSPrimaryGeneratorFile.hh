/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

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
#ifndef BDSPRIMARYGENERATORFILE_H
#define BDSPRIMARYGENERATORFILE_H

#include "G4Types.hh"
#include "G4VPrimaryGenerator.hh"

class BDSBunch;
class G4Event;

namespace GMAD
{
  class Beam;
}

/**
 * @brief Common interface for any primary generators that are file based.
 *
 * Any derived class must implement G4VPrimaryGeneratorAction::GeneratePrimaryVertex(event).
 * In this, it must update the protected members in this class of endOfFileReached and
 * vertexGeneratedSuccessfully.
 *
 * @author Laurie Nevay
 */

class BDSPrimaryGeneratorFile: public G4VPrimaryGenerator
{
public:
  BDSPrimaryGeneratorFile();
  virtual ~BDSPrimaryGeneratorFile();
  
  /// Unbound function to construct the right generator from file. Can return nullptr
  /// if no generator is required according to the bunch distribution type.
  static BDSPrimaryGeneratorFile* ConstructGenerator(const GMAD::Beam& beam,
                                                     BDSBunch* bunchIn,
                                                     G4bool recreate,
                                                     G4int eventOffset);

  /// Return false if not able to generate a primary vertex.
  G4bool GeneratePrimaryVertexSafe(G4Event* event);
  
  virtual void RecreateAdvanceToEvent(G4int eventOffset) = 0;
  
  /// Return the number of events in the file - not necessarily the number that
  /// match the filters but that are there in total.
  G4long NEventsInFile() const {return nEventsInFile;}
  
  /// Return number of available events (excluding any filters in derived classes) in the
  /// file. This is nominally nEventsInFile - currentFileEventIndex.
  G4long NEventsLeftInFile() const;
  
  /// Report whether the distribution is finished generating.
  G4bool DistributionIsFinished() const {return endOfFileReached;}

protected:
  G4bool endOfFileReached;
  G4bool matchFileLength;
  G4bool loopFile;
  G4bool vertexGeneratedSuccessfully;
  G4long currentFileEventIndex;
  G4long nEventsInFile;
  G4long nEventsReadThatPassedFilters;
};

#endif
