#ifndef BDSBEAMLINESET_H
#define BDSBEAMLINESET_H

class BDSBeamline;

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

  /// Destroy objects pointed to by this instance.
  void DeleteContents();

  BDSBeamline* massWorld              = nullptr;
  BDSBeamline* curvilinearWorld       = nullptr;
  BDSBeamline* curvilinearBridgeWorld = nullptr;
  BDSBeamline* endPieces              = nullptr;
};

#endif
