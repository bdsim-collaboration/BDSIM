#include "B1UserScoreWriter.hh"

#include "G4MultiFunctionalDetector.hh"
#include "G4SDParticleFilter.hh"
#include "G4VPrimitiveScorer.hh"
#include "G4VScoringMesh.hh"
#include "B1DetectorConstruction.hh"
#include "G4RunManager.hh"
#include <map>
#include <fstream>

B1UserScoreWriter::B1UserScoreWriter()
        : G4VScoreWriter()
{;}

B1UserScoreWriter::~B1UserScoreWriter()
{;}

void B1UserScoreWriter::DumpQuantityToFile(const G4String & psName,
                                           const G4String & fileName,
                                           const G4String & option) {

  // open the file
  std::ofstream ofile(fileName);
  if(!ofile) {
    G4cerr << "ERROR : DumpToFile : File open error -> "
           << fileName << G4endl;
    return;
  }

  /// Get the mesh properties
  G4ThreeVector mesh_size = fScoringMesh->GetSize(); // in mm
  G4ThreeVector mesh_translation = fScoringMesh->GetTranslation(); //in mm
  ///---------------------------

  /// Retrieve the map
  MeshScoreMap fSMap = fScoringMesh->GetScoreMap();
  MeshScoreMap::const_iterator msMapItr = fSMap.find(psName);
  if(msMapItr == fSMap.end()) {
    G4cerr << "ERROR : DumpToFile : Unknown quantity, \""
           << psName << "\"." << G4endl;
    return;
  }

  std::map<G4int, G4StatDouble*> * score = msMapItr->second->GetMap();
  ofile << "# primitive scorer name: " << msMapItr->first << std::endl;
  G4double unitValue = fScoringMesh->GetPSUnitValue(psName);
  G4String unit = fScoringMesh->GetPSUnit(psName);
  ///---------------------------

  /// write quantity


  G4double cell_sizeX = (mesh_size.x()/fNMeshSegments[0]);
  G4double cell_sizeY = (mesh_size.y()/fNMeshSegments[1]);
  G4double cell_sizeZ = (mesh_size.z()/fNMeshSegments[2]);

  ofile << std::setprecision(16); // for double value with 8 bytes
  G4double posX = mesh_translation.x()-mesh_size.x()+cell_sizeX;

  for(int x = 0; x < fNMeshSegments[0]; x++) {
    G4double posY = mesh_translation.y()-mesh_size.y()+cell_sizeY;

    for(int y = 0; y < fNMeshSegments[1]; y++) {
      G4double posZ = mesh_translation.z()-mesh_size.z()+cell_sizeZ;

      for(int z = 0; z < fNMeshSegments[2]; z++) {

        G4int idx = GetIndex(x, y, z);
        ofile << posX << ","
              << posY << ","
              << posZ << ",";

        auto value = score->find(idx);
        if(value == score->end())
        {
          ofile << 0. << "," << 0. << "," << 0 << "\n";
        }
        else {
          ofile << (value->second->sum_wx()) / unitValue << ","
                << (value->second->sum_wx2()) / unitValue / unitValue << ","
                << value->second->n() << "\n";


        }
        posZ += 2*cell_sizeZ;
      } // z
      posY += 2*cell_sizeY;
    } // y
    posX += 2*cell_sizeX;
  } // x
  ofile << std::setprecision(6);

  // close the file
  ofile.close();

}
