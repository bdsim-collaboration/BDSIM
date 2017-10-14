#include "Config.hh"
#include "HistogramDef1D.hh"
#include "HistogramDef2D.hh"
#include "HistogramDef3D.hh"

#include <algorithm>
#include <fstream>
#include <iostream>
#include <map>
#include <regex>
#include <sstream>
#include <stdexcept>
#include <vector>

ClassImp(Config)

Config* Config::instance = nullptr;

std::vector<std::string> Config::treeNames = {"Beam.", "Options.", "Model.", "Run.", "Event."};

Config::Config(std::string fileNameIn,
	       std::string inputFilePathIn,
	       std::string outputFileNameIn):
  allBranchesActivated(false)
{
  InitialiseOptions(fileNameIn);  
  ParseInputFile();

  if (inputFilePathIn != "")
    {optionsString["inputfilepath"] = inputFilePathIn;}
  if (outputFileNameIn != "")
    {optionsString["outputfilename"] = outputFileNameIn;}
}

Config::~Config()
{
  instance = nullptr;
}

void Config::InitialiseOptions(std::string analysisFile)
{
  optionsString["analysisfile"] = analysisFile;
  
  // for backwards compatability / verbose names
  alternateKeys["calculateopticalfunctions"]         = "calculateoptics";
  alternateKeys["calculateopticalfunctionsfilename"] = "opticsfilename";
  
  optionsBool["debug"]           = false;
  optionsBool["processsamplers"] = false;
  optionsBool["processlosses"]   = false;
  optionsBool["processalltrees"] = false;
  optionsBool["calculateoptics"] = false;
  optionsBool["mergehistograms"] = true;
  optionsBool["emittanceonthefly"] = false;

  optionsString["inputfilepath"]  = "./output.root";
  optionsString["outputfilename"] = "./output_ana.root";
  optionsString["opticsfilename"] = "./output_optics.dat";
  optionsString["gdmlfilename"]   = "./model.gdml";

  optionsNumber["printmodulofraction"] = 0.01;

  // ensure keys exist for all trees.
  for (auto name : treeNames)
    {
      histoDefs[name] = std::vector<HistogramDef*>();
      branches[name]  = std::vector<std::string>();
    }
}

Config* Config::Instance(std::string fileName,
			 std::string inputFilePath,
			 std::string outputFileName)
{
  if(!instance && fileName != "")
  {
    std::cout << "Config::Instance> No instance present, construct" << std::endl;
    instance = new Config(fileName, inputFilePath, outputFileName);
    //instance->ParseInputFile();
  }
  else if(instance && fileName != "")
  {
    std::cout << "Config::Instance> Instance present, delete and construct" << std::endl;
    delete instance;
    instance = new Config(fileName, inputFilePath, outputFileName);
    //instance->ParseInputFile();
  }
  // else return current instance (can be nullptr!)
  return instance;
}

void Config::ParseInputFile()
{ 
  std::ifstream f(optionsString.at("analysisfile").c_str());

  if(!f)
    {throw std::string("Config::ParseInputFile> could not open file");}

  lineCounter = 0;
  std::string line;

  // unique patterns to match
  // match a line starting with #
  std::regex comment("^\\#.*");
  // match a line starting with 'histogram', ignoring case
  std::regex histogram("^histogram.*", std::regex_constants::icase);

  while(std::getline(f, line))
    {
      lineCounter++;
      if (std::all_of(line.begin(), line.end(), isspace))
	{continue;} // skip empty lines
      else if (std::regex_search(line, comment))
	{continue;} // skip lines starting with '#'
      else if (std::regex_search(line, histogram))
	{ParseHistogramLine(line);} // any histogram - must be before settings
      else
	{ParseSetting(line);} // any setting
    }
  
  f.close();

  // set flags etc based on what options have been set
  if (optionsBool.at("calculateoptics"))
    {
      allBranchesActivated = true;
      optionsBool["processsamplers"] = true;
    }
  if (optionsBool.at("mergehistograms"))
    {
      branches["Event."].push_back("Histos.");
      branches["Run."].push_back("Histos.");
    }
}

void Config::ParseHistogramLine(const std::string& line)
{
  // we know the line starts with 'histogram'
  // extract number after it as 1st match and rest of line as 2nd match
  std::regex histNDim("^Histogram([1-3])D\\s+(.*)", std::regex_constants::icase);
  std::smatch match;
  
  if (std::regex_search(line, match, histNDim))
    {ParseHistogram(match[2], std::stoi(match[1]));}
  else
    {
      std::string errString = "Invalid histogram type on line #" + std::to_string(lineCounter)
	+ ": \n\"" + line + "\"\n";
      throw std::string(errString);
    }
}

void Config::ParseHistogram(const std::string line, const int nDim)
{
  // split line on white space
  // doesn't inspect words themselves
  // checks number of words, ie number of columns is correct
  std::vector<std::string> results;
  std::regex wspace("\\s+"); // any whitepsace
  // -1 here makes it point to the suffix, ie the word rather than the wspace
  std::sregex_token_iterator iter(line.begin(), line.end(), wspace, -1);
  std::sregex_token_iterator end;
  for (; iter != end; ++iter)
    {
      std::string res = (*iter).str();
      results.push_back(res);
    }
  
  if (results.size() < 6)
    {// ensure enough columns
      std::string errString = "Invalid line #" + std::to_string(lineCounter)
	+ " - invalid number of columns";
      throw std::string(errString);
    }
  
  std::string treeName  = results[0];
  if (InvalidTreeName(treeName))
    {throw std::string("Invalid tree name \"" + treeName + "\"");}
  
  std::string histName  = results[1];
  std::string bins      = results[2];
  std::string binning   = results[3];
  std::string variable  = results[4];
  std::string selection = results[5];

  int nBinsX = 1;
  int nBinsY = 1;
  int nBinsZ = 1;
  double xLow  = 0;
  double xHigh = 0;
  double yLow  = 0;
  double yHigh = 0;
  double zLow  = 0;
  double zHigh = 0;

  ParseBins(bins, nDim, nBinsX, nBinsY, nBinsZ);
  ParseBinning(binning, nDim, xLow, xHigh, yLow, yHigh, zLow, zHigh);

  HistogramDef1D* result = nullptr;
  switch (nDim)
    {
    case 1:
      {
	result = new HistogramDef1D(treeName, histName,
				    nBinsX, xLow, xHigh,
				    variable, selection);
	break;
      }
    case 2:
      {
	result = new HistogramDef2D(treeName, histName,
				    nBinsX, nBinsY,
				    xLow, xHigh,
				    yLow, yHigh,
				    variable, selection);
	break;
      }
    case 3:
      {
	result = new HistogramDef3D(treeName, histName,
				    nBinsX, nBinsY, nBinsZ,
				    xLow, xHigh,
				    yLow, yHigh,
				    zLow, zHigh,
				    variable, selection);
	break;
      }
    default:
      {break;}
    }

  if (result)
    {
      histoDefs[treeName].push_back(result);
      UpdateRequiredBranches(result);
    }
}

void Config::UpdateRequiredBranches(const HistogramDef* def)
{
  UpdateRequiredBranches(def->treeName, def->variable);
  UpdateRequiredBranches(def->treeName, def->selection);
}

void Config::UpdateRequiredBranches(const std::string treeName,
				    const std::string var)
{
  // This won't work properly for the options Tree that has "::" in the class
  // as well as double splitting. C++ regex does not support lookahead / behind
  // which makes it nigh on impossible to correctly identify the single : with
  // regex. For now, only the Options tree has this and we turn it all on, so it
  // it shouldn't be a problem (it only ever has one entry).
  // match word; '.'; word -> here we match the token rather than the bits inbetween
  std::regex branchLeaf("(\\w+)\\.(\\w+)");
  auto words_begin = std::sregex_iterator(var.begin(), var.end(), branchLeaf);
  auto words_end   = std::sregex_iterator();
  for (std::sregex_iterator i = words_begin; i != words_end; ++i)
    {
      std::string targetBranch = (*i)[1];
      SetBranchToBeActivated(treeName, targetBranch);
    }
}

void Config::SetBranchToBeActivated(const std::string treeName,
				    const std::string branchName)
{
  auto& v = branches.at(treeName);
  if (std::find(v.begin(), v.end(), branchName) == v.end())
    {v.push_back(branchName);}
}

bool Config::InvalidTreeName(const std::string& treeName) const
{
  return std::find(treeNames.begin(), treeNames.end(), treeName) == treeNames.end();
}


void Config::ParseBins(const std::string bins,
		       const int nDim,
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
    {(*binValues[counter]) = std::stod((*i).str());}
  if (counter < nDim-1)
    {throw std::string("Invalid bin specification on line #" + std::to_string(lineCounter));}
}

void Config::ParseBinning(const std::string binning,
			  const int nDim,
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
      catch (std::invalid_argument) // if stod can't convert number to double
	{throw std::string("Invalid binning number: \"" + match[0].str() + "\" on line #" + std::to_string(lineCounter));}
    }
  
  if (counter == 0)
    {throw std::string("Invalid binning specification: \"" + binning + "\" on line #" + std::to_string(lineCounter));}
  else if (counter < nDim)
    {
      std::string errString = "Insufficient number of binning dimensions on line #"
	+ std::to_string(lineCounter) + "\n"
	+ std::to_string(nDim) + " dimension histogram, but the following was specified:\n"
	+ binning + "\n";
      throw std::string(errString);
    }
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
      {throw std::string("Invalid option \"" + key + "\"");}
    }
  else
    {throw std::string("Invalid line #" + std::to_string(lineCounter) + "\n" + line);}
}
