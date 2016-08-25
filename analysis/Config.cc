#include "Config.hh"

#include <algorithm>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

ClassImp(Config)

Config* Config::instance = nullptr;

Config::Config(std::string fileNameIn,
	       std::string inputFilePathIn,
	       std::string outputFileNameIn)
{
  fileName = fileNameIn;
  ParseInputFile();

  if (inputFilePathIn != "")
    {inputFilePath = inputFilePathIn;}
  if (outputFileNameIn != "")
    {outputFileName = outputFileNameIn;}
}

Config::~Config()
{
  instance = nullptr;
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
  std::ifstream f(this->fileName.c_str());

  if(!f) {
    throw std::string("Config::ParseInputFile> could not open file");
  }
  
  std::string firstTok;
  std::string secondTok;
  std::string line;

  while(f >> firstTok) {    
    if(firstTok == "#" || firstTok[0] == '#') {
      std::getline(f,line);       
    }
    else if(firstTok == "Debug") {
      f >> secondTok;
      if(atoi(secondTok.c_str()) == 0) { 
	bDebug = 0;
      }
      else if(atoi(secondTok.c_str()) == 1) {
	bDebug = 1;
      };
    }
    else if(firstTok == "InputFilePath") {
      f >> secondTok;
      inputFilePath = secondTok;
    }
    else if(firstTok == "OutputFileName") { 
      f >> secondTok;
      outputFileName = secondTok;
    }
    else if(firstTok == "ProcessSamplers") {
      f >> secondTok;
      if(atoi(secondTok.c_str()) == 0) {
	bProcessSamplers = 0;
      }
    }
    else if(firstTok == "CalculateOpticalFunctionsFileName") { 
      f >> secondTok;
      calculateOpticalFunctionsFileName = secondTok;
    }
    else if(firstTok == "CalculateOpticalFunctions")  {
      f >> secondTok;
      if(atoi(secondTok.c_str()) == 0) {
        bCalculateOpticalFunctions = 0;
      }
      else if(atoi(secondTok.c_str()) == 1) {
        bCalculateOpticalFunctions = 1;
	bProcessSamplers = 1;
      }
    }
    else if(firstTok == "GdmlFileName") {
      f >> secondTok;
      gdmlFileName = secondTok;
    }
    else if(firstTok == "Histogram") { 
      std::string treeName; 
      std::string histName;
      std::string nbins;
      std::string binning;
      std::string plot;
      std::string select;
      
      f >> treeName >> histName >> nbins >> binning >> plot >> select;
      std::map<std::string,std::string> histoDef;
      histoDef["treeName"] = treeName;
      histoDef["histName"] = histName;
      histoDef["nbins"]    = nbins;
      histoDef["binning"]  = binning;
      histoDef["plot"]     = plot;
      histoDef["select"]   = select;
      histos.push_back(histoDef);
    }
    else {
      std::getline(f,line); 
    }
  }
  
  f.close();
}

int Config::Dimension(std::string nbins)
{
  int iCommaCount = std::count(nbins.begin(), nbins.end(),',');
  int iDim = iCommaCount + 1;
  return iDim;
}

int Config::NBins(std::string nbins, int iAxis)
{
  int ibins = 0;

  // Remove curly braces
  char chars[] = "{}";
  for(unsigned int i=0;i<strlen(chars); ++i) {
    nbins.erase(std::remove(nbins.begin(), nbins.end(), chars[i]), nbins.end());
  }  
  
  // Replace ',' with ' ' `
  std::replace(nbins.begin(), nbins.end(),',',' '); 

  std::istringstream snbins(nbins);  

  int icount = 1;
  do { 
    snbins >> ibins;
    if(icount == iAxis) 
      return ibins;
    else {
      icount++;
    }
  } while(snbins); 

  return ibins;
}

void Config::Binning(std::string binning, int iAxis, double &low, double &high)
{
  low = 0;
  high = 0;

  // Remove curly braces
  char chars[] = "{}";
  for(unsigned int i=0;i<strlen(chars); ++i) {
    binning.erase(std::remove(binning.begin(), binning.end(), chars[i]),binning.end());
  }

  // Replace ',' with ' ' `
  std::replace(binning.begin(),binning.end(),',',' '); 

  std::istringstream sbinning(binning);  
  int icount = 1;
  std::string abinning;
  do {
    sbinning >> abinning; 
    if(icount == iAxis) {
      std::replace(abinning.begin(),abinning.end(),':',' ');
      std::istringstream sabinning(abinning); 
      sabinning >> low >> high;
      return;
    }
    else {
      icount++;
    }
  } while(sbinning);
  return;
} 
