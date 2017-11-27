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
#ifndef TRKPlacement_h
#define TRKPlacement_h

#include "vector3.hh"

class TRKPlacement {
public : 
  TRKPlacement();
  TRKPlacement(double x, double y, double z);
  ~TRKPlacement(); 
  
  vector3 GetGlobal()const;

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKPlacement &placement);

private :
  double x;
  double y;
  double z;
};

#endif
