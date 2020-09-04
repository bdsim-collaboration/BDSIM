/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2020.

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

unsigned long BDSBH4DBase::GetEntries()
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

void BDSBH4DBase::SetEntries(double i)
{
  h_entries = static_cast<unsigned long>(i);
}
