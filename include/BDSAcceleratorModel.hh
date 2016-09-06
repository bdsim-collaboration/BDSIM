#ifndef BDSACCELERATORMODEL_H
#define BDSACCELERATORMODEL_H

#include "globals.hh"         // geant4 globals / types

#include <vector>

class BDSBeamline;
class G4LogicalVolume;
class G4VPhysicalVolume;

/**
 * @brief A holder class for all representations of the
 * accelerator model created in BDSIM. Can be extend to 
 * allow inspection of the model. Holds the readout geometry
 * physical world in a location independent of detector 
 * construction.
 * 
 * In future, there may be more than several beamlines - 
 * a flat one, and perhaps grouped into a more hierarchical
 * version.  These can be contained here and this class can
 * be extended as required.
 * 
 * @author
 */

class BDSAcceleratorModel
{
public:
  static BDSAcceleratorModel* Instance();
  ~BDSAcceleratorModel();

  /// Register the physical volume of the world
  inline void               RegisterWorldPV(G4VPhysicalVolume* worldIn)
  {worldPV = worldIn;}    

  /// Access the physical volume of the world
  inline G4VPhysicalVolume* GetWorldPV() const
  {return worldPV;}

  /// Register the flat beam line - flat means that each element in the beamline represents
  /// one element in the accelerator lattice
  inline void               RegisterFlatBeamline(BDSBeamline* beamlineIn)
  {flatBeamline = beamlineIn;}

  /// Access flat beam line
  inline BDSBeamline*       GetFlatBeamline() const
  {return flatBeamline;}

  /// Register the curvilinear geometry beam line.
  inline void RegisterCurvilinearBeamline(BDSBeamline* beamlineIn)
  {curvilinearBeamline = beamlineIn;}

  inline BDSBeamline*       GetCurvilinearBeamline() const
  {return curvilinearBeamline;}

  /// Register the beam line containing all the magnet supports
  inline void               RegisterSupportsBeamline(BDSBeamline* beamlineIn)
  {supportsBeamline = beamlineIn;}

  /// Access the beam line containing all the magnet supports
  inline BDSBeamline*       GetSupportsBeamline() const
  {return supportsBeamline;}

  /// Register the beam line containing all the tunnel segments
  inline void               RegisterTunnelBeamline(BDSBeamline* beamlineIn)
  {tunnelBeamline = beamlineIn;}

  /// Access the beam line containing all the tunnel segments
  inline BDSBeamline*       GetTunnelBeamline() const
  {return tunnelBeamline;}

  /// Register any physical volumes that should be managed - typically from world placement
  inline void               RegisterPhysicalVolume(G4VPhysicalVolume* physicalVolume);

  /// Register any physical volumes that should be managed - typically from world placement
  inline void               RegisterPhysicalVolume(std::vector<G4VPhysicalVolume*> physicalVolumes);

  /// Register the beam line of end pieces.
  inline void               RegisterEndPieceBeamline(BDSBeamline* beamlineIn)
  {endPieceBeamline = beamlineIn;}

  /// Access the beam line of end pieces.
  inline BDSBeamline*       GetEndPieceBeamline() const
  {return endPieceBeamline;}
  
private:
  BDSAcceleratorModel(); ///< Default constructor is private as singleton.

  static BDSAcceleratorModel* _instance;

  G4VPhysicalVolume* worldPV;             ///< Physical volume of the mass world.
  BDSBeamline*       flatBeamline;        ///< Flat beam line.
  BDSBeamline*       curvilinearBeamline; ///< Curvilinear geometry beamline.
  BDSBeamline*       supportsBeamline;    ///< Element supports beam line.
  BDSBeamline*       tunnelBeamline;      ///< Tunnel segments beam line.
  BDSBeamline*       endPieceBeamline;    ///< End Pieces beam line.
};

#endif
