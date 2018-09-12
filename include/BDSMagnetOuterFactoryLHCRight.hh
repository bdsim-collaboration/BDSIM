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
#ifndef BDSMAGNETOUTERFACTORYLHCRIGHT_H
#define BDSMAGNETOUTERFACTORYLHCRIGHT_H

#include "BDSMagnetOuterFactoryLHC.hh"

/**
 * @brief LHC outer magnet geometry offset to the right
 * 
 * This factory gets everything from the LHC factory and 
 * simply has it's own singleton accessor
 * 
 * @author Laurie Nevay
 */

class BDSMagnetOuterFactoryLHCRight: public BDSMagnetOuterFactoryLHC
{
public:
  static BDSMagnetOuterFactoryLHCRight* Instance(); /// singleton pattern
  
  virtual ~BDSMagnetOuterFactoryLHCRight();

private:
  BDSMagnetOuterFactoryLHCRight(); //private constructor as singleton
  static BDSMagnetOuterFactoryLHCRight* instance;
};

#endif
