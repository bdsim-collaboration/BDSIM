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
#include "BDSDebug.hh"
#include "BDSGlobalConstants.hh"
#include "BDSTemporaryFiles.hh"
#include "BDSUtilities.hh"

#include "globals.hh"

#include <cstdio>
#include <string>

BDSTemporaryFiles* BDSTemporaryFiles::instance = nullptr;

BDSTemporaryFiles::BDSTemporaryFiles():
  unamedFileCount(0)
{
  // determine temp directory
}

BDSTemporaryFiles::~BDSTemporaryFiles()
{
  if (BDSGlobalConstants::Instance()->RemoveTemporaryFiles())
    {
      if (allocatedFiles.empty())
	{// no need to warn user about deleting no files.
	  instance = nullptr;
	  return;
	}
      
      G4cout << __METHOD_NAME__<< "Removing temporary files" << G4endl;

      for (const auto& fn : allocatedFiles)
        {
	  if (BDS::FileExists(fn))
            {
#ifdef BDSDEBUG
	      G4cout << "Removing \"" << filename << "\"" << G4endl;
#endif
	      int result = remove(fn.c_str()); // delete file
	      if (result != 0)
#ifdef BDSDEBUG
		{G4cout << "Error deleting file: \"" << fn << "\"" << G4endl;}
#else
	        {continue;}
#endif
	    }
        }
      G4cout << __METHOD_NAME__ << "Temporary files removed" << G4endl;
    }
  instance = nullptr;
}

BDSTemporaryFiles* BDSTemporaryFiles::Instance()
{
  if (!instance)
    {instance = new BDSTemporaryFiles();}
  return instance;
}

G4String BDSTemporaryFiles::CreateTemporaryFile(G4String extension)
{
  G4String newFileName = temporaryDirectory + "/bdsTemp_" + std::to_string(unamedFileCount) + "." + extension;
  unamedFileCount += 1;

  allocatedFiles.push_back(newFileName);
  return newFileName;
}

G4String BDSTemporaryFiles::CreateTemporaryFile(const G4String& originalFilePath,
						G4String        fileNamePrefix,
						G4String        fileNameSuffix)
{
  G4String path     = "";
  G4String fileName = "";
  BDS::SplitPathAndFileName(originalFilePath, path, fileName);

  G4String newFileName = temporaryDirectory + "/" + fileNamePrefix + fileName + fileNameSuffix;
  allocatedFiles.push_back(newFileName);
  return newFileName;
}
   
