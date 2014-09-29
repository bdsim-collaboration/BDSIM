#ifndef TRKBunch_h
#define TRKBunch_h

#include <ostream>
#include <vector>
#include <utility>
#include <string>
#include "parser/options.h"
#include "TRKParticle.hh"

typedef std::vector<TRKParticle>::iterator TRKBunchIter;

/**
 * @brief vector of particles
 */
class TRKBunch { 
public:
  TRKBunch(struct Options& opt);
  ~TRKBunch();
  
  TRKBunchIter begin() {return bunch.begin();}
  TRKBunchIter end()   {return bunch.end();}

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKBunch &beam);

private:
  TRKBunch(); /// bunch must be instantiated with a number of particles  
  int population;
  std::vector <TRKParticle> bunch;
  
  void Populate(struct Options& opt); /// populate particles using options and random number generator
  std::pair<double,int> GetParticleMassAndCharge(std::string particlename);
};

#endif
