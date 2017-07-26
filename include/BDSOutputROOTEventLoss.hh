#ifndef BDSOUTPUTROOTEVENTLOSS_H
#define BDSOUTPUTROOTEVENTLOSS_H

#ifndef __ROOTBUILD__
class BDSEnergyCounterHit;
class BDSTrajectoryPoint;
#endif

#include "TObject.h"

#include <vector>

/**
 * @brief Data stored for energy deposition hits per event.
 * 
 * @author Stewart Boogert
 */

class BDSOutputROOTEventLoss: public TObject
{
public:
  int                       n = 0;   ///< Number of entries
  std::vector<float>        energy;  ///< Energy deposited in step
  std::vector<float>        S;       ///< Global curvilinear S coordinate
  std::vector<float>        weight;  ///< Weight associated with loss
  // TODO : These need to be put in as options
  std::vector<int>          partID;  ///< ParticleID that create the deposit
  std::vector<int>          trackID; ///< TrackID that created the deposit
  std::vector<int>          parentID;///< ParentID that created the deposit
  //
  std::vector<int>          modelID; ///< Geometry model index
  std::vector<int>          turn;    ///< Turn number

  /// @{ Local coordinate
  std::vector<float>        x;
  std::vector<float>        y;
  std::vector<float>        z;
  /// @}
  
  //// @{ Global coordinates
  std::vector<float>        X;
  std::vector<float>        Y;
  std::vector<float>        Z;
  /// @}
  
  BDSOutputROOTEventLoss();
  BDSOutputROOTEventLoss(bool storeLinks, bool storeLocal, bool storeGobal);
  virtual ~BDSOutputROOTEventLoss();
#ifndef __ROOTBUILD__
  void Fill(const BDSTrajectoryPoint* hit);
  void Fill(const BDSEnergyCounterHit* hit);
#endif
  virtual void Flush();

  bool storeLinks  = false; // Store links between Eloss and model and trajectors
  bool storeLocal  = false; // Store local coordinates
  bool storeGlobal = false; // Store global coordinates
  
  ClassDef(BDSOutputROOTEventLoss,1);
};


#endif
