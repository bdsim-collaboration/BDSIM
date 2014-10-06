#include "TRKApertureCircular.hh"
#include "TRKParticle.hh"
#include "vector3.hh"
#include <iostream>

TRKApertureCircular::TRKApertureCircular(double radiusIn):radius(radiusIn),radiusSq(radiusIn*radiusIn){;}

TRKApertureCircular::~TRKApertureCircular()
{;}

bool TRKApertureCircular::OutsideAperture(const TRKParticle& particle)
{
  vector3 pos = particle.Pos();
#ifdef TRKDEBUG
  std::cout << "particle pos    " << pos << std::endl;
  std::cout << "aperture rad sq " << radiusSq << std::endl;
  std::cout << "pos sq          " << (pos.X()*pos.X() + pos.Y()*pos.Y()) << std::endl;
#endif
  return radiusSq < (pos.X()*pos.X() + pos.Y()*pos.Y());
}

std::ostream& TRKApertureCircular::PrintDetails(std::ostream& out) const
{
  out << "Circular Aperture with radius (m): " << radius 
      << " and calculated radius squared (m^2): " << radiusSq;
  return out;
}
