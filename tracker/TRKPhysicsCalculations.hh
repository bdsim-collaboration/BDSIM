#ifndef TRKPhysicsCalculations
#define TRKPhysicsCalculations

namespace GMAD
{
  class Beam;
}

class BDSParticleDefinition;

namespace TRK
{
  BDSParticleDefinition* DefineParticle(const GMAD::Beam& beam);
}
#endif
