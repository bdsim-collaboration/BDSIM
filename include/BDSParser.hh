#ifndef BDSPARSER_H
#define BDSPARSER_H

#include <map>
#include <string>
#include <vector>

#include "parser/parser.h"

/**
 * @brief Entry to parser objects
 *
 * Singleton pattern
 *
 * @author Jochem Snuverink
 */
  
class BDSParser : private GMAD::Parser
{
public:
  /// No default constructor
  BDSParser() = delete;
  /// Constructor method
  static BDSParser* Instance(std::string filename);
  /// Access method
  static BDSParser* Instance();
  /// Destructor
  virtual ~BDSParser();
  
  /// Return options
  const GMAD::Options& GetOptions() const {return options;}

  /// Amalgamate the input options with the ones stored in the parser.
  void AmalgamateOptions(const GMAD::Options& optionsIn);
  /// Check options for consistency
  void CheckOptions();

  /// Return the vector of region objects.
  inline const std::vector<GMAD::Atom>& GetAtoms() const {return atom_list;}
  
  /// Return beamline
  inline const GMAD::FastList<GMAD::Element>& GetBeamline() const {return beamline_list;}
  
  /// Return biasing list
  inline const GMAD::FastList<GMAD::PhysicsBiasing>& GetBiasing() const {return xsecbias_list;}

  /// Return cavity model list
  inline const std::vector<GMAD::CavityModel>& GetCavityModels() const {return cavitymodel_list;}
  
  /// Return the vector of field objects.
  inline const std::vector<GMAD::Field>& GetFields() const {return field_list;}

  /// Return material list
  inline const std::vector<GMAD::Material>& GetMaterials() const {return material_list;}
  
  /// Return region list
  inline const std::vector<GMAD::Region>& GetRegions() const {return region_list;}
  
  /// Return the vector of placement objects.
  inline const std::vector<GMAD::Placement>& GetPlacements() const {return placement_list;}

protected:
  /// Constructor from filename
  BDSParser(std::string filename);

private:
  /// Instance
  static BDSParser* instance;
};

#endif
