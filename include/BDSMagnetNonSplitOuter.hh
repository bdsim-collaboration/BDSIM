/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2021.

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
#ifndef BDSMAGNETNONSPLITOUTER_H
#define BDSMAGNETNONSPLITOUTER_H

#include "BDSAcceleratorComponent.hh"
#include "BDSMagnet.hh"
#include "BDSMagnetType.hh"

#include "G4String.hh"
#include "G4Types.hh"

#include "parser/element.h"
#include "parser/elementtype.h"

class BDSBeamPipe;
class BDSBeamPipeInfo;
class BDSFieldInfo;
class BDSMagnetOuter;
class BDSMagnetOuterInfo;
class BDSMagnetStrength;
class BDSIntegratorSet;

namespace GMAD
{
  struct Element;
}

/**
 * @brief Magnet with a one piece outer and split beam pipe.
 *
 * @author Eliott Ramoisiaux.
 */

class BDSMagnetNonSplitOuter: public BDSMagnet
{
public:
  /// Magnet base class constructor that is for basic geometric information rather
  /// magnetic field details, which are handled in the derived classes
  BDSMagnetNonSplitOuter(BDSMagnetType           typeIn,
			 BDSBeamPipeInfo*        beamPipeInfoIn,
			 BDSMagnetOuterInfo*     magnetOuterInfoIn,
			 BDSFieldInfo*           vacuumFieldInfoIn,
			 BDSFieldInfo*           outerFieldInfoIn,
			 G4bool                  isThinIn,
			 const GMAD::Element*    elementIn,
			 BDSMagnetStrength*      stIn,
			 G4double                brhoIn,
			 const BDSIntegratorSet* integratorSetIn,
			 G4double                incomingFaceAngleIn,
			 G4double                outgoingFaceAngleIn,
			 G4bool                  buildFringeFieldsIn,
			 const GMAD::Element*    prevElementIn,
			 const GMAD::Element*    nextElementIn);

  void SBendWithSingleOuter(const G4String& elementName);

protected:
  virtual void Build();
  
private:
  const GMAD::Element*    element;
  BDSMagnetStrength*      st;
  G4double                brho;
  const BDSIntegratorSet* integratorSet;
  G4double                incomingFaceAngle;
  G4double                outgoingFaceAngle;
  G4bool                  buildFringeFields;
  const GMAD::Element*    prevElement;
  const GMAD::Element*    nextElement;
  std::vector<G4String>   namedVacuumVolumes;
};

#endif
