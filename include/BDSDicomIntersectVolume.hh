//
// Created by strangesyd on 3/7/21.
//

#ifndef BDSIM_BDSDICOMINTERSECTVOLUME_HH
#define BDSIM_BDSDICOMINTERSECTVOLUME_HH

#include "G4UImessenger.hh"

#include <vector>
#include <fstream>
#include "G4PhantomParameterisation.hh"
#include "G4ThreeVector.hh"
#include "G4RotationMatrix.hh"
class G4LogicalVolume;
class G4VPhysicalVolume;
class G4PhantomParameterisation;
class G4UIcmdWithAString;
class G4VSolid;

/// Manages intersections of DICOM files with volumes

class BDSDicomIntersectVolume : public G4UImessenger
{
public:
    BDSDicomIntersectVolume();
    ~BDSDicomIntersectVolume();

    virtual void SetNewValue(G4UIcommand * command,
                             G4String newValues);

private:

    void BuildUserSolid( std::vector<G4String> params );
    void BuildG4Solid( std::vector<G4String> params );
    G4PhantomParameterisation* GetPhantomParam(G4bool bMustExist);
    G4bool IsPhantomVolume( G4VPhysicalVolume* pv );
    std::vector<G4VPhysicalVolume*> GetPhysicalVolumes( const G4String& name,
                                                        G4bool exists, G4int nVols );
    std::vector<G4LogicalVolume*> GetLogicalVolumes( const G4String& name,
                                                     G4bool exists, G4int nVols );
    std::vector<G4String> GetWordsInString( const G4String& stemp);

private:
    G4UIcmdWithAString* fUserVolumeCmd;
    G4UIcmdWithAString* fG4VolumeCmd;

    G4VSolid* fSolid;

    std::ofstream fout;

    G4ThreeVector fPhantomMinusCorner;

    G4bool* fVoxelIsInside;
};

#endif //BDSIM_BDSDICOMINTERSECTVOLUME_HH
