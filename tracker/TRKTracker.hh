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
#ifndef TRKTracker_h
#define TRKTracker_h

#include "TRKBunch.hh"

class BDSOutput;

class TRKLine;
class TRKStrategy;
class TRKElement;
class TRKParticle;

namespace GMAD
{
  class Options;
}

/**
 * @brief class that takes care of the tracker
 */

class TRKTracker
{
public:
  TRKTracker(TRKLine*             line,
	     TRKStrategy*         strategy,
	     const GMAD::Options& options,
	     BDSOutput*           outputIn);
  ~TRKTracker();
  /// track bunch through beamline - main tracking loop
  void Track(TRKBunch* bunch);
  /// move a particle to bdsim tracking queue and delete from bunch, 
  void Shift(TRKBunch* bunch, TRKBunchIter& iter);

private:
  TRKLine*  line;
  /// tracking type
  TRKStrategy* strategy;
  /// aperture check
  void CheckAperture(TRKBunch* bunch, TRKElement* element);
  /// maximum number of turns
  unsigned int maxTurns;
  bool dontuseaperture;

  BDSOutput* output;
};

#endif
