/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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
#ifndef BDSIMLINK_H
#define BDSIMLINK_H
#include "BDSHitSamplerLink.hh"
#include "BDSLinkRunAction.hh"
#include "G4String.hh"

#include <map>
#include <string>
#include <vector>

class BDSBunch;
class BDSComponentConstructor;
class BDSComponentFactoryUser;
class BDSLinkDetectorConstruction;
class BDSLinkRunManager;
class BDSOutput;
class BDSParser;
class BDSParticleCoordsFull;
class BDSParticleDefinition;
class BDSParticleExternal;

/** 
 * @brief Interface class to use BDSIM with trackers.
 *
 * First way to use:
 * bds = new BDSIMLink(argc, argv);
 * bds->TrackThin(...)
 * 
 * Second way to use (delayed initialisation):
 * bds = new BDSIMLink();
 * // modify bds in some way
 * bds->Initialise(argc, argv);
 * bds->TrackThin(...)
 *
 * @author Laurie Nevay
 */

class BDSIMLink
{
public:
  /// Construct an instance but don't initialise. Requires initialisation with
  /// arguments argc and arv
  explicit BDSIMLink(BDSBunch* bunchIn);

  /// Initialise everything given these arguments.
  int Initialise(int argc, char** argv, bool usualPrintOut=true);

  /// Construct and initialise BDSIM.
  BDSIMLink(int argc, char** argv, bool usualPrintOut=true);

  /// The destructor opens the geometry in Geant4 and deletes everything.
  ~BDSIMLink();

  /// Accessor as to whether BDSIM kernel is initialised - ie all geometry and physics
  /// constructed.
  inline bool Initialised() const {return initialised;};

  inline int InitialisationResult() const {return initialisationResult;}

  /// Generate nGenerate events. If the default argument -1 is used, the number is taken
  /// from the standard input e.g. the executable option ngenerate and then the one specified
  /// in the input gmad files as an option.
  void BeamOn(int nGenerate=-1);

  void SelectLinkElement(const std::string& elementName);
  void SelectLinkElement(int index);

  void AddLinkCollimator(const std::string& collimatorName,
			 const std::string& materialName,
			 G4double length,
			 G4double aperture,
			 G4double rotation,
			 G4double xOffset,
			 G4double yOffset);

  BDSHitsCollectionSamplerLink* SamplerHits() const;
  void ClearSamplerHits() {runAction->ClearSamplerHits();}
  
private:
  /// The main function where everything is constructed.
  int Initialise();
  
  bool   ignoreSIGINT;         ///< For cmake testing.
  bool   usualPrintOut;        ///< Whether to allow the usual cout output.
  bool   initialised;          ///< Whether initialisation was completed safely
  int    initialisationResult; ///< Possible to not finish initialisation but have completed ok - flag for this.
  int    argcCache;            ///< Cache of argc.
  char** argvCache;            ///< Cache of argv.

  /// @{ Cache of main object in BDSIM.
  BDSParser*         parser;
  BDSOutput*         bdsOutput;
  BDSBunch*          bdsBunch;
  BDSLinkRunManager* runManager;
  BDSLinkDetectorConstruction* construction;
  BDSLinkRunAction*  runAction;
  /// @}
  
  std::vector<BDSParticleExternal*> externalParticles;
  std::map<std::string, int>        nameToElementIndex;
  int                               currentElementIndex; ///< Element to track in.
};

#endif