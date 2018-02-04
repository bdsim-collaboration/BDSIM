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
#include <vector>

class HistogramAccumulator;
class TDirectory;
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

  /// Similar but opens file first.
  bool IsBDSIMOutputFile(const std::string filePath);

  /// Whether the file type is a REBDSIM output one. Does not close file. May change
  /// the branch address for the header in the file.
  bool IsREBDSIMOutputFile(TFile* file);

  /// Similar but opens file first.
  bool IsREBDSIMOutputFile(const std::string filePath);

  int DetermineDimensionality(TH1* h);

  void WarningMissingHistogram(const std::string& histName,
			       const std::string& fileName);

  struct HistogramPath
  {
    std::string path; // without histogram name
    std::string name; // name of histogram
    HistogramAccumulator* accumulator;
    TDirectory* outputDir;
  };

  enum class MergeType {none, meanmerge, sum};

  MergeType DetermineMergeType(const std::string& parentDir);

}


class HistogramMap
{
public:
  HistogramMap(TFile* file,
	       TFile* output,
	       bool   debugIn = false);
  ~HistogramMap(){;}

  void MapDirectory(TDirectory* dir,
		    TFile*      output,
		    const std::string& parentDir);
  
  inline const std::vector<RBDS::HistogramPath> Histograms() const {return histograms;}
  
  //inline const std::vector<std::string>& HistogramMeanPaths() const {return histsMeanPath;}
  //inline const std::vector<std::string>& HistogramSumPaths()  const {return histsSumPath;}

private:
  HistogramMap() = delete;

  bool debug;

  std::vector<RBDS::HistogramPath> histograms;
};

#endif
