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
#ifndef BDSLINKDETECTORCONSTRUCTION_H
#define BDSLINKDETECTORCONSTRUCTION_H
#include "BDSExtent.hh"

#include "G4VUserDetectorConstruction.hh"
#include "G4ThreeVector.hh"
#include "G4Types.hh"

#include <string>

class BDSBeamline;
class BDSBeamlineElement;
class BDSLinkRegistry;
class BDSParticleDefinition;
class G4Box;
class G4ChannelingOptrMultiParticleChangeCrossSection;
class G4VPhysicalVolume;

class BDSLinkDetectorConstruction: public G4VUserDetectorConstruction
{
public:
  /// Default constructor
  BDSLinkDetectorConstruction(/* something. special GMAD syntax?*/);

  virtual ~BDSLinkDetectorConstruction();
  virtual G4VPhysicalVolume* Construct();

  BDSExtent WorldExtent() const {return worldExtent;}

  void AddLinkCollimator(const std::string& collimatorName,
			 const std::string& materialName,
			 G4double length,
			 G4double aperture,
			 G4double rotation,
			 G4double xOffset,
			 G4double yOffset);

  /// Set the design particle definition.
  inline void SetDesignParticle(const BDSParticleDefinition* defIn) {designParticle = defIn;}

  inline BDSLinkRegistry* LinkRegistry() const {return linkRegistry;}

  void BuildPhysicsBias();

 private:

  /// Create the worldSolid if it doesn't exist and if not expand it to the extent of the
  /// linkBeamline member. Update worldExtent member also.
  void UpdateWorldSolid();

  /// Place a beam line element in the world.
  void PlaceOneComponent(const BDSBeamlineElement* element);

  G4Box* worldSolid;
  G4VPhysicalVolume* worldPV;
  BDSExtent worldExtent;
  BDSBeamline* linkBeamline;
  BDSLinkRegistry* linkRegistry;

  /// Particle definition all components are built w.r.t. Includes rigidity etc.
  const BDSParticleDefinition* designParticle;

  G4ChannelingOptrMultiParticleChangeCrossSection* crystalBiasing;
};

#endif
