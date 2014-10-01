#include "TRKApertureCircular.hh"
#include "TRKParticle.hh"
#include "vector3.hh"

#include <vector>
#include <algorithm>
#include <iterator>

TRKApertureCircular::TRKApertureCircular(double radiusIn):radius(radiusIn),radiusSq(radiusIn*radiusIn)
{;}

TRKApertureCircular::~TRKApertureCircular(){}

bool TRKApertureCircular::OutsideAperture(const TRKParticle& particle)
{
  vector3 pos = particle.Pos();
  return radiusSq > (pos.X()*pos.X() + pos.Y()*pos.Y());
}
/*
void TRKApertureCircular::CheckBunch(TRKBunch& bunch)
{
  std::vector<TRKParticle> lp(1000); //lost particles
  std::copy_if(bunch.begin(),bunch.end(),std::back_inserter(lp),OutsideAperture());
}
*/
