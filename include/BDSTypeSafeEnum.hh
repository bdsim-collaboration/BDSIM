#ifndef BDSTYPESAFEENUM_H
#define BDSTYPESAFEENUM_H 

#include <map>
#include <ostream>
#include <string>

/**
 * @brief Improve type-safety of native enum data type in C++.
 * 
 * @author Jochem Snuverink
 *
 * more info at 
 * https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Type_Safe_Enum
 *
 * the class includes a dictionary map that returns a customisable std::string
 * 
 * Example:
 * struct color_def {
 *   enum type { red, green, blue };
 * };
 * typedef BDSTypeSafeEnum<color_def,int> color;
 * color c = color::red;
 * 
 */

template<typename def, typename inner = typename def::type>
class BDSTypeSafeEnum : public def
{
  typedef inner type;
  inner val;

protected:
  /// static map from to string (optional), needs to be defined explicitly
  /// can be used for printing (in << operator)
  // pointer used since static memory possibly doesn't know size of map at creation
  static std::map<BDSTypeSafeEnum<def,inner>,std::string>* dictionary;
  
public:
 
  BDSTypeSafeEnum() : val() {}
  BDSTypeSafeEnum(type v) : val(v) {}
  /// return underlying value (can be used in switch statement)
  type underlying() const { return val; }

  std::string ToString() const {
    if (dictionary) {return (*dictionary)[val];}
    // if no dictionary defined return empty string
    return "";
  }
  
  ///@{ operators for ordering
  friend bool operator == (const BDSTypeSafeEnum & lhs, const BDSTypeSafeEnum & rhs) { return lhs.val == rhs.val; }
  friend bool operator != (const BDSTypeSafeEnum & lhs, const BDSTypeSafeEnum & rhs) { return lhs.val != rhs.val; }
  friend bool operator <  (const BDSTypeSafeEnum & lhs, const BDSTypeSafeEnum & rhs) { return lhs.val <  rhs.val; }
  friend bool operator <= (const BDSTypeSafeEnum & lhs, const BDSTypeSafeEnum & rhs) { return lhs.val <= rhs.val; }
  friend bool operator >  (const BDSTypeSafeEnum & lhs, const BDSTypeSafeEnum & rhs) { return lhs.val >  rhs.val; }
  friend bool operator >= (const BDSTypeSafeEnum & lhs, const BDSTypeSafeEnum & rhs) { return lhs.val >= rhs.val; }
  ///@}
  /// stream operator, returns string from dictionary if defined, else underlying type (typically int)
  friend std::ostream& operator<< (std::ostream &out, const BDSTypeSafeEnum& a) {
    if (dictionary) {out << a.ToString();}
    else {out << a.underlying();}
    return out;
  }
};

/// Initialisation for clang 8.1 warning suppression.
template<typename def, typename inner>
std::map<BDSTypeSafeEnum<def,inner>, std::string>* BDSTypeSafeEnum<def, inner>::dictionary = nullptr;

#endif
