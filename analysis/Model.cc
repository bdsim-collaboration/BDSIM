#include "Model.hh"

#include "BDSOutputROOTEventModel.hh"

#include "TROOT.h"
#include "TTree.h"

ClassImp(Model)

Model::Model():
model(nullptr)
{;}

Model::~Model()
{
  delete model;
}

void Model::SetBranchAddress(TTree *t)
{
  t->GetEntry(0);  // initialises local copy of class
  t->SetBranchAddress("Model.",&model);
}

std::vector<std::string> Model::SamplerNames() const
{
  return model->samplerNamesUnique;
}
