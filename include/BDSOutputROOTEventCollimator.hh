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
#include "BDSOutputROOTEventCollimatorInfo.hh"
#include "G4Types.hh"
class BDSCollimatorHit;
#endif

#include "TObject.h"

#include <set>
#include <utility>
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
  std::vector<int>    parentID;
  std::vector<int>    turn;
  /// A primary can generate multiple hits on a single passage through a collimator,
  /// but we're most interested in the first one where it impacted, so this is a flag
  /// for that hit. The same primary may hit on subsequent turns (however unlikely) in
  /// a ring so we must do this each turn.
  std::vector<bool>   firstPrimaryHitThisTurn;

  std::vector<double> impactParameterX;
  std::vector<double> impactParameterY;

  std::vector<bool>   isIon;
  std::vector<int>    ionA;
  std::vector<int>    ionZ;

  /// Different length set of turn number.
  std::set<int>       turnSet;

  /// @{ These are not filled by default.
  std::vector<int>    charge;
  std::vector<double> kineticEnergy;
  std::vector<double> mass;
  std::vector<double> rigidity;
  /// @}

  double totalEnergyDeposited; ///< Sum of energy deposits including weights.
  
  virtual ~BDSOutputROOTEventCollimator();
#ifndef __ROOTBUILD__
  void Fill(const BDSCollimatorHit* hit,
	    const BDSOutputROOTEventCollimatorInfo& info,
	    const std::pair<G4double, G4double>& differences);
  void FillExtras(G4bool fillIonInfo,
		  G4bool fillLinks);

  /// Setter for one off flag per event.
  inline void SetPrimaryStopped(G4bool primaryStoppedIn) {primaryStopped = primaryStoppedIn;}
  
#endif
  virtual void Flush();

  static BDSOutputROOTGeant4Data* particleTable;

  ClassDef(BDSOutputROOTEventCollimator, 1);
};


#endif
