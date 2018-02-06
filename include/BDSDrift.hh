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
#ifndef BDSDRIFT_H
#define BDSDRIFT_H

#include "globals.hh"

#include "BDSAcceleratorComponent.hh"

class BDSBeamPipeInfo;

/**
 * @brief A piece of vacuum beam pipe.
 * 
 * A simple class that represents a section of beam pipe or drift
 * in an accelerator. This simply uses the BDSBeamPipeFactory to 
 * construct the piece of beam pipe based on the input parameters.
 *
 * @author Laurie Nevay
 */

class BDSDrift: public BDSAcceleratorComponent
{
public:
  BDSDrift(G4String         name, 
	   G4double         length,
	   BDSBeamPipeInfo* beamPipeInfo);
  virtual ~BDSDrift();

protected:
  void Build();

private:
  /// Void function to fulfill BDSAcceleratorComponent requirements.
  void BuildContainerLogicalVolume(){;}; 
};

#endif
