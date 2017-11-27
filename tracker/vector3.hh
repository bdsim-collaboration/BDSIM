/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2017.

This file is part of BDSIM.

BDSIM is free software: you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published 
by the Free Software Foundation version 3 of the License.

BDSIM is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with BDSIM.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef vector3_h
#define vector3_h

#include <cmath>
#include <ostream>
#include <iomanip>

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
  double X()const {return x;}  
  double Y()const {return y;}
  double Z()const {return z;} 

  vector3(const vector3 &v) {
    x = v.x;
    y = v.y;
    z = v.z;
  }
  
  vector3 operator+(const vector3 &rhs) {
    vector3 v;
    v.x = x + rhs.x;
    v.y = y + rhs.y; 
    v.z = z + rhs.z;
    return v;
  }

  vector3 &operator+=(const vector3 &rhs) {
    x += rhs.x;
    y += rhs.y; 
    z += rhs.z;
    return *this;
  }

  vector3 operator-(const vector3 &rhs) {
    vector3 v;
    v.x = x - rhs.x;
    v.y = y - rhs.y;
    v.z = z - rhs.z; 
    return v;
  }

  vector3 operator*(double d) { 
    vector3 v;
    v.x = x*d;
    v.y = y*d;
    v.z = z*d;
    return v;
  }
  
  vector3 &operator*=(const double d) {
    x *= d;
    y *= d; 
    z *= d;
    return *this;
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

  vector3 cross(const vector3 &rhs) {
    vector3 v;
    v.x = (y * rhs.z) - (rhs.y * z);
    v.y = (z * rhs.x) - (rhs.z * x);
    v.z = (x * rhs.y) - (rhs.x * y);
    return v;
  }

  friend std::ostream& operator<< (std::ostream &out, const vector3 &v) {
    out << std::scientific << std::setprecision(5);
    return out << "(" << v.x << "," << v.y << "," << v.z << ")";
  }
  
 private: 
    double x;
    double y;
    double z;   
};

#endif
