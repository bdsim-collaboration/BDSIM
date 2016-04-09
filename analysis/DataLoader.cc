#include "DataLoader.hh"

#include "glob.h"

#include "TFile.h"

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

void DataLoader::BuildTreeNameList()
{
  // open file
  TFile *f = new TFile(fileNames[0].c_str());

  TList *kl = f->GetListOfKeys();

  for(int i=0;i<kl->GetEntries();++i)
  {
    this->treeNames.push_back(std::string(kl->At(i)->GetName()));
  }

  f->Close();
  delete f;

  if(Config::Instance()->Debug())
  {
    for(auto i = this->treeNames.begin(); i != this->treeNames.end(); ++i)
      std::cout << "DataLoader::BuildTreeNameList> " <<  *i << std::endl;
  }



}

void DataLoader::BuildEventBranchNameList() {
  TFile *f = new TFile(fileNames[0].c_str());

  TTree *et = (TTree*)f->Get("Event");

  TObjArray *bl = et->GetListOfBranches();

  for(int i=0;i< bl->GetEntries();++i)
  {
    TString name = bl->At(i)->GetName();
    if(name.Contains("Sampler"))
    {
      this->samplerNames.push_back(name.Data());
    }
    else
    {
      this->branchNames.push_back(name.Data());
    }
  }

  if(Config::Instance()->Debug())
  {
    for(auto i = this->branchNames.begin(); i != this->branchNames.end(); ++i)
      std::cout << "DataLoader::BuildEventBranchNameList> Non-sampler : " <<  *i << std::endl;

    for(auto i = this->samplerNames.begin(); i != this->samplerNames.end(); ++i)
      std::cout << "DataLoader::BuildEventBranchNameList> Sampler     : " <<  *i << std::endl;

  }

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
  // loop over files and chain trees
  for(auto i = this->fileNames.begin(); i != this->fileNames.end(); ++i)
  {
    optChain->Add((*i).c_str());
    modChain->Add((*i).c_str());
    evtChain->Add((*i).c_str());
  }
}

void DataLoader::SetBranchAddress() 
{
  optChain->SetBranchAddress("Options.",&opt);
  modChain->SetBranchAddress("Model.",&mod);
  evt->SetBranchAddress(evtChain);
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
