#ifndef TRKBunch_h
#define TRKBunch_h

#include <ostream>
#include <vector>
#include <utility>
#include <string>

namespace GMAD
{
  class Beam;
}

#include "TRKParticle.hh"

typedef std::vector<TRKParticle>::iterator TRKBunchIter;

/**
 * @brief Vector of particles
 */

class TRKBunch
{ 
public:
  TRKBunch(const GMAD::Beam& beam);
  TRKBunch(const std::vector<TRKParticle>& particleVectorIn);
  ~TRKBunch();
  
  TRKBunchIter begin() {return bunch.begin();}
  TRKBunchIter end()   {return bunch.end();}
  
  int  size()  const {return bunch.size();}
  bool empty() const {return bunch.empty();}

  /// erase method, returns iterator to element
  TRKBunchIter Erase(TRKBunchIter iter) {return bunch.erase(iter);}
  TRKBunchIter Erase(TRKBunchIter start, TRKBunchIter finish) {return bunch.erase(start,finish);}
  
  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKBunch &beam);

private:
  TRKBunch(); /// bunch must be instantiated with a number of particles  

  int population;
  std::vector <TRKParticle> bunch;

  /// populate particles using options and random number generator
  void Populate(const GMAD::Beam& beam);
  
  std::pair<double,int> GetParticleMassAndCharge(std::string particlename);
};

#endif
