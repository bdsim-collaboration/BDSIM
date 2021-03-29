//
// Created by strangesyd on 3/7/21.
//

#ifndef BDSIM_BDSDICOMFILEMGR_HH
#define BDSIM_BDSDICOMFILEMGR_HH

#include <vector>
#include <map>
#include "globals.hh"
#include "BDSDicomFileStructure.hh"
#include "BDSDicomFilePlan.hh"

#include "dcmtk/dcmdata/dcfilefo.h"
class BDSDicomVFile;
class BDSDicomFileCT;

//typedef std::multimap<OFString,DicomVFile*> msd;
typedef std::map<G4double,BDSDicomFileCT*> mdct;
enum VerbLevel {silentVerb = -1, errorVerb = 0, warningVerb = 1, infoVerb=2, debugVerb=3,
    testVerb=4};

class BDSDicomFileMgr
{
public:
    static BDSDicomFileMgr* GetInstance();
    ~BDSDicomFileMgr(){};

private:
    BDSDicomFileMgr();

public:
    std::vector<BDSDicomFileStructure*> GetStructFiles() const {
        return theStructFiles;
    }

    void SetCompression( G4String fComp );
    void AddFile( G4String fComp );
    void AddMaterial( std::vector<G4String> data );
    void AddMaterialDensity( std::vector<G4String> data );
    void AddCT2Density( std::vector<G4String> data );

    void Convert(G4String filePath, G4String fFileName);
    void CheckNColumns(std::vector<G4String> wl, size_t vsizeTh );
    void ProcessFiles();
    void CheckCTSlices();
    G4double Hounsfield2density(Uint32 Hval);
    size_t GetMaterialIndex( G4double Hval );
    size_t GetMaterialIndexByDensity( G4double density );
    void BuildCTMaterials();
    void MergeCTFiles();
    void DumpToTextFile();
    void SetStructureNCheck( G4int nsc ){
        theStructureNCheck = nsc;
    }
    G4int GetStructureNCheck() const {
        return theStructureNCheck;
    }
    void SetStructureNMaxROI( G4int nsc ){
        theStructureNMaxROI = nsc;
    }
    G4int GetStructureNMaxROI() const {
        return theStructureNMaxROI;
    }
    G4int GetCompression() const {
        return fCompression;
    }
    G4String GetFileOutName() const {
        return theFileOutName;
    }

    void SetControlPointMetersets();
    G4bool IsMaterialsDensity() const {
        return bMaterialsDensity;
    }

protected:
    G4int fCompression;

private:
    static BDSDicomFileMgr* theInstance;

    G4String theFileOutName;
    //  msd theFiles;
    mdct theCTFiles;
    std::vector<BDSDicomFileStructure*> theStructFiles;
    std::vector<BDSDicomFilePlan*> thePlanFiles;
    std::map<G4double,G4String> theMaterials;
    std::map<G4double,G4String> theMaterialsDensity;
    std::map<G4int,G4double> theCT2Density;

    BDSDicomFileCT* theCTFileAll;
    G4int theStructureNCheck;
    G4int theStructureNMaxROI;

public:
    static int verbose;
    G4bool bMaterialsDensity;
};

#endif //BDSIM_BDSDICOMFILEMGR_HH
