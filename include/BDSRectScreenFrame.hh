#ifndef BDSRECTSCREENFRAME_H
#define BDSRECTSCREENFRAME_H

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

class BDSRectScreenFrame: public BDSScreenFrame
{
public:
  BDSRectScreenFrame(G4String      name,
		     G4ThreeVector size,  ///< Full width in each dimension of inner surface.
		     G4TwoVector   windowSize, ///< Full width of h,v of outer surface.
		     G4TwoVector   windowOffset,
		     G4Material*   material);
  
  virtual ~BDSRectScreenFrame();

  /// Construct the geometry.
  virtual void Build();
  
private:
  /// Remove default constructor to force use of supplied one.
  BDSRectScreenFrame() = delete;
};

#endif
