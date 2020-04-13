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
#include "Config.hh"
#include "HistogramDefSet.hh"
#include "HistogramDef1D.hh"
#include "HistogramDef2D.hh"
#include "HistogramDef3D.hh"
#include "RBDSException.hh"
#include "SpectraParticles.hh"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <regex>
#include <set>
#include <stdexcept>
#include <string>
#include <vector>

ClassImp(Config)

Config* Config::instance = nullptr;

std::vector<std::string> Config::treeNames = {"Beam.", "Options.", "Model.", "Run.", "Event."};

Config::Config(const std::string& inputFilePathIn,
	           const std::string& outputFileNameIn):
  allBranchesActivated(false)
{
  InitialiseOptions("");

  optionsString["inputfilepath"]  = inputFilePathIn;
  optionsString["outputfilename"] = outputFileNameIn;

  // turn on merging only
  branches["Event."].push_back("Histos");
  branches["Run."].push_back("Histos");
  optionsBool["perentryevent"] = true;
}

Config::Config(const std::string& fileNameIn,
	           const std::string& inputFilePathIn,
	           const std::string& outputFileNameIn):
  allBranchesActivated(false)
{
  InitialiseOptions(fileNameIn);  
  ParseInputFile();

  if (!inputFilePathIn.empty())
    {optionsString["inputfilepath"] = inputFilePathIn;}
  if (!outputFileNameIn.empty())
    {optionsString["outputfilename"] = outputFileNameIn;}
}

Config::~Config()
{
  instance = nullptr;

  for (auto& nameDefs : histoDefs)
    {
      for (auto& histoDef : nameDefs.second)
	    {delete histoDef;}
    }
  for (auto def : eventHistoDefSetsSimple)
    {delete def;}
  for (auto def : eventHistoDefSetsPerEntry)
    {delete def;}
}

void Config::InitialiseOptions(const std::string& analysisFile)
{
  optionsString["analysisfile"] = analysisFile;
  
  // for backwards compatibility / verbose names
  alternateKeys["calculateopticalfunctions"]         = "calculateoptics";
  alternateKeys["calculateopticalfunctionsfilename"] = "opticsfilename";
  
  optionsBool["debug"]             = false;
  optionsBool["processsamplers"]   = false;
  optionsBool["calculateoptics"]   = false;
  optionsBool["mergehistograms"]   = true;
  optionsBool["emittanceonthefly"] = false;
  optionsBool["perentrybeam"]      = false;
  optionsBool["perentryevent"]     = false;
  optionsBool["perentryrun"]       = false;
  optionsBool["perentryoption"]    = false;
  optionsBool["perentrymodel"]     = false;
  optionsBool["backwardscompatible"] = false; // ignore file types for old data

  optionsString["inputfilepath"]  = "./output.root";
  optionsString["outputfilename"] = "./output_ana.root";
  optionsString["opticsfilename"] = "./output_optics.dat";
  optionsString["gdmlfilename"]   = "./model.gdml";

  optionsNumber["printmodulofraction"] = 0.01;
  optionsNumber["eventstart"]          = 0;
  optionsNumber["eventend"]            = -1;

  // ensure keys exist for all trees.
  for (const auto& name : treeNames)
    {
      histoDefs[name]         = std::vector<HistogramDef*>();
      histoDefsSimple[name]   = std::vector<HistogramDef*>();
      histoDefsPerEntry[name] = std::vector<HistogramDef*>();
      branches[name]          = std::vector<std::string>();
    }
}

Config* Config::Instance(const std::string& fileName,
			             const std::string& inputFilePath,
			             const std::string& outputFileName)
{
  if(!instance && !fileName.empty())
    {instance = new Config(fileName, inputFilePath, outputFileName);}
  else if(instance && !fileName.empty())
    {
      std::cout << "Config::Instance> Instance present, delete and construct" << std::endl;
      delete instance;
      instance = new Config(fileName, inputFilePath, outputFileName);
    }
  else if (!instance && fileName.empty())
    {instance = new Config(inputFilePath, outputFileName);}
  // else return current instance (can be nullptr!)
  return instance;
}

void Config::ParseInputFile()
{ 
  std::ifstream f(optionsString.at("analysisfile").c_str());

  if(!f)
    {throw RBDSException("Config::ParseInputFile>", "could not open file");}

  lineCounter = 0;
  std::string line;

  // unique patterns to match
  // match a line starting with #
  std::regex comment("^\\#.*");
  // match a line starting with 'histogram', ignoring case
  std::regex histogram("(?:simple)*histogram.*", std::regex_constants::icase);
  // match a line starting with 'spectra', ignoring case - quite exact to avoid mismatching 'spectra' in file name in options
  std::regex spectra("(?:simple)*spectra(?:TE|rigidity)*(?:log)*(?:\\s+)", std::regex_constants::icase);
  // match particleset ignoring case
  std::regex particleSet("particleset", std::regex_constants::icase);

  while (std::getline(f, line))
    {
      lineCounter++;
      if (std::all_of(line.begin(), line.end(), isspace))
	{continue;} // skip empty lines
      else if (std::regex_search(line, comment))
	{continue;} // skip lines starting with '#'
      else if (std::regex_search(line, histogram))
	{ParseHistogramLine(line);} // any histogram - must be before settings
      else if (std::regex_search(line, spectra))
	{ParseSpectraLine(line);}
	  else if (std::regex_search(line, particleSet))
        {ParseParticleSetLine(line);}
      else
	{ParseSetting(line);} // any setting
    }
  
  f.close();

  // set flags etc based on what options have been set
  if (optionsBool.at("calculateoptics"))
    {
      allBranchesActivated = true;
      optionsBool["processsamplers"] = true;
      optionsBool["perentryevent"]   = true;
    }
  if (optionsBool.at("mergehistograms"))
    {
      branches["Event."].push_back("Histos");
      branches["Run."].push_back("Histos");
      optionsBool["perentryevent"]   = true;
    }

  // checks on event numbers
  double eS = optionsNumber.at("eventstart");
  double eE = optionsNumber.at("eventend");
  if (eE < 0) // default -1
    {eE = std::numeric_limits<double>::max();}
  if (eS < 0 || eS > eE)
    {
      std::cerr << "Invalid starting event number " << eS << std::endl;
      exit(1);
    }
}

void Config::ParseHistogramLine(const std::string& line)
{
  // Settings with histogram in name can be misidentified - check here.
  // This is the easiest way to do it for now.
  std::string copyLine = line;
  std::transform(copyLine.begin(), copyLine.end(), copyLine.begin(), ::tolower); // convert to lower case
  if (copyLine.find("mergehistograms") != std::string::npos)
    {
      ParseSetting(line);
      return;
    }
  
  // we know the line starts with 'histogram'
  // extract number after it as 1st match and rest of line as 2nd match
  std::regex histNDim("(?:Simple)*Histogram([1-3])D[a-zA-Z]*\\s+(.*)", std::regex_constants::icase);
  //std::regex histNDim("^Histogram([1-3])D[a-zA-Z]*\\s+(.*)", std::regex_constants::icase);
  std::smatch match;
  
  if (std::regex_search(line, match, histNDim))
    {
      int nDim = std::stoi(match[1]);
      ParseHistogram(line, nDim);
    }
  else
    {
      std::string errString = "Invalid histogram type on line #" + std::to_string(lineCounter)
	+ ": \n\"" + line + "\"\n";
      throw RBDSException(errString);
    }
}

void Config::ParseSpectraLine(const std::string& line)
{
  // split line on white space
  std::vector<std::string> results = SplitOnWhiteSpace(line);
  
  if (results.size() < 6)
    {// ensure enough columns
      std::string errString = "Invalid line #" + std::to_string(lineCounter)
	+ " - invalid number of columns (too few)";
      throw RBDSException(errString);
    }
  if (results.size() > 6)
    {// ensure not too many columns
      std::string errString = "Invalid line #" + std::to_string(lineCounter)
      + " - too many columns - check no extra whitespace";
      throw RBDSException(errString);
    }

  bool log  = false;
  bool logy = false; // duff values to fulfill function
  bool logz = false;
  ParseLog(results[0], log, logy, logz);

  bool perEntry = true;
  ParsePerEntry(results[0], perEntry);

  std::string variable = ".kineticEnergy"; // kinetic energy by default
  if (ContainsWordCI(results[0], "TE"))
    {variable = ".energy";}
  else if (ContainsWordCI(results[0], "Rigidity"))
    {variable = ".rigidity";}

  std::string samplerName = results[1];
  // because we can have multiple spectra on a branch and there are no user-specified names for this
  int nSpectraThisBranch = 0;
  auto search = spectraNames.find(samplerName);
  if (search != spectraNames.end())
    {// branch name already exists
      nSpectraThisBranch = search->second;
      search->second++;
    }
  else
    {spectraNames[samplerName] = 1;}
  std::string histogramName = samplerName + "_" + std::to_string(nSpectraThisBranch);
  std::string selection = results[5];
  
  Config::Binning b = ParseBinsAndBinning(results[2], results[3], 1);

  std::set<ParticleSpec> particles;
  try
    {particles = ParseParticles(results[4]);}
  catch (const std::exception& e)
    {
      std::string err = e.what();
      err += "\nError in spectra particle definition on line " + std::to_string(lineCounter) + "\n";
      throw RBDSException(err);
    }

  // simple spectra using 'top' or 'ions' or 'particles' won't dynamically build up the pdg ids
  // per event so we should warn the user about this as it'll create no histograms
  if (particles.empty() && !perEntry)
    {throw RBDSException("Simple spectra used but no specific particles named - only works for specific particles");}

  HistogramDef1D* def = new HistogramDef1D("Event.", histogramName,
					   b.nBinsX, b.xLow, b.xHigh,
					   samplerName + variable,
					   selection, perEntry, log);

  HistogramDefSet* result = new HistogramDefSet(samplerName,
						def,
						particles,
						results[4]);
  delete def; // no longer needed
  
  if (perEntry)
    {eventHistoDefSetsPerEntry.push_back(result);}
  else
    {eventHistoDefSetsSimple.push_back(result);}

  SetBranchToBeActivated("Event.", samplerName);
}

void Config::ParseParticleSetLine(const std::string& line)
{
  std::vector<std::string> results = SplitOnWhiteSpace(line);
  if (results.size() < 2)
    {;}
}

void Config::ParseHistogram(const std::string& line, const int nDim)
{
  std::vector<std::string> results = SplitOnWhiteSpace(line);
  if (results.size() < 7)
    {// ensure enough columns
      std::string errString = "Invalid line #" + std::to_string(lineCounter)
	+ " - invalid number of columns (too few)";
      throw RBDSException(errString);
    }
  if (results.size() > 7)
    {// ensure not too many columns
      std::string errString = "Invalid line #" + std::to_string(lineCounter)
      + " - too many columns - check no extra whitespace";
      throw RBDSException(errString);
    }

  bool xLog = false;
  bool yLog = false;
  bool zLog = false;
  ParseLog(results[0], xLog, yLog, zLog);

  bool perEntry = true;
  ParsePerEntry(results[0], perEntry);
  
  std::string treeName = results[1];
  CheckValidTreeName(treeName);

  if (perEntry)
    {
      std::string treeNameWithoutPoint = treeName; // make copy to modify
      treeNameWithoutPoint.pop_back();             // delete last character
      treeNameWithoutPoint = LowerCase(treeNameWithoutPoint);
      optionsBool["perentry"+treeNameWithoutPoint] = true;
    }
  
  std::string histName  = results[2];
  std::string bins      = results[3];
  std::string binning   = results[4];
  std::string variable  = results[5];
  std::string selection = results[6];
  
  Config::Binning b;
  ParseBinsAndBinning(bins, binning, nDim);
  
  // make a check that the number of variables supplied in ttree with y:x syntax doesn't
  // exceed the number of declared dimensions - this would result in a segfault from ROOT
  // a single (not 2) colon with at least one character on either side
  std::regex singleColon("\\w+(:{1})\\w+");
  // count the number of matches by distance of match iterator from beginning
  int nColons = static_cast<int>(std::distance(std::sregex_iterator(variable.begin(),
								    variable.end(),
								    singleColon),
					       std::sregex_iterator()));
  if (nColons > nDim - 1)
    {
      std::string err = "Error: Histogram \"" + histName + "\" variable includes too many single\n";
	  err += "colons specifying more dimensions than the number of specified dimensions.\n";
      err += "Declared dimensions: " + std::to_string(nDim) + "\n";
      err += "Number of dimensions in variables " + std::to_string(nColons + 1);
      throw RBDSException(err);
    }

  HistogramDef1D* result = nullptr;
  switch (nDim)
    {
    case 1:
      {
	result = new HistogramDef1D(treeName, histName,
				    b.nBinsX, b.xLow, b.xHigh,
				    variable, selection,
				    perEntry, xLog);
	break;
      }
    case 2:
      {
	result = new HistogramDef2D(treeName, histName,
				    b.nBinsX, b.nBinsY,
				    b.xLow,   b.xHigh,
				    b.yLow,   b.yHigh,
				    variable, selection,
				    perEntry, xLog, yLog);
	break;
      }
    case 3:
      {
	result = new HistogramDef3D(treeName, histName,
				    b.nBinsX, b.nBinsY, b.nBinsZ,
				    b.xLow, b.xHigh,
				    b.yLow, b.yHigh,
				    b.zLow, b.zHigh,
				    variable, selection, perEntry,
				    xLog, yLog, zLog);
	break;
      }
    default:
      {break;}
    }

  if (result)
    {
      histoDefs[treeName].push_back(result);
      if (perEntry)
	    {histoDefsPerEntry[treeName].push_back(result);}
      else
	    {histoDefsSimple[treeName].push_back(result);}
      UpdateRequiredBranches(result);
    }
}

void Config::ParsePerEntry(const std::string& name, bool& perEntry) const
{
  std::string res = name;
  std::transform(res.begin(), res.end(), res.begin(), ::tolower); // convert to lower case
  perEntry = res.find("simple") == std::string::npos;
}

bool Config::ContainsWordCI(const std::string& input,
			    const std::string& word) const
{
  std::string il = input;
  std::string wl = word;
  std::transform(il.begin(), il.end(), il.begin(), ::tolower); // convert to lower case
  std::transform(wl.begin(), wl.end(), wl.begin(), ::tolower);
  return il.find(wl) != std::string::npos;
}

void Config::ParseLog(const std::string& definition,
		      bool& xLog,
		      bool& yLog,
		      bool& zLog) const
{
  // capture any lin log definitions after Histogram[1-3]D
  std::regex linLogWords("(Lin)|(Log)", std::regex_constants::icase);
  std::sregex_token_iterator iter(definition.begin(), definition.end(), linLogWords);
  std::sregex_token_iterator end;
  std::vector<bool*> results = {&xLog, &yLog, &zLog};
  int index = 0;
  for (; iter != end; ++iter, ++index)
    {
      std::string res = (*iter).str();
      std::transform(res.begin(), res.end(), res.begin(), ::tolower); // convert to lower case
      *(results[index]) = res == "log";
    }
}

void Config::UpdateRequiredBranches(const HistogramDef* def)
{
  UpdateRequiredBranches(def->treeName, def->variable);
  UpdateRequiredBranches(def->treeName, def->selection);
}

void Config::UpdateRequiredBranches(const std::string& treeName,
				                    const std::string& var)
{
  // This won't work properly for the options Tree that has "::" in the class
  // as well as double splitting. C++ regex does not support lookahead / behind
  // which makes it nigh on impossible to correctly identify the single : with
  // regex. For now, only the Options tree has this and we turn it all on, so it
  // it shouldn't be a problem (it only ever has one entry).
  // match word; '.'; word -> here we match the token rather than the bits in-between
  std::regex branchLeaf("(\\w+)\\.(\\w+)");
  auto words_begin = std::sregex_iterator(var.begin(), var.end(), branchLeaf);
  auto words_end   = std::sregex_iterator();
  for (std::sregex_iterator i = words_begin; i != words_end; ++i)
    {
      std::string targetBranch = (*i)[1];
      SetBranchToBeActivated(treeName, targetBranch);
    }
}

void Config::SetBranchToBeActivated(const std::string& treeName,
				                    const std::string& branchName)
{
  auto& v = branches.at(treeName);
  if (std::find(v.begin(), v.end(), branchName) == v.end())
    {v.push_back(branchName);}
}

void Config::CheckValidTreeName(std::string& treeName) const
{
  // check it has a point at the end (simple mistake)
  if (strcmp(&treeName.back(), ".") != 0)
    {treeName += ".";}
  
  if (InvalidTreeName(treeName))
    {
      std::string err = "Invalid tree name \"" + treeName + "\"\n";
      err += "Tree names are one of: ";
      for (const auto& n : treeNames)
	    {err += "\"" + n + "\" ";}
      throw RBDSException(err);
    }
}

bool Config::InvalidTreeName(const std::string& treeName) const
{
  return std::find(treeNames.begin(), treeNames.end(), treeName) == treeNames.end();
}

void Config::ParseBins(const std::string& bins,
		       int  nDim,
		       int& xBins,
		       int& yBins,
		       int& zBins) const
{
  // match a number including +ve exponentials (should be +ve int)
  std::regex number("([0-9e\\+]+)+", std::regex_constants::icase);
  int* binValues[3] = {&xBins, &yBins, &zBins};
  auto words_begin = std::sregex_iterator(bins.begin(), bins.end(), number);
  auto words_end   = std::sregex_iterator();
  int counter = 0;
  for (std::sregex_iterator i = words_begin; i != words_end; ++i, ++counter)
    {(*binValues[counter]) = std::stoi((*i).str());}
  if (counter < nDim-1)
    {throw RBDSException("Invalid bin specification on line #" + std::to_string(lineCounter));}
}

Config::Binning Config::ParseBinsAndBinning(const std::string& bins,
					    const std::string& binning,
					    int                nDim) const
{
  Binning b;
  ParseBinning(binning, nDim, b.xLow, b.xHigh, b.yLow, b.yHigh, b.zLow, b.zHigh);
  ParseBins(bins, nDim, b.nBinsX, b.nBinsY, b.nBinsZ);
  return b;
}

void Config::ParseBinning(const std::string& binning,
			  int nDim,
			  double& xLow, double& xHigh,
			  double& yLow, double& yHigh,
			  double& zLow, double& zHigh) const
{
  // simple match - let stod throw exception if wrong
  std::regex oneDim("([0-9eE.+-]+):([0-9eE.+-]+)");
  auto words_begin = std::sregex_iterator(binning.begin(), binning.end(), oneDim);
  auto words_end   = std::sregex_iterator();

  std::vector<double*> vals = {&xLow, &xHigh, &yLow, &yHigh, &zLow, &zHigh};
  int counter = 0;
  for (std::sregex_iterator i = words_begin; i != words_end; ++i, ++counter)
    {// iterate over all matches and pull out first and second number
      std::smatch match = *i;
      try
	{
	  (*vals[2*counter])     = std::stod(match[1]);
	  (*vals[(2*counter)+1]) = std::stod(match[2]);
	}
      catch (std::invalid_argument&) // if stod can't convert number to double
	{throw RBDSException("Invalid binning number: \"" + match[0].str() + "\" on line #" + std::to_string(lineCounter));}
    }
  
  if (counter == 0)
    {throw RBDSException("Invalid binning specification: \"" + binning + "\" on line #" + std::to_string(lineCounter));}
  else if (counter < nDim)
    {
      std::string errString = "Insufficient number of binning dimensions on line #"
	+ std::to_string(lineCounter) + "\n"
	+ std::to_string(nDim) + " dimension histogram, but the following was specified:\n"
	+ binning + "\n";
      throw RBDSException(errString);
    }
}

std::vector<std::string> Config::SplitOnWhiteSpace(const std::string& line) const
{
  std::vector<std::string> results;
  std::regex wspace("\\s+"); // any whitespace
  // -1 here makes it point to the suffix, ie the word rather than the wspace
  std::sregex_token_iterator iter(line.begin(), line.end(), wspace, -1);
  std::sregex_token_iterator end;
  for (; iter != end; ++iter)
    {
      std::string res = (*iter).str();
      results.push_back(res);
    }
  return results;
}

std::set<ParticleSpec> Config::ParseParticles(const std::string& word) const
{
  std::string wordLower = LowerCase(word);
  std::vector<std::string> specialKeys = {"top", "particles", "all", "ions"};
  for (const auto& key : specialKeys)
    {
      if (wordLower.find(key) != std::string::npos)
        {return std::set<ParticleSpec>();}
    }

  // some numbers in brackets -> try to split up
  // detect brackets and strip off
  std::regex inBrackets("^\\{(.+)\\}$");
  std::smatch match;
  auto firstSearch = std::regex_search(word, match, inBrackets);
  if (!firstSearch)
    {throw RBDSException("Invalid particle definition - no braces");}

  // split up numbers inside brackets on commas
  std::string numbers = match[1];
  std::set<ParticleSpec> result;  
  std::regex commas(",");
  // -1 here makes it point to the suffix, ie the word rather than the wspace
  std::sregex_token_iterator iter(numbers.begin(), numbers.end(), commas, -1);
  std::sregex_token_iterator end;
  for (; iter != end; ++iter)
    {
      std::string res = (*iter).str();
      std::regex selection("^([a-zA-Z]){1}(\\d+)");
      std::smatch matchSelection;
      if (std::regex_search(res, matchSelection, selection))
	{
	  auto keySearch = RBDS::spectraParticlesKeys.find(matchSelection[1]);
      RBDS::SpectraParticles which;
	  if (keySearch != RBDS::spectraParticlesKeys.end())
	    {which = keySearch->second;}
	  else
	    {throw RBDSException("Invalid particle specifier \"" + matchSelection[1].str() + "\"");}
	  try
	    {
	      long long int id = std::stoll(matchSelection[2].str());
	      result.insert(ParticleSpec(id, which));
	    }
	  catch (const std::exception& e)
	    {throw RBDSException(e.what());}
	}
      else
	{
	  try
	    {
	      long long int id = std::stoll(res);
	      result.insert(ParticleSpec(id, RBDS::SpectraParticles::all));
	    }
	  catch (const std::exception& e)
	    {throw RBDSException(e.what());}
	}
    }
  return result;
}

std::string Config::LowerCase(const std::string& st) const
{
  std::string res = st;
  std::transform(res.begin(), res.end(), res.begin(), ::tolower);
  return res;
}
  
void Config::ParseSetting(const std::string& line)
{
  // match a word; at least one space; and a word including '.' and _ and /
  std::regex setting("(\\w+)\\s+([\\.\\/_\\w\\*]+)", std::regex_constants::icase);
  std::smatch match;
  if (std::regex_search(line, match, setting))
    {
      std::string key   = LowerCase(match[1]);
      std::string value = match[2];

      if (alternateKeys.find(key) != alternateKeys.end())
	{key = alternateKeys[key];} // reassign value to correct key
      
      if (optionsBool.find(key) != optionsBool.end())
	{
	  // match optional space; true or false; optional space - ignore case
	  std::regex tfRegex("\\s*(true|false)\\s*", std::regex_constants::icase);
	  std::smatch tfMatch;
	  if (std::regex_search(value, tfMatch, tfRegex))
	    {
	      std::string flag = tfMatch[1];
	      std::smatch tMatch;
	      std::regex tRegex("true", std::regex_constants::icase);
	      bool result = std::regex_search(flag, tMatch, tRegex);
	      optionsBool[key] = result;
	    }
	  else
	    {optionsBool[key] = (bool)std::stoi(value);}
	}
      else if (optionsString.find(key) != optionsString.end())
	{optionsString[key] = value;}
      else if (optionsNumber.find(key) != optionsNumber.end())
	{optionsNumber[key] = std::stod(value);}
      else
      {throw RBDSException("Invalid option \"" + key + "\"");}
    }
  else
    {throw RBDSException("Invalid line #" + std::to_string(lineCounter) + "\n" + line);}
}
