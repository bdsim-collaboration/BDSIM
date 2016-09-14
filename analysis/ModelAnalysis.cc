#include "ModelAnalysis.hh"

#include "TROOT.h"
#include "Model.hh"
#include "TChain.h"

ClassImp(ModelAnalysis)

ModelAnalysis::ModelAnalysis():
Analysis("Model.", nullptr, "ModelHistogramsMerged"),
  model(nullptr)
{}

ModelAnalysis::ModelAnalysis(Model* modelIn, TChain* chain, bool debug):
  Analysis("Model.", chain, "ModelHistogramsMerged", debug),
  model(modelIn)
{;}
