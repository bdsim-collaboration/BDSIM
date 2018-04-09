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
#ifndef BDSOUTPUTROOTEVENTSAMPLER_H
#define BDSOUTPUTROOTEVENTSAMPLER_H

#include "TROOT.h"
#include <vector>

class BDSOutputROOTGeant4Data;

#ifndef __ROOTBUILD__ 
#include "globals.hh"
class BDSSamplerHit;
#endif

#include "TObject.h"
#include "TTree.h"

/**
 * @brief Information stored per sampler per event.
 * 
 * @author Stewart Boogert
 */

template<class U> class BDSOutputROOTEventSampler: public TObject
{
public:
  std::string samplerName; //|| Don't split the header
  
  int                n;
  std::vector<U>     energy;
  std::vector<U>     x;
  std::vector<U>     y;
  U                  z;
  std::vector<U>     xp;
  std::vector<U>     yp;
  std::vector<U>     zp;
  std::vector<U>     T;

  std::vector<U>     weight;
  std::vector<int>   partID;
  std::vector<int>   parentID;
  std::vector<int>   trackID;
  int                modelID;
  std::vector<int>   turnNumber;

  U                  S;   // Will not need this when have global transforms

  /// @{ These are not filled by default.
  std::vector<int>     charge;
  std::vector<U>       mass;
  std::vector<U>       rigidity;
  std::vector<bool>    isIon;
  std::vector<int>     ionA;
  std::vector<int>     ionZ;
  /// @}

  /// @{ Function to calculate on the fly the parameters.
  std::vector<int>     getCharge();
  std::vector<U>       getMass();
  std::vector<U>       getRigidity();
  std::vector<bool>    getIsIon();
  std::vector<int>     getIonA();
  std::vector<int>     getIonZ();
  /// @}
  
  
  BDSOutputROOTEventSampler();
  explicit BDSOutputROOTEventSampler(std::string samplerNameIn);
  virtual ~BDSOutputROOTEventSampler();
#ifndef __ROOTBUILD__
  void Fill(G4double E,
            G4double x0, G4double y0, G4double z0,
            G4double xp, G4double yp, G4double zp,
            G4double t,
            G4double weight,
	    G4int    PDGType,
	    G4int    nEvent,
	    G4int    TurnsTaken,
            G4int beamlineIndex);
  void Fill(const BDSSamplerHit* hit);
#endif

  /// @{ Calculate and fill calculated variables.
  inline void FillCharge()   {charge   = getCharge();}
  inline void FillMass()     {mass     = getMass();}
  inline void FillRigidity() {rigidity = getRigidity();}
  inline void FillIon()      {isIon = getIsIon(); ionA = getIonA(); ionZ = getIonZ();}
  /// @}
  
  void FillCMR();  ///< Calculate and fill charge, mass and rigidity.
  void FillCMRI(); ///< Calculate and fill charge, mass, rigidity and ion properties.
  
  void SetBranchAddress(TTree *);
  void Flush();  ///< Clean Sampler

  static BDSOutputROOTGeant4Data* particleTable;

  ClassDef(BDSOutputROOTEventSampler,2);
};

#endif
