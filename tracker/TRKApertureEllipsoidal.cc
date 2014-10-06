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

std::ostream& TRKApertureEllipsoidal::PrintDetails(std::ostream& out) const
{
  out << "Ellipsoidal Aperture with x Width (m): " << xWidth
      << "; and y Width (m): " << yWidth
      << "; calculated parameters xSq, ySq, xSq*ySq are (m2,m2,m4) "
      << xWidthSq << " " << yWidthSq << " " << xyWidthSqProd;
  return out;
}
