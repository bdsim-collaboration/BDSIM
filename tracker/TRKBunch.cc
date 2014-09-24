#include "TRKBunch.hh"

#include "TRKParticle.hh"

TRKBunch::TRKBunch()
{}

TRKBunch::~TRKBunch() {}


std::ostream& operator<< (std::ostream &out, const TRKBunch &beam)
{
  std::vector<TRKParticle*>::const_iterator iter = beam.bunch.begin();
  std::vector<TRKParticle*>::const_iterator end = beam.bunch.end();
  
  for (;iter!=end;++iter) {
    out << (**iter) << std::endl;
  }
  return out;
}
