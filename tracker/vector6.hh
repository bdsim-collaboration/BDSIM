/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2019.

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
  vector6(const double v[]) {
    position = vector3(v[0],v[1],v[2]);
    momentum = vector3(v[3],v[4],v[5]);
  }

  double X()const  {return position.X();}  
  double Y()const  {return position.Y();}
  double Z()const  {return position.Z();}
  double Xp()const {return momentum.X();}
  double Yp()const {return momentum.Y();}
  double Zp()const {return momentum.Z();}

  vector3 Pos()const {return position;}
  vector3 Mom()const {return momentum;}

  void    SetPosMom(vector6 posmomIn) {position = posmomIn.Pos(); momentum=posmomIn.Mom();}
  //should SetPosMom really be the '=' operator?
  void    SetPos(vector3 posIn) { position = posIn;}
  void    SetMom(vector3 momIn) { momentum = momIn;}

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

  vector6 &operator+=(const vector6 &rhs) {
    position += rhs.position;
    momentum += rhs.momentum; 
    return *this;
  }

  void PlusMom(const vector3 &rhs) {
    momentum += rhs; 
  }

  void PlusPos(const vector3 &rhs) {
    position += rhs; 
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
  
  vector6 &operator*=(double d) {
    position *= d;
    momentum *= d; 
    return *this;
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

};
#endif
