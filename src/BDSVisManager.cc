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
#include "BDSUtilities.hh"

BDSVisManager::BDSVisManager()
{;}

void BDSVisManager::StartSession(G4int argc, char** argv)
{
  G4UIsession* session=nullptr;
#ifdef G4UI_USE_TCSH
  session = new G4UIterminal(new G4UItcsh);
#else
  session = new G4UIterminal();
#endif

#ifdef G4VIS_USE
#ifdef BDSDEBUG 
  G4cout<< __METHOD_NAME__ << "Initializing Visualisation Manager"<<G4endl;
#endif
  // Initialize visualisation
  G4VisManager* visManager = new G4VisExecutive;
  // G4VisExecutive can take a verbosity argument - see /vis/verbose guidance.
  // G4VisManager* visManager = new G4VisExecutive("Quiet");
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
  std::string localPath = bdsimPath + "vis/vis.mac";
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
  G4String visMacroFilename = BDS::GetFullPath(visMacroName);
  if (!useDefault)
    {
      FILE* file = nullptr;
      // first relative to main path:
      file = fopen(visMacroFilename.c_str(), "r");
      if (file)
	{fclose(file);}
      else
	{
	  // if not present use a default one (OGLSQt or DAWNFILE)
	  G4cout << __METHOD_NAME__ << "WARNING: visualisation file "
		 << visMacroFilename <<  " file not present, using default!" << G4endl;
	  useDefault = true;
	}
    }
  if (useDefault)
    {
#ifdef G4VIS_USE_OPENGLQT
      visMacroFilename = visPath + "vis.mac";
#else
      visMacroFilename = visPath + "dawnfile.mac";
#endif
    }
  // execute visualisation file
  G4UImanager* UIManager = G4UImanager::GetUIpointer();
  UIManager->ApplyCommand("/control/execute " + visMacroFilename);
  
  // add default gui
  if (session2->IsGUI())
    {
#if G4VERSION_NUMBER < 1030
      // Add icons
      std::string iconMacroFilename = visPath + "icons.mac";
      UIManager->ApplyCommand("/control/execute " + iconMacroFilename);
#endif
      // add menus
      std::string guiMacroFilename  = visPath + "gui.mac";
      UIManager->ApplyCommand("/control/execute " + guiMacroFilename);
      // add run icon:
      std::string runButtonFilename = visPath + "run.png";
      UIManager->ApplyCommand("/gui/addIcon \"Run beam on\" user_icon \"/run/beamOn 1\" " + runButtonFilename);
    }
#endif
  session2->SessionStart();
  delete session2;
#endif
  delete session;
}
