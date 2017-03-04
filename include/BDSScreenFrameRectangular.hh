#ifndef BDSSCREENFRAMERECTANGULAR_H
#define BDSSCREENFRAMERECTANGULAR_H

#include "BDSScreenFrame.hh"

#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4TwoVector.hh"

class G4Material;

/**
 * @brief Rectangular screen frame.
 *
 * Rectangular hollow frame / tube. Length is determined by size.z().
 * 
 * @author Lawrence Deacon
 */

class BDSScreenFrameRectangular: public BDSScreenFrame
{
public:
  BDSScreenFrameRectangular(G4String      name,
			    G4ThreeVector size,///< Full width in each dimension of inner surface.
			    G4TwoVector   windowSize,   ///< Full width of h,v of outer surface.
			    G4TwoVector   windowOffset, ///< Offset of intertior to exterior.
			    G4Material*   material);
  
  virtual ~BDSScreenFrameRectangular();

  /// Construct the geometry.
  virtual void Build();
  
private:
  /// Remove default constructor to force use of supplied one.
  BDSScreenFrameRectangular() = delete;
};

#endif
