#ifndef TRK_h
#define TRK_h

#include <cmath>

namespace TRK {
  // tracking parameters
  /// steps for thin lens tracking
  const int DEFAULT_TRACKING_STEPS=10;
  /// maximum number of turns for rings
  const int NR_TURNS=200;

  // constants
  const double SOL=2.9e8;
  const double e = 1.6e-19;

  /** tracking type
   * thin: thin lens tracking
   * thin symplectic
   * thick: thick lens tracking
   * hybrid: Geant4 field stepper
   */
  enum TRKType {thin, thinsymplectic, thick, hybrid};

  /** 
   * fast way to calculate both sinh and cosh
   * syntax similar to sincos(phi,s,c),
   * except sincosh expects doubles and no pointers!
   */
  static inline void sincosh(const double a, double &s, double &c) {
    
    if (std::abs(a) <= 0.05) {
      s = sinh(a);
      c = sqrt(1.0 + s*s);
    } else {
      double ea = exp(a);
      double inv_ea = 1./(ea);
      s = (ea - inv_ea)*0.5;
      c = (ea + inv_ea)*0.5;
    }
  }
}

#endif
