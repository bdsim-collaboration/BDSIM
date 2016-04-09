#ifndef __CONFIG_H
#define __CONFIG_H

#include "TROOT.h"

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

class Config {
 public:
  virtual ~Config();
  static Config* Instance(std::string fileName = std::string(""));
  void ParseInputFile();
    
  std::string InputFilePath()                     {return inputFilePath;}
  std::string OutputFileName()                    {return outputFileName;}
  std::string CalculateOpticalFunctionsFileName() {return calculateOpticalFunctionsFileName;}
  bool        Debug()                             {return bDebug;}
  bool        CalculateOpticalFunctions()         {return bCalculateOpticalFunctions;}
  bool        ProcessAllTrees()                   {return bProcessAllTrees;}
  
  std::vector<std::map<std::string, std::string>> GetHistoDefs() { return histos;}
  std::map<std::string, std::string>              GetHistoDef(int i) { return histos[i];}

  static int  Dimension(std::string nbins);
  static int  NBins(std::string nbins, int iAxis);
  static void Binning(std::string binning, int iAxis, double &low, double &high);
  
 protected: 
  Config(std::string fileName);
  std::string     fileName                      = "Config.txt";
  static Config* instance;

  std::string inputFilePath                     = "";
  std::string outputFileName                    = "";
  std::string calculateOpticalFunctionsFileName = "";

  bool bDebug                                   = false;
  bool bCalculateOpticalFunctions               = false;
  bool bProcessAllTrees                         = false;
  bool bEventDisplay                            = false;
  bool bProcessLosses                           = false;
  bool bProcessSamplers                         = false;

  std::vector<std::map<std::string, std::string>>  histos;

  ClassDef(Config,1);
};

#endif
