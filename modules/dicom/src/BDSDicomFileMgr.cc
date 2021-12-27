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

//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//

#include "BDSDicomFileMgr.hh"
#include "BDSDicomFileCT.hh"

#include "G4tgrFileIn.hh"
#include "G4UIcommand.hh"
#include "G4PhysicsOrderedFreeVector.hh"

#include "dcmtk/dcmdata/dcdeftag.h"

#include <map>
#include <vector>

BDSDicomFileMgr* BDSDicomFileMgr::theInstance = nullptr;
int BDSDicomFileMgr::verbose = 1;

BDSDicomFileMgr* BDSDicomFileMgr::GetInstance()
{
  if (!theInstance)
    {theInstance = new BDSDicomFileMgr();}
  return theInstance;
}

BDSDicomFileMgr::BDSDicomFileMgr()
{
  fCompression = 1.;
  theCTFileAll = nullptr;
}

void BDSDicomFileMgr::Convert(G4String filePath,
			      G4String fileName)
{
  G4tgrFileIn fin = G4tgrFileIn::GetInstance(filePath + fileName);
  std::vector<G4String> wl;
  
  std::vector<G4double> hUnits;
  std::vector<G4double> dens;
  
  // Read each file in file list
  theFileOutName = "default.g4dcm";
  int ii;
  for (ii = 0;; ii++)
    {
      if (!fin.GetWordsInLine(wl))
	break;
      if (wl[0] == ":COMPRESSION")
        {
	  CheckNColumns(wl, 2);
	  SetCompression(wl[1]);
        }
      else if (wl[0] == ":FILE")
        {
	  CheckNColumns(wl, 2);
	  G4cout << "@@@@@@@ Reading FILE: " << wl[1] << G4endl;
	  AddFile(filePath + wl[1]);
        }
      else if (wl[0] == ":FILE_OUT")
        {
	  CheckNColumns(wl, 2);
	  theFileOutName = wl[1];
        }
      else if (wl[0] == ":MATE_DENS")
        {
	  CheckNColumns(wl, 3);
	  AddMaterialDensity(wl);
        }
      else if (wl[0] == ":MATE")
        {
	  CheckNColumns(wl, 3);
	  AddMaterial(wl);
        }
      else if (wl[0] == ":CT2D")
        {
	  CheckNColumns(wl, 3);
	  hUnits.push_back(G4UIcommand::ConvertToDouble(wl[1]));
	  dens.push_back(G4UIcommand::ConvertToDouble(wl[2]));
        }
      else
        {
	  G4Exception("DICOM2G4",
		      "Wrong argument",
		      FatalErrorInArgument,
		      G4String("UNKNOWN TAG IN FILE  " + wl[0]).c_str());
        }
    }
  
  results = new G4PhysicsOrderedFreeVector(&hUnits[0], &dens[0], hUnits.size());
  
  ProcessFiles();
}

void BDSDicomFileMgr::CheckNColumns(std::vector<G4String> wl,
				    size_t vsizeTh)
{
  if (wl.size() != vsizeTh)
    {
      G4cerr << " Reading line " << G4endl;
      for (const auto & ii : wl)
        {G4cerr << ii << " ";}
      G4cerr << G4endl;
      G4Exception("DICOM2G4",
		  "D2G0010",
		  FatalErrorInArgument,
		  ("Wrong number of columns in line " + std::to_string(wl.size()) + " <> " + std::to_string(vsizeTh)).c_str());
    }
}

void BDSDicomFileMgr::SetCompression(G4String fComp)
{
  fCompression = G4UIcommand::ConvertToDouble(fComp);
}

void BDSDicomFileMgr::AddFile(G4String fileName)
{
  DcmFileFormat dfile;
  if (!(dfile.loadFile(fileName.c_str())).good())
    {
      G4Exception("DicomHandler::ReadFile",
		  "dfile.loadFile",
		  FatalErrorInArgument,
		  ("Error reading file " + fileName).c_str());
    }
  DcmDataset *dset = dfile.getDataset();
  
  OFString dModality;
  if (!dset->findAndGetOFString(DCM_Modality, dModality).good())
    {
      G4Exception("DicomHandler::ReadData ",
		  "",
		  FatalException,
		  " Have not read Modality");
    }
  
  if (dModality == "CT" || dModality == "OT")
    {
      auto *df = new BDSDicomFileCT(dset);
      df->ReadData();
      df->SetFileName(fileName);
      // reorder by location
      theCTFiles[df->GetMaxZ()] = df;
      G4cout << "Number of voxels: " << df->GetNoVoxels() << G4endl;
    }
  else
    {
      G4Exception("DicomFileMgr::AddFIle()",
		  "DFM001",
		  FatalErrorInArgument,
		  (G4String("File is not of type CT or RTSTRUCT or RTPLAN, but: ") + dModality).c_str());
    }
}

void BDSDicomFileMgr::AddMaterial(std::vector<G4String> wl)
{
  if (!theMaterials.empty() && bMaterialsDensity)
    {
      G4Exception("DicomFileMgr::AddMaterial",
		  "DFM005",
		  FatalException,
		  "Trying to add a Material with :MATE and another with :MATE_DENS, check your input file");
    }
  bMaterialsDensity = false;
  // Material (G4string) is associated with Hounsfield value (double???)
  double inter = G4UIcommand::ConvertToDouble(wl[2]);
  theMaterials[inter] = wl[1];
}

void BDSDicomFileMgr::AddMaterialDensity(std::vector<G4String> wl)
{
  if (!theMaterialsDensity.empty() && !bMaterialsDensity)
    {
      G4Exception("DicomFileMgr::AddMaterial",
		  "DFM005",
		  FatalException,
		  "Trying to add a Material with :MATE and another with :MATE_DENS, check your input file");
    }
  bMaterialsDensity = true;
  theMaterialsDensity[G4UIcommand::ConvertToDouble(wl[2])] = wl[1];
}

void BDSDicomFileMgr::AddCT2Density(std::vector<G4String> wl)
{
  theCT2Density[G4UIcommand::ConvertToInt(wl[1])] = G4UIcommand::ConvertToDouble(wl[2]);
  G4cout << this << " AddCT2density " << theCT2Density.size() << G4endl; //GDEB
}

G4double BDSDicomFileMgr::Hounsfield2density(G4double Hval)
{
  return results ? results->Value(Hval) : 1.0;
}

size_t BDSDicomFileMgr::GetMaterialIndex(G4double Hval)
{
  auto ite = theMaterials.upper_bound(Hval);
  if (ite == theMaterials.end())
    {
      ite--;
      G4Exception("DicomFileMgr::GetMaterialIndex",
		  "DFM004",
		  FatalException,
		  ("Hounsfield value too big, change input file " + std::to_string(Hval) + " > " + std::to_string((*ite).first)).c_str());
    }
  
  size_t dist = std::distance(theMaterials.begin(), ite);
  
  return dist;
}

size_t BDSDicomFileMgr::GetMaterialIndexByDensity(G4double density)
{
  auto ite = theMaterialsDensity.upper_bound(density);
  if (ite == theMaterialsDensity.end())
    {
      ite--;
      G4Exception("DicomFileMgr::GetMaterialIndexByDensity",
		  "DFM003",
		  FatalException,
		  ("Density too big, change input file " + std::to_string(density) + " > " + std::to_string((*ite).first)).c_str());
    }
  
  size_t dist = std::distance(theMaterialsDensity.begin(), ite);
  
  return dist;
}

void BDSDicomFileMgr::ProcessFiles()
{
  if (theCTFiles.empty())
    {
      G4Exception("CheckCTSlices",
		  "DCM004",
		  JustWarning,
		  "No :FILE of type CT in input file");
    }
  else
    {
      CheckCTSlices();
      BuildCTMaterials();
      MergeCTFiles();
    }
  DumpToTextFile();
}

void BDSDicomFileMgr::CheckCTSlices()
{
  size_t nSlices = theCTFiles.size();
  G4cout << " DicomFileMgr::Checking CT slices: " << nSlices << G4endl;
  
  if (nSlices > 1)
    {
      if (nSlices == 2)
        {
	  auto ite = theCTFiles.begin();
	  BDSDicomFileCT *one = (*ite).second;
	  ite++;
	  BDSDicomFileCT *two = (*ite).second;
	  
	  G4double real_distance = (two->GetLocation() - one->GetLocation()) / 2.;
	  
	  if (one->GetMaxZ() != two->GetMinZ())
            {
	      one->SetMaxZ(one->GetLocation() + real_distance);
	      two->SetMinZ(two->GetLocation() - real_distance);
	      one->SetMinZ(one->GetLocation() - real_distance);
	      two->SetMaxZ(two->GetLocation() + real_distance);
            }
        }
      else
        {
	  auto ite0 = theCTFiles.begin();
	  auto ite1 = ite0;
	  ite1++;
	  auto ite2 = ite1;
	  ite2++;
	  for (; ite2 != theCTFiles.end(); ++ite0, ++ite1, ++ite2)
            {
	      auto *prev  = (BDSDicomFileCT *)((*ite0).second);
	      auto *slice = (BDSDicomFileCT *)((*ite1).second);
	      auto *next  = (BDSDicomFileCT *)((*ite2).second);
	      G4double real_up_distance = (next->GetLocation() - slice->GetLocation()) / 2.;
	      G4double real_down_distance = (slice->GetLocation() - prev->GetLocation()) / 2.;
	      G4double real_distance = real_up_distance + real_down_distance;
	      G4double stated_distance = slice->GetMaxZ() - slice->GetMinZ();
	      
	      if (std::fabs(real_distance - stated_distance) > 1.E-9)
                {
		  unsigned int sliceNum = std::distance(theCTFiles.begin(), ite1);
		  G4cerr << "\tDicomFileMgr::CheckCTSlices - Slice Distance Error in slice [" << sliceNum
			 << "]: Distance between this slice and slices up and down = "
			 << real_distance
			 << " <> Slice width = " << stated_distance
			 << " Slice locations " << prev->GetLocation() << " : " << slice->GetLocation()
			 << " : " << next->GetLocation()
			 << " DIFFERENCE= " << real_distance - stated_distance
			 << G4endl;
		  G4cerr << "!! WARNING: Geant4 will reset slice width so that it extends between "
			 << "lower and upper slice " << G4endl;
		  
		  slice->SetMinZ(slice->GetLocation() - real_down_distance);
		  slice->SetMaxZ(slice->GetLocation() + real_up_distance);
		  
		  if (ite0 == theCTFiles.begin())
                    {
		      prev->SetMaxZ(slice->GetMinZ());
		      // Using below would make all slice same thickness
		      prev->SetMinZ(prev->GetLocation() - real_down_distance);
                    }
		  if (static_cast<unsigned int>(std::distance(theCTFiles.begin(), ite2) + 1) ==
		      nSlices)
                    {
		      next->SetMinZ(slice->GetMaxZ());
		      // Using below would make all slice same thickness
		      next->SetMaxZ(next->GetLocation() + real_up_distance);
                    }
                }
            }
        }
    }
}

void BDSDicomFileMgr::BuildCTMaterials()
{
  G4cout << " DicomFileMgr::Building Materials " << theCTFiles.size() << G4endl; //GDEB
  auto ite = theCTFiles.begin();
  for (; ite != theCTFiles.end(); ite++)
    {(*ite).second->BuildMaterials();}
}

void BDSDicomFileMgr::MergeCTFiles()
{
  G4cout << " DicomFileMgr::Merging CT Files " << theCTFiles.size() << G4endl; //GDEB
  auto ite = theCTFiles.begin();
  theCTFileAll = new BDSDicomFileCT(*((*ite).second));
  ite++;
  for (; ite != theCTFiles.end(); ite++)
    {
      (*theCTFileAll) += *((*ite).second);
    }
}

void BDSDicomFileMgr::DumpToTextFile()
{
  G4cout << " DicomFileMgr::Dumping To Text File " << G4endl; //GDEB
  if (!theCTFiles.empty())
    {
      std::ofstream fout(theFileOutName);
      
      if (!bMaterialsDensity)
        {
	  fout << theMaterials.size() << std::endl;
	  std::map<G4double, G4String>::const_iterator ite;
	  G4int ii = 0;
	  for (ite = theMaterials.begin(); ite != theMaterials.end(); ite++, ii++)
            {
	      fout << ii << " \"" << (*ite).second << "\"" << std::endl;
            }
        }
      else
        {
	  fout << theMaterialsDensity.size() << std::endl;
	  std::map<G4double, G4String>::const_iterator ite;
	  G4int ii = 0;
	  for (ite = theMaterialsDensity.begin(); ite != theMaterialsDensity.end(); ite++, ii++)
            {
	      fout << ii << " \"" << (*ite).second << "\"" << std::endl;
            }
        }
      
      theCTFileAll->DumpHeaderToTextFile(fout);
      for (auto & theCTFile : theCTFiles)
        {
	  theCTFile.second->DumpMateIDsToTextFile(fout);
        }
      for (auto & theCTFile : theCTFiles)
        {
	  theCTFile.second->DumpDensitiesToTextFile(fout);
        } 
    }
}
