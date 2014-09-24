#ifndef TRKParticle_h
#define TRKParticle_h

#include <ostream>
#include "vector6.hh"

class TRKParticle : public vector6 { 
public: 
  TRKParticle();
  TRKParticle(vector6);
  ~TRKParticle();

  /// output stream
  //  friend std::ostream& operator<< (std::ostream &out, const TRKParticle &part);

private:
  // vector3 pos;
  // vector3 mom;
  // double mass;
  // double charge;   

public:
  // double X0(){return pos.X();
  // double Y0(){return pos.Y();
  // double Z0(){return pos.Z();
  // double Xp(){return mom.X();
  // double Yp(){return mom.Y();
  // double Zp(){return mom.Z();

  // double x0;
  // double y0;
  // double z0;
  // double xp;
  // double yp;
  // double zp;
};

#endif
