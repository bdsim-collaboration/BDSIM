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
#ifndef BDSPARALLELWORLDSAMPER_H
#define BDSPARALLELWORLDSAMPER_H

#include "BDSApertureType.hh"
#include "BDSSamplerType.hh"

#include "G4String.hh"
#include "G4VUserParallelWorld.hh"

#include <vector>

class G4VisAttributes;
class G4VPhysicalVolume;
class BDSSampler;

/**
 * @brief A parallel world for sampler planes.
 *
 * @author Laurie Nevay
 */

class BDSParallelWorldSampler: public G4VUserParallelWorld
{
public:
  explicit BDSParallelWorldSampler(G4String name);
  virtual ~BDSParallelWorldSampler();

  /// Construct the required parallel world geometry. This must
  /// overload the pure virtual method in G4VUserParallelWorld.
  virtual void Construct();

private:
  /// No default constructor.
  BDSParallelWorldSampler() = delete;

  /// Construct required sampler if required, else return plane one. Raiuds of sampler
  /// is provided in case of constructing a general sampler.
  BDSSampler* BuildSampler(BDSSamplerType samplerType,
			   G4double       samplerRadius) const;

  BDSSampler* BuildSampler(BDSApertureType samplerShape,
			   G4double aper1,
			   G4double aper2,
			   G4double aper3,
			   G4double aper4) const;
  
  /// Cache of the placements to clean up at the end.
  std::vector<G4VPhysicalVolume*> placements;

  /// Cache of samplers to delete at the end
  std::vector<BDSSampler*> samplers;

  /// Just the input part of the world name.
  G4String suffix;

  /// Visualisation attributes for the sampler world.
  G4VisAttributes* samplerWorldVis;

  /// General single sampler we use for plane samplers.
  BDSSampler* generalPlane;
};

#endif
