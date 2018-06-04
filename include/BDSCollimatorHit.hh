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
#ifndef BDSCOLLIMATORHIT_H
#define BDSCOLLIMATORHIT_H

#include "G4VHit.hh"

class BDSCollimatorHit: public G4VHit
{
public:
  BDSCollimatorHit();

  virtual ~BDSCollimatorHit();

  inline void* operator new(size_t) ;
  inline void operator delete(void *aHit);

private:

  G4bool interacted;
  G4bool primaryStopped;

  G4double xIn;
  G4double yIn;
  G4double zIn;
  G4double SIn;
  
  G4double energyDeposit;
  G4double partID;
  G4dobule weight;
  G4double stepLength;

  G4int collimatorIndex;


};

#endif
