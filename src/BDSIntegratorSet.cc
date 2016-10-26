#include "BDSIntegratorSet.hh"
#include "BDSIntegratorSetType.hh"
#include "BDSIntegratorType.hh"

#include "globals.hh" // geant4 types / globals


BDSIntegratorSet::BDSIntegratorSet(BDSIntegratorType solenoidIn,
				   BDSIntegratorType dipoleIn,
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
  const BDSIntegratorSet* integratorsBDSIM =
    new BDSIntegratorSet(BDSIntegratorType::solenoid,       // solenoid
			 BDSIntegratorType::dipole,         // dipole
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
			 BDSIntegratorType::multipole);     // thin multipole
  const BDSIntegratorSet* integratorsGeant4 =
    new BDSIntegratorSet(BDSIntegratorType::g4classicalrk4, // solenoid
			 BDSIntegratorType::g4classicalrk4, // dipole
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
			 BDSIntegratorType::multipole);     // thin multipole
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
    case BDSIntegratorSetType::bdsim:
      {return BDS::integratorsBDSIM;  break;}
    default:
      {return BDS::integratorsBDSIM;  break;}
    }
}

BDSIntegratorType BDS::Integrator(const BDSIntegratorSet* set,
				  const BDSFieldType      field)
{
  switch (field.underlying())
    {
    case BDSFieldType::none:
      {return set->general;     break;}
    case BDSFieldType::zero:
      {return set->general;     break;}
    case BDSFieldType::threed:
      {return set->general;     break;}
    case BDSFieldType::xy:
      {return set->general;     break;}
    case BDSFieldType::mokka:
      {return set->general;     break;}
    case BDSFieldType::solenoid:
      {return set->solenoid;    break;}
    case BDSFieldType::dipole:
      {return set->dipole;      break;}
    case BDSFieldType::quadrupole:
      {return set->quadrupole;  break;}
    case BDSFieldType::sextupole:
      {return set->sextupole;   break;}
    case BDSFieldType::octupole:
      {return set->octupole;    break;}
    case BDSFieldType::decapole:
      {return set->decapole;    break;}
    case BDSFieldType::multipole:
      {return set->multipole;   break;}
    case BDSFieldType::muonspoiler:
      {return set->muonspoiler; break;}
    case BDSFieldType::rfcavity:
      {return set->rfcavity;    break;}
    case BDSFieldType::rf:
      {return set->rf;          break;}
    case BDSFieldType::skewquadrupole:
      {return set->skewQuadrupole; break;}
    case BDSFieldType::skewsextupole:
      {return set->skewSextupole;  break;}
    case BDSFieldType::skewoctupole:
      {return set->skewOctupole;   break;}
    case BDSFieldType::skewdecapole:
      {return set->skewDecapole;   break;}
    default:
      {return set->general;     break;}
    }
}
