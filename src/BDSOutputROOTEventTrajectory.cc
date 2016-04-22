#include "BDSOutputROOTEventTrajectory.hh"

ClassImp(BDSOutputROOTEventTrajectory)

BDSOutputROOTEventTrajectory::BDSOutputROOTEventTrajectory()
{;}

BDSOutputROOTEventTrajectory::~BDSOutputROOTEventTrajectory()
{;}

#ifndef __ROOTBUILD__
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
      trajectory.push_back(TVector3(pos.getX() / CLHEP::m,
                                    pos.getY() / CLHEP::m,
                                    pos.getY() / CLHEP::m,
                                    pos.getZ() / CLHEP::m));
    }
    trajectories.push_back(trajectory);
    trajectory.clear();
  }
}

void BDSOutputROOTEventTrajectory::Fill(BDSEnergyCounterHitsCollection *phc)
{
  G4cout << phc->GetSize() << G4endl;
}


#endif

void BDSOutputROOTEventTrajectory::Flush()
{
  trajectories.clear();
  trackID.clear();

}
