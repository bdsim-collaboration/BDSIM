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
#include "BinGeneration.hh"
#include "Config.hh"
#include "HistogramDef.hh"
#include "HistogramDef1D.hh"
#include "HistogramDef2D.hh"
#include "HistogramDef3D.hh"
#include "PerEntryHistogram.hh"
#include "rebdsim.hh"

#include "TChain.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"

Analysis::Analysis(std::string treeNameIn,
		   TChain*     chainIn,
		   std::string mergedHistogramNameIn,
		   bool        perEntryAnalysis,
		   bool        debugIn):
  treeName(treeNameIn),
  chain(chainIn),
  mergedHistogramName(mergedHistogramNameIn),
  histoSum(nullptr),
  debug(debugIn),
  perEntry(perEntryAnalysis)
{
  entries = chain->GetEntries();
  // only activate per entry histograms if at least 2 entries.
  runPerEntryHistograms = entries > 1;
}

Analysis::~Analysis()
{
  delete chain;
  delete histoSum;
}

void Analysis::Execute()
{
  std::cout << "Analysis on \"" << treeName << "\" beginning" << std::endl;
  if (perEntry)
    {
      // ensure new histograms are added to file
      // crucial for draw command to work as it identifies the histograms by name
      TH1::AddDirectory(kTRUE);
      TH2::AddDirectory(kTRUE);
      TH3::AddDirectory(kTRUE);
      PreparePerEntryHistograms();
      Process();
    }
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
      auto definitions = Config::Instance()->HistogramDefinitionsSimple(treeName);
      for (auto definition : definitions)
	{FillHistogram(definition);}
    }
}

void Analysis::PreparePerEntryHistograms()
{
  auto definitions = Config::Instance()->HistogramDefinitionsPerEntry(treeName);
  if (!runPerEntryHistograms && definitions.size() > 0)
    {
      std::cout << "Warning: per-entry histograms specified, but insufficient\n ";
      std::cout << "        number of entries (" << entries << ") to calculate means and variances." << std::endl;
      std::cout << "Per-entry histograms will not be produced for this Tree." << std::endl;
      return;
    }
  for (const auto& def : definitions)
    {perEntryHistograms.push_back(new PerEntryHistogram(def, chain));}
}

void Analysis::AccumulatePerEntryHistograms()
{
  if (!runPerEntryHistograms)
    {return;}
  auto definitions = Config::Instance()->HistogramDefinitionsPerEntry(treeName);
  for (auto& peHist : perEntryHistograms)
    {peHist->AccumulateCurrentEntry();}
}

void Analysis::TerminatePerEntryHistograms()
{
  if (!runPerEntryHistograms)
    {return;}
  auto definitions = Config::Instance()->HistogramDefinitionsPerEntry(treeName);
  for (auto& peHist : perEntryHistograms)
    {peHist->Terminate();}
}

void Analysis::Terminate()
{
  if (debug)
    {std::cout << __METHOD_NAME__ << std::endl;}
  if (histoSum)
    {histoSum->Terminate();}
  if (perEntry)
    {TerminatePerEntryHistograms();}
}

void Analysis::Write(TFile* outputFile)
{
  // treeName typically has a "." at the end, deleting it here:
  std::string cleanedName     = treeName.erase(treeName.size() - 1);
  std::string outputDirName   = cleanedName;
  std::string perEntryDirName = "PerEntryHistograms";
  std::string simpleDirName   = "SimpleHistograms";
  std::string mergedDirName   = "MergedHistograms";
  TDirectory* rebdsimDir  = outputFile->mkdir(outputDirName.c_str());
  TDirectory* perEntryDir = rebdsimDir->mkdir(perEntryDirName.c_str());
  TDirectory* simpleDir   = rebdsimDir->mkdir(simpleDirName.c_str());
  TDirectory* mergedDir   = rebdsimDir->mkdir(mergedDirName.c_str());

  // per entry histograms
  if (runPerEntryHistograms)
    {
      perEntryDir->cd();
      for (auto h : perEntryHistograms)
	{h->Write(perEntryDir);}
    }

  // simple histograms
  simpleDir->cd();
  for (auto& h : histograms1D)
    {simpleDir->Add(h.second);}
  for (auto& h : histograms2D)
    {simpleDir->Add(h.second);}
  for (auto& h : histograms3D)
    {simpleDir->Add(h.second);}
  for (auto& h : histograms1D)
    {h.second->Write();}
  for (auto& h : histograms2D)
    {h.second->Write();}
  for (auto& h : histograms3D)
    {h.second->Write();}

  // merged histograms
  if (histoSum)
    {
      mergedDir->cd();
      std::cout << "Merging histograms from \"" << treeName << "\" analysis" << std::endl;
      histoSum->Write(outputFile, mergedDir);
    }

  outputFile->cd("/");  // return to root of the file
}

void Analysis::FillHistogram(HistogramDef* definition)
{
  // ensure new histograms are added to file..
  // this is crucial for the draw command to work as it finds the histograms by name
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
	TH1D* h;
	if (d->logarithmicX)
          {// note ROOT requires len(binEdges) = nBins + 1
	    std::vector<double> binEdges = RBDS::LogSpace(d->xLow, d->xHigh, d->xNBins);
	    h = new TH1D(name.c_str(), name.c_str(), d->xNBins, binEdges.data());
          }
	else
          {
	    h = new TH1D(name.c_str(), name.c_str(),
			 d->xNBins, d->xLow, d->xHigh);
          }
	chain->Draw(command.c_str(), selection.c_str(),"goff");
	histogramNames.push_back(name);
	histograms1D[name] = h;
	break;
      }
    case 2:
      {
	HistogramDef2D* d = static_cast<HistogramDef2D*>(definition);
	TH2D* h;
	if (d->logarithmicX && d->logarithmicY)
          {
	    std::vector<double> xBinEdges = RBDS::LogSpace(d->xLow, d->xHigh, d->xNBins);
	    std::vector<double> yBinEdges = RBDS::LogSpace(d->yLow, d->yHigh, d->yNBins);
	    h = new TH2D(name.c_str(), name.c_str(),
			 d->xNBins, xBinEdges.data(),
			 d->yNBins, yBinEdges.data());
          }
	else if (d->logarithmicX)
          {
	    std::vector<double> xBinEdges = RBDS::LogSpace(d->xLow, d->xHigh, d->xNBins);
	    h = new TH2D(name.c_str(), name.c_str(),
			 d->xNBins, xBinEdges.data(),
			 d->yNBins, d->yLow, d->yHigh);
          }
	else if (d->logarithmicY)
          {
	    std::vector<double> yBinEdges = RBDS::LogSpace(d->yLow, d->yHigh, d->yNBins);
	    h = new TH2D(name.c_str(), name.c_str(),
			 d->xNBins, d->xLow, d->xHigh,
			 d->yNBins, yBinEdges.data());
          }
	else
          {
	    h = new TH2D(name.c_str(), name.c_str(),
			 d->xNBins, d->xLow, d->xHigh,
			 d->yNBins, d->yLow, d->yHigh);
          }
	chain->Draw(command.c_str(), selection.c_str(),"goff");
	histogramNames.push_back(name);
	histograms2D[name] = h;
	break;
      }
    case 3:
      {
	HistogramDef3D* d = static_cast<HistogramDef3D*>(definition);
	TH3D* h;
	if (d->logarithmicX || d->logarithmicY || d->logarithmicZ)
	  {
	    std::vector<double> xBinEdges;
	    std::vector<double> yBinEdges;
	    std::vector<double> zBinEdges;
	    if (d->logarithmicX)
	      {xBinEdges = RBDS::LogSpace(d->xLow, d->xHigh, d->xNBins);}
	    else
	      {xBinEdges = RBDS::LinSpace(d->xLow, d->xHigh, d->xNBins);}
	    if (d->logarithmicY)
	      {yBinEdges = RBDS::LogSpace(d->yLow, d->yHigh, d->yNBins);}
	    else
	      {yBinEdges = RBDS::LinSpace(d->yLow, d->yHigh, d->yNBins);}
	    if (d->logarithmicZ)
	      {zBinEdges = RBDS::LogSpace(d->zLow, d->zHigh, d->zNBins);}
	    else
	      {zBinEdges = RBDS::LinSpace(d->zLow, d->zHigh, d->zNBins);}
	    h = new TH3D(name.c_str(), name.c_str(),
			 d->xNBins, xBinEdges.data(),
			 d->yNBins, yBinEdges.data(),
			 d->zNBins, zBinEdges.data());
	  }
	else
	  {
	    h = new TH3D(name.c_str(), name.c_str(),
			 d->xNBins, d->xLow, d->xHigh,
			 d->yNBins, d->yLow, d->yHigh,
			 d->zNBins, d->zLow, d->zHigh);
	  }
	chain->Draw(command.c_str(), selection.c_str(),"goff");
	histogramNames.push_back(name);
	histograms3D[name] = h;
	break;
      }
    default:
      {break;}
    }
}

