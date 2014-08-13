#ifndef vector3_h
#define vector3_h

#include <cmath>

class vector3 { 
 public: 
  vector3() : x(0.0), y(0.0), z(0.0) {     
  }; 

  vector3(double xIn, double yIn, double zIn) { 
    x = xIn;
    y = yIn;
    z = zIn; 
  }

  ~vector3() {};
  double& X() {return x;}  
  double& Y() {return y;}
  double& Z() {return z;} 

  vector3(const vector3 &v) {
    x = v.x;
    y = v.y;
    z = v.z;
  }

  vector3& operator=(vector3 rhs) {
    x = rhs.x;
    y = rhs.y;
    z = rhs.z;
    return *this;
  }
  
  vector3 operator+(vector3 rhs) {
    vector3 v;
    v.x = x + rhs.x;
    v.y = y + rhs.y; 
    v.z = z + rhs.z;
    return *this;
  }

  vector3 operator-(vector3 rhs) {
    vector3 v;
    v.x = x - rhs.x;
    v.y = y - rhs.y;
    v.z = z - rhs.z; 
    return *this;
  }

  vector3 operator*(double d) { 
    vector3 v;
    v.x = x*d;
    v.y = y*d;
    v.z = z*d;
    return v;
  }
  
  vector3 operator/(double d) { 
    vector3 v;
    v.x = x/d;
    v.y = y/d;
    v.z = z/d;
    return v;
  }

  void setArray(double v[]) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
  }

  double mag() {
    return sqrt(x*x+y*y+z*z);
  }

  vector3 unit() {
    double m = this->mag();
    double xu = x/m;
    double yu = y/m;
    double zu = z/m;
    return vector3(xu,yu,zu);
  }

  
 private: 
    double x;
    double y;
    double z;   
};

#endif
