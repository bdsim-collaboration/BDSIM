#include "Model.hh"
#include "RebdsimTypes.hh"

#include "BDSOutputROOTEventModel.hh"

#include "TROOT.h"
#include "TTree.h"

#include <iostream>
#include <string>
#include <vector>

ClassImp(Model)

Model::Model():
  Model(false)
{;}

Model::Model(bool debugIn):
  model(nullptr),
  debug(debugIn)
{
  model = new BDSOutputROOTEventModel();
}

Model::~Model()
{
  delete model;
}

void Model::SetBranchAddress(TTree* t,
			     bool                      allBranchesOn,
			     const RBDS::VectorString* branchesToTurnOn)
{
  // turn off all branches by default.
  t->SetBranchStatus("*", 0);

  if (allBranchesOn)
    {t->SetBranchStatus("*", 1);}
  else if (branchesToTurnOn)
    {
      for (auto name : *branchesToTurnOn)
	{
	  std::string nameStar = name + ".*"; // necessary because of the splitting
	  if (debug)
	    {std::cout << "Turning on branch \"" << nameStar << "\"" << std::endl;}
	  t->SetBranchStatus(nameStar.c_str(), 1);
	}
    }
  
  t->SetBranchAddress("Model.",&model);
}

std::vector<std::string> Model::SamplerNames() const
{
  if (model)
    {return model->samplerNamesUnique;}
  else
    {return std::vector<std::string>();}
}
