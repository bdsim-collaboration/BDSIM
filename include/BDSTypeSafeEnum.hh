#ifndef BDSTypeSafeEnum_h
#define BDSTypeSafeEnum_h 

#include <ostream>

/**
 * @brief Improve type-safety of native enum data type in C++.
 * 
 * @author Jochem Snuverink <Jochem.Snuverink@rhul.ac.uk>
 *
 * more info at 
 * https://en.wikibooks.org/wiki/More_C%2B%2B_Idioms/Type_Safe_Enum
 *
 * Example:
 * struct color_def {
 *   enum type { red, green, blue };
 * };
 * typedef BDSTypeSafeEnum<color_def> color;
 * color c = color::red;
 * 
 */

template<typename def, typename inner = typename def::type>
class BDSTypeSafeEnum : public def
{
  typedef inner type;
  inner val;
 
public:
 
  BDSTypeSafeEnum() : val() {}
  BDSTypeSafeEnum(type v) : val(v) {}
  type underlying() const { return val; }
 
  friend bool operator == (const BDSTypeSafeEnum & lhs, const BDSTypeSafeEnum & rhs) { return lhs.val == rhs.val; }
  friend bool operator != (const BDSTypeSafeEnum & lhs, const BDSTypeSafeEnum & rhs) { return lhs.val != rhs.val; }
  friend bool operator <  (const BDSTypeSafeEnum & lhs, const BDSTypeSafeEnum & rhs) { return lhs.val <  rhs.val; }
  friend bool operator <= (const BDSTypeSafeEnum & lhs, const BDSTypeSafeEnum & rhs) { return lhs.val <= rhs.val; }
  friend bool operator >  (const BDSTypeSafeEnum & lhs, const BDSTypeSafeEnum & rhs) { return lhs.val >  rhs.val; }
  friend bool operator >= (const BDSTypeSafeEnum & lhs, const BDSTypeSafeEnum & rhs) { return lhs.val >= rhs.val; }
  friend std::ostream& operator<< (std::ostream &out, const BDSTypeSafeEnum& a) {out << a.underlying(); return out;}
};

#endif
