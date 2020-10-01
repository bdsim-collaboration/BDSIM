
#include "TFile.h"
#include "TChain.h"

#include "BDSOutputROOTEventHeader.hh"
#include "analysis/EventAnalysis.hh"
#include "analysis/SamplerAnalysis.hh"

#include "OpticsAccumulator.hh"
#include "TRKParticle.hh"

#include "CLHEP/Units/SystemOfUnits.h"


namespace trk {
void OpticsAccumulator::AccumulateParticle(int samplerIndex,
                                           const TRKParticle &particle)
{
  analyses[samplerIndex]
    ->AddParticle(particle.x / CLHEP::m,
		  particle.Xp(),
		  particle.y / CLHEP::m,
		  particle.Yp(),
		  particle.Momentum() / CLHEP::GeV,
		  1);
}

void OpticsAccumulator::AppendAnalysis(double s)
{
  sam = new SamplerAnalysis(s / CLHEP::m);
  sam->UpdateMass(938.272); // XXX: Hardcoded proton mass
  analyses.push_back(sam);
}

void OpticsAccumulator::Write(std::string filename)
{
  EventAnalysis ea;
  ea.AddSamplerAnalyses(std::move(analyses));
  ea.Terminate();
  TFile *file = new TFile((filename + "-optics.root").c_str(), "RECREATE");
  file->cd();

  BDSOutputROOTEventHeader *headerOut = new BDSOutputROOTEventHeader();
  // headerOut->Fill(dl->GetFileNames()); // updates time stamp
  headerOut->SetFileType("REBDSIM");
  TTree *headerTree = new TTree("Header", "REBDSIM Header");
  headerTree->Branch("Header.", "BDSOutputROOTEventHeader", headerOut);
  headerTree->Fill();
  headerTree->Write("", TObject::kOverwrite);

  ea.Write(file);
  file->Close();
  std::cout << "FINISHED"
            << "\n";
  delete file;
}

} // namespace trk
