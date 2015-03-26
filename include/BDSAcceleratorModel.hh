#ifndef BDSACCELERATORMODEL_H
#define BDSACCELERATORMODEL_H

#include "globals.hh"         // geant4 globals / types
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"

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

  /// register the physical volume for the read out geometry
  inline void               SetReadOutWorldPV(G4VPhysicalVolume* readOutWorldIn);

  /// access the physical volume for the read out geometry
  inline G4VPhysicalVolume* GetReadOutWorldPV();

  /// register the logical volume for the read out geometry - needed
  /// for placement of read out geometry items
  inline void               SetReadOutWorldLV(G4LogicalVolume* readOutWorldIn);

  /// access the logical volume for the read out geometry
  inline G4LogicalVolume*   GetReadOutWorldLV();

private:
  BDSAcceleratorModel(); ///< default constructor is private as singleton

  static BDSAcceleratorModel* _instance;

  G4VPhysicalVolume* readOutWorldPV; ///< physical volume for read out geometry
  G4LogicalVolume*   readOutWorldLV; ///< logical volume for read out geometry
  
};

inline void BDSAcceleratorModel::SetReadOutWorldPV(G4VPhysicalVolume* readOutWorldIn)
{readOutWorldPV = readOutWorldIn;}

inline G4VPhysicalVolume* BDSAcceleratorModel::GetReadOutWorldPV()
{return readOutWorldPV;}

inline void BDSAcceleratorModel::SetReadOutWorldLV(G4LogicalVolume* readOutWorldIn)
{readOutWorldLV = readOutWorldIn;}

inline G4LogicalVolume* BDSAcceleratorModel::GetReadOutWorldLV()
{return readOutWorldLV;}

#endif
