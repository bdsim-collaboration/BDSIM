/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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

#include "TfsFile.hh"

#include <exception>
#include <map>
#include <string>
#include <vector>


PTC::TfsFile::TfsFile(const std::string& fileNameIn):
  fileName(fileNameIn)
{
  Load();
}

void PTC::TfsFile::Load()
{;}

std::string PTC::TfsFile::HeaderValue(const std::string& key) const
{
  auto result = header.find(key);
  if (result != header.end())
    {return result->second;}
  else
    {throw std::string("No such segment name");}
}

PTC::segment PTC::TfsFile::Segment(const int& index) const
{
  if (index > (int)segments.size() || index < 0)
    {throw std::string("Index > number of segments");}
  return segments.at(index);
}
