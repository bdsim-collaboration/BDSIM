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

class BDSBeamline;
class BDSLinkRegistry;
class BDSParticleDefinition;
class G4Box;
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

private:
  G4Box* worldSolid;
  BDSExtent worldExtent;
  BDSBeamline* linkBeamline;
  BDSLinkRegistry* linkRegistry;

  /// Particle definition all components are built w.r.t. Includes rigidity etc.
  const BDSParticleDefinition* designParticle;

  /// Position of the centre of the collimator entrances in global coordinates.
  std::vector<G4Transform3D> collimatorTransforms;
};

#endif
