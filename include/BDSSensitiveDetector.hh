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
#ifndef BDSSENSITIVEDETECTOR_H
#define BDSSENSITIVEDETECTOR_H

#include "G4VSensitiveDetector.hh"

#include "G4String.hh"

class G4VHit;

/**
 * @brief Virtual class to define interface for ordered multi-sensitive detector.
 * 
 * @author Laurie Nevay
 */

class BDSSensitiveDetector: public G4VSensitiveDetector
{
public:
  BDSSensitiveDetector(G4String name):
    G4VSensitiveDetector(name)
  {;}
  BDSSensitiveDetector() = delete;

  virtual ~BDSSensitiveDetector(){;}

  /// Derived class shoudl implement method to return pointer of base
  /// class type G4VHit to the last hit. Not called back to avoid confusion
  /// with the STL back() that would return a reference. We must return a
  /// pointer here.
  virtual G4VHit* last() const = 0;
};

#endif

