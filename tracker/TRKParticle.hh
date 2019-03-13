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
#ifndef TRKParticle_h
#define TRKParticle_h

#include <ostream>
#include "vector3.hh"
#include "vector6.hh"

/**
 * @brief Particle class has two 6 vectors for before and after stepping
 * pos/mom plus mass, momentum, charge. 3 vector momentum 'mom' is the 
 * differential momentum (x',y',z') and momentum is the total current
 * absolute momentum. Momentum is required for rigidity and therefore
 * tracking. Geant4 requires the kinetic energy of the particle as well
 * as the particle definition - the kinetic energy can be calculate at
 * output time / when information is passed to bdsim.
*/

class TRKParticle
{ 
public:
  //constructors
  //  TRKParticle();
  TRKParticle(vector6 coordsIn, double energyIn, double massIn, int chargeIn, int eventIDIn);
  TRKParticle(double paramsIn[], int chargeIn, int eventIDIn);
  ~TRKParticle();
  
  //accessors
  /// return coordinate in micrometres
  ///@{
  double X()const      {return posmom.X();}  
  double Y()const      {return posmom.Y();}
  ///@}
  /// return coordinate in metres
  double Z()const      {return posmom.Z();}
  /// return momentum coordinate in rad
  ///@{
  double Xp()const     {return posmom.Xp();}
  double Yp()const     {return posmom.Yp();}
  double Zp()const     {return posmom.Zp();}
  ///@}
  /// return kinetic energy in MeV
  double Ek()const      {return sqrt(p*p+mass*mass)-mass;}
  /// return mass in MeV / c^2
  double M()const      {return mass;}
  /// return momentum in MeV / c^2
  double P()const      {return p;}
  /// return elementary charge
  int    Charge()const {return charge;}
  /// return eventID
  int    EventID()const {return eventID;}

  vector6 PosMom()const {return posmom;}
  vector3 Pos()const    {return posmom.Pos();}
  vector3 Mom()const    {return posmom.Mom();}

  vector6 PosMomBefore()const {return posmombefore;}
  vector3 PosBefore()const    {return posmombefore.Pos();}
  vector3 MomBefore()const    {return posmombefore.Mom();}
  double  PBefore()const      {return pbefore;}

  //setting functions
  void    SetPosMom(vector6 posmomIn);
  void    SetPosMom(vector3 posIn, vector3 momIn);
  void    SetP(double pIn){p=pIn;}

  //toggle the beforeindex
  void ConfirmNewCoordinates() {pbefore = p; posmombefore=posmom;}

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKParticle &part);

private:
  /// position in micrometre (transverse) and metre (longitudinal) and momentum in rad
  vector6 posmom;
  vector6 posmombefore;

  //mass and charge don't change in the tracker!
  /// momentum in MeV - can change in tracker
  double p; 
  double pbefore;
  /// mass in MeV / c^2
  double mass;
  /// charge in units of elementary charge
  int    charge;
  /// event id for referencing in both tracker and bdsim output
  int    eventID;
};

#endif
