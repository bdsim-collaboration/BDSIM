/*
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway,
University of London 2001 - 2020.

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
#include "BDSAcceleratorComponent.hh"
#include "BDSColours.hh"
#include "BDSCT.hh"
#include "BDSDicomFileMgr.hh"
#include "BDSDicomIntersectVolume.hh"
#include "BDSDicomPhantomParameterisationColour.hh"
#include "BDSDicomPhantomZSliceHeader.hh"
#include "BDSDicomPhantomZSliceMerged.hh"
#include "BDSExtent.hh"

#include "G4Box.hh"
#include "G4Colour.hh"
#include "G4LogicalVolume.hh"
#include "G4MultiFunctionalDetector.hh"
#include "G4NistManager.hh"
#include "G4PSDoseDeposit.hh"
#include "G4PSDoseDeposit3D.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4SDManager.hh"
#include "G4String.hh"
#include "G4Tubs.hh"
#include "G4Types.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

#include "CLHEP/Units/SystemOfUnits.h"

BDSCT::BDSCT(const G4String& nameIn,
             const G4String& dataFilePath):
  BDSAcceleratorComponent(nameIn, 1, 0, "ct"),
  dicomDataPath(dataFilePath)
{
  theFileMgr = BDSDicomFileMgr::GetInstance();
  theFileMgr->Convert(dicomDataPath, "data.dat");
  new BDSDicomIntersectVolume();
}

BDSCT::~BDSCT()
{;}

void BDSCT::Build()
{
    if(!fConstructed || fWorld_phys == nullptr) {
        fConstructed = true;
        InitialisationOfMaterials();

        //----- Build world
        G4double worldXDimension = 1. * CLHEP::m;
        G4double worldYDimension = 1. * CLHEP::m;
        G4double worldZDimension = 1. * CLHEP::m;

        fWorld_solid = new G4Box("WorldSolid",
                                 worldXDimension,
                                 worldYDimension,
                                 worldZDimension);

        fWorld_logic = new G4LogicalVolume(fWorld_solid,
                                           fAir,
                                           "WorldLogical",
                                           nullptr, nullptr, nullptr);

        fWorld_phys = new G4PVPlacement(nullptr,
                                        G4ThreeVector(0, 0, 0),
                                        "World",
                                        fWorld_logic,
                                        nullptr,
                                        false,
                                        0);
        ReadPhantomData();
        BuildContainerLogicalVolume();

        BuildPhantom();

    }
}

void BDSCT::BuildPhantom()
{
    //----- Create parameterisation
    BDSDicomPhantomParameterisationColour* param =
            new BDSDicomPhantomParameterisationColour(dicomDataPath + "ColourMap.dat");

    //----- Set voxel dimensions
    param->SetVoxelDimensions( fVoxelHalfDimX, fVoxelHalfDimY, fVoxelHalfDimZ );

    //----- Set number of voxels
    param->SetNoVoxel( fNVoxelX, fNVoxelY, fNVoxelZ );

    //----- Set list of materials
    param->SetMaterials( fMaterials );

    //----- Set list of material indices: for each voxel it is a number that
    // correspond to the index of its material in the vector of materials
    // defined above
    param->SetMaterialIndices( fMateIDs );

    //----- Define voxel logical volume
    G4Box* voxel_solid =
            new G4Box( "Voxel", fVoxelHalfDimX, fVoxelHalfDimY, fVoxelHalfDimZ);
    G4LogicalVolume* voxel_logic =
            new G4LogicalVolume(voxel_solid,fMaterials[0],"VoxelLogical",
                                0,0,0);
    // material is not relevant, it will be changed by the
    // ComputeMaterial method of the parameterisation

    voxel_logic->SetVisAttributes(
            new G4VisAttributes(G4VisAttributes::GetInvisible()));

    //--- Assign the fContainer volume of the parameterisation
    param->BuildContainerSolid(fContainer_phys);

    //--- Assure yourself that the voxels are completely filling the
    // fContainer volume
    param->CheckVoxelsFillContainer( fContainer_solid->GetXHalfLength(),
                                     fContainer_solid->GetYHalfLength(),
                                     fContainer_solid->GetZHalfLength() );

    //----- The G4PVParameterised object that uses the created parameterisation
    // should be placed in the fContainer logical volume
    G4PVParameterised* phantom_phys =
            new G4PVParameterised("phantom",voxel_logic,containerLogicalVolume,
                                  kXAxis, fNVoxelX*fNVoxelY*fNVoxelZ, param);
    // if axis is set as kUndefined instead of kXAxis, GEANT4 will
    //  do an smart voxel optimisation
    // (not needed if G4RegularNavigation is used)

    //----- Set this physical volume as having a regular structure of type 1,
    // so that G4RegularNavigation is used
    phantom_phys->SetRegularStructureId(1); // if not set, G4VoxelNavigation
    //will be used instead

    SetScorer(voxel_logic);
}

void BDSCT::InitialisationOfMaterials()
{
    // Creating elements :
    G4double z, a, density;
    G4String name, symbol;

    auto* elC = new G4Element( name = "Carbon",
                                    symbol = "C",
                                    z = 6.0, a = 12.011 * CLHEP::g/CLHEP::mole );
    auto* elH = new G4Element( name = "Hydrogen",
                                    symbol = "H",
                                    z = 1.0, a = 1.008  * CLHEP::g/CLHEP::mole );
    auto* elN = new G4Element( name = "Nitrogen",
                                    symbol = "N",
                                    z = 7.0, a = 14.007 * CLHEP::g/CLHEP::mole );
    auto* elO = new G4Element( name = "Oxygen",
                                    symbol = "O",
                                    z = 8.0, a = 16.00  * CLHEP::g/CLHEP::mole );
    auto* elNa = new G4Element( name = "Sodium",
                                     symbol = "Na",
                                     z= 11.0, a = 22.98977* CLHEP::g/CLHEP::mole );
    auto* elMg = new G4Element( name = "Magnesium",
                                     symbol = "Mg",
                                     z = 12.0, a = 24.3050* CLHEP::g/CLHEP::mole );
    auto* elP = new G4Element( name = "Phosphorus",
                                    symbol = "P",
                                    z = 15.0, a = 30.973976* CLHEP::g/CLHEP::mole );
    auto* elS = new G4Element( name = "Sulfur",
                                    symbol = "S",
                                    z = 16.0,a = 32.065* CLHEP::g/CLHEP::mole );
    auto* elCl = new G4Element( name = "Chlorine",
                                     symbol = "P",
                                     z = 17.0, a = 35.453* CLHEP::g/CLHEP::mole );
    auto* elK = new G4Element( name = "Potassium",
                                    symbol = "P",
                                    z = 19.0, a = 30.0983* CLHEP::g/CLHEP::mole );

    auto* elFe = new G4Element( name = "Iron",
                                     symbol = "Fe",
                                     z = 26, a = 56.845* CLHEP::g/CLHEP::mole );

    auto* elCa = new G4Element( name="Calcium",
                                     symbol = "Ca",
                                     z = 20.0, a = 40.078* CLHEP::g/CLHEP::mole );

    auto* elZn = new G4Element( name = "Zinc",
                                     symbol = "Zn",
                                     z = 30.0,a = 65.382* CLHEP::g/CLHEP::mole );

    // Creating Materials :
    G4int numberofElements;

    // Air
    fAir = new G4Material( "Air",
                           1.290*CLHEP::mg/CLHEP::cm3,
                           numberofElements = 2 );
    fAir->AddElement(elN, 0.7);
    fAir->AddElement(elO, 0.3);


    // Soft tissue (ICRP - NIST)
    auto* softTissue = new G4Material ("SoftTissue", 1.00*CLHEP::g/CLHEP::cm3,
                                             numberofElements = 13);
    softTissue->AddElement(elH, 10.4472*CLHEP::perCent);
    softTissue->AddElement(elC, 23.219*CLHEP::perCent);
    softTissue->AddElement(elN, 2.488*CLHEP::perCent);
    softTissue->AddElement(elO, 63.0238*CLHEP::perCent);
    softTissue->AddElement(elNa, 0.113*CLHEP::perCent);
    softTissue->AddElement(elMg, 0.0113*CLHEP::perCent);
    softTissue->AddElement(elP, 0.113*CLHEP::perCent);
    softTissue->AddElement(elS, 0.199*CLHEP::perCent);
    softTissue->AddElement(elCl, 0.134*CLHEP::perCent);
    softTissue->AddElement(elK, 0.199*CLHEP::perCent);
    softTissue->AddElement(elCa, 0.023*CLHEP::perCent);
    softTissue->AddElement(elFe, 0.005*CLHEP::perCent);
    softTissue->AddElement(elZn, 0.003*CLHEP::perCent);

    //  Lung Inhale
    auto* lunginhale = new G4Material( "LungInhale",
                                             density = 0.217*CLHEP::g/CLHEP::cm3,
                                             numberofElements = 9);
    lunginhale->AddElement(elH,0.103);
    lunginhale->AddElement(elC,0.105);
    lunginhale->AddElement(elN,0.031);
    lunginhale->AddElement(elO,0.749);
    lunginhale->AddElement(elNa,0.002);
    lunginhale->AddElement(elP,0.002);
    lunginhale->AddElement(elS,0.003);
    lunginhale->AddElement(elCl,0.002);
    lunginhale->AddElement(elK,0.003);

    // Lung exhale
    G4Material* lungexhale = new G4Material( "LungExhale",
                                             density = 0.508*CLHEP::g/CLHEP::cm3,
                                             numberofElements = 9 );
    lungexhale->AddElement(elH,0.103);
    lungexhale->AddElement(elC,0.105);
    lungexhale->AddElement(elN,0.031);
    lungexhale->AddElement(elO,0.749);
    lungexhale->AddElement(elNa,0.002);
    lungexhale->AddElement(elP,0.002);
    lungexhale->AddElement(elS,0.003);
    lungexhale->AddElement(elCl,0.002);
    lungexhale->AddElement(elK,0.003);

    // Adipose tissue
    auto* adiposeTissue = new G4Material( "AdiposeTissue",
                                                density = 0.967*CLHEP::g/CLHEP::cm3,
                                                numberofElements = 7);
    adiposeTissue->AddElement(elH,0.114);
    adiposeTissue->AddElement(elC,0.598);
    adiposeTissue->AddElement(elN,0.007);
    adiposeTissue->AddElement(elO,0.278);
    adiposeTissue->AddElement(elNa,0.001);
    adiposeTissue->AddElement(elS,0.001);
    adiposeTissue->AddElement(elCl,0.001);

    // Brain (ICRP - NIST)
    auto* brainTissue = new G4Material ("BrainTissue", 1.03 * CLHEP::g/CLHEP::cm3,
                                              numberofElements = 13);
    brainTissue->AddElement(elH, 11.0667*CLHEP::perCent);
    brainTissue->AddElement(elC, 12.542*CLHEP::perCent);
    brainTissue->AddElement(elN, 1.328*CLHEP::perCent);
    brainTissue->AddElement(elO, 73.7723*CLHEP::perCent);
    brainTissue->AddElement(elNa, 0.1840*CLHEP::perCent);
    brainTissue->AddElement(elMg, 0.015*CLHEP::perCent);
    brainTissue->AddElement(elP, 0.356*CLHEP::perCent);
    brainTissue->AddElement(elS, 0.177*CLHEP::perCent);
    brainTissue->AddElement(elCl, 0.236*CLHEP::perCent);
    brainTissue->AddElement(elK, 0.31*CLHEP::perCent);
    brainTissue->AddElement(elCa, 0.009*CLHEP::perCent);
    brainTissue->AddElement(elFe, 0.005*CLHEP::perCent);
    brainTissue->AddElement(elZn, 0.001*CLHEP::perCent);


    // Breast
    auto* breast = new G4Material( "Breast",
                                         density = 0.990*CLHEP::g/CLHEP::cm3,
                                         numberofElements = 8 );
    breast->AddElement(elH,0.109);
    breast->AddElement(elC,0.506);
    breast->AddElement(elN,0.023);
    breast->AddElement(elO,0.358);
    breast->AddElement(elNa,0.001);
    breast->AddElement(elP,0.001);
    breast->AddElement(elS,0.001);
    breast->AddElement(elCl,0.001);

    // Spinal Disc
    auto* spinalDisc = new G4Material ("SpinalDisc", 1.10 * CLHEP::g/CLHEP::cm3,
                                             numberofElements = 8);
    spinalDisc->AddElement(elH, 9.60*CLHEP::perCent);
    spinalDisc->AddElement(elC, 9.90*CLHEP::perCent);
    spinalDisc->AddElement(elN, 2.20*CLHEP::perCent);
    spinalDisc->AddElement(elO, 74.40*CLHEP::perCent);
    spinalDisc->AddElement(elNa, 0.50*CLHEP::perCent);
    spinalDisc->AddElement(elP, 2.20*CLHEP::perCent);
    spinalDisc->AddElement(elS, 0.90*CLHEP::perCent);
    spinalDisc->AddElement(elCl, 0.30*CLHEP::perCent);


    // Water
    auto* water = new G4Material( "Water",
                                        density = 1.0*CLHEP::g/CLHEP::cm3,
                                        numberofElements = 2 );
    water->AddElement(elH,0.112);
    water->AddElement(elO,0.888);

    // Muscle
    auto* muscle = new G4Material( "Muscle",
                                         density = 1.061*CLHEP::g/CLHEP::cm3,
                                         numberofElements = 9 );
    muscle->AddElement(elH,0.102);
    muscle->AddElement(elC,0.143);
    muscle->AddElement(elN,0.034);
    muscle->AddElement(elO,0.710);
    muscle->AddElement(elNa,0.001);
    muscle->AddElement(elP,0.002);
    muscle->AddElement(elS,0.003);
    muscle->AddElement(elCl,0.001);
    muscle->AddElement(elK,0.004);

    // Liver
    auto* liver = new G4Material( "Liver",
                                        density = 1.071*CLHEP::g/CLHEP::cm3,
                                        numberofElements = 9);
    liver->AddElement(elH,0.102);
    liver->AddElement(elC,0.139);
    liver->AddElement(elN,0.030);
    liver->AddElement(elO,0.716);
    liver->AddElement(elNa,0.002);
    liver->AddElement(elP,0.003);
    liver->AddElement(elS,0.003);
    liver->AddElement(elCl,0.002);
    liver->AddElement(elK,0.003);

    // Tooth Dentin
    auto* toothDentin = new G4Material ("ToothDentin", 2.14 * CLHEP::g/CLHEP::cm3,
                                              numberofElements = 10);
    toothDentin->AddElement(elH, 2.67*CLHEP::perCent);
    toothDentin->AddElement(elC, 12.77*CLHEP::perCent);
    toothDentin->AddElement(elN, 4.27*CLHEP::perCent);
    toothDentin->AddElement(elO, 40.40*CLHEP::perCent);
    toothDentin->AddElement(elNa, 0.65*CLHEP::perCent);
    toothDentin->AddElement(elMg, 0.59*CLHEP::perCent);
    toothDentin->AddElement(elP, 11.86*CLHEP::perCent);
    toothDentin->AddElement(elCl, 0.04*CLHEP::perCent);
    toothDentin->AddElement(elCa, 26.74*CLHEP::perCent);
    toothDentin->AddElement(elZn, 0.01*CLHEP::perCent);


    // Trabecular Bone
    auto* trabecularBone = new G4Material("TrabecularBone",
                                                density = 1.159*CLHEP::g/CLHEP::cm3,
                                                numberofElements = 12 );
    trabecularBone->AddElement(elH,0.085);
    trabecularBone->AddElement(elC,0.404);
    trabecularBone->AddElement(elN,0.058);
    trabecularBone->AddElement(elO,0.367);
    trabecularBone->AddElement(elNa,0.001);
    trabecularBone->AddElement(elMg,0.001);
    trabecularBone->AddElement(elP,0.034);
    trabecularBone->AddElement(elS,0.002);
    trabecularBone->AddElement(elCl,0.002);
    trabecularBone->AddElement(elK,0.001);
    trabecularBone->AddElement(elCa,0.044);
    trabecularBone->AddElement(elFe,0.001);

    // Trabecular bone used in the DICOM Head

    auto* trabecularBone_head = new G4Material ("TrabecularBone_HEAD",
                                                      1.18 * CLHEP::g/CLHEP::cm3,
                                                      numberofElements = 12);
    trabecularBone_head->AddElement(elH, 8.50*CLHEP::perCent);
    trabecularBone_head->AddElement(elC, 40.40*CLHEP::perCent);
    trabecularBone_head->AddElement(elN, 2.80*CLHEP::perCent);
    trabecularBone_head->AddElement(elO, 36.70*CLHEP::perCent);
    trabecularBone_head->AddElement(elNa, 0.10*CLHEP::perCent);
    trabecularBone_head->AddElement(elMg, 0.10*CLHEP::perCent);
    trabecularBone_head->AddElement(elP, 3.40*CLHEP::perCent);
    trabecularBone_head->AddElement(elS, 0.20*CLHEP::perCent);
    trabecularBone_head->AddElement(elCl, 0.20*CLHEP::perCent);
    trabecularBone_head->AddElement(elK, 0.10*CLHEP::perCent);
    trabecularBone_head->AddElement(elCa, 7.40*CLHEP::perCent);
    trabecularBone_head->AddElement(elFe, 0.10*CLHEP::perCent);

    // Dense Bone
    auto* denseBone = new G4Material( "DenseBone",
                                            density = 1.575*CLHEP::g/CLHEP::cm3,
                                            numberofElements = 11 );
    denseBone->AddElement(elH,0.056);
    denseBone->AddElement(elC,0.235);
    denseBone->AddElement(elN,0.050);
    denseBone->AddElement(elO,0.434);
    denseBone->AddElement(elNa,0.001);
    denseBone->AddElement(elMg,0.001);
    denseBone->AddElement(elP,0.072);
    denseBone->AddElement(elS,0.003);
    denseBone->AddElement(elCl,0.001);
    denseBone->AddElement(elK,0.001);
    denseBone->AddElement(elCa,0.146);

    // Cortical Bone (ICRP - NIST)
    auto* corticalBone = new G4Material ("CorticalBone", 1.85 * CLHEP::g/CLHEP::cm3,
                                               numberofElements = 9);
    corticalBone->AddElement(elH, 4.7234*CLHEP::perCent);
    corticalBone->AddElement(elC, 14.4330*CLHEP::perCent);
    corticalBone->AddElement(elN, 4.199*CLHEP::perCent);
    corticalBone->AddElement(elO, 44.6096*CLHEP::perCent);
    corticalBone->AddElement(elMg, 0.22*CLHEP::perCent);
    corticalBone->AddElement(elP, 10.497*CLHEP::perCent);
    corticalBone->AddElement(elS, 0.315*CLHEP::perCent);
    corticalBone->AddElement(elCa, 20.993*CLHEP::perCent);
    corticalBone->AddElement(elZn, 0.01*CLHEP::perCent);


    // Tooth enamel
    auto* toothEnamel = new G4Material ("ToothEnamel", 2.89 * CLHEP::g/CLHEP::cm3,
                                              numberofElements = 10);
    toothEnamel->AddElement(elH, 0.95*CLHEP::perCent);
    toothEnamel->AddElement(elC, 1.11*CLHEP::perCent);
    toothEnamel->AddElement(elN, 0.23*CLHEP::perCent);
    toothEnamel->AddElement(elO,41.66*CLHEP::perCent);
    toothEnamel->AddElement(elNa, 0.79*CLHEP::perCent);
    toothEnamel->AddElement(elMg, 0.23*CLHEP::perCent);
    toothEnamel->AddElement(elP, 18.71*CLHEP::perCent);
    toothEnamel->AddElement(elCl, 0.34*CLHEP::perCent);
    toothEnamel->AddElement(elCa, 35.97*CLHEP::perCent);
    toothEnamel->AddElement(elZn, 0.02*CLHEP::perCent);

    fOriginalMaterials.push_back(fAir); // rho = 0.00129
    fOriginalMaterials.push_back(lunginhale); // rho = 0.217
    fOriginalMaterials.push_back(lungexhale); // rho = 0.508
    fOriginalMaterials.push_back(adiposeTissue); // rho = 0.967
    fOriginalMaterials.push_back(breast ); // rho = 0.990
    fOriginalMaterials.push_back(water); // rho = 1.018
    fOriginalMaterials.push_back(muscle); // rho = 1.061
    fOriginalMaterials.push_back(liver); // rho = 1.071
    fOriginalMaterials.push_back(trabecularBone); // rho = 1.159 - HEAD PHANTOM
    fOriginalMaterials.push_back(denseBone); // rho = 1.575
    G4cout << "Default materials of the DICOM Extended examples have been used"
           << G4endl;
}

void BDSCT::ReadPhantomData()
{

    G4String fileName = BDSDicomFileMgr::GetInstance()->GetFileOutName();

    std::ifstream fin(fileName);
    std::vector<G4String> wl;
    G4int nMaterials;
    fin >> nMaterials;
    G4String mateName;
    G4int nmate;
    for( G4int ii = 0; ii < nMaterials; ++ii )
    {
    fin >> nmate;
    fin >> mateName;
    if( mateName[0] == '"' && mateName[mateName.length()-1] == '"' ) {
      mateName = mateName.substr(1,mateName.length()-2);
    }
    G4cout << "GmReadPhantomG4Geometry::ReadPhantomData reading nmate "
           << ii << " = " << nmate
           << " mate " << mateName << G4endl;
    if( ii != nmate )
    G4Exception("GmReadPhantomG4Geometry::ReadPhantomData",
                "Wrong argument", FatalErrorInArgument,
        "Material number should be in increasing order:wrong material number");

    G4Material* mate = 0;
    const G4MaterialTable* matTab = G4Material::GetMaterialTable();
    for( auto matite = matTab->cbegin(); matite != matTab->cend(); ++matite )
    {
      if( (*matite)->GetName() == mateName ) {
        mate = *matite;
      }
    }
    if( mate == 0 ) {
      mate = G4NistManager::Instance()->FindOrBuildMaterial(mateName);
    }
    if( !mate ) G4Exception("GmReadPhantomG4Geometry::ReadPhantomData",
                            "Wrong argument",
                            FatalErrorInArgument,
                            ("Material not found" + mateName).c_str());
    thePhantomMaterialsOriginal[nmate] = mate;
    }

    fin >> fNVoxelX >> fNVoxelY >> fNVoxelZ;
    G4cout << "GmReadPhantomG4Geometry::ReadPhantomData fNVoxel X/Y/Z "
         << fNVoxelX << " "
         << fNVoxelY << " " << fNVoxelZ << G4endl;
    fin >> fMinX >> fMaxX;
    fin >> fMinY >> fMaxY;
    fin >> fMinZ >> fMaxZ;
    fVoxelHalfDimX = (fMaxX-fMinX)/fNVoxelX/2.;
    fVoxelHalfDimY = (fMaxY-fMinY)/fNVoxelY/2.;
    fVoxelHalfDimZ = (fMaxZ-fMinZ)/fNVoxelZ/2.;

    fMateIDs = new size_t[fNVoxelX*fNVoxelY*fNVoxelZ];
    for( G4int iz = 0; iz < fNVoxelZ; ++iz ) {
    for( G4int iy = 0; iy < fNVoxelY; ++iy ) {
      for( G4int ix = 0; ix < fNVoxelX; ++ix ) {
        G4int mateID;
        fin >> mateID;
        G4int nnew = ix + (iy)*fNVoxelX + (iz)*fNVoxelX*fNVoxelY;
        if( mateID < 0 || mateID >= nMaterials ) {
          G4Exception("GmReadPhantomG4Geometry::ReadPhantomData",
                      "Wrong index in phantom file",
                      FatalException,
                      G4String("It should be between 0 and "
                              + G4UIcommand::ConvertToString(nMaterials-1)
                              + ", while it is "
                              + G4UIcommand::ConvertToString(mateID)).c_str());
        }
        fMateIDs[nnew] = mateID;
      }
    }
    }

    ReadVoxelDensities( fin );

    fin.close();

}

void BDSCT::ReadVoxelDensities( std::ifstream& fin )
{
    G4String stemp;
    std::map<G4int, std::pair<G4double,G4double> > densiMinMax;
    std::map<G4int, std::pair<G4double,G4double> >::iterator mpite;
    for( G4int ii = 0; ii < G4int(thePhantomMaterialsOriginal.size()); ++ii )
    {
        densiMinMax[ii] = std::pair<G4double,G4double>(DBL_MAX,-DBL_MAX);
    }

    char* part = std::getenv( "DICOM_CHANGE_MATERIAL_DENSITY" );
    G4double densityDiff = -1.;
    if( part ) densityDiff = G4UIcommand::ConvertToDouble(part);

    std::map<G4int,G4double> densityDiffs;
    for( G4int ii = 0; ii < G4int(thePhantomMaterialsOriginal.size()); ++ii )
    {
        densityDiffs[ii] = densityDiff; //currently all materials with same step
    }
    //  densityDiffs[0] = 0.0001; //air

    //--- Calculate the average material density for each material/density bin
    std::map< std::pair<G4Material*,G4int>, matInfo* > newMateDens;

    //---- Read the material densities
    G4double dens;
    for( G4int iz = 0; iz < fNVoxelZ; ++iz ) {
        for( G4int iy = 0; iy < fNVoxelY; ++iy ) {
            for( G4int ix = 0; ix < fNVoxelX; ++ix ) {
                fin >> dens;
                G4int copyNo = ix + (iy)*fNVoxelX + (iz)*fNVoxelX*fNVoxelY;

                if( densityDiff != -1. ) continue;

                //--- store the minimum and maximum density for each material
                mpite = densiMinMax.find( G4int(fMateIDs[copyNo]) );
                if( dens < (*mpite).second.first ) (*mpite).second.first = dens;
                if( dens > (*mpite).second.second ) (*mpite).second.second = dens;
                //--- Get material from original list of material in file
                G4int mateID = G4int(fMateIDs[copyNo]);
                std::map<G4int,G4Material*>::const_iterator imite =
                        thePhantomMaterialsOriginal.find(mateID);

                //--- Check if density is equal to the original material density
                if(std::fabs(dens - (*imite).second->GetDensity()/CLHEP::g*CLHEP::cm3 )
                   < 1.e-9 ) continue;

                //--- Build material name with thePhantomMaterialsOriginal name+density
                G4int densityBin = (G4int(dens/densityDiffs[mateID]));

                G4String mateName = (*imite).second->GetName()
                                    + G4UIcommand::ConvertToString(densityBin);
                //--- Look if it is the first voxel with this material/densityBin
                std::pair<G4Material*,G4int> matdens((*imite).second, densityBin );

                auto mppite = newMateDens.find( matdens );
                if( mppite != newMateDens.cend() ){
                    matInfo* mi = (*mppite).second;
                    mi->fSumdens += dens;
                    mi->fNvoxels++;
                    fMateIDs[copyNo] = thePhantomMaterialsOriginal.size()-1 + mi->fId;
                } else {
                    matInfo* mi = new matInfo;
                    mi->fSumdens = dens;
                    mi->fNvoxels = 1;
                    mi->fId = G4int(newMateDens.size()+1);
                    newMateDens[matdens] = mi;
                    fMateIDs[copyNo] = thePhantomMaterialsOriginal.size()-1 + mi->fId;
                }
            }
        }
    }

    //----- Build the list of phantom materials that go to Parameterisation
    //--- Add original materials
    for( auto mimite = thePhantomMaterialsOriginal.cbegin();
         mimite != thePhantomMaterialsOriginal.cend(); ++mimite )
    {
        fMaterials.push_back( (*mimite).second );
    }
    //
    //---- Build and add new materials
    for( auto mppite= newMateDens.cbegin(); mppite!=newMateDens.cend(); ++mppite )
    {
        G4double averdens = (*mppite).second->fSumdens/(*mppite).second->fNvoxels;
        G4double saverdens = G4int(1000.001*averdens)/1000.;
        G4String mateName = ((*mppite).first).first->GetName() + "_"
                            + G4UIcommand::ConvertToString(saverdens);
        fMaterials.push_back( BuildMaterialWithChangingDensity(
                (*mppite).first.first, G4float(averdens), mateName ) );
    }
}

void BDSCT::ReadPhantomDataFile(const G4String& fname)
{
    G4cout << " DicomDetectorConstruction::ReadPhantomDataFile opening file "
           << fname << G4endl; //GDEB

#ifdef G4VERBOSE
    G4cout << " DicomDetectorConstruction::ReadPhantomDataFile opening file "
           << fname << G4endl;
#endif

    std::ifstream fin(fname.c_str(), std::ios_base::in);
    if( !fin.is_open() ) {
        G4Exception("DicomDetectorConstruction::ReadPhantomDataFile",
                    "",
                    FatalErrorInArgument,
                    G4String("File not found " + fname ).c_str());
    }
    //----- Define density differences (maximum density difference to create
    // a new material)
    char* part = std::getenv( "DICOM_CHANGE_MATERIAL_DENSITY" );
    G4double densityDiff = -1.;
    if( part ) densityDiff = G4UIcommand::ConvertToDouble(part);
    if( densityDiff != -1. )
    {
        for( unsigned int ii = 0; ii < fOriginalMaterials.size(); ++ii )
        {
            fDensityDiffs[ii] = densityDiff; //currently all materials with
            // same difference
        }
    }
    else
    {
        if( fMaterials.size() == 0 ) { // do it only for first slice
            for( unsigned int ii = 0; ii < fOriginalMaterials.size(); ++ii )
            {
                fMaterials.push_back( fOriginalMaterials[ii] );
            }
        }
    }

    //----- Read data header
    auto sliceHeader = new BDSDicomPhantomZSliceHeader( fin );
    fZSliceHeaders.push_back( sliceHeader );

    //----- Read material indices
    G4int nVoxels = sliceHeader->GetNoVoxels();

    //--- If first slice, initiliaze fMateIDs
    if( fZSliceHeaders.size() == 1 ) {
        //fMateIDs = new unsigned int[fNoFiles*nVoxels];
        fMateIDs = new size_t[fNoFiles*nVoxels];

    }

    unsigned int mateID;
    // number of voxels from previously read slices
    G4int voxelCopyNo = G4int((fZSliceHeaders.size()-1)*nVoxels);
    for( G4int ii = 0; ii < nVoxels; ++ii, voxelCopyNo++ )
    {
        fin >> mateID;
        fMateIDs[voxelCopyNo] = mateID;
    }

    //----- Read material densities and build new materials if two voxels have
    //  same material but its density is in a different density interval
    // (size of density intervals defined by densityDiff)
    G4double density;
    // number of voxels from previously read slices
    voxelCopyNo = G4int((fZSliceHeaders.size()-1)*nVoxels);
    for( G4int ii = 0; ii < nVoxels; ++ii, voxelCopyNo++ )
    {
        fin >> density;

        //-- Get material from list of original materials
        mateID = unsigned(fMateIDs[voxelCopyNo]);
        G4Material* mateOrig  = fOriginalMaterials[mateID];

        //-- Get density bin: middle point of the bin in which the current
        // density is included
        G4String newMateName = mateOrig->GetName();
        G4float densityBin = 0.;
        if( densityDiff != -1.) {
            densityBin = G4float(fDensityDiffs[mateID]) *
                         (G4int(density/fDensityDiffs[mateID])+0.5);
            //-- Build the new material name
            newMateName += G4UIcommand::ConvertToString(densityBin);
        }

        //-- Look if a material with this name is already created
        //  (because a previous voxel was already in this density bin)
        unsigned int im;
        for( im = 0; im < fMaterials.size(); ++im )
        {
            if( fMaterials[im]->GetName() == newMateName ) {
                break;
            }
        }
        //-- If material is already created use index of this material
        if( im != fMaterials.size() ) {
            fMateIDs[voxelCopyNo] = im;
            //-- else, create the material
        } else {
            if( densityDiff != -1.) {
                fMaterials.push_back( BuildMaterialWithChangingDensity( mateOrig,
                                                                        densityBin, newMateName ) );
                fMateIDs[voxelCopyNo] = fMaterials.size()-1;
            } else {
                G4cerr << " im " << im << " < " << fMaterials.size() << " name "
                       << newMateName << G4endl;
                G4Exception("DicomDetectorConstruction::ReadPhantomDataFile",
                            "",
                            FatalErrorInArgument,
                            "Wrong index in material"); //it should never reach here
            }
        }
    }
}

void BDSCT::MergeZSliceHeaders()
{
    //----- Images must have the same dimension ...
    fZSliceHeaderMerged = new BDSDicomPhantomZSliceHeader( *fZSliceHeaders[0] );
    for( unsigned int ii = 1; ii < fZSliceHeaders.size(); ++ii )
    {
        *fZSliceHeaderMerged += *fZSliceHeaders[ii];
    }
}

G4Material* BDSCT::BuildMaterialWithChangingDensity(
        const G4Material* origMate, G4float density, G4String newMateName )
{
    //----- Copy original material, but with new density
    G4int nelem = G4int(origMate->GetNumberOfElements());
    G4Material* mate = new G4Material( newMateName, density*CLHEP::g/CLHEP::cm3, nelem,
                                       kStateUndefined, CLHEP::STP_Temperature );

    for( G4int ii = 0; ii < nelem; ++ii )
    {
        G4double frac = origMate->GetFractionVector()[ii];
        G4Element* elem = const_cast<G4Element*>(origMate->GetElement(ii));
        mate->AddElement( elem, frac );
    }

    return mate;
}

void BDSCT::BuildContainerLogicalVolume()
{
    //---- Extract number of voxels and voxel dimensions

    //----- Define the volume that contains all the voxels
    fContainer_solid = new G4Box("phantomContainer",fNVoxelX*fVoxelHalfDimX,
                                 fNVoxelY*fVoxelHalfDimY,
                                 fNVoxelZ*fVoxelHalfDimZ);

    containerSolid = fContainer_solid->GetConstituentSolid(0);

    containerLogicalVolume =
            new G4LogicalVolume( fContainer_solid,
                    //the material is not important, it will be fully filled by the voxels
                                 fMaterials[0],
                                 "phantomContainer",
                                 0, 0, 0 );

    G4ThreeVector posCentreVoxels((fMinX+fMaxX)/2.,
                                  (fMinY+fMaxY)/2.,
                                  (fMinZ+fMaxZ)/2.);

    fContainer_phys =
            new G4PVPlacement(0,  // rotation
                              posCentreVoxels,
                              containerLogicalVolume,     // The logic volume
                              "phantomContainer",  // Name
                              fWorld_logic,  // Mother
                              false,           // No op. bool.
                              1);              // Copy number
}

void BDSCT::BuildUserLimits()
{
    userLimits = new G4UserLimits(1, // max 1mm step into dump
                                  0, // max track length
                                  0, // max time
                                  0, // max kinetic energy
                                  std::numeric_limits<double>::max());

    RegisterUserLimits(userLimits);
}

void BDSCT::SetScorer(G4LogicalVolume* voxel_logic)
{
    fScorers.insert(voxel_logic);
}
