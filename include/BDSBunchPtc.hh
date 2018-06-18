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
#ifndef BDSBUNCHPTC_H
#define BDSBUNCHPTC_H

#include "BDSBunch.hh"

#include "globals.hh"

#include <vector>

/**
 * @brief A bunch distribution that reads a PTC inrays file.
 * 
 * @author Stewart Boogert
 */

class BDSBunchPtc: public BDSBunch
{  
public: 
  BDSBunchPtc();
  virtual ~BDSBunchPtc(); 
  virtual void SetOptions(const BDSParticleDefinition* beamParticle,
			  const GMAD::Beam& beam,
			  const BDSBunchType& distrType,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity);
  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
                               G4double& xp, G4double& yp, G4double& zp,
                               G4double& t , G4double&  E, G4double& weight);
  
private:
  /// Load the PTC file into memory
  void LoadPtcFile();

  /// Assign the distribution file by finding the full path of it.
  void SetDistrFile(G4String distrFileNameIn);
  
  G4int    nRays;     ///< Number of rays in file (1 counting).
  G4String fileName;  ///< File name.
  G4int    iRay;      ///< Iterator counter for current ray.
  std::vector<double*> ptcData; ///< Data.

  G4bool loopedOver;  ///< Whether we've reset to loop over the file again.
};

#endif
