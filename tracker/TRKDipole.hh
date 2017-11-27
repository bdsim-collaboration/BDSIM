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
#ifndef TRKDipole_h
#define TRKDipole_h

#include "TRKElement.hh"

/**
 * @brief dipole/kicker. Strength in Tesla/metre.
 */

class TRKDipole: public TRKElement
{
public:
  TRKDipole(double      strength,
	    double      bField,
	    std::string name,
	    double      length,
	    TRKAperture *aperture,
	    TRKPlacement *placement);
  virtual ~TRKDipole();

  virtual void Track(TRKBunch* bunch, TRKStrategy* strategy);  
  
  double GetIntStrength()const{return intStrength;}

protected:
  /// output stream
  virtual void Print(std::ostream& out) const;

private:
  TRKDipole(); ///< not implemented

  /// magnetic field in Tesla - only used for hybrid tracking
  double bField;
  /// integrated strength MeV * urad
  double intStrength;
};

#endif
