//
// Created by strangesyd on 3/7/21.
//

#ifndef BDSIM_BDSDICOMFILECT_HH
#define BDSIM_BDSDICOMFILECT_HH

#include "BDSDicomVFile.hh"
#include "BDSDicomFileMgr.hh"
#include "G4ThreeVector.hh"

class BDSDicomFileCT : public BDSDicomVFile
{
public:
    BDSDicomFileCT();
    BDSDicomFileCT(DcmDataset* dset);
    ~BDSDicomFileCT(){};

public:
    virtual void ReadData();
    void BuildMaterials();
    void DumpMateIDsToTextFile(std::ofstream& fout);
    void DumpDensitiesToTextFile(std::ofstream& fout);
    void BuildStructureIDs();
    void DumpStructureIDsToTextFile(std::ofstream& fout);

    void operator+=( const BDSDicomFileCT& rhs );
    BDSDicomFileCT operator+( const BDSDicomFileCT& rhs );
    // add two slices that have the same dimensions, merging them in Z

    void DumpHeaderToTextFile(std::ofstream& fout);

    // Get and set methods
    G4int GetNoVoxelX() const { return fNoVoxelX; };
    G4int GetNoVoxelY() const { return fNoVoxelY; };
    G4int GetNoVoxelZ() const { return fNoVoxelZ; };
    G4int GetNoVoxels() const { return fNoVoxelX*fNoVoxelY*fNoVoxelZ; };

    G4double GetMinX() const { return fMinX; };
    G4double GetMinY() const { return fMinY; };
    G4double GetMinZ() const { return fMinZ; };
    G4double GetMaxX() const { return fMaxX; };
    G4double GetMaxY() const { return fMaxY; };
    G4double GetMaxZ() const { return fMaxZ; };

    std::vector<G4double> GetDensities() const { return fDensities; }
    std::vector<size_t> GetMateIDs() const { return fMateIDs; }

    void SetNoVoxelX(const G4int& val) { fNoVoxelX = val; }
    void SetNoVoxelY(const G4int& val) { fNoVoxelY = val; }
    void SetNoVoxelZ(const G4int& val) { fNoVoxelZ = val; }

    void SetMinX(const G4double& val) { fMinX = val; };
    void SetMaxX(const G4double& val) { fMaxX = val; };
    void SetMinY(const G4double& val) { fMinY = val; };
    void SetMaxY(const G4double& val) { fMaxY = val; };
    void SetMinZ(const G4double& val) { fMinZ = val; };
    void SetMaxZ(const G4double& val) { fMaxZ = val; };

    const G4double& GetLocation() const { return fLocation; }

    void SetLocation(const G4double& val) { fLocation = val; }

    G4ThreeVector GetOrientationRows() const { return fOrientationRows; }
    G4ThreeVector GetOrientationColumns() const { return fOrientationColumns; }

private:
    template <typename T> inline bool CheckConsistency(const T&, const T&, G4String);

    void ReadPixelData();
    void Print( std::ostream& out );

    std::vector<size_t> fMateIDs;
    std::vector<G4double> fDensities;
    std::vector<G4int> fStructure;
    //  G4int* fStructure;

protected:
    G4double fLocation;
    G4double fBitAllocated;
    G4double fRescaleSlope;
    G4double fRescaleIntercept;

    G4int fNoVoxelX, fNoVoxelY, fNoVoxelZ;  // number of voxels in each dimensions
    G4double fMinX,fMinY,fMinZ; // minimum extension of voxels (position of wall)
    G4double fMaxX,fMaxY,fMaxZ; // maximum extension of voxels (position of wall)
    G4double fVoxelDimX,fVoxelDimY,fVoxelDimZ; // maximum extension of voxels (position of wall)

    G4ThreeVector fOrientationRows;
    G4ThreeVector fOrientationColumns;

    std::vector<int> fHounsfieldV;

    BDSDicomFileMgr* theFileMgr;
};

//============================================================================
template <typename T>
inline bool BDSDicomFileCT::CheckConsistency(const T& val1, const T& val2,
                                              G4String category) {
    if(val1 != val2) {
        G4Exception("DicomVFileImager::CheckConsistency",
                    "Consistency Mismatch : Keeping previous value if nonzero",
                    JustWarning, category.c_str());
        return false;
    }
    return true;
}

#endif //BDSIM_BDSDICOMFILECT_HH
