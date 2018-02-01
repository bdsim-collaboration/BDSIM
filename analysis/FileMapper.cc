/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#include "FileMapper.hh"
#include "Header.hh"

#include "BDSOutputROOTEventHeader.hh"

#include "TDirectory.h"
#include "TFile.h"
#include "TH1.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"
#include "TList.h"
#include "TKey.h"
#include "TTree.h"

#include <algorithm>
#include <iostream>
#include <string>
#include <vector>

bool RBDS::GetFileType(TFile*       file,
		       std::string& fileType)
{
  // check if valid file at all
  if (file->IsZombie())
    {return false;}

  std::vector<std::string> treeNames;
  TList* kl = file->GetListOfKeys();
  for (int i = 0; i < kl->GetEntries(); ++i)
    {treeNames.push_back(std::string(kl->At(i)->GetName()));}

  auto result = std::find(treeNames.begin(), treeNames.end(), "Header");
  if (result == treeNames.end())
    {return false;} // no header so definitely not a bdsim file

  // load header to get which type of file it is
  Header* headerLocal = new Header();
  TTree* headerTree = static_cast<TTree*>(file->Get("Header"));
  headerLocal->SetBranchAddress(headerTree);
  headerTree->GetEntry(0);
  fileType = headerLocal->header->fileType;
  delete headerLocal;
  return true;
}

bool RBDS::IsBDSIMOutputFile(TFile* file)
{
  // check if valid file at all
  if (file->IsZombie())
    {return false;}
  
  std::string fileType;
  bool success = GetFileType(file, fileType);
  if (!success)
    {return false;}

  return fileType == "BDSIM";
}

bool RBDS::IsREBDSIMOutputFile(TFile* file)
{
  // check if valid file at all
  if (file->IsZombie())
    {return false;}

  std::string fileType;
  bool success = GetFileType(file, fileType);
  if (!success)
    {return false;}

  return fileType == "REBDISM";
}

int RBDS::DetermineDimensionality(TH1* h)
{
  if (dynamic_cast<TH1D*>(h))
    {return 1;}
  else if (dynamic_cast<TH2D*>(h))
    {return 2;}
  else if (dynamic_cast<TH3D*>(h))
    {return 3;}
  else
    {return 1;}
}

void RBDS::WarningMissingHistogram(const std::string& histName,
				   const std::string& fileName)
{
  std::cout << "No histogram \"" << histName << "\" in file "
	    << fileName << std::endl;
}

