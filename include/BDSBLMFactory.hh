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
#ifndef BDSBLMFACTORY_H
#define BDSBLMFACTORY_H

#include "globals.hh"           // geant4 globals / types

class BDSBLM;
class BDSExtent;
class G4VSensitiveDetector;

/**
 * @brief Factory for building BLMs. 
 * 
 * @author Laurie Nevay
 */

class BDSBLMFactory
{
public:
  BDSBLMFactory();
  ~BDSBLMFactory();

  BDSBLM* BuildBLM(G4String name,
		   G4String geometryFile,
		   G4String geometryType,
		   G4String material,
		   G4double blm1,
		   G4double blm2,
		   G4double blm3,
		   G4double blm4,
		   G4VSensitiveDetector* sd);

private:

  BDSBLM* BuildBLMCylinder(G4String name,
			   G4String material,
			   G4double halfLength,
			   G4double radius);

  BDSBLM* BuildBLMCube(G4String name,
		       G4String material,
		       G4double halfLengthX,
		       G4double halfLengthY,
		       G4double halfLengthZ);

  BDSBLM* BuildBLMSphere(G4String name,
			 G4String material,
			 G4double radius);

  BDSBLM* CommonConstruction(G4String  name,
			     G4String  material,
			     G4VSolid* shape,
			     BDSExtent extent);
};



#endif
