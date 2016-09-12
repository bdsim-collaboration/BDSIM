#ifndef CONFIG_H
#define CONFIG_H

#include "TROOT.h"

#include <string>
#include <vector>
#include <map>

/**
 * @brief Configuration and configuartion parser class.
 * 
 * @author Stewart Boogert
 */

class Config
{
 public:
  virtual ~Config();

  /// Singleton accessor
  static Config* Instance(std::string fileName = "",
			  std::string inputFilePath = "",
			  std::string outputFileName = "");

  void ParseInputFile();
    
  std::string InputFilePath()                     {return inputFilePath;}
  std::string OutputFileName()                    {return outputFileName;}
  std::string CalculateOpticalFunctionsFileName() {return calculateOpticalFunctionsFileName;}
  bool        Debug()                             {return bDebug;}
  bool        CalculateOpticalFunctions()         {return bCalculateOpticalFunctions;}
  bool        ProcessSamplers()                   {return bProcessSamplers;}
  bool        ProcessLosses()                     {return bProcessLosses;}
  bool        ProcessAllTrees()                   {return bProcessAllTrees;}
  double      PrintModuloFraction()               {return printModuloFraction;}
  
  std::vector<std::map<std::string, std::string>> GetHistoDefs() { return histos;}
  std::map<std::string, std::string>              GetHistoDef(int i) { return histos[i];}

  static int  Dimension(std::string nbins);
  static int  NBins(std::string nbins, int iAxis);
  static void Binning(std::string binning, int iAxis, double &low, double &high);
  
 protected:
  /// Private constructor for singleton pattern.
  Config();
  /// Desired constructor, also private for singleton pattern.
  Config(std::string fileNameIn,
	 std::string inputFilePathIn,
	 std::string outputFileNameIn);

  std::string     fileName                      = "Config.txt";
  static Config* instance;

  std::string inputFilePath                     = "";
  std::string outputFileName                    = "";
  std::string calculateOpticalFunctionsFileName = "";
  std::string gdmlFileName                      = "";

  bool bDebug                                   = false;
  bool bCalculateOpticalFunctions               = false;
  bool bProcessAllTrees                         = false;
  bool bEventDisplay                            = false;
  bool bProcessLosses                           = false;
  bool bProcessSamplers                         = false;
  double  printModuloFraction                   = 0.01; // 10% print out rate

  std::vector<std::map<std::string, std::string>>  histos;

  ClassDef(Config,1);
};

#endif
