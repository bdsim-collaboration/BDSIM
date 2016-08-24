#include "DataLoader.hh"

#include "BDSDebug.hh"

#include <glob.h>

#include "TFile.h"

ClassImp(DataLoader)

DataLoader::DataLoader()
{
  this->CommonCtor();
}

DataLoader::~DataLoader()
{
  delete opt;
  delete mod;
  delete evt;
  delete run;

  delete optChain;
  delete modChain;
  delete evtChain;
  delete runChain;
}

void DataLoader::CommonCtor()
{
  opt = new Options();
  mod = new Model();
  evt = new Event();
  run = new Run();

  optChain = new TChain("Options","Options");
  modChain = new TChain("Model","Model");
  evtChain = new TChain("Event","Event");
  runChain = new TChain("Run","Run");

  this->BuildInputFileList();
  this->BuildTreeNameList();
  this->BuildEventBranchNameList();
  this->ChainTrees();
  this->SetBranchAddress();
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
    glob(inputPath.c_str(),GLOB_TILDE,nullptr,&glob_result);
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
    glob(inputPath.c_str(),GLOB_TILDE,nullptr,&glob_result);
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

void DataLoader::BuildTreeNameList()
{
  // open file - this is the first opening so test here if it's valid
  TFile *f = new TFile(fileNames[0].c_str());
  if (f->IsZombie())
  {
    std::cout << __METHOD_NAME__ << " no such file \"" << fileNames[0] << "\"" << std::endl;
    exit(1);
  }

  TList *kl = f->GetListOfKeys();

  for(int i=0;i<kl->GetEntries();++i)
    {this->treeNames.push_back(std::string(kl->At(i)->GetName()));}

  f->Close();
  delete f;

  if(Config::Instance()->Debug())
  {
    for(auto i = this->treeNames.begin(); i != this->treeNames.end(); ++i)
      std::cout << "DataLoader::BuildTreeNameList> " <<  *i << std::endl;
  }
}

void DataLoader::BuildEventBranchNameList()
{
  TFile *f = new TFile(fileNames[0].c_str());
  if (f->IsZombie())
  {
    std::cout << __METHOD_NAME__ << " no such file \"" << fileNames[0] << "\"" << std::endl;
    exit(1);
  }

  TTree *et = (TTree*)f->Get("Event");
  if (!et)
  {return;}
  TObjArray *bl = et->GetListOfBranches();

  for(int i=0;i< bl->GetEntries();++i)
  {
    TString name = bl->At(i)->GetName();
    if(name.Contains("Sampler"))
      {this->samplerNames.push_back(name.Data());}
    else
      {this->branchNames.push_back(name.Data());}
  }

  f->Close();
  delete f;

  if(Config::Instance()->Debug())
  {
    for(auto i = this->branchNames.begin(); i != this->branchNames.end(); ++i)
      {std::cout << "DataLoader::BuildEventBranchNameList> Non-sampler : " <<  *i << std::endl;}

    for(auto i = this->samplerNames.begin(); i != this->samplerNames.end(); ++i)
      {std::cout << "DataLoader::BuildEventBranchNameList> Sampler     : " <<  *i << std::endl;}
  }
}

void DataLoader::ChainTrees()
{
  // loop over files and chain trees
  for (auto filename : fileNames)
    {
      optChain->Add(filename.c_str());
      modChain->Add(filename.c_str());
      evtChain->Add(filename.c_str());
      runChain->Add(filename.c_str());
    }
}

void DataLoader::SetBranchAddress()
{
  mod->SetBranchAddress(modChain);
  opt->SetBranchAddress(optChain);
  evt->SetBranchAddress(evtChain,this->samplerNames);
  run->SetBranchAddress(runChain);
}
