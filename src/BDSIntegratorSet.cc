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
#include "BDSDebug.hh"
#include "BDSIntegratorSet.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSIntegratorType.hh"

#include "globals.hh" // geant4 types / globals


BDSIntegratorSet::BDSIntegratorSet(BDSIntegratorType solenoidIn,
				   BDSIntegratorType dipoleIn,
				   BDSIntegratorType dipole3dIn,
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
				   BDSIntegratorType multipolethinIn):
  solenoid(solenoidIn),
  dipole(dipoleIn),
  dipole3d(dipole3dIn),
  quadrupole(quadrupoleIn),
  sextupole(sextupoleIn),
  octupole(octupoleIn),
  decapole(decapoleIn),
  multipole(multipoleIn),
  muonspoiler(muonspoilerIn),
  rfcavity(rfcavityIn),
  rf(rfIn),
  general(generalIn),
  skewQuadrupole(skewQuadrupoleIn),
  skewSextupole(skewSextupoleIn),
  skewOctupole(skewOctupoleIn),
  skewDecapole(skewDecapoleIn),
  dipolefringe(dipolefringeIn),
  multipolethin(multipolethinIn)
{;}

namespace BDS
{  
  const BDSIntegratorSet* integratorsBDSIMOne =
    new BDSIntegratorSet(BDSIntegratorType::solenoid,       // solenoid
			 BDSIntegratorType::dipole,         // dipole
			 BDSIntegratorType::g4classicalrk4, // dipole3d
			 BDSIntegratorType::quadrupole,     // quadrupole
			 BDSIntegratorType::sextupole,      // sextupole
			 BDSIntegratorType::octupole,       // octupole
			 BDSIntegratorType::decapole,       // decapole
			 BDSIntegratorType::g4classicalrk4, // (thick) multipole
			 BDSIntegratorType::g4classicalrk4, // muonspoiler
			 BDSIntegratorType::g4classicalrk4, // rfcavity
			 BDSIntegratorType::g4classicalrk4, // rf
			 BDSIntegratorType::g4classicalrk4, // general
			 BDSIntegratorType::g4classicalrk4, // skew quadrupole
			 BDSIntegratorType::g4classicalrk4, // skew sextupole
			 BDSIntegratorType::g4classicalrk4, // skew octupole
			 BDSIntegratorType::g4classicalrk4, // skew decapole
			 BDSIntegratorType::dipolefringe,   // dipole fringe field
			 BDSIntegratorType::multipolethin); // thin multipole
   const BDSIntegratorSet* integratorsBDSIMTwo =
    new BDSIntegratorSet(BDSIntegratorType::solenoid,       // solenoid
			 BDSIntegratorType::dipole2,        // dipole
			 BDSIntegratorType::dipole2,        // dipole3d
			 BDSIntegratorType::quadrupole,     // quadrupole
			 BDSIntegratorType::euler,          // sextupole
			 BDSIntegratorType::euler,          // octupole
			 BDSIntegratorType::euler,          // decapole
			 BDSIntegratorType::g4classicalrk4, // (thick) multipole
			 BDSIntegratorType::g4classicalrk4, // muonspoiler
			 BDSIntegratorType::g4classicalrk4, // rfcavity
			 BDSIntegratorType::g4classicalrk4, // rf
			 BDSIntegratorType::g4classicalrk4, // general
			 BDSIntegratorType::g4classicalrk4, // skew quadrupole
			 BDSIntegratorType::g4classicalrk4, // skew sextupole
			 BDSIntegratorType::g4classicalrk4, // skew octupole
			 BDSIntegratorType::g4classicalrk4, // skew decapole
			 BDSIntegratorType::dipolefringe,   // dipole fringe field
			 BDSIntegratorType::multipolethin); // thin multipole
  /// All 4th Order Runge Kutte.
  const BDSIntegratorSet* integratorsGeant4 =
    new BDSIntegratorSet(BDSIntegratorType::g4classicalrk4, // solenoid
			 BDSIntegratorType::g4classicalrk4, // dipole
			 BDSIntegratorType::g4classicalrk4, // dipole3d
			 BDSIntegratorType::g4classicalrk4, // quadrupole
			 BDSIntegratorType::g4classicalrk4, // sextupole
			 BDSIntegratorType::g4classicalrk4, // octupole
			 BDSIntegratorType::g4classicalrk4, // decapole
			 BDSIntegratorType::g4classicalrk4, // multipole
			 BDSIntegratorType::g4classicalrk4, // muonspoiler
			 BDSIntegratorType::g4classicalrk4, // rfcavity
			 BDSIntegratorType::g4classicalrk4, // rf
			 BDSIntegratorType::g4classicalrk4, // general
			 BDSIntegratorType::g4classicalrk4, // skew quadrupole
			 BDSIntegratorType::g4classicalrk4, // skew sextupole
			 BDSIntegratorType::g4classicalrk4, // skew octupole
			 BDSIntegratorType::g4classicalrk4, // skew decapole
			 BDSIntegratorType::dipolefringe,   // dipole fringe field
			 BDSIntegratorType::multipolethin); // thin multipole
}

const BDSIntegratorSet* BDS::IntegratorSet(G4String sets)
{
  return BDS::IntegratorSet(BDS::DetermineIntegratorSetType(sets));
}

const BDSIntegratorSet* BDS::IntegratorSet(BDSIntegratorSetType set)
{
  switch (set.underlying())
    {
    case BDSIntegratorSetType::geant4:
      {return BDS::integratorsGeant4; break;}
    case BDSIntegratorSetType::bdsimone:
      {return BDS::integratorsBDSIMOne;  break;}
    case BDSIntegratorSetType::bdsimtwo:
      {return BDS::integratorsBDSIMTwo;}
    default:
      {return BDS::integratorsBDSIMOne;  break;}
    }
}

BDSIntegratorType BDSIntegratorSet::Integrator(const BDSFieldType field) const
{
  switch (field.underlying())
    {
    case BDSFieldType::none:
      {return general; break;}// shouldn't really happen, but for completeness.
    case BDSFieldType::bmap1d:
    case BDSFieldType::bmap2d:
    case BDSFieldType::bmap3d:
    case BDSFieldType::bmap4d:
    case BDSFieldType::ebmap1d:
    case BDSFieldType::ebmap2d:
    case BDSFieldType::ebmap3d:
    case BDSFieldType::ebmap4d:
    case BDSFieldType::emap1d:
    case BDSFieldType::emap2d:
    case BDSFieldType::emap3d:
    case BDSFieldType::emap4d:
    case BDSFieldType::mokka:
      {
	G4cout << __METHOD_NAME__ << "Warning - this is overriding the specified field maps integrator" << G4endl;
	return general;
	break;
      }
    case BDSFieldType::solenoid:
      {return solenoid;    break;}
    case BDSFieldType::dipole:
      {return dipole;      break;}
    case BDSFieldType::dipole3d:
      {return dipole3d;    break;}
    case BDSFieldType::quadrupole:
      {return quadrupole;  break;}
    case BDSFieldType::sextupole:
      {return sextupole;   break;}
    case BDSFieldType::octupole:
      {return octupole;    break;}
    case BDSFieldType::decapole:
      {return decapole;    break;}
    case BDSFieldType::multipole:
      {return multipole;   break;}
    case BDSFieldType::muonspoiler:
      {return muonspoiler; break;}
    case BDSFieldType::skewquadrupole:
      {return skewQuadrupole; break;}
    case BDSFieldType::skewsextupole:
      {return skewSextupole;  break;}
    case BDSFieldType::skewoctupole:
      {return skewOctupole;   break;}
    case BDSFieldType::skewdecapole:
      {return skewDecapole;   break;}
    case BDSFieldType::rfcavity:
      {return rfcavity;    break;}
    case BDSFieldType::rf:
      {return rf;          break;}
    default:
      {return general;     break;}
    }
}
