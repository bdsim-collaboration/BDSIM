#ifndef BDSPARSER_H
#define BDSPARSER_H

#include <list>
#include <map>
#include <string>
#include <vector>

#include "parser/parser.h"

/**
 * @brief Entry to parser objects
 *
 * Singleton pattern
 *
 * @author Jochem Snuverink <Jochem.Snuverink@rhul.ac.uk>
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
  const GMAD::Options& GetOptions()const;
  /// Return beamline
  const GMAD::FastList<GMAD::Element>& GetBeamline()const;
  /// Return biasing list
  const GMAD::FastList<GMAD::PhysicsBiasing>& GetBiasing()const;
  /// Return material list
  const std::list<GMAD::Element>& GetMaterials()const;
  /// Return atom list
  const std::list<GMAD::Element>& GetAtoms()const;

protected:
  /// Constructor from filename
  BDSParser(std::string filename);

private:
  /// Instance
  static BDSParser* instance;
};

#endif
