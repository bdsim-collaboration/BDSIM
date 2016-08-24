#ifndef BDSACCELERATORMODEL_H
#define BDSACCELERATORMODEL_H

#include "globals.hh"         // geant4 globals / types

#include <vector>

class BDSBeamline;
class BDSFieldObjects;
class G4LogicalVolume;
class G4VPhysicalVolume;

/**
 * @brief A holder class for all representations of the
 * accelerator model created in BDSIM. 
 *
 * This can be extended to 
 * allow inspection of the model. Holds the readout geometry
 * physical world in a location independent of detector 
 * construction.
 * 
 * In future, there may be several more beamlines than just
 * a flat one, and perhaps grouped into a more hierarchical
 * version.  These can be contained here and this class can
 * be extended as required.
 * 
 * @author Laurie Nevay
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

  /// Register the physical volume for the read out geometry
  inline void               RegisterReadOutWorldPV(G4VPhysicalVolume* readOutWorldIn)
  {readOutWorldPV = readOutWorldIn;}

  /// Access the physical volume for the read out geometry
  inline G4VPhysicalVolume* GetReadOutWorldPV() const
  {return readOutWorldPV;}

  /// Register the logical volume for the read out geometry - needed
  /// for the placement of tunnel read out geometry items
  inline void               RegisterReadOutWorldLV(G4LogicalVolume* readOutWorldIn)
  {readOutWorldLV = readOutWorldIn;}

  /// Access the logical volume for the read out geometry
  inline G4LogicalVolume*   GetReadOutWorldLV() const
  {return readOutWorldLV;}

  /// Register the physical volume for the TUNNEL read out geometry
  inline void               RegisterTunnelReadOutWorldPV(G4VPhysicalVolume* tunnelReadOutWorldPVIn)
  {tunnelReadOutWorldPV = tunnelReadOutWorldPVIn;}
  
  /// Access the physical volume for the TUNNEL read out geometry
  inline G4VPhysicalVolume* GetTunnelReadOutWorldPV() const
  {return tunnelReadOutWorldPV;}

  /// Register the logical volume for the TUNNEL read out geometry - needed
  /// for the placement of tunnel read out geometry items
  inline void               RegisterTunnelReadOutWorldLV(G4LogicalVolume* tunnelReadOutWorldIn)
  {tunnelReadOutWorldLV = tunnelReadOutWorldIn;}
  
  /// Access the logical volume for the TUNNEL read out geometry
  inline G4LogicalVolume*   GetTunnelReadOutWorldLV() const
  {return tunnelReadOutWorldLV;}

  /// Register the flat beam line - flat means that each element in the beamline represents
  /// one element in the accelerator lattice
  inline void               RegisterFlatBeamline(BDSBeamline* beamlineIn)
  {flatBeamline = beamlineIn;}

  /// Access flat beam line
  inline BDSBeamline*       GetFlatBeamline() const
  {return flatBeamline;}

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
  
  /// Register all field objects
  inline void               RegisterFields(std::vector<BDSFieldObjects*>& fieldsIn);

private:
  BDSAcceleratorModel(); ///< default constructor is private as singleton

  static BDSAcceleratorModel* _instance;

  G4VPhysicalVolume* worldPV;              ///< Physical volume of the mass world.
  G4VPhysicalVolume* readOutWorldPV;       ///< Physical volume for read out geometry.
  G4LogicalVolume*   readOutWorldLV;       ///< Logical volume for read out geometry.
  G4VPhysicalVolume* tunnelReadOutWorldPV; ///< Physical volume for tunnel read out geometry.
  G4LogicalVolume*   tunnelReadOutWorldLV; ///< Logical volume for tunnel read out geometry.

  BDSBeamline*       flatBeamline;     ///< Flat beam line.
  BDSBeamline*       supportsBeamline; ///< Element supports beam line.
  BDSBeamline*       tunnelBeamline;   ///< Tunnel segments beam line.
  BDSBeamline*       endPieceBeamline; ///< End Pieces beam line.

  std::vector<BDSFieldObjects*> fields; ///< All field objects.
};

#endif
