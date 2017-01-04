#ifndef BDSSAMPLERPLANE_H
#define BDSSAMPLERPLANE_H 

#include "BDSSampler.hh"

#include "globals.hh" // geant4 types / globals
#include "G4Transform3D.hh"

/** 
 * @brief Rectangular sampler with fixed thickness but variable x,y.
 * 
 * This produces a square box that's 4e-8m thick but has square half
 * width of boxHalfWidth. This thickness is chosen to be just above
 * the geometry tolerance set in bdsim.cc.  If any dimension is smaller 
 * than the geometry tolerance, Geant4 will exit.
 */

class BDSSamplerPlane: public BDSSampler
{
public:
  BDSSamplerPlane(G4String name,
		  G4double boxHalfWidth);

  virtual ~BDSSamplerPlane(){;}

  /// Access the sampler plane length in other classes.
  inline static G4double ChordLength();

private:
  /// Private default constructor to ensure use of provided one.
  BDSSamplerPlane();

  /// The chord length for all is fixed and can be static.
  /// Geometry tolerance is 1e-12 and dz of the box must be greater than this
  static constexpr G4double chordLength = 0.01*CLHEP::nm;
};

inline G4double BDSSamplerPlane::ChordLength()
{return chordLength;}

#endif
