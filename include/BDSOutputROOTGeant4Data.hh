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

  int         Charge(const int& pdgID) const;
  double      Mass(const int& pdgID) const;
  double      Rigidity(const int& pdgID) const;
  std::string Name(const int& pdgID) const;
  int         IonA(const int& pdgID) const;
  int         IonZ(const int& pdgID) const;

  inline bool IsIon(const int& pdgID) const {return pdgID > 1000000000;}
  
#ifndef __ROOTBUILD__
  /// Fill maps of particle information from Geant4.
  void Fill(const G4bool& fillIons);
#endif

  struct ParticleInfo
  {
    std::string name;   ///< Name of particle.
    int         charge; ///< Charge in units of e.
    double      mass;   ///< Mass in GeV.
  };

  struct IonInfo
  {
    std::string name;   ///< Name of particle.
    int         charge; ///< Charge in units of e.
    double      mass;   ///< Mass in GeV.
    int a; ///< Mass number - number of nucleons in nucleus.
    int z; ///< Atomic number - number of protons in nucleus.

  };

  std::map<int, ParticleInfo> particles;
  std::map<int, IonInfo>      ions;

  ClassDef(BDSOutputROOTGeant4Data,1);
};

#endif
