#ifndef BDSSCREENFRAME_H
#define BDSSCREENFRAME_H

#include "globals.hh"
#include "G4TwoVector.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"

class G4LogicalVolume;
class G4Material;
class G4VisAttributes;

/**
 * @brief A frame for the vacuum window in e.g. BDSMultilayerScreen.
 *
 * This is a virtual class and the derived class must implement Build()
 * that ultimately sets the member logVol.
 * 
 * @author Lawrence Deacon.
 */

class BDSScreenFrame
{
public:
  BDSScreenFrame(G4String      name,
		 G4ThreeVector size,
		 G4TwoVector   windowSize,
		 G4TwoVector   windowOffset,
		 G4Material*   material);

  virtual ~BDSScreenFrame();

  /// Build method to construct geometry.
  virtual void Build() = 0;

  /// Accessor.
  G4LogicalVolume* LogVol() const {return logVol;}

protected:
  /// Set the visual attributes to member logVol.
  virtual void SetVisAtts();

  /// Construct default visualisation attribute for light transparent grey.
  void SetDefaultVisAtts();

  /// Attach visualisation attributes to logical volume if they exist and if not
  /// use the default.
  void SetVisAtts(G4LogicalVolume* logVolIn,
		  G4VisAttributes* visAttsIn = nullptr);
  
  G4String         name;
  G4ThreeVector    size;
  G4TwoVector      windowSize;
  G4TwoVector      windowOffset;
  G4Material*      material;
  G4LogicalVolume* logVol;
  G4bool           checkOverlaps; ///< Cache of checking overlaps from global constants.
  G4VisAttributes* visAtt;

private:
  BDSScreenFrame() = delete;
};

#endif
