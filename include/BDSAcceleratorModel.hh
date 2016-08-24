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
  inline void               RegisterWorldPV(G4VPhysicalVolume* worldIn);

  /// Access the physical volume of the world
  inline G4VPhysicalVolume* GetWorldPV();

  /// Register the physical volume for the read out geometry
  inline void               RegisterReadOutWorldPV(G4VPhysicalVolume* readOutWorldIn);

  /// Access the physical volume for the read out geometry
  inline G4VPhysicalVolume* GetReadOutWorldPV();

  /// Register the logical volume for the read out geometry - needed
  /// for the placement of tunnel read out geometry items
  inline void               RegisterReadOutWorldLV(G4LogicalVolume* tunnelReadOutWorldin);

  /// Access the logical volume for the read out geometry
  inline G4LogicalVolume*   GetReadOutWorldLV();

  /// Register the physical volume for the TUNNEL read out geometry
  inline void               RegisterTunnelReadOutWorldPV(G4VPhysicalVolume* tunnelReadOutWorldIn);
  
  /// Access the physical volume for the TUNNEL read out geometry
  inline G4VPhysicalVolume* GetTunnelReadOutWorldPV();

  /// Register the logical volume for the TUNNEL read out geometry - needed
  /// for the placement of tunnel read out geometry items
  inline void               RegisterTunnelReadOutWorldLV(G4LogicalVolume* tunnelReadOutWorldin);

  /// Access the logical volume for the TUNNEL read out geometry
  inline G4LogicalVolume*   GetTunnelReadOutWorldLV();

  /// Register the flat beam line - flat means that each element in the beamline represents
  /// one element in the accelerator lattice
  inline void               RegisterFlatBeamline(BDSBeamline* beamlineIn);

  /// Access flat beam line
  inline BDSBeamline*       GetFlatBeamline();

  /// Register the beam line containing all the magnet supports
  inline void               RegisterSupportsBeamline(BDSBeamline* beamlineIn);

  /// Access the beam line containing all the magnet supports
  inline BDSBeamline*       GetSupportsBeamline();

  /// Register the beam line containing all the tunnel segments
  inline void               RegisterTunnelBeamline(BDSBeamline* beamlineIn);

  /// Access the beam line containing all the tunnel segments
  inline BDSBeamline*       GetTunnelBeamline();

  /// Register any physical volumes that should be managed - typically from world placement
  inline void               RegisterPhysicalVolume(G4VPhysicalVolume* physicalVolume);

  /// Register any physical volumes that should be managed - typically from world placement
  inline void               RegisterPhysicalVolume(std::vector<G4VPhysicalVolume*> physicalVolumes);

  /// Register the beam line of end pieces.
  inline void               RegisterEndPieceBeamline(BDSBeamline* beamlineIn);

  /// Access the beam line of end pieces.
  inline BDSBeamline*       GetEndPieceBeamline() const {return endPieceBeamline;}
  
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
};

inline void BDSAcceleratorModel::RegisterWorldPV(G4VPhysicalVolume* worldIn)
{worldPV = worldIn;}

inline G4VPhysicalVolume* BDSAcceleratorModel::GetWorldPV()
{return worldPV;}

inline void BDSAcceleratorModel::RegisterReadOutWorldPV(G4VPhysicalVolume* readOutWorldIn)
{readOutWorldPV = readOutWorldIn;}

inline G4VPhysicalVolume* BDSAcceleratorModel::GetReadOutWorldPV()
{return readOutWorldPV;}

inline void BDSAcceleratorModel::RegisterReadOutWorldLV(G4LogicalVolume* readOutWorldIn)
{readOutWorldLV = readOutWorldIn;}

inline G4LogicalVolume* BDSAcceleratorModel::GetReadOutWorldLV()
{return readOutWorldLV;}

inline void BDSAcceleratorModel::RegisterTunnelReadOutWorldPV(G4VPhysicalVolume* worldIn)
{tunnelReadOutWorldPV = worldIn;}

inline G4VPhysicalVolume* BDSAcceleratorModel::GetTunnelReadOutWorldPV()
{return tunnelReadOutWorldPV;}

inline void BDSAcceleratorModel::RegisterTunnelReadOutWorldLV(G4LogicalVolume* readOutWorldIn)
{tunnelReadOutWorldLV = readOutWorldIn;}

inline G4LogicalVolume* BDSAcceleratorModel::GetTunnelReadOutWorldLV()
{return tunnelReadOutWorldLV;}

inline void BDSAcceleratorModel::RegisterFlatBeamline(BDSBeamline* beamlineIn)
{flatBeamline = beamlineIn;}

inline BDSBeamline* BDSAcceleratorModel::GetFlatBeamline()
{return flatBeamline;}

inline void BDSAcceleratorModel::RegisterSupportsBeamline(BDSBeamline* beamlineIn)
{supportsBeamline = beamlineIn;}

inline BDSBeamline* BDSAcceleratorModel::GetSupportsBeamline()
{return supportsBeamline;}

inline void BDSAcceleratorModel::RegisterTunnelBeamline(BDSBeamline* beamlineIn)
{tunnelBeamline = beamlineIn;}

inline BDSBeamline* BDSAcceleratorModel::GetTunnelBeamline()
{return tunnelBeamline;}

inline void BDSAcceleratorModel::RegisterEndPieceBeamline(BDSBeamline* beamlineIn)
{endPieceBeamline = beamlineIn;}

#endif
