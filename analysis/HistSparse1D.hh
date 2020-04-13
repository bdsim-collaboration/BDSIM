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
#ifndef HISTSPARSE1D_H
#define HISTSPARSE1D_H

#include <cmath>
#include <map>
#include <string>

/**
 * @brief Sparse 1D histogram based on a map.
 *
 * Simple implementation for what we need. ROOT's THnSparse is really complicated
 * and couldn't get to work - overly complex for what we need. Boost histograms are
 * better but we don't have a dependency on that yet, so for now a super simple one
 * of our own.
 * 
 * This doesn't have any limits and just accumulates for each unique key forming a
 * set of keys.
 *
 * @author L. Nevay
 */

template<class T>
class HistSparse1D
{
public:
  /// Public constructor only for compatibility with ROOT - not intended for use.
  HistSparse1D():
    name("sparse_hist"),
    entries(0)
  {;}

  /// Use this constructor.
  explicit HistSparse1D(const std::string& nameIn):
    name(nameIn),
    entries(0)
  {;}
  ~HistSparse1D(){;}
  
  struct BinWorking
  {
    double sumWeights;
    double sumWeightsSquared;
  };

  struct Bin
  {
    double value;
    double error;
  };

  inline void Fill(T x, double weight=1.0)
  {
    auto& v = data[x];
    v.sumWeights += weight; 
    v.sumWeightsSquared += weight*weight;
    entries++;
  }
  inline long long int Entries() const {return entries;}
  inline size_t        size()    const {return data.size();}
  std::map<T, Bin> Result() const
  {
    std::map<T, Bin> result;
    for (const auto& kv : data)
      {result[kv.first] = Bin {kv.second.sumWeights, std::sqrt(kv.second.sumWeightsSquared)};}
    return result;
  };
  
  std::string name;
  std::map<T, BinWorking> data;
  long long int entries;
};

#endif
