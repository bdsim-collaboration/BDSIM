#ifndef BDSOUTPUTROOTEVENTTRAJECTORY_H
#define BDSOUTPUTROOTEVENTTRAJECTORY_H

#include "TROOT.h"
#include "TVector3.h"
#include <vector>

#ifndef __ROOTBUILD__
class BDSEnergyCounterHit;
class BDSTrajectory;
template <class T> class G4THitsCollection;
typedef G4THitsCollection<BDSEnergyCounterHit> BDSEnergyCounterHitsCollection;
#endif

class BDSOutputROOTEventTrajectory : public TObject
{
public:
  BDSOutputROOTEventTrajectory();
  virtual ~BDSOutputROOTEventTrajectory();
#ifndef __ROOTBUILD__
  void Fill(std::vector<BDSTrajectory*> &trajVec);
  void Fill(BDSEnergyCounterHitsCollection *phc);
#endif
  void Flush();

  int n;
  std::vector<int> partID;
  std::vector<unsigned int> trackID;
  std::vector<unsigned int> parentID;
  std::vector<std::vector<int>> preProcessTypes;
  std::vector<std::vector<int>> preProcessSubTypes;
  std::vector<std::vector<int>> postProcessTypes;
  std::vector<std::vector<int>> postProcessSubTypes;

  std::vector<std::vector<double>> preWeights;
  std::vector<std::vector<double>> postWeights;
  std::vector<std::vector<double>> energys;

  std::vector<std::vector<TVector3>> trajectories;

  int primary();
  int primaryElectromagnetic();
  int primaryHadronic();
  void primaryPrint();

  friend std::ostream& operator<< (std::ostream& out, BDSOutputROOTEventTrajectory const &p);

  ClassDef(BDSOutputROOTEventTrajectory,1);
};


#endif
