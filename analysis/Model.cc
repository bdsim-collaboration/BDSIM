#include "Model.hh"

#include "BDSOutputROOTEventModel.hh"

#include "TChain.h"
#include "TROOT.h"

ClassImp(Model)

Model::Model():
model(nullptr)
{;}

Model::~Model()
{
  delete model;
}

void Model::SetBranchAddress(TChain *t)
{
  t->GetEntry(0);  // Pointers don't appear to be valid without this
  t->SetBranchAddress("Model.",&model);
}
