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
#ifndef BDSSCORERQUANTITY3D_H
#define BDSSCORERQUANTITY3D_H

#include "globals.hh"
#include "G4THitsMap.hh"
#include "G4VPrimitiveScorer.hh"

class BDSHistBinMapper3D;
class G4PhysicsVector;

/**
 * @brief Primitive scorer for a 3D mesh with a conversion factor.
 * 
 * @author Robin Tesse
 */

class BDSScorerQuantity3D: public G4VPrimitiveScorer
{
public:

    BDSScorerQuantity3D(const G4String            scorer_name,
                        const BDSHistBinMapper3D* mapperIn,
                        G4int ni=1,G4int nj=1, G4int nk=1,
                        G4int depi=2, G4int depj=1, G4int depk=0);

    BDSScorerQuantity3D(const G4String            scorer_name,
		      const BDSHistBinMapper3D* mapperIn,
		      const G4String           filename,
		      G4int ni=1,G4int nj=1, G4int nk=1,
		      G4int depi=2, G4int depj=1, G4int depk=0);
  
    virtual ~BDSScorerQuantity3D() override;
  
public:
  void Initialize(G4HCofThisEvent* HCE) override;
  void EndOfEvent(G4HCofThisEvent* HCE) override;
  void clear() override;
  G4bool ProcessHits(G4Step* aStep, G4TouchableHistory*) override;
  G4int  GetIndex(G4Step* aStep) override;

  virtual G4double GetConversionFactor(G4int    particleID,
				       G4double energy) const;
  
private:
  G4int                 HCID3D;   ///< Collection ID.
  G4THitsMap<G4double>* EvtMap3D; ///< Hits map.

  /// @{ Depth in replica to look for each dimension.
  G4int fDepthi;
  G4int fDepthj;
  G4int fDepthk;
  /// @}

  /// Conversion factor interpolator object.
  G4PhysicsVector* conversionFactor;

  /// Mapping from coordinate systems in mesh to global replica number.
  const BDSHistBinMapper3D* mapper; ///< We don't own this.

  std::map<G4VSolid*, G4double> volumeCache;
};

#endif
