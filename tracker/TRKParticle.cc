#include "TRKParticle.hh"
#include "vector6.hh"

//this is need puerly for initialising a vector (reserving rather) for quicker push backs -> JS not true, not with proper reserve
//TRKParticle::TRKParticle():posmom(vector6()),posmombefore(vector6()),energy(0),energybefore(0),mass(0),charge(0){};

TRKParticle::TRKParticle(vector6 coordsIn, double energyIn, double massIn, int chargeIn):
  posmom(coordsIn),posmombefore(coordsIn),energy(energyIn),energybefore(energyIn),
  mass(massIn),charge(chargeIn)
{}

TRKParticle::TRKParticle(double vIn[], int chargeIn):charge(chargeIn)
{
  //x,y,s,xp,yp,sp,energy,mass,charge
  posmom = vector6(vector3(vIn[0],vIn[1],vIn[2]),vector3(vIn[3],vIn[4],vIn[5]));
  posmombefore = posmom;
  energy       = vIn[6];
  energybefore = energy;
  mass         = vIn[7];
}

TRKParticle::~TRKParticle() {}

void TRKParticle::SetPosMom(vector6 posmomIn)
{
  posmom = posmomIn;
}

void TRKParticle::SetPosMom(vector3 posIn, vector3 momIn)
{
  posmom.SetPos(posIn);
  posmom.SetMom(momIn);
}

std::ostream& operator<< (std::ostream &out, const TRKParticle &part)
{
  out << "E: " << part.E() << " M: " << part.M() << " Chrg: " << part.Charge() << std::endl;
  out << "Before: " << part.PosMomBefore() << " After: " << part.PosMom();
  return out;
}
