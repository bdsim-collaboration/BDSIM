/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2023.

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
#ifndef TH2DOpt_H
#define TH2DOpt_H

#include "TH2.h"
#include "TH2D.h"

#include <set>

/**
 * @brief TH1D but with a category axis.
 * 
 * @author Laurie Nevay
 */

class TH2DOpt: public TH2D
{
public:
  using TH2D::TH2D;

  virtual ~TH2DOpt(){;}

  virtual Int_t Fill(Double_t x, Double_t y, Double_t w)
  {
    auto binIndex = TH2D::Fill(x,y,w);
    binsFilled.insert(binIndex);
  }

  const std::set<Int_t>& FilledBins() const {return binnsFilled;}
  
  virtual void Flush();
  
  std::set<Int_t> binsFilled;

  ClassDefOverride(TH2DOpt,1)
};

#endif
