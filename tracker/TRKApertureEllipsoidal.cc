#include "TRKAperture.hh"
#include "TRKApertureEllipsoidal.hh"

TRKApertureEllipsoidal::TRKApertureEllipsoidal(double xWidthIn, double yWidthIn):
  xWidth(xWidthIn),yWidth(yWidthIn),xWidthSq(xWidthIn*xWidthIn),yWidthSq(yWidthIn*yWidthIn)
{
  xyWidthSqProd = xWidthSq * yWidthSq;
}

TRKApertureEllipsoidal::~TRKApertureEllipsoidal(){}

bool TRKApertureEllipsoidal::OutsideAperture(const TRKParticle& particle)
{
  vector3 pos = particle.Pos();
  return (yWidthSq*pos.X()*pos.X() + xWidthSq*pos.Y()*pos.Y()) > xyWidthSqProd;
}
