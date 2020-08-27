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
#ifndef TRKSampler_h
#define TRKSampler_h

#include <memory>

#include "TRKElement.hh"
#include "TRKStrategy.hh"
#include "TRKBunch.hh"

class TRKOutput;

/**
 * @brief sampler class - output from tracker
 */

class TRKSampler: public TRKElement
{
public:
  TRKSampler(std::string nameIn,
	     int         indexIn,
	     std::shared_ptr<TRKOutput> outputIn,
	     double s);
  virtual ~TRKSampler(){;}

  virtual void Track(TRKParticle &particle, double step,
                     TRKStrategy*) override;
  bool OutsideAperture(TRKParticle const &p) const override;

protected:
  /// output stream
  void Print(std::ostream& out) const override;

private:
  TRKSampler(); ///< not implemented

  int        index;
  std::shared_ptr<TRKOutput> output;
  double s;
};

#endif
