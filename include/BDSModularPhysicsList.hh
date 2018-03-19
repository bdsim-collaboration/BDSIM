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
#ifndef BDSMODULARPHYSICSLIST_H
#define BDSMODULARPHYSICSLIST_H

#include "G4VModularPhysicsList.hh"

#include <map>
#include <string>
#include <vector>

class BDSGlobalConstants;
class BDSModularPhysicsList;
class BDSParticleDefinition;
class G4OpticalPhysics;
class G4VPhysicsConstructor;

namespace GMAD {
  class PhysicsBiasing;
  template<typename T> class FastList;
}

/**
 * @brief Modular physics list based on Geant4 constructors.
 *
 * Dynamically construct phyiscs process constructors based on 
 * key words in parser physics list option. Being modular, these
 * can be built up and added to individually to give the required
 * set of physics processes. Note: the transportation process is 
 * constructed by default with classes that derive from G4VModularPhysicsList.
 *
 * Note, Geant4 will call the ConstructParticle method before ConstructProcess,
 * so we're guaranteed that decay physics for example will apply to all particles.
 *
 * @author L. Deacon, S. Boogert & L. Nevay
 */

class BDSModularPhysicsList: public G4VModularPhysicsList
{
public:
  explicit BDSModularPhysicsList(G4String physicsList);
  virtual ~BDSModularPhysicsList();

  /// Call base class method to construct all particles from constructors,
  /// but also set the particle definition and rigidity in globals.
  virtual void ConstructParticle();

  /// Call base class method to construct all processes from constructors,
  /// but also set cuts and print physics table.
  virtual void ConstructProcess();

  /// Construct beam particle definition. Ensure that particle is instantiated
  /// from a Geant4 point of view.  'ffact' is typically 1 or -1 used to flip
  /// the sign of the rigidity for difference between convention and what's required.
  BDSParticleDefinition* ConstructBeamParticle(G4String particleName,
					       G4double totalEnergy,
					       G4double ffact = 1) const;
  
  /// Print out which physics lists are activated.
  void Print();

  /// Print all the processes by name as registered to the primary particle type.
  /// Note, this should only be done after the physics lists are fully constructed.
  void PrintPrimaryParticleProcesses() const;

  /// Print all constructed particle names. Note, this should only be done after the
  /// physics lists are fully constructed.
  void PrintDefinedParticles() const;

  /// Set Production cuts for photons, elecrons, positrons, protons and the default.
  virtual void SetCuts();

  void BuildAndAttachBiasWrapper(const GMAD::FastList<GMAD::PhysicsBiasing>& biases);

  inline G4bool UsingIons() const {return usingIons;}

private:
  /// Private default constructor to force use of supplied one.
  BDSModularPhysicsList();

  /// Ensure required beam particle has been constructed for Geant4 purposes.
  void ConstructBeamParticleG4(G4String name) const;
  
  /// Construct the minimum particle set required (gamma, electron, positron,
  /// proton and anti-proton.
  void ConstructMinimumParticleSet();

  /// Neutrinos are not constructed by default in many (most) physics lists
  /// yet this results in crashes when they're produced but not defined by
  /// physics processes, so purposively define for ones where it's a problem.
  void ConstructAllLeptons();

  /// Construct resonances and quarks - sometimes required explicitly.
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
  
  /// Typedef for function pointers to simplify syntax.
  typedef void(BDSModularPhysicsList::*Constructor)(void);

  /// A map of physics list names to their constructors.
  std::map<std::string, Constructor> physicsConstructors;

  /// A vector of all the physics list names for searching. This is
  /// constructed from the above map physicsConstructors.
  std::vector<G4String> physicsLists;

  /// Many physics lists require other physics lists - keep a record
  /// of which ones have been activated and if the required ones haven't,
  /// activate them. This is also constructed from the physicsConstructors map.
  std::map<G4String, G4bool> physicsActivated;

  /// Map of possible aliases for a given physics list.
  std::map<G4String, G4String> aliasToOriginal;
  
  G4OpticalPhysics* opticalPhysics;
  std::vector<G4VPhysicsConstructor*> constructors;

  /// Keep a local reference to global constants to avoid getting it all the time
  BDSGlobalConstants* globals;

  /// Flag as to whether em will be used - avoids duplicate processes being registered.
  G4bool emWillBeUsed;

  /// Flag telling whether ions are being used either in physics list or in beam particle.
  mutable G4bool usingIons;

  /// @{Physics constructor loader.
  void ChargeExchange();
  void Cherenkov();
  void CutsAndLimits();
  void Decay();
  void DecayRadioactive();
  void Em();
  void EmExtra();
  void EmLivermore();
  void EmLivermorePolarised();
  void EmLowEP();
  void EmPenelope();
  void EmSS();
  void EmWVI();
  void Em1();
  void Em2();
  void Em3();
  void Em4();
  void FTFPBERT();
  void FTFPBERTHP();
  void HadronicElastic();
  void HadronicElasticD();
  void HadronicElasticH();
  void HadronicElasticHP();
  void HadronicElasticLEND();
  void HadronicElasticXS();
  void Ion();
  void IonBinary();
  void IonElastic();
  void IonElasticQMD();
  void IonINCLXX();
  void LaserWire();
  void Muon();
  void Optical();
  void QGSPBERT();
  void QGSPBERTHP();
  void QGSPBIC();
  void QGSPBICHP();
  void Shielding();
  void SynchRad();
#if G4VERSION_NUMBER > 1019
  void EmGS();
#endif
#if G4VERSION_NUMBER > 1020
  void DecaySpin();
#endif
#if G4VERSION_NUMBER > 1022
  void IonPHP();
#endif
#if G4VERSION_NUMBER > 1029
  void DecayMuonicAtom();
#endif
  /// @}
};

#endif

