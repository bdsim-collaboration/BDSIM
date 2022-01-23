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
#ifndef BDSPARSER_H
#define BDSPARSER_H

#include <map>
#include <string>
#include <vector>

#include "parser/parser.h"

/**
 * @brief Const entry to parser objects
 *
 * Singleton pattern
 *
 * @author Jochem Snuverink
 */
  
class BDSParser: private GMAD::Parser
{
public:
  /// No default constructor.
  BDSParser() = delete;
  /// Constructor method.
  static BDSParser* Instance(const std::string& filename);
  /// Access method.
  static BDSParser* Instance();
  /// Returns if parser is initialised.
  static bool IsInitialised();
  /// Destructor.
  virtual ~BDSParser();
  
  /// Return options
  const GMAD::Options& GetOptions() const {return options;}

  /// Return bare options base class.
  const GMAD::OptionsBase* GetOptionsBase() const {return dynamic_cast<const GMAD::OptionsBase*>(&options);}

  /// Return beam.
  const GMAD::Beam& GetBeam() const {return beam;}

  /// Return beam non-const. Required when loading a file and need to change beam parameters to match.
  GMAD::Beam& GetBeam() {return beam;}

  /// Return bare beam base class.
  const GMAD::BeamBase* GetBeamBase() const {return dynamic_cast<const GMAD::BeamBase*>(&beam);}

  /// Amalgamate the input options with the ones stored in the parser.
  void AmalgamateOptions(const GMAD::Options& optionsIn);
  /// Amalgamate the input beam definition with the ones stored in teh parser.
  void AmalgamateBeam(const GMAD::Beam& beamIn, bool recreate);
  /// Check options for consistency. This also checks the beam options.
  void CheckOptions();
  
  /// Return the beamline. See GMAD::Parser. Our inheritance here is private, so
  /// we re-expose this function as public for use in BDSIM, without redefining it
  /// or reimplementing it. const FastList<Element>& GMAD::Parser::GetBeamline() const;
  using GMAD::Parser::GetBeamline;
  
  /// Import privately inherited function to access sampler filter map.
  /// const std::map<int, std::set<int>>& GetSamplerFilterIDToSet() const {return samplerFilterIDToSet;}
  using GMAD::Parser::GetSamplerFilterIDToSet;

  /// Return sequence.
  inline const GMAD::FastList<GMAD::Element>& GetSequence(const std::string& name) {return get_sequence(name);}
  
  /// Return an element definition. Returns nullptr if not found. Note the element_list is
  /// emptied after parsing.
  inline const GMAD::Element* GetElement(const std::string& name) {return find_element_safe(name);}
  
  /// Return the vector of atom objects.
  inline std::vector<GMAD::Atom> GetAtoms() const {return atom_list.getVector();}
  
  /// Return a placement element definition. Returns nullptr if not found. Only searches
  /// elements used in the placement.
  inline const GMAD::Element* GetPlacementElement(const std::string& name) {return find_placement_element_safe(name);}
  
  /// Return biasing list.
  inline const GMAD::FastList<GMAD::PhysicsBiasing>& GetBiasing() const {return xsecbias_list;}
  inline const std::vector<GMAD::PhysicsBiasing> GetBiasingVector() const {return xsecbias_list.getVector();}

  /// Return cavity model list.
  inline std::vector<GMAD::CavityModel> GetCavityModels() const {return cavitymodel_list.getVector();}

  /// Return colour model list.
  inline std::vector<GMAD::NewColour> GetColours() const {return colour_list.getVector();}

  /// Return crystal model list.
  inline std::vector<GMAD::Crystal> GetCrystals() const {return crystal_list.getVector();}

  /// Return coolingchannel model list.
  inline std::vector<GMAD::CoolingChannel> GetCoolingChannels() const {return coolingchannel_list.getVector();}
  
  /// Return a cooling channel object by name. Returns nullptr if doesn't exist.
  /// Uses a map of names to objects that is cached for resuse. If doesn't exist or
  /// exapnded, it's built up again.
  const GMAD::CoolingChannel* GetCoolingChannel(const std::string& objectName);

  /// Return the vector of field objects.
  inline std::vector<GMAD::Field> GetFields() const {return field_list.getVector();}

  /// Return material list.
  inline std::vector<GMAD::Material> GetMaterials() const {return material_list.getVector();}
  
  /// Return the vector of placement objects.
  inline std::vector<GMAD::Placement> GetPlacements() const {return placement_list.getVector();}

  /// Query list.
  inline std::vector<GMAD::Query> GetQuery() const {return query_list.getVector();}
  
  /// Return region list.
  inline std::vector<GMAD::Region> GetRegions() const {return region_list.getVector();}

  /// Return sampler placement list.
  inline std::vector<GMAD::SamplerPlacement> GetSamplerPlacements() const {return samplerplacement_list.getVector();}

  /// Return scorer list.
  inline std::vector<GMAD::Scorer> GetScorers() const {return scorer_list.getVector();}

  /// Return scorermesh list.
  inline std::vector<GMAD::ScorerMesh> GetScorerMesh() const {return scorermesh_list.getVector();}

  /// Return blm list.
  inline std::vector<GMAD::BLMPlacement> GetBLMs() const {return blm_list.getVector();}

  /// Return aperture list.
  inline std::vector<GMAD::Aperture> GetApertures() const {return aperture_list.getVector();}
  
protected:
  /// Constructor from filename.
  explicit BDSParser(const std::string& filename);

private:
  /// Instance.
  static BDSParser* instance;

  /// Keep a cached map of all cooling object definitions to retrieve them by name.
  /// Do this as these don't map 1:1 to a simple recipe object and we don't want to
  /// loop over them all each time we construct one.
  std::map<std::string, GMAD::CoolingChannel*>* coolingChannelObjectMap;
};

#endif
