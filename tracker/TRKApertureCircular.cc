#include "TRKApertureCircular.hh"
#include "TRKParticle.hh"
#include "vector3.hh"
#include <iostream>

// radiusIn in meters
TRKApertureCircular::TRKApertureCircular(double radiusIn):radius(radiusIn),radiusSq(radiusIn*radiusIn){;}

TRKApertureCircular::~TRKApertureCircular()
{;}

bool TRKApertureCircular::OutsideAperture(const TRKParticle& particle)
{
  vector3 pos = particle.Pos();

#ifdef TRKDEBUG
  std::cout << "particle pos (um)    " << pos << std::endl;
  std::cout << "aperture rad sq (um) " << radiusSq*1e12 << std::endl;
  std::cout << "pos sq          " << (pos.X()*pos.X() + pos.Y()*pos.Y()) << std::endl;
#endif

  // radiusSq in m^2, convert to um^2
  return radiusSq*1e12 < (pos.X()*pos.X() + pos.Y()*pos.Y());
}

std::ostream& TRKApertureCircular::PrintDetails(std::ostream& out) const
{
  out << "Circular Aperture with radius (m): " << radius
      << " and calculated radius squared (m^2): " << radiusSq << " ";
  return out;
}
