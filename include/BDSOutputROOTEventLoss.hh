#ifndef BDSOUTPUTROOTEVENTLOSS_H
#define BDSOUTPUTROOTEVENTLOSS_H

#ifndef __ROOTBUILD__
class BDSEnergyCounterHit;
class BDSTunnelHit;
class BDSTrajectoryPoint;
#endif

#include "TObject.h"

#include <vector>

class BDSOutputROOTEventLoss: public TObject
{
public:
  int                       n = 0;   ///< Number of entries
  std::vector<float>        energy;  ///< Energy deposited in step
  std::vector<float>        S;       ///< Global curvilinear S coordinate
  std::vector<float>        weight;  ///< Weight associated with loss
  std::vector<float>        trackID; ///< TrackID that created the deposit
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
  BDSOutputROOTEventLoss(bool storeLocal, bool storeGobal);
  virtual ~BDSOutputROOTEventLoss();
#ifndef __ROOTBUILD__
  void Fill(BDSTrajectoryPoint* hit);
  void Fill(BDSEnergyCounterHit* hit);
#endif
  virtual void Flush();

  bool storeLocal  = false;
  bool storeGlobal = false;
  
  ClassDef(BDSOutputROOTEventLoss,1);
};


#endif
