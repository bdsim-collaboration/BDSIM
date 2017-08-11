#include "ModelAnalysis.hh"

#include "TROOT.h"
#include "Model.hh"
#include "TChain.h"

ClassImp(ModelAnalysis)

ModelAnalysis::ModelAnalysis():
  Analysis("Model.", nullptr, "ModelHistogramsMerged"),
  model(nullptr)
{}

ModelAnalysis::ModelAnalysis(Model* modelIn, TChain* chainIn, bool debugIn):
  Analysis("Model.", chainIn, "ModelHistogramsMerged", debugIn),
  model(modelIn)
{;}
