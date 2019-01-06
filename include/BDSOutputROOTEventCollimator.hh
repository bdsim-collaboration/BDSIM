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
#ifndef BDSOUTPUTROOTEVENTCOLLIMATOR_H
#define BDSOUTPUTROOTEVENTCOLLIMATOR_H

#ifndef __ROOTBUILD__
#include "G4Types.hh"
class BDSCollimatorHit;
#endif

#include "TObject.h"

#include <vector>

class BDSOutputROOTGeant4Data;

/**
 * @brief Data stored for each collimator per event.
 * 
 * @author Laurie Nevay
 */

class BDSOutputROOTEventCollimator: public TObject
{
public:
  /// Default constructor for ROOT
  BDSOutputROOTEventCollimator();  

  bool   primaryInteracted;
  bool   primaryStopped;

  int                 n;
  std::vector<double> totalEnergy;
  std::vector<double> energyDeposited;
  std::vector<double> xIn;
  std::vector<double> yIn;
  std::vector<double> zIn;
  std::vector<double> xpIn;
  std::vector<double> ypIn;
  std::vector<double> zpIn;
  std::vector<double> T;
  std::vector<double> weight;
  std::vector<int>    partID;
  std::vector<int>    turnNumber;

  std::vector<double> impactParameterX;
  std::vector<double> impactParameterY;

  std::vector<bool>   isIon;
  std::vector<int>    ionA;
  std::vector<int>    ionZ;

  /// @{ These are not filled by default.
  std::vector<int>    charge;
  std::vector<double> kineticEnergy;
  std::vector<double> mass;
  std::vector<double> rigidity;
  /// @}

  double totalEnergyDeposited; ///< Sum of energy deposits including weights.
  
  virtual ~BDSOutputROOTEventCollimator();
#ifndef __ROOTBUILD__
  void Fill(const BDSCollimatorHit* hit);
  void FillExtras(G4bool fillIonInfo,
		  G4bool fillAllExtras);
#endif
  virtual void Flush();

  static BDSOutputROOTGeant4Data* particleTable;

  ClassDef(BDSOutputROOTEventCollimator, 1);
};


#endif
