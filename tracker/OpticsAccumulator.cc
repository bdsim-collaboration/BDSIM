/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2022.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/

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
