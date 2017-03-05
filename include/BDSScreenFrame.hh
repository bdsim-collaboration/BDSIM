#ifndef BDSSCREENFRAME_H
#define BDSSCREENFRAME_H

#include "globals.hh"
#include "G4TwoVector.hh"
#include "G4ThreeVector.hh"
#include "G4VisAttributes.hh"

class G4LogicalVolume;
class G4Material;


/**
 * @brief A frame for the vacuum window in e.g. BDSMultilayerScreen.
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
  G4String         name;
  G4ThreeVector    size;
  G4TwoVector      windowSize;
  G4TwoVector      windowOffset;
  G4Material*      material;
	G4LogicalVolume* logVol;
	G4VisAttributes* visAtt;
	virtual void SetVisAtts();
		void SetVisAtts(G4LogicalVolume* logVolIn, G4VisAttributes* visAttsIn = nullptr);

private:
  BDSScreenFrame() = delete;
		void SetDefaultVisAtts();
};

#endif
