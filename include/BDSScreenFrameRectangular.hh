#ifndef BDSIM_BDSSCREENFRAMERECTANGULAR_H
#define BDSIM_BDSSCREENFRAMERECTANGULAR_H

#include "BDSScreenFrame.hh"
#include "G4RotationMatrix.hh"
#include "G4LogicalVolume.hh"

class G4Material;

/**
 * @brief Rectangular screen frame.
 *
 * Rectangular hollow frame / tube. Length is determined by size.z().
 *
 * @author Lawrence Deacon
 */


class BDSScreenFrameRectangular : public BDSScreenFrame{
public:
    BDSScreenFrameRectangular(G4String name,
                              G4ThreeVector size,
                              G4TwoVector windowSize,
                              G4TwoVector windowOffset,
                              G4Material* material);

    virtual ~BDSScreenFrameRectangular();
    G4LogicalVolume* CavityLogVol(){return cavityLogVol;}

    /// Construct the geometry
    virtual void Build();
    /// Place the geometry
    void Place(G4RotationMatrix* rot, G4ThreeVector pos, G4LogicalVolume* motherVol);
protected:
    virtual void SetVisAtts();
private:
    /// Remove default constuctor to force use of supplied one.
    BDSScreenFrameRectangular() = delete;
    G4LogicalVolume* cavityLogVol;
    G4String cavityName;
    G4ThreeVector cavityPos;
    G4RotationMatrix* zeroRot;
};


#endif //BDSIM_BDSSCREENFRAMERECTANGULAR_H
