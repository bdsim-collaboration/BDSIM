/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#include "BDSException.hh"
#include "BDSOutputROOTEventTrajectory.hh"
#include "BDSOutputROOTEventTrajectoryPoint.hh"
#include "DataLoader.hh"
#include "Event.hh"

#include "TROOT.h"
#include "TSystem.h"

#include <algorithm>
#include <iostream>
#include <stdexcept>
#include <vector>

int main(int argc, char** argv)
{
  if (argc < 2 || argc > 2)
    {std::cout << "usage: BDSTrajectoryTest <datafile>" << std::endl;}

  std::string dataFile = std::string(argv[1]);

  // to simply syntax
  typedef BDSOutputROOTEventTrajectoryPoint point;
  typedef std::vector<BDSOutputROOTEventTrajectoryPoint> pointV;
  
  try
    {
      DataLoader* dl = new DataLoader(dataFile);
      TTree* eventTree = dl->GetEventTree();
      Event* event     = dl->GetEvent();
      
      eventTree->GetEntry(0);

      // test each function in the trajectory class
      // use the last trackID as our random starting particle to inspect
      int lastTrackID = (int)event->Trajectory->trackID.back();

      pointV trackInteractions = event->Trajectory->trackInteractions(lastTrackID);
      
      point parentProcessPoint = event->Trajectory->parentProcessPoint(lastTrackID);
      
      //int nTrajectories = event->Trajectory->n;
      std::vector<unsigned int> trackIDsSorted = event->Trajectory->trackID;
      std::sort(trackIDsSorted.begin(), trackIDsSorted.end());
      int highTrackID = trackIDsSorted.back();
      
      point  primProcessPoint  = event->Trajectory->primaryProcessPoint(highTrackID);

      pointV processHistory    = event->Trajectory->processHistory(lastTrackID);

      event->Trajectory->printTrajectoryInfo(lastTrackID);
      bool parentIsPrimary = event->Trajectory->parentIsPrimary(lastTrackID);
      std::string answer = parentIsPrimary ? "True" : "False";
      std::cout << "Parent of trackID " << lastTrackID << " is a primary? : " << answer << std::endl;


      // now test them with an invalid trackID to ensure no bad exception thrown
      // trackID should be >= 1
      event->Trajectory->trackInteractions(0);
      event->Trajectory->primaryProcessPoint(0);
      event->Trajectory->processHistory(0);
      event->Trajectory->parentIsPrimary(0);

      // now try with the primary which has no parent. The primary should by construction
      // have trackID = 1
      event->Trajectory->trackInteractions(1);
      event->Trajectory->primaryProcessPoint(1);
      event->Trajectory->processHistory(1);
      event->Trajectory->parentIsPrimary(1);
    }
  catch (const BDSException& e)
    {std::cout << e.what() << std::endl;}
  catch (const std::exception& e)
    {std::cout << e.what() << std::endl;}
  return 0;
}
