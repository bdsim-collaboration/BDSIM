#include "ModelAnalysis.hh"

#include "TROOT.h"
#include "Model.hh"
#include "TChain.h"

ClassImp(ModelAnalysis)

ModelAnalysis::ModelAnalysis():
  Analysis("Model.")
{}

ModelAnalysis::ModelAnalysis(Model* modelIn, TChain *chainIn):
  Analysis("Model.")
{
  model   = modelIn;
  chain = chainIn;
}
