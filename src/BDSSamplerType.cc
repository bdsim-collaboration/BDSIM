#include "BDSSamplerType.hh"
#include "BDSDebug.hh"

#include <iterator>
#include <map>
#include <string>

// dictionary for BDSSamplerType
template<>
std::map<BDSSamplerType, std::string>* BDSSamplerType::dictionary =
  new std::map<BDSSamplerType, std::string> ({
  {BDSSamplerType::none,     "none"},
  {BDSSamplerType::plane,    "plane"},
  {BDSSamplerType::cylinder, "cylinder"}
});
