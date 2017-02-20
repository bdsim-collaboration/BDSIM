#include <iostream>

#include "BDSOutputROOTEventTrajectory.hh"

#ifndef __ROOTBUILD__
#include "BDSDebug.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSTrajectory.hh"
#endif

ClassImp(BDSOutputROOTEventTrajectory)

BDSOutputROOTEventTrajectory::BDSOutputROOTEventTrajectory():
  n(0)
{;}

BDSOutputROOTEventTrajectory::~BDSOutputROOTEventTrajectory()
{;}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventTrajectory::Fill(std::vector<BDSTrajectory*> &trajVec)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << " " << trajVec.size() << G4endl;
#endif

  n = trajVec.size();
  for(auto iT = trajVec.begin(); iT != trajVec.end(); ++iT)
  {
    BDSTrajectory* traj = *iT;
    partID.push_back( (int &&) traj->GetPDGEncoding() );
    trackID.push_back( (unsigned int &&) traj->GetTrackID() );
    //parentID.push_back((unsigned int &&) traj->GetParentID());
    parentID.push_back((int &&) traj->GetNewParentID());
#ifdef BDSDEBUG
//    G4cout << __METHOD_NAME__ << traj->GetTrackID() << " " << traj->GetParentID() << G4endl;
#endif

    std::vector<TVector3> trajectory;
    std::vector<std::vector<int>> tempSecondaryID;
    std::vector<int> preProcessType;
    std::vector<int> preProcessSubType;
    std::vector<int> postProcessType;
    std::vector<int> postProcessSubType;
    std::vector<double> preWeight;
    std::vector<double> postWeight;
    std::vector<double> energy;

    for(auto i = 0; i< traj->GetPointEntries();++i)
    {
      BDSTrajectoryPoint* point = static_cast<BDSTrajectoryPoint*>(traj->GetPoint(i));
      G4ThreeVector      pos = point->GetPosition();
      trajectory.push_back(TVector3(pos.getX() / CLHEP::m,
                                    pos.getY() / CLHEP::m,
                                    pos.getZ() / CLHEP::m));
      preProcessType.push_back(point->GetPreProcessType());
      preProcessSubType.push_back(point->GetPreProcessSubType());
      postProcessType.push_back(point->GetPostProcessType());
      postProcessSubType.push_back(point->GetPostProcessSubType());

      preWeight.push_back(point->GetPreWeight());
      postWeight.push_back(point->GetPostWeight());
      energy.push_back(point->GetEnergy());
      tempSecondaryID.push_back(point->GetSecondaryID());
    }
    trajectories.push_back(trajectory);
    preProcessTypes.push_back(preProcessType);
    preProcessSubTypes.push_back(preProcessSubType);
    postProcessTypes.push_back(postProcessType);
    postProcessSubTypes.push_back(postProcessSubType);
    preWeights.push_back(preWeight);
    postWeights.push_back(postWeight);
    energys.push_back(energy);
    secondaryID.push_back(tempSecondaryID);
  }
}

void BDSOutputROOTEventTrajectory::Fill(BDSEnergyCounterHitsCollection *phc)
{
  G4cout << phc->GetSize() << G4endl;
}


#endif

void BDSOutputROOTEventTrajectory::Flush()
{
  n = 0;
  partID.clear();
  trackID.clear();
  parentID.clear();
  preProcessTypes.clear();
  preProcessSubTypes.clear();
  postProcessTypes.clear();
  postProcessSubTypes.clear();
  preWeights.clear();
  postWeights.clear();
  energys.clear();
  trajectories.clear();
  secondaryID.clear();
}

int BDSOutputROOTEventTrajectory::primary()
{
  for(size_t j=0;j<parentID.size();++j) {
    if(parentID[j] == 0) {
      return j;
    }
  }
  return -1;
}

int BDSOutputROOTEventTrajectory::primaryElectromagnetic()
{
  // loop over primary and find first electromagnetic
  int i = 0;
  for(size_t j=0;j<trajectories[i].size();++j) {
    if(postProcessTypes[i][j] == 2) {
      return postProcessSubTypes[i][j];
    }
  }
  return -1;
}

int BDSOutputROOTEventTrajectory::primaryHadronic()
{
  // loop over primary and find fist hadronic 
  int i = 0;
  for(size_t j=0;j<trajectories[i].size();++j) {
    if(postProcessTypes[i][j] == 4) {
      return postProcessSubTypes[i][j];
    }
  }
  return -1;
}

void BDSOutputROOTEventTrajectory::print(int i)
{
  for(size_t j=0;j<trajectories[i].size();++j)
  {
    std::cout << j << " " << trackID[i] << " " << parentID[i] << " " << partID[i] << " "
              << preProcessTypes[i][j]  << " " << preProcessSubTypes[i][j] << " "
              << postProcessTypes[i][j] << " " << postProcessSubTypes[i][j] << " "
              << trajectories[i][j].X() << " " << trajectories[i][j].Y() << " " <<  trajectories[i][j].Z() << " "
              << secondaryID[i][j].size() << std::endl;
  }
}

void BDSOutputROOTEventTrajectory::primaryPrint()
{
  print(0);
}

std::ostream& operator<< (std::ostream& out, BDSOutputROOTEventTrajectory const &t)
{
  for(int i=0; i< (int)t.preProcessTypes.size();++i)
  {
    for(int j=0; j< (int)t.preProcessTypes[i].size(); ++j)
    {
      //if(t.preProcessTypes[i][j] != 1 && t.preProcessTypes[i][j] != 7)
      //{
        out << i << " " << j
            << " " << t.preProcessTypes[i][j]   << " " << t.preProcessSubTypes[i][j]
            << " " << t.postProcessTypes[i][j]  << " " << t.postProcessSubTypes[i][j]
            << " " << t.preWeights[i][j]        << " " << t.postWeights[i][j]
            << " " << t.energys[i][j]      << std::endl;
      //}
    }
  }
  return out;
}
