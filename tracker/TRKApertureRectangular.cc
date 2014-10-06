#include "TRKApertureRectangular.hh"

#include <cmath>

#include "TRKParticle.hh"
#include "vector3.hh"

TRKApertureRectangular::TRKApertureRectangular(double xWidthIn, double yWidthIn):
  xWidth(xWidthIn),yWidth(yWidthIn)
{}

TRKApertureRectangular::~TRKApertureRectangular(){}

bool TRKApertureRectangular::OutsideAperture(const TRKParticle& particle)
{
  vector3 pos = particle.Pos();
  return ((std::abs(pos.X()) > xWidth) || (std::abs(pos.Y()) > yWidth));
}

std::ostream& TRKApertureRectangular::PrintDetails(std::ostream& out) const
{
  out << "Rectangular Aperture with x Width (m): " << xWidth
      << "; and y Width (m): " << yWidth;
  return out;
}
