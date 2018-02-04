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
#ifndef BDSINTEGRATORSET_H
#define BDSINTEGRATORSET_H

#include "BDSFieldType.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSIntegratorType.hh"

#include "globals.hh" // geant4 types / globals

class BDSIntegratorSet;

namespace BDS
{
  /// @{ Return the appropriate set of integrators to use for each magnet type.
  const BDSIntegratorSet* IntegratorSet(G4String             set);
  const BDSIntegratorSet* IntegratorSet(BDSIntegratorSetType set);
  /// @}
}

/**
 * @brief Which integrator to use for each type of magnet / field object.
 * 
 * There is one to match every type of field in BDSFieldType. There are also
 * a few others such as dipolefringe and multipolethin that can be used in
 * explicit circumstances as these do not have a different physical field, 
 * merely a different integrator in different situations. These should be 
 * accessed directly rather than through the method Integrator().
 * 
 * @author Laurie Nevay
 */

class BDSIntegratorSet
{
public:
  BDSIntegratorSet(BDSIntegratorType solenoidIn,
		   BDSIntegratorType dipoleIn,
		   BDSIntegratorType dipolematrixIn,
		   BDSIntegratorType quadrupoleIn,
		   BDSIntegratorType sextupoleIn,
		   BDSIntegratorType octupoleIn,
		   BDSIntegratorType decapoleIn,
		   BDSIntegratorType multipoleIn,
		   BDSIntegratorType muonspoilerIn,
		   BDSIntegratorType rfcavityIn,
		   BDSIntegratorType rfIn,
		   BDSIntegratorType generalIn,
		   BDSIntegratorType skewQuadrupoleIn,
		   BDSIntegratorType skewSextupoleIn,
		   BDSIntegratorType skewOctupoleIn,
		   BDSIntegratorType skewDecapoleIn,
		   BDSIntegratorType dipolefringeIn,
		   BDSIntegratorType multipolethinIn);

  /// Get appropriate integrator based on the field type.
  BDSIntegratorType Integrator(const BDSFieldType field)const;

  BDSIntegratorType solenoid;
  BDSIntegratorType dipole;
  BDSIntegratorType dipolematrix;
  BDSIntegratorType quadrupole;
  BDSIntegratorType sextupole;
  BDSIntegratorType octupole;
  BDSIntegratorType decapole;
  BDSIntegratorType multipole;
  BDSIntegratorType muonspoiler;
  BDSIntegratorType rfcavity;
  BDSIntegratorType rf;
  BDSIntegratorType general;
  BDSIntegratorType skewQuadrupole;
  BDSIntegratorType skewSextupole;
  BDSIntegratorType skewOctupole;
  BDSIntegratorType skewDecapole;
  BDSIntegratorType dipolefringe;
  BDSIntegratorType multipolethin;
};

#endif
