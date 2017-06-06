#ifndef BDSBEAMLINEPLACEMENT_H
#define BDSBEAMLINEPLACEMENT_H

class BDSBeamline;
class BDSGlobalConstants;

class G4VPhysicalVolume;

/**
 * @brief Common functionality of placing beam line in a parallel world.
 *
 * @author Laurie Nevay
 */

class BDSBeamlinePlacement
{
public:
  BDSBeamlinePlacement();
  virtual ~BDSBeamlinePlacement();

  void PlaceBeamlineInWorld(G4VPhysicalVolume*        world,
			    BDSBeamline*              beamline,
			    const BDSGlobalConstants* globals);
};

#endif
