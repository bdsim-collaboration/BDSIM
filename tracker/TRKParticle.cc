#include "TRKParticle.hh"

TRKParticle::TRKParticle()
{}

TRKParticle::TRKParticle(vector6 v6):vector6(v6)
{}

TRKParticle::~TRKParticle() {}


std::ostream& operator<< (std::ostream &out, const TRKParticle &part)
{
  return out << (const vector6&)part;
}
