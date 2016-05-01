#include "BDSOutputROOTEventTrajectory.hh"

ClassImp(BDSOutputROOTEventTrajectory)

BDSOutputROOTEventTrajectory::BDSOutputROOTEventTrajectory()
{;}

BDSOutputROOTEventTrajectory::~BDSOutputROOTEventTrajectory()
{;}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventTrajectory::Fill(std::vector<BDSTrajectory*> &trajVec)
{
  G4cout << __METHOD_NAME__ << " " << trajVec.size() << G4endl;
  for(auto iT = trajVec.begin(); iT != trajVec.end(); ++iT)
  {
    BDSTrajectory* traj = *iT;
    trackID.push_back( (unsigned int &&) traj->GetTrackID() );
    parentID.push_back((unsigned int &&) traj->GetParentID());

    std::vector<TVector3> trajectory;
    std::vector<int> preProcessType;
    std::vector<int> preProcessSubType;
    std::vector<int> postProcessType;
    std::vector<int> postProcessSubType;

    for(auto i = 0; i< traj->GetPointEntries();++i)
    {
      BDSTrajectoryPoint *point = dynamic_cast<BDSTrajectoryPoint*>(traj->GetPoint(i));
      G4ThreeVector      pos = point->GetPosition();
      trajectory.push_back(TVector3(pos.getX() / CLHEP::m,
                                    pos.getY() / CLHEP::m,
                                    pos.getZ() / CLHEP::m));
      preProcessType.push_back(point->GetPreProcessType());
      preProcessSubType.push_back(point->GetPreProcessSubType());
      postProcessType.push_back(point->GetPostProcessType());
      postProcessSubType.push_back(point->GetPostProcessSubType());
    }
    trajectories.push_back(trajectory);
    preProcessTypes.push_back(preProcessType);
    preProcessSubTypes.push_back(preProcessSubType);
    postProcessTypes.push_back(postProcessType);
    postProcessSubTypes.push_back(postProcessSubType);
  }
}

void BDSOutputROOTEventTrajectory::Fill(BDSEnergyCounterHitsCollection *phc)
{
  G4cout << phc->GetSize() << G4endl;
}


#endif

void BDSOutputROOTEventTrajectory::Flush()
{
  trackID.clear();
  parentID.clear();
  trajectories.clear();
  preProcessTypes.clear();
  preProcessSubTypes.clear();
  postProcessTypes.clear();
  postProcessSubTypes.clear();
}

std::ostream& operator<< (std::ostream& out, BDSOutputROOTEventTrajectory const &t)
{
  for(int i=0; i< t.preProcessTypes.size();++i)
  {
    for(int j=0; j< t.preProcessTypes[i].size(); ++j)
    {
      out << i << " " << j
      << " " << t.preProcessTypes[i][j]  << " " << t.preProcessSubTypes[i][j]
      << " " << t.postProcessTypes[i][j] << " " << t.postProcessSubTypes[i][j] << std::endl;
    }
  }
  return out;
}