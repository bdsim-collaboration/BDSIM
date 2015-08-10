#ifndef BDSMAGNETTYPE_H
#define BDSMAGNETTYPE_H

#include "BDSTypeSafeEnum.hh"
#include "globals.hh"         // geant4 globals / types
#include <map>

/**
 * @brief Type definition for everything that inherits
 * BDSMagnet - ie is a magnet
 * 
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */


/// Magnet types enum - note, these must be manually duplicated in the
/// corresponding source file for this header in a string map!
struct magnettypes_def {
  enum type { decapole, hkicker, vkicker, muspoiler, octupole, quadrupole,
	      sectorbend, rectangularbend, rfcavity, sextupole,
	      solenoid, multipole};
};

typedef BDSTypeSafeEnum<magnettypes_def,int> BDSMagnetType;

/// Here we have a static map that allows us to look up a string version
/// so we can use the BDSMagnetType in the initialiser list of the derived
/// magnet class and retain the constness in BDSAcceleratorComponent. This is
/// needed to provide magnet types on output to geometry writer.
struct BDSMagnetTypeMap
{
  static std::map<BDSMagnetType, G4String> types;
};

#endif



