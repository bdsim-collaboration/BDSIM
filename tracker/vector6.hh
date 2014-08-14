#ifndef vector6_h
#define vector6_h

#include "vector3.hh"

/**
 * @brief 6 dimensional vector representing particle position and momentum
 */
class vector6 { 

 private:
  vector3 position;
  vector3 momentum;

 public: 
  vector6() : position(), momentum() {};
  vector6(vector3 pos, vector3 mom) : position(pos), momentum(mom) {     
  };

  double& X()  {return position.X();}  
  double& Y()  {return position.Y();}
  double& Z()  {return position.Z();}
  double& Xp() {return momentum.X();}
  double& Yp() {return momentum.Y();}
  double& Zp() {return momentum.Z();}

  vector3& pos() {return position;}
  vector3& mom() {return momentum;}

};
#endif
