#ifndef TRKParticle_h
#define TRKParticle_h

#include <ostream>
#include "vector3.hh"
#include "vector6.hh"

/**
 * @brief particle class has two 6 vectors for before and after stepping
 pos/mom plus mass, energy, charge.
*/

class TRKParticle { 
public:
  //constructors
  //  TRKParticle();
  TRKParticle(vector6 coordsIn, double energyIn, double massIn, int chargeIn);
  TRKParticle(double paramsIn[], int chargeIn);
  ~TRKParticle();
  
  //accessors
  /// return coordinate in micrometre
  ///@{
  double X()const      {return posmom.X();}  
  double Y()const      {return posmom.Y();}
  ///@}
  /// return coordinate in metre
  double Z()const      {return posmom.Z();}
  /// return momentum coordinate in rad
  ///@{
  double Xp()const     {return posmom.Xp();}
  double Yp()const     {return posmom.Yp();}
  double Zp()const     {return posmom.Zp();}
  ///@}
  /// return energy in MeV
  double E()const      {return energy;}
  /// return mass in MeV / c^2
  double M()const      {return mass;}
  /// return elementary charge
  int    Charge()const {return charge;}

  vector6 PosMom()const {return posmom;}
  vector3 Pos()const    {return posmom.Pos();}
  vector3 Mom()const    {return posmom.Mom();}

  vector6 PosMomBefore()const {return posmombefore;}
  vector3 PosBefore()const    {return posmombefore.Pos();}
  vector3 MomBefore()const    {return posmombefore.Mom();}
  double  EBefore()const      {return energybefore;}

  //setting functions
  void    SetPosMom(vector6 posmomIn);
  void    SetPosMom(vector3 posIn, vector3 momIn);
  void    SetEnergy(double energyIn){energy=energyIn;}

  //toggle the beforeindex
  void ConfirmNewCoordinates() {energybefore = energy; posmombefore=posmom;}

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKParticle &part);

private:
  /// position in micrometre (transverse) and metre (longitudinal) and momentum in rad
  vector6 posmom;
  vector6 posmombefore;

  //mass and charge don't change in the tracker!
  /// energy in MeV - can change in tracker
  double  energy; // JS: might be faster to store and use the inverse energy
  double  energybefore;
  /// mass in MeV / c^2
  double  mass;
  /// charge in units of elementary charge
  int     charge;
};

#endif
