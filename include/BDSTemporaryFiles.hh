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
#ifndef BDSTEMPORARYFILES_H
#define BDSTEMPORARYFILES_H

#include "G4String.hh"

#include <vector>

/* @brief Holder for all temporary files created by BDSIM.
 * 
 * A single place where all temporary file names are held. The
 * class determines a directory to put them when required and 
 * cleans up all temporary files upon deletion.
 * 
 * @author Laurie Nevay
 */

class BDSTemporaryFiles
{
public:
  ~BDSTemporaryFiles();

  /// Singleton accessor.
  static BDSTemporaryFiles* Instance();

  /// Create a temporary file for use in BDSIM. A unique file name will be returned.
  /// It's the caller's resopnsibility to open this file.
  G4String CreateTemporaryFile(G4String extension = "tmp");

  /// Create a temporary file for use in BDSIM based on a currently existing one. A file
  /// with the same name but with the file name prefix and suffix in the temporary
  /// directory will be returned. It's the caller's resopnsibility to open this file.
  G4String CreateTemporaryFile(const G4String& originalFilePath,
			       G4String        fileNamePrefix = "",
			       G4String        fileNameSuffix = "");

private:
  /// Private default constructor as singleton.
  BDSTemporaryFiles();

  /// Singleton instance.
  static BDSTemporaryFiles* instance;

  G4String              temporaryDirectory; ///< Directory all files will be placed in.
  std::vector<G4String> allocatedFiles;     ///< Record of of all files allocated.
  G4int                 unamedFileCount;    ///< Count of unnamed files created.
};

#endif
