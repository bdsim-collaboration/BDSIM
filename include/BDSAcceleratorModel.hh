#ifndef BDSACCELERATORMODEL_H
#define BDSACCELERATORMODEL_H

#include "globals.hh"         // geant4 globals / types
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

#include "BDSBeamline.hh"

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
 * @author Laurie Nevay <laurie.nevay@rhul.ac.uk>
 */

class BDSAcceleratorModel {
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
  /// for placement of read out geometry items
  inline void               RegisterReadOutWorldLV(G4LogicalVolume* readOutWorldIn);

  /// Access the logical volume for the read out geometry
  inline G4LogicalVolume*   GetReadOutWorldLV();

  /// Register the flat beamline - flat means that each element in the beamline represents
  /// one element in the accelerator lattice
  inline void               RegisterFlatBeamline(BDSBeamline* beamlineIn);

  /// Access flat beamline
  inline BDSBeamline*       GetFlatBeamline();
  

private:
  BDSAcceleratorModel(); ///< default constructor is private as singleton

  static BDSAcceleratorModel* _instance;

  G4VPhysicalVolume* worldPV;        ///< physical volume of the mass world
  G4VPhysicalVolume* readOutWorldPV; ///< physical volume for read out geometry
  G4LogicalVolume*   readOutWorldLV; ///< logical volume for read out geometry

  BDSBeamline*       flatBeamline;   ///< flat beam line
  
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

inline void BDSAcceleratorModel::RegisterFlatBeamline(BDSBeamline* beamlineIn)
{flatBeamline = beamlineIn;}

inline BDSBeamline* BDSAcceleratorModel::GetFlatBeamline()
{return flatBeamline;}

#endif
