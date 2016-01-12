#ifndef BDSMAGNETSTRENGTH_H
#define BDSMAGNETSTRENGTH_H

#include "globals.hh" // geant4 globals / types

#include <map>
#include <vector>

/**
 * @brief Efficient storage of magnet strengths.
 *
 * Based on std::map, this class stores magnet strengths. As there will be one of 
 * these for every magnet and they may in future have to go up to high order 
 * normal plus skew (~40 doubles), it is more efficient to only store the required
 * parameters. A pure quadrupole has no need of k20s, but a multipole may do. A static
 * vector contains the possible magnet strength parameters that can be checked against.
 * If a parameter is not specified in the map, its value is return as 0.
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSMagnetStrength
{
public:
  /// Default constructor does nothing as class will return 0 for uninitialised keys
  BDSMagnetStrength(){;}
  /// This constructor allows instantiation with a map of keys and values.
  BDSMagnetStrength(std::map<G4String, G4double> keyvalues);

  /// Output stream
  friend std::ostream& operator<< (std::ostream &out, BDSMagnetStrength const &st);

  /// Accessors with array / map [] operator
  G4double& operator[](const G4String key);
  const G4double& operator[](G4String key) const;

  /// Accessor for normal component keys
  inline std::vector<G4String> NormalComponentKeys() const;

  /// Accessor for skew component keys
  inline std::vector<G4String> SkewComponentKeys() const;

  /// Accessor for all normal components
  std::vector<G4double> NormalComponents() const;

  /// Accessor for all skew components
  std::vector<G4double> SkewCompoents() const;
  
private:
  /// A map containing the values for various magnet strengths. It only contains
  /// the set magnet strengths. If a strength isn't specified, it's assumed to be zero
  /// without having to store the 0 value.
  std::map<G4String, G4double> strengths;

  /// Whether or not the supplied key is a valid magnet strength parameter
  G4bool ValidKey(const G4String key) const;

  /// Accessor similar to [] but without linear search through keys to check validity
  /// for fast internal use.
  const G4double& GetValue(const G4String key) const;

  /// Vector of the allowed strength parameters
  static const std::vector<G4String> keys;

  /// Vector of the normal component strength parameters
  static const std::vector<G4String> normalComponentKeys;

  /// Vector of the normal component strength parameters
  static const std::vector<G4String> skewComponentKeys;

  /// Keep a single copy of 0.0 as it needs to be returned as a reference not a value
  static const G4double zero;

  /// Dummy variable that can be overwritten
  static G4double variable;
};

inline std::vector<G4String> NormalComponentKeys() const
{return normalComponentKeys;}

inline std::vector<G4String> SkewComponentKeys() const
{return skewComponentKeys;}


#endif
