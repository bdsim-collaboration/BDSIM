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
#ifndef BDSCOMPONENTFACTORYUSER_H
#define BDSCOMPONENTFACTORYUSER_H

#include "BDSFieldType.hh"
#include "BDSMagnetStrength.hh"
#include "BDSMagnetType.hh"
#include "BDSIntegratorSetType.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4Transform3D.hh"

#include "CLHEP/Units/PhysicalConstants.h"

#include <map>

class G4Colour;
class G4Material;

namespace GMAD
{
  struct Element;
}
class BDSAcceleratorComponent;
class BDSBeamPipeInfo;
class BDSCavityInfo;
class BDSComponentConstructor;
class BDSCrystalInfo;
class BDSFieldInfo;
class BDSIntegratorSet;
class BDSMagnet;
class BDSMagnetOuterInfo;
class BDSTiltOffset;

/**
 * @brief Factory for user specified accelerator components.
 * 
 * @author Laurie Nevay
 */

class BDSComponentFactoryUser
{
public:
  BDSComponentFactoryUser();
  ~BDSComponentFactoryUser(); 

  /// Register a constructor instance by a given name.
  void RegisterComponent(G4String componentTypeName,
			 BDSComponentConstructor* componentConstructor);

  /// Check whether a component can be constructed - ie if the name exists.
  G4bool CanConstructComponentByName(G4String componentTypeName) const;
  
  /// Find and call the constructor. This assumes the name definitely exists
  /// otherwise an STL access exception will be thrown.
  BDSAcceleratorComponent* ConstructComponent(G4String componentTypeName,
					      GMAD::Element const* elementIn,
					      GMAD::Element const* prevElementIn,
					      GMAD::Element const* nextElementIn,
					      G4double currentArcLength = 0);


  /// Update values for nominal rigidity and relativisitic beta of the beam particle.
  void SetRigidityAndLorentzBeta(const G4double brhoIn,
				 const G4double beta0In);
  
  
private:
  /// Map of user component name with constructors to build a component.
  std::map<G4String, BDSComponentConstructor*> userFunctions;
  G4double brho;  ///< Cache of nominal beam rigidity.
  G4double beta0; ///< Cache of nominal relativistic beta for the beam particle.
};
#endif
