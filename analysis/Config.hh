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
#ifndef CONFIG_H
#define CONFIG_H

#include "TROOT.h"

#include "RebdsimTypes.hh"

#include <string>
#include <vector>
#include <map>

class HistogramDef;

/**
 * @brief Configuration and configuration parser class.
 * 
 * @author Stewart Boogert
 */

class Config
{
  /// Private members first as required in accessors.
private:
  /// Optional version of option keys in file.
  std::map<std::string, std::string> alternateKeys;

  /// @{ Storage of options.
  std::map<std::string, bool>        optionsBool;
  std::map<std::string, std::string> optionsString;
  std::map<std::string, double>      optionsNumber;
  /// @}

  /// Storage of histogram options.
  std::map<std::string, std::vector<HistogramDef*> > histoDefs;
  
public:
  virtual ~Config();

  /// Singleton accessor
  static Config* Instance(std::string fileName = "",
			  std::string inputFilePath = "",
			  std::string outputFileName = "");

  void ParseInputFile();

  /// @{ General accessor for option.
  inline std::string GetOptionString(std::string key) const {return optionsString.at(key);}
  inline bool        GetOptionBool(std::string key)   const {return optionsBool.at(key);}
  inline double      GetOptionNumber(std::string key) const {return optionsNumber.at(key);}
  /// @}

  /// Access all histogram definitions.
  inline const std::vector<HistogramDef*>& HistogramDefinitions(std::string treeName) const
  {return histoDefs.at(treeName);}

  /// Access all branches that are required for activation. This does not specialise on the
  /// leaf inside the branch and if one variable is required, the whole branch will be activated
  /// as there isn't much difference.  This can of course be revised in future.
  const RBDS::VectorString& BranchesToBeActivated(std::string treeName) const
  {return branches.at(treeName);}

  /// Access the map of all branches to be activated per tree.
  inline const RBDS::BranchMap& BranchesToBeActivated() const {return branches;}

  /// Boolean whether all branches should be turned on irrespective of map of
  /// individual branches to turn on.
  inline bool AllBranchesToBeActivated() const {return allBranchesActivated;}

  /// Set a branch to be activated if not already.
  void SetBranchToBeActivated(std::string treeName, std::string branchName);

  /// @{ Accessor.
  inline std::string InputFilePath() const             {return optionsString.at("inputfilepath");}
  inline std::string OutputFileName() const            {return optionsString.at("outputfilename");}
  inline std::string CalculateOpticalFunctionsFileName() const {return optionsString.at("opticslfilename");}
  inline bool   Debug() const                     {return optionsBool.at("debug");}
  inline bool   CalculateOpticalFunctions() const {return optionsBool.at("calculateoptics");}
  inline bool   ProcessSamplers() const           {return optionsBool.at("processsamplers");}
  inline bool   ProcessLosses() const             {return optionsBool.at("processlosses");}
  inline bool   ProcessAllTrees() const           {return optionsBool.at("processalltrees");}
  inline double PrintModuloFraction() const       {return optionsNumber.at("printmodulofraction");}
  /// @}
  
 protected:
  /// Private constructor for singleton pattern.
  Config() = delete;
  /// Desired constructor, also private for singleton pattern.
  Config(std::string fileNameIn,
	 std::string inputFilePathIn,
	 std::string outputFileNameIn);

  /// Set defaults in member maps for all options so that the keys can
  /// always be accessed.
  void InitialiseOptions(std::string analysisFile);

  /// Parse a line beginning with histogram. Uses other functions if appropriately defined.
  void ParseHistogramLine(const std::string& line);

  /// Parse everything after the histogram declaration and check all parameters.
  void ParseHistogram(const std::string line, const int nDim);

  /// Update the vector of required branches for a particular tree to be
  /// activated for analysis.
  void UpdateRequiredBranches(const HistogramDef* def);

  /// Update the vector of required branches for a particular tree to be
  /// activated for anlysis based on a single string defintion such as Primary.x.
  void UpdateRequiredBranches(const std::string treeName,
			      const std::string var);

  /// Check if the supplied tree name is one of the static member vector of
  /// allowed tree names.
  bool InvalidTreeName(const std::string& treeName) const;

  /// Parse the bin substring and check it has the right number of dimensions.
  /// Writes out via reference to pre-existing variables.
  void ParseBins(const std::string bins,
		 const int nDim,
		 int& xBins,
		 int& yBins,
		 int& zBins) const;

  /// Parse binning substring and check it has the right number of dimensions.
  /// Writes out via reference to pre-existing variables.
  void ParseBinning(const std::string binning,
		    const int nDim,
		    double& xLow, double& xHigh,
		    double& yLow, double& yHigh,
		    double& zLow, double& zHigh) const;

  /// Parse a settings line in input file and appropriate update member map.
  void ParseSetting(const std::string& line);

  /// Return a lower case copy of a string.
  std::string LowerCase(const std::string& st) const;

  static Config* instance;

  /// Vector of permitted tree names.
  static std::vector<std::string> treeNames;

  /// Index of which line in the file we're on while parsing - for feedback.
  int lineCounter = 0;

  /// Cache of which branches need to be activated for this analysis.
  RBDS::BranchMap branches;

  /// Whether all branches will be activatd - ie for optics.
  bool allBranchesActivated;

  ClassDef(Config,1);
};

#endif
