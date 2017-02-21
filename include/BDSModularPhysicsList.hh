#ifndef BDSMODULARPHYSICSLIST_H
#define BDSMODULARPHYSICSLIST_H

#include "G4VModularPhysicsList.hh"

#include <map>
#include <string>
#include <vector>

class BDSGlobalConstants;
class BDSModularPhysicsList;
class G4OpticalPhysics;
class G4VPhysicsConstructor;

/// Typedef for function pointers to simplify syntax.
typedef void(BDSModularPhysicsList::*Constructor)(void);

/**
 * @brief Modular physics list based on Geant4 constructors.
 *
 * Dynamically construct phyiscs process constructors based on 
 * key words in parser physics list option. Being modular, these
 * can be built up and added to individually to give the required
 * set of physics processes. Note: the transportation process is 
 * constructed by default with classes that derive from G4VModularPhysicsList.
 *
 * @author L. Deacon, S. Boogert & L. Nevay
 */

class BDSModularPhysicsList: public G4VModularPhysicsList
{
public:
  BDSModularPhysicsList(G4String physicsList);
  virtual ~BDSModularPhysicsList();

  /// Print out which physics lists are activated.
  void Print();

  /// Print all the processes by name as registered to the primary particle type.
  /// Note, this should only be done after the physics lists are fully constructed.
  void PrintPrimaryParticleProcesses() const;

  /// Print all constructed particle names. Note, this should only be done after the
  /// physics lists are fully constructed.
  void PrintDefinedParticles() const;
  
  virtual void SetCuts();

private:
  /// Private default constructor to force use of supplied one.
  BDSModularPhysicsList();
  
  G4bool verbose;
#ifdef BDSDEBUG 
  bool debug = true;
#else 
  bool debug = false;
#endif

  /// Update variables in global constants with all information about the primary particle
  /// momenta etc.
  void SetParticleDefinition();

  /// Construct the minimum particle set required (gamma, electron, positron,
  /// proton and anti-proton.
  void ConstructMinimumParticleSet();

  /// Neutrinos are not constructed by default in many (most) physics lists
  /// yet this results in crashes when they're produced but not defined by
  /// physics processes, so purposively define for ones where it's a problem.
  void ConstructAllLeptons();

  ///  Construct resonances and quarks - sometimes required explicitly.
  void ConstructAllShortLived();

  /// Construct mesons.
  void ConstructAllMesons();

  /// Construct baryons.
  void ConstructAllBaryons();

  /// Construct ions.
  void ConstructAllIons();

  /// Interpret the string of physics lists given from the user through the parser.
  void ParsePhysicsList(G4String physListName);

  /// Set particular options for various physics lists from the parser. This is
  /// currently only done for the optical physics list.
  void ConfigurePhysics();

  /// Delegate function for the specific optical physics processes controllable from
  /// the parser.
  void ConfigureOptical();
  
  /// A map of physics list names to their constructors.
  std::map<std::string, Constructor> physicsConstructors;

  /// A vector of all the physics list names for searching. This is
  /// constructed from the above map physicsConstructors.
  std::vector<G4String> physicsLists;

  /// Many physics lists require other physics lists - keep a record
  /// of which ones have been activated and if the required ones haven't,
  /// activate them. This is also constructed from the physicsConstructors map.
  std::map<G4String, G4bool> physicsActivated;
  
  G4OpticalPhysics* opticalPhysics;
  std::vector<G4VPhysicsConstructor*> constructors;

  /// Keep a local reference to global constants to avoid getting it all the time
  BDSGlobalConstants* globals;

  /// @{Physics constructor loader.
  void ParameterisationPhysics();  
  void CutsAndLimits();
  void Em();
  void EmExtra();
  void EmLow();
  void HadronicElastic();
  void Hadronic();
  void HadronicHP();
  void SynchRad();
  void Muon();					
  void Optical();
  void Decay();
  void QGSPBERT();
  void QGSPBERTHP();
  void QGSPBIC();
  void QGSPBICHP();
  void FTFP();
  void FTFPBERT();
  void FTFPBERTHP();
  void LaserWire();
  /// @}
};

#endif

