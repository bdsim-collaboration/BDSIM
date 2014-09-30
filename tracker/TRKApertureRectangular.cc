#include "TRKApertureRectangular.hh"
#include "TRKParticle.hh"
#include "vector3.hh"

TRKApertureRectangular::TRKApertureRectangular(double xWidthIn, double yWidthIn):
  xWidth(xWidthIn),yWidth(yWidthIn)
{}

TRKApertureRectangular::~TRKApertureRectangular(){}

bool TRKApertureRectangular::OutsideAperture(const TRKParticle& particle)
{
  vector3 pos = particle.PosAfter();
  return ((abs(pos.X()) > xWidth) || (abs(pos.Y()) > yWidth));
}
