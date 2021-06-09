/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#include "BDSBH4DBase.hh"

#include "TFile.h"

#include <string>

ClassImp(BDSBH4DBase)

BDSBH4DBase::BDSBH4DBase():
  h_nxbins(0),
  h_nybins(0),
  h_nzbins(0),
  h_nebins(0),
  h_xmin(0), h_xmax(0),
  h_ymin(0), h_ymax(0),
  h_zmin(0), h_zmax(0),
  h_emin(0), h_emax(0),
  h_entries(0)
{;}

int BDSBH4DBase::GetNbinsX() const
{
  return static_cast<int>(h_nxbins);
}

int BDSBH4DBase::GetNbinsY() const
{
  return static_cast<int>(h_nybins);
}

int BDSBH4DBase::GetNbinsZ() const
{
  return static_cast<int>(h_nzbins);
}

int BDSBH4DBase::GetNbinsE() const
{
  return static_cast<int>(h_nebins);
}

const char* BDSBH4DBase::GetName() const
{
  return h_name.c_str();
}

const char* BDSBH4DBase::GetTitle() const
{
  return h_title.c_str();
}

unsigned long BDSBH4DBase::GetEntries_BDSBH4D()
{
  return h_entries;
}

void BDSBH4DBase::SetName(const char* name)
{
  h_name = std::string(name);
}

void BDSBH4DBase::SetTitle(const char* title)
{
  h_title = std::string(title);
}

void BDSBH4DBase::SetEntries_BDSBH4D(double i)
{
  h_entries = static_cast<unsigned long>(i);
}
