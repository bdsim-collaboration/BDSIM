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
#include "HistogramAccumulator.hh"
#include "HistogramAccumulatorMerge.hh"
#include "HistogramAccumulatorSum.hh"
#include "HistogramAccumulatorV.hh"

#include "BDSOutputROOTEventHistograms.hh"

#include <exception>
#include <string>
#include <vector>

#include "TH1D.h"
#include "TH2D.h"
#include "TH3D.h"


template <class T>
HistogramAccumulatorV<T>::HistogramAccumulatorV(BDSOutputROOTEventHistograms* tempHistograms)
{
  std::vector<TH1D*> odh = tempHistograms->Get1DHistograms();
  for (const auto h : odh)
    {accumulators1d.push_back(new T(h, 1, std::string(h->GetName())+"Temp", h->GetTitle()));}
  std::vector<TH2D*> tdh = tempHistograms->Get2DHistograms();
  for (const auto h : tdh)
    {accumulators2d.push_back(new T(h, 2, std::string(h->GetName())+"Temp", h->GetTitle()));}
  std::vector<TH3D*> thdh = tempHistograms->Get3DHistograms();
  for (const auto h : thdh)
    {accumulators3d.push_back(new T(h, 3, std::string(h->GetName())+"Temp", h->GetTitle()));}
}

template <class T>
HistogramAccumulatorV<T>::~HistogramAccumulatorV()
{;}

template <class T>
void HistogramAccumulatorV<T>::Accumulate(BDSOutputROOTEventHistograms* oneEventHistograms)
{
  if (oneEventHistograms->Get1DHistograms().size() != accumulators1d.size())
    {throw std::runtime_error("Mismatching histogram container for accumulation.");}
  if (oneEventHistograms->Get2DHistograms().size() != accumulators2d.size())
    {throw std::runtime_error("Mismatching histogram container for accumulation.");}
  if (oneEventHistograms->Get3DHistograms().size() != accumulators3d.size())
    {throw std::runtime_error("Mismatching histogram container for accumulation.");}

  std::vector<TH1D*> odh  = oneEventHistograms->Get1DHistograms();
  std::vector<TH2D*> tdh  = oneEventHistograms->Get2DHistograms();
  std::vector<TH3D*> thdh = oneEventHistograms->Get3DHistograms();
  for (int i = 0; i < (int)odh.size(); i++)
    {accumulators1d[i]->Accumulate(odh[i]);}
  for (int i = 0; i < (int)tdh.size(); i++)
    {accumulators2d[i]->Accumulate(tdh[i]);}
  for (int i = 0; i < (int)thdh.size(); i++)
    {accumulators3d[i]->Accumulate(thdh[i]);}
}

template <class T>
BDSOutputROOTEventHistograms* HistogramAccumulatorV<T>::Terminate()
{
  std::vector<TH1D*> r1;
  for (auto a : accumulators1d)
    {r1.push_back(static_cast<TH1D*>(a->Terminate()));}
  std::vector<TH2D*> r2;
  for (auto a : accumulators2d)
    {r2.push_back(static_cast<TH2D*>(a->Terminate()));}
  std::vector<TH3D*> r3;
  for (auto a : accumulators3d)
    {r3.push_back(static_cast<TH3D*>(a->Terminate()));}
  
  BDSOutputROOTEventHistograms* r = new BDSOutputROOTEventHistograms(r1,r2,r3);
  return r;
}

template class HistogramAccumulatorV<HistogramAccumulator>;
template class HistogramAccumulatorV<HistogramAccumulatorMerge>;
template class HistogramAccumulatorV<HistogramAccumulatorSum>;
