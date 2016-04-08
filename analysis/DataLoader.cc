#include "DataLoader.hh"
#include "glob.h"

ClassImp(DataLoader);

DataLoader::DataLoader()
{
  this->CommonCtor();
}

void DataLoader::CommonCtor()
{
  opt = new BDSOutputROOTEventOptions();
  mod = new BDSOutputROOTEventModel();
  evt = new Event();

  optChain = new TChain("Model","Model");
  modChain = new TChain("Options","Options");
  evtChain = new TChain("Event","Event");
  
  this->BuildInputFileList();
  this->ChainTrees();
}

void DataLoader::BuildInputFileList() 
{
  std::string inputPath  = Config::Instance()->InputFilePath();
  if(inputPath == "") {
    throw std::string("DataLoader::BuildInputFileList> DataLoader needs to be constructed after Config");
  }

  // wild card
  if(inputPath.find("*") != std::string::npos) {
    glob_t glob_result;    
    glob(inputPath.c_str(),GLOB_TILDE,NULL,&glob_result);
    for(unsigned int i=0;i<glob_result.gl_pathc;++i) {
      fileNames.push_back(glob_result.gl_pathv[i]);
    }
    globfree(&glob_result);
  }
  // single file
  else if(inputPath.find(".root") != std::string::npos) {
    fileNames.push_back(inputPath);
  }
  // directory
  else if(inputPath[inputPath.length()-1] == std::string("/")) {
    // find all files in directory
    inputPath.append("/*.root");

    glob_t glob_result;    
    glob(inputPath.c_str(),GLOB_TILDE,NULL,&glob_result);
    for(unsigned int i=0;i<glob_result.gl_pathc;++i) {
      fileNames.push_back(glob_result.gl_pathv[i]);
    }
    globfree(&glob_result);    
  }

  if(Config::Instance()->Debug()) { 
    for(auto fn = fileNames.begin();fn != fileNames.end(); ++fn) {
      std::cout << "DataLoader::BuildInputFileList> " << *fn << std::endl;
    }
  }
}

void DataLoader::BuildTreeNameList() {

}

void DataLoader::BuildSamplerNameList() {

}

DataLoader::~DataLoader()
{
  delete opt;
  delete mod;
  delete evt;

  delete optChain;
  delete modChain;
  delete evtChain;
}

void DataLoader::ChainTrees()
{
  
}

BDSOutputROOTEventOptions* DataLoader::GetOptions()
{
  return opt;
}

BDSOutputROOTEventModel* DataLoader::GetModel()
{
  return mod;
}

Event* DataLoader::GetEvent()
{
  return evt;
}
