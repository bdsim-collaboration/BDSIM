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
#ifndef BDSBUNCHFILEBASED_H
#define BDSBUNCHFILEBASED_H 

#include "BDSBunch.hh"

#include "G4String.hh"

/**
 * @brief An intermediate layer for any bunch classes that are file based.
 *
 * This class is purely to hold a record of variables that will be passed
 * to the end of run action. Even for 'distributions' like bdsimsampler and
 * hepmc that don't really use the BDSBunch classes fully, they can publish
 * information in the BDSBunchFileBased instance they get.
 * 
 * @author Laurie Nevay
 */

class BDSBunchFileBased: public BDSBunch
{
public: 
  explicit BDSBunchFileBased(const G4String& distributionName);
  virtual ~BDSBunchFileBased(); 
  /// @{ Assignment and copy constructor not implemented nor used
  BDSBunchFileBased& operator=(const BDSBunchFileBased&) = delete;
  BDSBunchFileBased(BDSBunchFileBased&) = delete;
  /// @}

  /// @{ Accessor.
  unsigned long long int NEventsInFile() const {return nEventsInFile;}
  unsigned long long int NEventsInFileSkipped() const {return nEventsInFileSkipped;}
  /// @}

  void SetNEventsInFile(unsigned long long int nEventsInFileIn) {nEventsInFile = nEventsInFileIn;}

  void IncrementNEventsInFileSkipped() {nEventsInFileSkipped += 1;}
  void IncrementNEventsInFileSkipped(unsigned long long int plus) {nEventsInFileSkipped += plus;}
  
protected:
  unsigned long long int nEventsInFile;
  unsigned long long int nEventsInFileSkipped;
};

#endif
