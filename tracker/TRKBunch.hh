#ifndef TRKBunch_h
#define TRKBunch_h

#include <ostream>
#include <vector>
#include "vector3.hh"

class TRKParticle;

typedef std::vector<TRKParticle*>::iterator TRKBunchIter;

/**
 * @brief vector of particles
 */
class TRKBunch { 
public:
  TRKBunch();
  ~TRKBunch();

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKBunch &part);

private:
  std::vector <TRKParticle*> bunch;

public:

  TRKBunchIter begin() {return bunch.begin();}
  TRKBunchIter end() {return bunch.end();}
};

#endif
