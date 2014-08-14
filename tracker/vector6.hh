#ifndef vector6_h
#define vector6_h

#include "vector3.hh"

#include <ostream>

/**
 * @brief 6 dimensional vector representing particle position and momentum
 */
class vector6 { 

 private:
  vector3 position;
  vector3 momentum;

 public: 
  vector6() : position(), momentum() {};
  vector6(vector3 pos, vector3 mom) : position(pos), momentum(mom) {};
  vector6(double v[]) {
    position = vector3(v[0],v[1],v[2]);
    momentum = vector3(v[3],v[4],v[5]);
  }

  double X()const  {return position.X();}  
  double Y()const  {return position.Y();}
  double Z()const  {return position.Z();}
  double Xp()const {return momentum.X();}
  double Yp()const {return momentum.Y();}
  double Zp()const {return momentum.Z();}

  vector3 pos()const {return position;}
  vector3 mom()const {return momentum;}

  vector6(const vector6 &v) {
    position = v.position;
    momentum = v.momentum;
  }

  vector6 operator+(const vector6 &rhs) {
    vector6 v;
    v.position = position + rhs.position;
    v.momentum = momentum + rhs.momentum; 
    return v;
  }

  vector6 operator-(const vector6 &rhs) {
    vector6 v;
    v.position = position - rhs.position;
    v.momentum = momentum - rhs.momentum;
    return v;
  }

  vector6 operator*(double d) { 
    vector6 v;
    v.position = position*d;
    v.momentum = momentum*d;
    return v;
  }
  
  vector6 operator/(double d) { 
    vector6 v;
    v.position = position*d;
    v.momentum = momentum*d;
    return v;
  }

  void setArray(double v[]) {
    v[0] = position.X();
    v[1] = position.Y();
    v[2] = position.Z();
    v[3] = momentum.X();
    v[4] = momentum.Y();
    v[5] = momentum.Z();
  }

  friend std::ostream& operator<< (std::ostream &out, const vector6 &v) {
    return out << v.position << " " << v.momentum;
  }
  

  // double mag() {
  //   return sqrt(x*x+y*y+z*z);
  // }

  // vector3 unit() {
  //   double m = this->mag();
  //   double xu = x/m;
  //   double yu = y/m;
  //   double zu = z/m;
  //   return vector3(xu,yu,zu);
  // }


};
#endif
