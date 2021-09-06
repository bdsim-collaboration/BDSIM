/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2021.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef BDSCT_H
#define BDSCT_H
#include "BDSAcceleratorComponent.hh"

#include "G4String.hh"
#include "G4Types.hh"

#include <fstream>
#include <map>
#include <set>
#include <vector>

class BDSDicomFileMgr;
class BDSDicomPhantomZSliceHeader;
class BDSDicomPhantomZSliceMerged;
class G4Box;
class G4LogicalVolume;
class G4Material;
class G4VPhysicalVolume;

/**
 * @brief An object based on a DICOM X-ray Computed Tomograph (CT) scan.
 *
 * @author Nathan Farber
 */

//--- New data structure containing informations about a material
struct matInfo
{
  // The sum of all densities of all the voxels containing this material
  G4double fSumdens;
  // The number of voxels that contain this material
  G4int fNvoxels;
  // This ID assoiated with thi material
  G4int fId;
};

class BDSCT : public BDSAcceleratorComponent
{
public:
  BDSCT(const G4String &nameIn,
        const G4String &dicomDataPath,
        const G4String &dicomDataFile);

  virtual ~BDSCT();

  /// Override base class version and return beam pipe material if it exists.
  virtual G4String Material() const override { return "infiniteabsorber"; }

protected:
  /// Call default build then override visualisation attributes.
  virtual void Build() override;

  /// Build a simple box or cylinder.
  virtual void BuildContainerLogicalVolume() override;

  /// Customised user limits.
  virtual void BuildUserLimits() override;

private:
  /// No default constructor.
  BDSCT() = delete;

  /// @{ Assignment and copy constructor not implemented nor used
  BDSCT &operator=(const BDSCT &) = delete;
  BDSCT(BDSCT &) = delete;
  /// @}

  void InitialisationOfMaterials();
  // create the original materials

  void ReadPhantomData();
  // read the DICOM files describing the phantom
  void ReadVoxelDensities(std::ifstream &fin);

  void ReadPhantomDataFile(const G4String &fname);
  // read one of the DICOM files describing the phantom
  // (usually one per Z slice).
  //  Build a DicomPhantomZSliceHeader for each file

  void MergeZSliceHeaders();
  // merge the slice headers of all the files

  G4Material *BuildMaterialWithChangingDensity(const G4Material *origMate,
                                               G4float density,
                                               G4String newMateName);
  // build a new material if the density of the voxel is different
  // to the other voxels

  void BuildPhantom();
  // construct the phantom volumes.
  //  This method should be implemented for each of the derived classes

  void SetScorer(G4LogicalVolume *voxel_logic);

  G4Material *fAir;

  // World ...
  G4Box *fWorld_solid;
  G4LogicalVolume *fWorld_logic;
  G4VPhysicalVolume *fWorld_phys;

  G4Box *fContainer_solid;
  G4VPhysicalVolume *fContainer_phys;

  G4int fNoFiles;                               // number of DICOM files
  std::vector<G4Material *> fOriginalMaterials; // list of original materials
  std::vector<G4Material *> fMaterials;
  // list of new materials created to distinguish different density
  //  voxels that have the same original materials
  size_t *fMateIDs; // index of material of each voxel
  //unsigned int* fMateIDs; // index of material of each voxel

  std::map<G4int, G4double> fDensityDiffs;
  // Density difference to distinguish material for each original
  // material (by index)

  std::vector<BDSDicomPhantomZSliceHeader *> fZSliceHeaders;
  // list of z slice header (one per DICOM files)
  BDSDicomPhantomZSliceHeader *fZSliceHeaderMerged;
  // z slice header resulted from merging all z slice headers

  G4int fNVoxelX, fNVoxelY, fNVoxelZ;
  G4double fVoxelHalfDimX, fVoxelHalfDimY, fVoxelHalfDimZ;
  G4double fMinX, fMinY, fMinZ; // minimum extension of voxels (position wall)
  G4double fMaxX, fMaxY, fMaxZ; // maximum extension of voxels (position wall)

  std::map<G4int, G4Material *> thePhantomMaterialsOriginal;
  // map numberOfMaterial to G4Material. They are the list of materials as
  // built from .geom file

  std::set<G4LogicalVolume *> fScorers;

  G4bool fConstructed;

  BDSDicomFileMgr *theFileMgr;

  G4String dicomDataPath;
  G4String dicomDataFile;
};

#endif
