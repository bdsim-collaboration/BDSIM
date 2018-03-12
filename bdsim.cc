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
/**
 * @file bdsim.cc
 *
 * \mainpage
 * BDSIM © 2001-@CURRENT_YEAR@
 *
 * version @BDSIM_VERSION@
 */

#include "BDSExecOptions.hh"     // executable command line options 
#include "BDSGlobalConstants.hh" //  global parameters

#include <cstdlib>      // standard headers 
#include <cstdio>
#include <signal.h>

#include "G4EventManager.hh" // Geant4 includes
#include "G4GeometryManager.hh"
#include "G4ParallelWorldPhysics.hh"
#include "G4TrackingManager.hh"
#include "G4SteppingManager.hh"
#include "G4GeometryTolerance.hh"
#include "G4GenericBiasingPhysics.hh"

#include "BDSAcceleratorModel.hh"
#include "BDSBunch.hh"
#include "BDSBunchFactory.hh"
#include "BDSColours.hh"
#include "BDSDetectorConstruction.hh"   
#include "BDSEventAction.hh"
#include "BDSFieldFactory.hh"
#include "BDSFieldLoader.hh"
#include "BDSGeometryFactorySQL.hh"
#include "BDSGeometryWriter.hh"
#include "BDSMaterials.hh"
#include "BDSModularPhysicsList.hh"
#include "BDSOutput.hh" 
#include "BDSOutputFactory.hh"
#include "BDSParallelWorldUtilities.hh"
#include "BDSParser.hh" // Parser
#include "BDSParticleDefinition.hh"
#include "BDSPrimaryGeneratorAction.hh"
#include "BDSRandom.hh" // for random number generator from CLHEP
#include "BDSRunAction.hh"
#include "BDSRunManager.hh"
#include "BDSSamplerRegistry.hh"
#include "BDSSDManager.hh"
#include "BDSSteppingAction.hh"
#include "BDSStackingAction.hh"
#include "BDSTemporaryFiles.hh"
#include "BDSTrackingAction.hh"
#include "BDSUtilities.hh"
#include "BDSVisManager.hh"


int main(int argc,char** argv)
{
  /// Print header & program information
  G4cout<<"bdsim : version @BDSIM_VERSION@"<<G4endl;
  G4cout<<"        (C) 2001-@CURRENT_YEAR@ Royal Holloway University London"<<G4endl;
  G4cout<<"        http://www.pp.rhul.ac.uk/bdsim"<<G4endl;
  G4cout<<G4endl;

  /// Initialize executable command line options reader object
  const BDSExecOptions* execOptions = new BDSExecOptions(argc,argv);
  execOptions->Print();
  G4bool ignoreSIGINT = execOptions->IgnoreSIGINT(); // different sig catching for cmake
  
#ifdef BDSDEBUG
  G4cout << __FUNCTION__ << "> DEBUG mode is on." << G4endl;
#endif

  /// Parse lattice file
  BDSParser* parser = BDSParser::Instance(execOptions->InputFileName());
  /// Update options generated by parser with those from executable options.
  parser->AmalgamateOptions(execOptions->Options());
  parser->AmalgamateBeam(execOptions->Beam(), execOptions->Options().recreate);
  /// Check options for consistency
  parser->CheckOptions();
  
  /// Explicitly initialise materials to construct required materials before global constants.
  BDSMaterials::Instance()->PrepareRequiredMaterials(execOptions->Options().verbose);

  /// No longer needed. Everything can safely use BDSGlobalConstants from now on.
  delete execOptions; 

  /// Force construction of global constants after parser has been initialised (requires
  /// materials first). This uses the options and beam from BDSParser.
  const BDSGlobalConstants* globalConstants = BDSGlobalConstants::Instance();

  /// Initialize random number generator
  BDSRandom::CreateRandomNumberGenerator();
  BDSRandom::SetSeed(); // set the seed from options
  
  /// Instantiate the specific type of bunch distribution.
  BDSBunch* bdsBunch = BDSBunchFactory::CreateBunch(parser->GetBeam(),
						    globalConstants->BeamlineTransform());

  /// Construct output
#ifdef BDSDEBUG
  G4cout << __FUNCTION__ << "> Setting up output." << G4endl;
#endif
  /// Optionally generate primaries only and exit
  BDSOutput* bdsOutput = BDSOutputFactory::CreateOutput(globalConstants->OutputFormat(),
                                                        globalConstants->OutputFileName());
  if (globalConstants->GeneratePrimariesOnly())
    {
      // output creation is duplicated below but with this if loop, we exit so ok.
      bdsOutput->NewFile();
      G4double x0=0.0, y0=0.0, z0=0.0, xp=0.0, yp=0.0, zp=0.0, t=0.0, E=0.0, weight=1.0;
      const G4int nToGenerate = globalConstants->NGenerate();
      const G4int printModulo = globalConstants->PrintModuloEvents();
      for (G4int i = 0; i < nToGenerate; i++)
      {
	if (i%printModulo == 0)
	  {G4cout << "\r Primary> " << std::fixed << i << " of " << nToGenerate << G4endl;}
        bdsBunch->GetNextParticle(x0,y0,z0,xp,yp,zp,t,E,weight);
        bdsOutput->FillEventPrimaryOnly(E, x0, y0, z0, xp, yp, zp, t, weight, 1, i, 1);
      }
      bdsOutput->CloseFile();
      delete bdsBunch;
      delete bdsOutput;
      return 0;
    }

  /// Check geant4 exists in the current environment
  if (!BDS::Geant4EnvironmentIsSet())
    {G4cout << "No Geant4 environmental variables found - please source geant4.sh environment" << G4endl; return 1;}

  /// Construct mandatory run manager (the G4 kernel) and
  /// register mandatory initialization classes.
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Constructing run manager"<<G4endl;
#endif
  BDSRunManager* runManager = new BDSRunManager;
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - detector construction"<<G4endl;
#endif
  /// Register the geometry and parallel world construction methods with run manager.
  BDSDetectorConstruction* realWorld = new BDSDetectorConstruction();
  /// Here the geometry isn't actually constructed - this is called by the runManager->Initialize()
  auto samplerWorlds = BDS::ConstructAndRegisterParallelWorlds(realWorld);
  runManager->SetUserInitialization(realWorld);  

  /// For geometry sampling, phys list must be initialized before detector.
  /// BUT for samplers we use a parallel world and this HAS to be before the physcis
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Constructing physics processes" << G4endl;
#endif
  G4String physicsListName = parser->GetOptions().physicsList;
  // Note, we purposively don't create a parallel world process for the curvilinear
  // world as we don't need the track information from it - unreliable that way. We
  // query the geometry directly using our BDSAuxiliaryNavigator class.
  auto samplerPhysics = BDS::ConstructSamplerParallelPhysics(samplerWorlds);
  BDSModularPhysicsList* physList  = new BDSModularPhysicsList(physicsListName);

  // Construction of the physics lists defines the necessary particles and therefore
  // we can calculate the beam rigidity for the particle the beam is designed w.r.t. This
  // must happen before the geometry is constructed (which is called by
  // runManager->Initialize()).
  BDSParticleDefinition* beamParticle;
  beamParticle = physList->ConstructBeamParticle(globalConstants->ParticleName(),
						 globalConstants->BeamTotalEnergy(),
						 globalConstants->FFact());
  G4cout << "main> Beam particle properties: " << G4endl << *beamParticle;
  realWorld->SetRigidityForConstruction(beamParticle->BRho());
  BDSFieldFactory::SetDefaultRigidity(beamParticle->BRho());       // used for field loading
  BDSGeometryFactorySQL::SetDefaultRigidity(beamParticle->BRho()); // used for sql field loading
  
  BDS::RegisterSamplerPhysics(samplerPhysics, physList);
  physList->BuildAndAttachBiasWrapper(parser->GetBiasing());
  runManager->SetUserInitialization(physList);
  
  /// Print the geometry tolerance
  G4GeometryTolerance* theGeometryTolerance = G4GeometryTolerance::GetInstance();
  G4cout << __FUNCTION__ << "> Geometry Tolerances: "     << G4endl;
  G4cout << __FUNCTION__ << ">" << std::setw(22) << "Surface: " << std::setw(10) << theGeometryTolerance->GetSurfaceTolerance() << " mm"   << G4endl;
  G4cout << __FUNCTION__ << ">" << std::setw(22) << "Angular: " << std::setw(10) << theGeometryTolerance->GetAngularTolerance() << " rad"  << G4endl;
  G4cout << __FUNCTION__ << ">" << std::setw(22) << "Radial: "  << std::setw(10) << theGeometryTolerance->GetRadialTolerance()  << " mm"   << G4endl;

  /// Set user action classes
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - Run Action"<<G4endl;
#endif
  runManager->SetUserAction(new BDSRunAction(bdsOutput, bdsBunch));

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - Event Action"<<G4endl;
#endif
  runManager->SetUserAction(new BDSEventAction(bdsOutput));

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - Stepping Action"<<G4endl;
#endif
  // Only add steppingaction if it is actually used, so do check here (for performance reasons)
  if (globalConstants->VerboseStep())
    {
      G4int verboseEventNumber = globalConstants->VerboseEventNumber();
      runManager->SetUserAction(new BDSSteppingAction(true, verboseEventNumber));
    }
  
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - Tracking Action"<<G4endl;
#endif
  runManager->SetUserAction(new BDSTrackingAction(globalConstants->Batch()));

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - Stacking Action"<<G4endl;
#endif
  runManager->SetUserAction(new BDSStackingAction(globalConstants));

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Registering user action - Primary Generator"<<G4endl;
#endif
  runManager->SetUserAction(new BDSPrimaryGeneratorAction(bdsBunch, beamParticle));

  /// Initialize G4 kernel
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> Initialising Geant4 kernel"<<G4endl;
#endif
  runManager->Initialize();

  /// Implement bias operations on all volumes only after G4RunManager::Initialize()
  realWorld->BuildPhysicsBias();

#ifdef BDSDEBUG
  auto physics = runManager->GetUserPhysicsList();
  if (const BDSModularPhysicsList* modPhysics = dynamic_cast<const BDSModularPhysicsList*>(physics))
    {
      modPhysics->PrintPrimaryParticleProcesses();
      modPhysics->PrintDefinedParticles();
    }
#endif

  /// Set verbosity levels
  runManager->SetVerboseLevel(globalConstants->VerboseRunLevel());
  G4EventManager::GetEventManager()->SetVerboseLevel(globalConstants->VerboseEventLevel());
  G4EventManager::GetEventManager()->GetTrackingManager()->SetVerboseLevel(globalConstants->VerboseTrackingLevel());
  G4EventManager::GetEventManager()->GetTrackingManager()->GetSteppingManager()->SetVerboseLevel(globalConstants->VerboseSteppingLevel());
  
  /// Close the geometry in preparation for running - everything is now fixed.
  G4bool bCloseGeometry = G4GeometryManager::GetInstance()->CloseGeometry();
  if(!bCloseGeometry)
    { 
      G4cerr << __FUNCTION__ << "> error - geometry not closed." << G4endl;
      return 1;
    }

  if (globalConstants->ExportGeometry())
    {
      BDSGeometryWriter geometrywriter;
      geometrywriter.ExportGeometry(globalConstants->ExportType(),
				    globalConstants->ExportFileName());
    }
  else
    {
      G4cout.precision(10);
      /// Catch aborts to close output stream/file. perhaps not all are needed.
      struct sigaction act;
      act.sa_handler = &BDS::HandleAborts;
      sigemptyset(&act.sa_mask);
      act.sa_flags = 0;
      if (!ignoreSIGINT)
	{sigaction(SIGINT,  &act, 0);}
      sigaction(SIGABRT, &act, 0);
      sigaction(SIGTERM, &act, 0);
      sigaction(SIGSEGV, &act, 0);
      
      /// Run in either interactive or batch mode
      if(!globalConstants->Batch())   // Interactive mode
	{
	  BDSVisManager visManager;
	  visManager.StartSession(argc,argv);
	}
      else           // Batch mode
	{runManager->BeamOn(globalConstants->NGenerate());}
    }

  /// Termination & clean up.
  G4GeometryManager::GetInstance()->OpenGeometry();

#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> BDSOutput deleting..."<<G4endl;
#endif
  delete bdsOutput;
  
#ifdef BDSDEBUG 
  G4cout << __FUNCTION__ << "> instances deleting..."<<G4endl;
#endif
  // Order important here because of singletons relying on each other
  delete BDSAcceleratorModel::Instance();
  delete BDSTemporaryFiles::Instance();
  delete BDSFieldFactory::Instance(); // this uses BDSGlobalConstants which uses BDSMaterials
  delete globalConstants;
  delete BDSMaterials::Instance();

  // instances not used in this file, but no other good location for deletion
  delete BDSColours::Instance();
  delete BDSFieldLoader::Instance();
  delete BDSSDManager::Instance();
  delete BDSSamplerRegistry::Instance();
  
#ifdef BDSDEBUG 
  G4cout<< __FUNCTION__ << "> BDSRunManager deleting..."<<G4endl;
#endif
  delete runManager;
  delete bdsBunch;
  delete parser;

  G4cout << __FUNCTION__ << "> End of Run. Thank you for using BDSIM!" << G4endl;

  return 0;
}
