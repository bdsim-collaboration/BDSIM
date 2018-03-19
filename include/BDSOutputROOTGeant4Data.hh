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
#ifndef BDSOUTPUTROOTGEANT4DATA_H
#define BDSOUTPUTROOTGEANT4DATA_H

#include "BDSOutputROOTParticleInfo.hh"
#include "BDSOutputROOTParticleInfoIon.hh"

#include "TObject.h"
#include "TROOT.h"

#ifndef __ROOTBUILD__
#include "globals.hh"
#endif

#include <map>

class BDSOutputROOTGeant4Data: public TObject
{
public:
  BDSOutputROOTGeant4Data();
  virtual ~BDSOutputROOTGeant4Data(){;}

  /// Clear maps.
  virtual void Flush();
  
#ifndef __ROOTBUILD__
  /// Fill maps of particle information from Geant4.
  void Fill(const G4bool& fillIons);
#endif

  std::map<int, BDSOutputROOTParticleInfo>    particles;
  std::map<int, BDSOutputROOTParticleInfoIon> ions;

  ClassDef(BDSOutputROOTGeant4Data,1);
};

#endif
