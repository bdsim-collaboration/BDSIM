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
#ifndef FILEMAPPER_H
#define FILEMAPPER_H

#include <string>

class TFile;
class TH1;

namespace RBDS
{
  /// Get the format of the root file open (bdsim, rebdsim, combined). Returns
  /// true for success and false otherwise. Filetype string is written to the
  /// string passed by reference. Changes the branch address of header if it's
  /// already set.
  bool GetFileType(TFile* file,
		   std::string& fileType);

  /// Whether the file type is a BDSIM output one. Does not close file. May change
  /// the branch address for the header in the file.
  bool IsBDSIMOutputFile(TFile* file);

  /// Whether the file type is a REBDSIM output one. Does not close file. May change
  /// the branch address for the header in the file.
  bool IsREBDSIMOutputFile(TFile* file);

  int DetermineDimensionality(TH1* h);

  void WarningMissingHistogram(const std::string& histName,
			       const std::string& fileName);
}

#endif
