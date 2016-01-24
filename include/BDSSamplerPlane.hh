#ifndef BDSSAMPLERPLANE_H
#define BDSSAMPLERPLANE_H 

#include "BDSSampler.hh"

class G4Transform3D;

/** 
 * @brief Rectangular sampler with fixed thickness but variable x,y.
 * 
 * This produces a square box that's 1pm thick but has square half
 * width of boxHalfWidth.
 */

class BDSSamplerPlane: public BDSSampler
{
public:
  BDSSamplerPlane(G4String       name,
		  G4Transform3D* transform,
		  G4double       boxHalfWidth);

  virtual ~BDSSamplerPlane();

private:
  /// Private default constructor to ensure use of provided one.
  BDSSamplerPlane();

  /// The chord length for all is fixed and can be static.
  static const G4double chordLength;
};

#endif
