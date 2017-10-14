#include "Beam.hh"
#include "BeamAnalysis.hh"

#include "TROOT.h"

class TChain;

ClassImp(BeamAnalysis)

BeamAnalysis::BeamAnalysis():
  Analysis("Beam.", nullptr, "BeamHistogramsMerged"),
  beam(nullptr)
{;}

BeamAnalysis::BeamAnalysis(Beam* beamIn, TChain* chainIn, bool debugIn):
  Analysis("Beam.", chainIn, "BeamHistogramsMerged", debugIn),
  beam(beamIn)
{;}

BeamAnalysis::~BeamAnalysis()
{
  delete beam;
}
