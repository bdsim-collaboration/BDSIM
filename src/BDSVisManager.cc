/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
#include "BDSVisManager.hh"

#include "G4UIterminal.hh"
#ifdef G4UI_USE_TCSH
#include "G4UItcsh.hh"
#endif

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#ifdef G4VIS_USE
#include "G4UImanager.hh"        // G4 session managers
#endif
#include "G4UIExecutive.hh"
#endif

#include "G4TrajectoryDrawByCharge.hh"
#include "G4Version.hh"

#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSMessenger.hh"
#include "BDSUtilities.hh"

BDSVisManager::BDSVisManager()
{;}

void BDSVisManager::StartSession(G4int argc, char** argv)
{
  /// Create BDS UI messenger
  BDSMessenger *bdsMessenger = new BDSMessenger();

#ifdef G4UI_USE_TCSH
  G4UIsession* session = new G4UIterminal(new G4UItcsh);
#else
  G4UIsession* session = new G4UIterminal();
#endif

#ifdef G4VIS_USE
#ifdef BDSDEBUG 
  G4cout<< __METHOD_NAME__ << "Initializing Visualisation Manager"<<G4endl;
#endif
  // Initialize visualisation
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
      
  G4TrajectoryDrawByCharge* trajModel1 = new G4TrajectoryDrawByCharge("trajModel1");
  visManager->RegisterModel(trajModel1);
  visManager->SelectTrajectoryModel(trajModel1->Name());
#endif
 
#ifdef G4UI_USE
  G4UIExecutive* session2 = new G4UIExecutive(argc, argv);
#ifdef G4VIS_USE

  std::string bdsimPath = BDS::GetBDSIMExecPath();
  // difference between local build and install build:
  std::string visPath;
  std::string localPath   = bdsimPath + "vis/vis.mac";
  std::string installPath = bdsimPath + "../share/bdsim/vis/vis.mac";
      
  if (FILE *file = fopen(localPath.c_str(), "r"))
    {
      fclose(file);
      visPath = bdsimPath + "vis/";
    }
  else if ( (file = fopen(installPath.c_str(), "r")) )
    {
      fclose(file);
      visPath = bdsimPath + "../share/bdsim/vis/";
    }
  else
    {G4cout << __METHOD_NAME__ << "ERROR: default visualisation file could not be found!" << G4endl;}

  // check if visualisation file is present and readable
  G4String visMacroName = BDSGlobalConstants::Instance()->VisMacroFileName();
  bool useDefault = false;
  // if not set use default visualisation file
  if (visMacroName.empty())
    {useDefault = true;}
  // build full filename
  G4String visMacroFilename = visMacroName;
  if (useDefault)
    {
#ifdef G4VIS_USE_OPENGLQT
      visMacroFilename = visPath + "vis.mac";
#else
      visMacroFilename = visPath + "dawnfile.mac";
#endif
    }
  else
    {
      // check if file exists and if not present don't start session
      // (need to use std::cout otherwise not printed)
      if (BDS::FileExists(visMacroFilename) == false)
	{
          std::cout << __METHOD_NAME__ << "ERROR: visualisation file "
                    << visMacroFilename << " not present!" << G4endl;
          return;
        }
    }
  // execute visualisation file
  G4UImanager* UIManager = G4UImanager::GetUIpointer();
  UIManager->ApplyCommand("/control/execute " + visMacroFilename);
  
  // run gui
  if (session2->IsGUI())
    {
#if G4VERSION_NUMBER < 1030
      // these were added by default in Geant4.10.3 onwards
      // Add icons
      std::string iconMacroFilename = visPath + "icons.mac";
      UIManager->ApplyCommand("/control/execute " + iconMacroFilename);
      // add run icon:
      std::string runButtonFilename = visPath + "run.png";
      UIManager->ApplyCommand("/gui/addIcon \"Run beam on\" user_icon \"/run/beamOn 1\" " + runButtonFilename);
#endif
      // add menus
      std::string guiMacroFilename  = visPath + "gui.mac";
      UIManager->ApplyCommand("/control/execute " + guiMacroFilename);
    }
#endif
  session2->SessionStart();
  delete session2;
#endif
  delete session;
  delete bdsMessenger;
}
