#include <algorithm>
#include <iostream>

#include "BDSOutputROOTEventTrajectory.hh"

#ifndef __ROOTBUILD__
#include "G4VPhysicalVolume.hh"

#include "BDSDebug.hh"
#include "BDSEnergyCounterHit.hh"
#include "BDSTrajectory.hh"
#include "BDSAuxiliaryNavigator.hh"
#include "BDSPhysicalVolumeInfoRegistry.hh"
#include "BDSPhysicalVolumeInfo.hh"
#endif

ClassImp(BDSOutputROOTEventTrajectory)

BDSOutputROOTEventTrajectory::BDSOutputROOTEventTrajectory():
auxNavigator(nullptr),
  n(0)
{;}

BDSOutputROOTEventTrajectory::~BDSOutputROOTEventTrajectory()
{
#ifndef __ROOTBUILD__
  delete auxNavigator;
#endif
}

#ifndef __ROOTBUILD__
void BDSOutputROOTEventTrajectory::Fill(const std::vector<BDSTrajectory*> &trajVec)
{
  if(!auxNavigator) {
    /// Navigator for checking points in read out geometry
    auxNavigator = new BDSAuxiliaryNavigator();
  }

  n = (int) trajVec.size();
  for(auto iT = trajVec.begin(); iT != trajVec.end(); ++iT) {
    BDSTrajectory *traj = *iT;
    partID.push_back((int &&) traj->GetPDGEncoding());
    trackID.push_back((unsigned int &&) traj->GetTrackID());
    parentID.push_back((unsigned int &&) traj->GetParentID());
    parentIndex.push_back((int &&) traj->GetParentIndex());

    std::vector<int> preProcessType;
    std::vector<int> preProcessSubType;
    std::vector<int> postProcessType;
    std::vector<int> postProcessSubType;
    std::vector<double> preWeight;
    std::vector<double> postWeight;
    std::vector<double> energy;

    std::vector<TVector3> trajectory;
    std::vector<TVector3> momentum;
    std::vector<int>      modelIndex;


    for (auto i = 0; i < traj->GetPointEntries(); ++i) {
      BDSTrajectoryPoint *point = static_cast<BDSTrajectoryPoint *>(traj->GetPoint(i));

      // Position
      G4ThreeVector pos = point->GetPosition();
      trajectory.push_back(TVector3(pos.getX() / CLHEP::m,
                                    pos.getY() / CLHEP::m,
                                    pos.getZ() / CLHEP::m));

      G4VPhysicalVolume *vol = auxNavigator->LocateGlobalPointAndSetup(pos,nullptr,true,true,true);
      BDSPhysicalVolumeInfo* theInfo = BDSPhysicalVolumeInfoRegistry::Instance()->GetInfo(vol);
      if(theInfo) {
        modelIndex.push_back(theInfo->GetBeamlineIndex());
      }
      else {
        modelIndex.push_back(-1);
      }

      // Process types
      preProcessType.push_back(point->GetPreProcessType());
      preProcessSubType.push_back(point->GetPreProcessSubType());
      postProcessType.push_back(point->GetPostProcessType());
      postProcessSubType.push_back(point->GetPostProcessSubType());

      preWeight.push_back(point->GetPreWeight());
      postWeight.push_back(point->GetPostWeight());
      energy.push_back(point->GetEnergy());
      G4ThreeVector mom = point->GetPreMomentum();
      momentum.push_back(TVector3(mom.getX(),
                                  mom.getY(),
                                  mom.getZ()));
    }

    trajectories.push_back(trajectory);
    modelIndicies.push_back(modelIndex);
    momenta.push_back(momentum);
    preProcessTypes.push_back(preProcessType);
    preProcessSubTypes.push_back(preProcessSubType);
    postProcessTypes.push_back(postProcessType);
    postProcessSubTypes.push_back(postProcessSubType);
    preWeights.push_back(preWeight);
    postWeights.push_back(postWeight);
    energies.push_back(energy);

    // loop over parent trajectory steps and find production location and add

    // primary (special case)
    if (traj->GetTrackID() == 1) {
      parentStepIndex.push_back(-1);
      continue;
    };

    // secondary
    auto newParentID = traj->GetParentIndex();
    if (newParentID >= 0) {
      auto trajParent = trajVec.at((int)newParentID);
      auto trajDaughterPos = traj->GetPoint(0)->GetPosition();
      for (auto i = 0; i < trajParent->GetPointEntries(); ++i) {
        if (trajParent->GetPoint(i)->GetPosition() == trajDaughterPos) {
          parentStepIndex.push_back(i);
          break;
        }
      }
    }

    // could not find the vertex, possible for disconnected trees
    if (parentStepIndex.size() == 0) {
      parentStepIndex.push_back(-1);
    }
  }

  // Fill maps for later analysis
  int trackIndex = 0;
  for(auto iT = trajVec.begin(); iT != trajVec.end(); ++iT) {
    BDSTrajectory *traj = *iT;

    // map of trackID to trackIndex
    trackID_trackIndex.insert(std::pair<int, int>(traj->GetTrackID(),trackIndex));

    // map of trackIndex to trackProcess
    auto processPair = findParentProcess(trackIndex);
    trackIndex_trackProcess.insert(std::pair<int,std::pair<int,int>>(trackIndex,processPair));

    // map of modelIndex to trackProcess
    if(processPair.first != -1) {
      int mi = modelIndicies[processPair.first][processPair.second];
      trackIndex_modelIndex.insert(std::pair<int,int>(trackIndex, mi));
      try {
        modelIndex_trackIndex.at(mi).push_back(trackIndex);
      }
      catch(std::exception)
      {
        modelIndex_trackIndex.insert(std::pair<int,std::vector<int>>(mi,std::vector<int>()));
        modelIndex_trackIndex.at(mi).push_back(trackIndex);
      }
    }

    ++trackIndex;
  }
}

void BDSOutputROOTEventTrajectory::Fill(const BDSEnergyCounterHitsCollection *phc)
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
  parentIndex.clear();
  parentStepIndex.clear();
  preProcessTypes.clear();
  preProcessSubTypes.clear();
  postProcessTypes.clear();
  postProcessSubTypes.clear();
  preWeights.clear();
  postWeights.clear();
  energies.clear();
  trajectories.clear();
  momenta.clear();
  modelIndicies.clear();
  trackID_trackIndex.clear();
  trackIndex_trackProcess.clear();
  trackIndex_modelIndex.clear();
  modelIndex_trackIndex.clear();
}

std::pair<int,int> BDSOutputROOTEventTrajectory::findParentProcess(int trackIndex) {

  int tid = trackIndex;
  int pid = parentID.at(tid);
  int pin = parentIndex.at(tid);
  if(pin == -1) {
    return std::pair<int,int>(-1,-1);
  }
  int sin = parentStepIndex.at(tid);

  while(pid > 0) {
    if(pin == 0) {
      break;
    }
    tid = pin;
    pid = parentID.at(tid);
    pin = parentIndex.at(tid);
    sin = parentStepIndex.at(tid);

  }

  return std::pair<int,int>(pin,sin);
}

std::vector<BDSOutputROOTEventTrajectoryPoint> BDSOutputROOTEventTrajectory::trackInteractions(int trackid)
{
  int                ti = trackID_trackIndex.at(trackid);  // get track index

  std::vector<BDSOutputROOTEventTrajectoryPoint> tpv;      // trajectory point vector

  int nstep = trajectories[ti].size();
  for(int i = 0;i<nstep; ++i)
  {
    int ppt = postProcessTypes[ti][i];
    if(ppt != -1 && ppt != 1 && ppt != 10)
    {
      BDSOutputROOTEventTrajectoryPoint p(partID[ti], trackID[ti],
                                          parentID[ti], parentIndex[ti],
                                          postProcessTypes[ti][i], postProcessSubTypes[ti][i],
                                          postWeights[ti][i],energies[ti][i],
                                          trajectories[ti][i], momenta[ti][i],
                                          modelIndicies[ti][i]);
      tpv.push_back(p);
    }
  }
  return tpv;
}

BDSOutputROOTEventTrajectoryPoint BDSOutputROOTEventTrajectory::primaryProcessPoint(int trackid)
{
  int                ti = trackID_trackIndex.at(trackid);  // get track index
  std::pair<int,int> pp = trackIndex_trackProcess.at(ti);  // get track and index of start proccess

  BDSOutputROOTEventTrajectoryPoint p(partID[pp.first], trackID[pp.first],
                                      parentID[pp.first], parentIndex[pp.first],
                                      postProcessTypes[pp.first][pp.second], postProcessSubTypes[pp.first][pp.second],
                                      postWeights[pp.first][pp.second],energies[pp.first][pp.second],
                                      trajectories[pp.first][pp.second], momenta[pp.first][pp.second],
                                      modelIndicies[pp.first][pp.second]);
  return p;
}

std::vector<BDSOutputROOTEventTrajectoryPoint> BDSOutputROOTEventTrajectory::processHistory(int trackid)
{
  int                ti = trackID_trackIndex.at(trackid);  // get track index

  std::vector<BDSOutputROOTEventTrajectoryPoint> tpv;      // trajectory point vector
  while(ti != 0)
  {
    int pi  = parentIndex.at(ti);
    int psi = parentStepIndex.at(ti);

    BDSOutputROOTEventTrajectoryPoint p(partID[pi], trackID[pi],
                                        parentID[pi], parentIndex[pi],
                                        postProcessTypes[pi][psi], postProcessSubTypes[pi][psi],
                                        postWeights[pi][psi],energies[pi][psi],
                                        trajectories[pi][psi], momenta[pi][psi],
                                        modelIndicies[pi][psi]);
    tpv.push_back(p);
    ti = pi;
  }
  std::reverse(tpv.begin(),tpv.end());
  return tpv;
}

void BDSOutputROOTEventTrajectory::print(int i)
{
  for(size_t j=0;j<trajectories[i].size();++j)
  {
    std::cout << j << " " << trackID[i] << " " << parentID[i] << " " << parentIndex[i] << " " << parentStepIndex[i] << " " << partID[i] << " "
              << preProcessTypes[i][j]  << " " << preProcessSubTypes[i][j] << " "
              << postProcessTypes[i][j] << " " << postProcessSubTypes[i][j] << " "
              << trajectories[i][j].X() << " " << trajectories[i][j].Y() << " " <<  trajectories[i][j].Z() << " "
              << energies[i][j] << " " << momenta[i][j].Mag() << " " << momenta[i][j].X()      << " " << momenta[i][j].Y()      << " " <<  momenta[i][j].Z() << std::endl;
  }
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
            << " " << t.energies[i][j]      << std::endl;
      //}
    }
  }
  return out;
}
