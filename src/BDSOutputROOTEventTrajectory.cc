#include "BDSOutputROOTEventTrajectory.hh"

ClassImp(BDSOutputROOTEventTrajectory);

BDSOutputROOTEventTrajectory::BDSOutputROOTEventTrajectory()
{

}

BDSOutputROOTEventTrajectory::~BDSOutputROOTEventTrajectory()
{

}

void BDSOutputROOTEventTrajectory::Flush()
{
  trackID.clear();
  trajectories.clear();
}