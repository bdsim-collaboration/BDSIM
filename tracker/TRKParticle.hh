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
  TRKParticle(vector6 coordsIn, double energyIn, double massIn, int chargeIn);
  TRKParticle(double paramsIn[], int chargeIn);
  ~TRKParticle();
  
  //accessors
  /// return coordinate in meter
  ///@{
  double X()const      {return posmom[beforeindex].X();}  
  double Y()const      {return posmom[beforeindex].Y();}
  double Z()const      {return posmom[beforeindex].Z();}
  ///@}
  /// return momentum coordinate in rad
  ///@{
  double Xp()const     {return posmom[beforeindex].Xp();}
  double Yp()const     {return posmom[beforeindex].Yp();}
  double Zp()const     {return posmom[beforeindex].Zp();}
  ///@}
  /// return energy in MeV
  double E()const      {return energy;}
  /// return mass in MeV / c^2
  double M()const      {return mass;}
  /// return elementary charge
  int    Charge()const {return charge;}

  vector6 PosMom()const {return posmom[beforeindex];}
  vector3 Pos()const    {return posmom[beforeindex].Pos();}
  vector3 Mom()const    {return posmom[beforeindex].Mom();}

  vector6 PosMomAfter()const {return posmom[!beforeindex];}
  vector3 PosAfter()const    {return posmom[!beforeindex].Pos();}
  vector3 MomAfter()const    {return posmom[!beforeindex].Mom();}

  //setting functions
  void    SetPosMom(vector6 posmomIn);
  void    SetPosMom(vector3 posIn,vector3 momIn);
  void    SetEnergy(double energyIn){energy=energyIn;}

  //toggle the beforeindex
  void ConfirmNewCoordinates() {beforeindex = !beforeindex;}

  /// output stream
  friend std::ostream& operator<< (std::ostream &out, const TRKParticle &part);

private:
  //position and momentum have two values - before and after
  /// position in meter and momentum in rad
  vector6 posmom [2];
  //energy,mass and charge don't change in the tracker!
  /// energy in MeV
  double  energy;
  /// mass in MeV / c^2
  double  mass;
  /// charge in units of elementary charge
  int     charge;
  
  //need a 0 or a 1 for addresing
  //implicit conversion from bool to int useful
  //before index is the index of the "before state" of the vector array
  //using this toggle avoids copying each time the vector is updated as you
  //simply flip the bool to point to the other
  bool beforeindex;
};

#endif
