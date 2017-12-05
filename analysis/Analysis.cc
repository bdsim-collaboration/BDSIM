/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

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
#include "Analysis.hh"
#include "Config.hh"
#include "HistogramDef.hh"
#include "HistogramDef1D.hh"
#include "HistogramDef2D.hh"
#include "HistogramDef3D.hh"
#include "rebdsim.hh"

#include "TChain.h"

Analysis::Analysis(std::string treeNameIn,
		   TChain*     chainIn,
		   std::string mergedHistogramNameIn,
		   bool        debugIn):
  treeName(treeNameIn),
  chain(chainIn),
  mergedHistogramName(mergedHistogramNameIn),
  histoSum(nullptr),
  debug(debugIn)
{;}

Analysis::~Analysis()
{
  delete chain;
  delete histoSum;
}

void Analysis::Execute()
{
  std::cout << "Analysis on \"" << treeName << "\" beginning" << std::endl;
  Process();
  SimpleHistograms();
  Terminate();
  std::cout << "Analysis on \"" << treeName << "\" complete" << std::endl;
}

void Analysis::UserProcess()
{;}

void Analysis::SimpleHistograms()
{
  if(debug)
    {std::cout << __METHOD_NAME__ << std::endl;}

  // loop over histogram specifications and fill
  auto c = Config::Instance();
  if (c)
    {
      auto definitions = Config::Instance()->HistogramDefinitions(treeName);
      for (auto definition : definitions)
	{FillHistogram(definition);}
    }
}

void Analysis::Terminate()
{
  if (debug)
    {std::cout << __METHOD_NAME__ << std::endl;}
  if (histoSum)
    {histoSum->Terminate();}
}

void Analysis::FillHistogram(HistogramDef* definition)
{
  // ensure new histograms are added to file..
  TH1::AddDirectory(kTRUE);
  TH2::AddDirectory(kTRUE);
  TH3::AddDirectory(kTRUE);
  
  // pull out communal information in base class
  int         nDim      = definition->nDimensions;
  std::string name      = definition->histName;
  std::string command   = definition->variable + " >> " + definition->histName;
  std::string selection = definition->selection;
  
  switch (nDim)
    {
    case 1:
      {
	HistogramDef1D* d = static_cast<HistogramDef1D*>(definition);
	TH1D* h = new TH1D(name.c_str(), name.c_str(),
			   d->xNBins, d->xLow, d->xHigh);
	chain->Draw(command.c_str(), selection.c_str(),"goff");
	histogramNames.push_back(name);
	histograms1D[name] = h;
	break;
      }
    case 2:
      {
	HistogramDef2D* d = static_cast<HistogramDef2D*>(definition);
	TH2D* h = new TH2D(name.c_str(), name.c_str(),
			   d->xNBins, d->xLow, d->xHigh,
			   d->yNBins, d->yLow, d->yHigh);
	chain->Draw(command.c_str(), selection.c_str(),"goff");
	histogramNames.push_back(name);
	histograms2D[name] = h;
	break;
      }
    case 3:
      {
	HistogramDef3D* d = static_cast<HistogramDef3D*>(definition);
	TH3D* h = new TH3D(name.c_str(), name.c_str(),
			   d->xNBins, d->xLow, d->xHigh,
			   d->yNBins, d->yLow, d->yHigh,
			   d->zNBins, d->zLow, d->zHigh);
	chain->Draw(command.c_str(), selection.c_str(),"goff");
	histogramNames.push_back(name);
	histograms3D[name] = h;
	break;
      }
    default:
      {break;}
    }
}

void Analysis::Write(TFile* outputFile)
{
  //treeName typically has a "." at the end, deleting it here:
  std::string cleanedName = treeName.erase(treeName.size() - 1);
  std::string outputDirName = cleanedName + std::string("Histograms");
  TDirectory *rebdsimDir = outputFile->mkdir(outputDirName.c_str());
  rebdsimDir->cd();
  for(auto h : histograms1D)
    {h.second->Write();}
  for(auto h : histograms2D)
    {h.second->Write();}
  for (auto h : histograms3D)
    {h.second->Write();}
  outputFile->cd("/");
  
  // Merged Histograms for this analysis instance (could be run, event etc)
  if (histoSum)
    {
      std::cout << "Merging histograms from \"" << treeName << "\" analysis" << std::endl;
      TDirectory* bdsimDir = outputFile->mkdir(mergedHistogramName.c_str());
      bdsimDir->cd();
      histoSum->Write(outputFile);
    }
}
