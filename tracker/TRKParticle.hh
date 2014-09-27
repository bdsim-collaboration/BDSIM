#ifndef TRKParticle_h
#define TRKParticle_h

#include <ostream>
#include "vector6.hh"

class TRKParticle : public vector6 { 
public: 
  TRKParticle();
  TRKParticle(vector6 coordsIn, double energyIn, double massIn, int chargeIn);
  TRKParticle(double []);
  ~TRKParticle();
  
  //accessors
  vector6 GetBefore();
  vector6 GetAfter();

  //toggle the beforeindex
  void ConfirmNewCoordinates();

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKParticle &part);

private:
  //position and momentum have two values - before and after
  vector6 posmom [2];
  //energy,mass and charge don't change in the tracker!
  double  energy;
  double  mass;
  double  charge;
  
  //need the index of posmom to look for 
  //flip each tracking step to avoid copying
  //only modify index which getters use
  int beforeindex; 
};

#endif
