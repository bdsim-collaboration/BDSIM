#include "BDSOutputROOTEventTrajectory.hh"

ClassImp(BDSOutputROOTEventTrajectory)

BDSOutputROOTEventTrajectory::BDSOutputROOTEventTrajectory()
{;}

BDSOutputROOTEventTrajectory::~BDSOutputROOTEventTrajectory()
{;}

#ifndef __MAKECINT__
void BDSOutputROOTEventTrajectory::Fill(std::vector<BDSTrajectory*> &trajVec)
{
  for(auto iT = trajVec.begin(); iT != trajVec.end(); ++iT)
  {
    G4Trajectory* traj=(G4Trajectory*)(*iT);
    trackID.push_back((unsigned int &&) traj->GetTrackID());

    std::vector<TVector3> trajectory;

    for(auto i = 0; i< traj->GetPointEntries();++i)
    {
      G4VTrajectoryPoint *point = traj->GetPoint(i);
      G4ThreeVector      pos = point->GetPosition();
      trajectory.push_back(TVector3(pos.getX(),pos.getY(), pos.getZ()));
    }
    trajectories.push_back(trajectory);
    trajectory.clear();
  }
}
#endif

void BDSOutputROOTEventTrajectory::Flush()
{
  trackID.clear();
  trajectories.clear();
}
