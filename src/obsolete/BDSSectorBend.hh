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
#ifndef BDSSECTORBEND_H
#define BDSSECTORBEND_H 

#include "globals.hh"

#include "BDSMagnet.hh"

class  BDSBeamPipeInfo;
struct BDSMagnetOuterInfo;

class BDSSectorBend: public BDSMagnet
{
public:
  BDSSectorBend(G4String            nameIn,
		G4double            lengthIn,
		G4double            angleIn,
		G4double            bFieldIn,
		G4double            bGradIn,
		BDSBeamPipeInfo*    beamPipeInfo,
		BDSMagnetOuterInfo* magnetOuterInfo);
  ~BDSSectorBend(){;}

private:
  G4double itsBField;
  G4double itsBGrad;
  
  virtual void Build();
  virtual void BuildBPFieldAndStepper();

};

#endif
