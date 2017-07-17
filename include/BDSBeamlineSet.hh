#ifndef BDSBEAMLINESET_H
#define BDSBEAMLINESET_H

/**
 * @brief Simple struct to return a beamline plus associated beam lines.
 *
 * @author L. Nevay
 */

class BDSBeamlineSet
{
public:
  BDSBeamlineSet(){;}
  ~BDSBeamlineSet(){;}

  BDSBeamline* massWorld              = nullptr;
  BDSBeamline* curvilinearWorld       = nullptr;
  BDSBeamline* curvilinearBridgeWorld = nullptr;
};

#endif
