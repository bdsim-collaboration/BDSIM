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
#ifndef PARTICLESET_H
#define PARTICLESET_H

#include <cmath>
#include <map>

/**
 * @brief A very simple 'map' type histogram axis.
 *
 * A map of type T (e.g. integer for PDG ID) to an accumulated value which
 * is a double. The sum of the weights squared is also accumulated for error
 * estimation.
 * 
 * @author Laurie Nevay
 */

class BinValue
{
public:
  BinValue():
    value(0),
    sumOfWeightsSquared(0)
  {;}

  inline void Fill(double weight = 1)
  {
    value += weight;
    sumOfWeightsSquared += weight*weight;
  };

  inline void Reset() {value = 0; sumOfWeightsSquared = 0;}
  inline double Error() const {return std::sqrt(sumOfWeightsSquared);}

  double value;
  double sumOfWeightsSquared;
};


template<class T> class ParticleSet
{
public:
  void Fill(T      x,
	    double weight = 1)
  {data[x].Fill(weight);}

  std::map<T, BinValue> data;
};
  
#endif
