#include "TRKApertureCircular.hh"
#include "TRKParticle.hh"
#include "vector3.hh"

TRKApertureCircular::TRKApertureCircular(double radiusIn):radius(radiusIn),radiusSq(radiusIn*radiusIn){;}

TRKApertureCircular::~TRKApertureCircular()
{;}

bool TRKApertureCircular::OutsideAperture(const TRKParticle& particle)
{
  vector3 pos = particle.Pos();
  return radiusSq > (pos.X()*pos.X() + pos.Y()*pos.Y());
}
