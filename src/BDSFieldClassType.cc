#include "BDSDebug.hh"
#include "BDSFieldClassType.hh"

#include "globals.hh"
#include "G4String.hh"

#include <map>
#include <string>

// dictionary for BDSFieldClassType for reflexivity
template<>
std::map<BDSFieldClassType, std::string>* BDSFieldClassType::dictionary =
  new std::map<BDSFieldClassType, std::string> ({
      {BDSFieldClassType::magnetic,        "magnetic"},
      {BDSFieldClassType::electric,        "electric"},
      {BDSFieldClassType::electromagnetic, "electromagnetic"},
      {BDSFieldClassType::irregular,       "irregular"}
    });

BDSFieldClassType BDS::DetermineFieldClassType(BDSFieldType fType)
{
  switch (fType.underlying())
    {
    case BDSFieldType::bzero:
    case BDSFieldType::bmap1d:
    case BDSFieldType::bmap2d:
    case BDSFieldType::bmap3d:
    case BDSFieldType::bmap4d:
    case BDSFieldType::mokka:
    case BDSFieldType::solenoid:
    case BDSFieldType::dipole:
    case BDSFieldType::dipole3d:
    case BDSFieldType::quadrupole:
    case BDSFieldType::dipolequadrupole:
    case BDSFieldType::sextupole:
    case BDSFieldType::octupole:
    case BDSFieldType::decapole:
    case BDSFieldType::multipole:
    case BDSFieldType::muonspoiler:
    case BDSFieldType::skewquadrupole:
    case BDSFieldType::skewsextupole:
    case BDSFieldType::skewoctupole:
    case BDSFieldType::skewdecapole:
      {return BDSFieldClassType::magnetic;}
    case BDSFieldType::ezero:
    case BDSFieldType::emap1d:
    case BDSFieldType::emap2d:
    case BDSFieldType::emap3d:
    case BDSFieldType::emap4d:
    case BDSFieldType::rf:
      {return BDSFieldClassType::electric;}
    case BDSFieldType::none:
    case BDSFieldType::ebzero:
    case BDSFieldType::ebmap1d:
    case BDSFieldType::ebmap2d:
    case BDSFieldType::ebmap3d:
    case BDSFieldType::ebmap4d:
    case BDSFieldType::rfcavity:
      {return BDSFieldClassType::electromagnetic;}
    case BDSFieldType::teleporter:
      {return BDSFieldClassType::irregular;}
    default:
      {return BDSFieldClassType::electromagnetic;}
    }
}
