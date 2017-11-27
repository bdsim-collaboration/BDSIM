/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#ifndef BDSBUNCHSIXTRACK_H
#define BDSBUNCHSIXTRACK_H

#include <vector>

#include "BDSBunch.hh"

/**
 * @brief A bunch distribution that reads a SixTrack hits file.
 * 
 * @author Regina Kwee-Hinzmann
 */

class BDSBunchSixTrack: public BDSBunch
{ 
private: 
  G4int    nPart;
  G4String fileName;

  G4int    iPart; // current ray
  std::vector<double*> sixtrackData; 
  
public: 
  BDSBunchSixTrack();
  explicit BDSBunchSixTrack(G4String fileNameIn);
  virtual ~BDSBunchSixTrack(); 
  void LoadSixTrackFile(); 
  virtual void SetOptions(const GMAD::Beam& beam,
			  G4Transform3D beamlineTransformIn = G4Transform3D::Identity);
  virtual void GetNextParticle(G4double& x0, G4double& y0, G4double& z0, 
                               G4double& xp, G4double& yp, G4double& zp,
                               G4double& t , G4double&  E, G4double& weight);
  inline void SetDistrFile(G4String distrFileNameIn) {fileName = distrFileNameIn;}
  G4String GetDistribFile() {return fileName;}
  G4int GetNParticles() {return nPart;}
};

#endif
