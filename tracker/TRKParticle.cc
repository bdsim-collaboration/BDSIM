#include "TRKParticle.hh"
#include "vector6.hh"

// // TRKParticle::TRKParticle():energy(0),mass(0),charge(0),beforeindex(0)
// {
//   posmom[0] = vector6();
//   posmom[1] = vector6();
// }

// TRKParticle::TRKParticle(double energyIn, double massIn, int chargeIn):
//   energy(energyIn),mass(massIn),charge(chargeIn),beforeindex(0)
// {
//   posmom[0] = vector6();
//   posmom[1] = vector6();
// }

TRKParticle::TRKParticle(vector6 coordsIn, double energyIn, double massIn, int chargeIn):
  energy(energyIn),mass(massIn),charge(chargeIn),beforeindex(0)
{
  posmom[0] = coordsIn;
  posmom[1] = coordsIn;
}

TRKParticle::TRKParticle(double vIn[], int chargeIn):charge(chargeIn),beforeindex(0)
{
  //x,y,s,xp,yp,sp,energy,mass,charge
  posmom[0] = vector6(vector3(vIn[0],vIn[1],vIn[2]),vector3(vIn[3],vIn[4],vIn[5]));
  posmom[1] = posmom[0];
  energy    = vIn[6];
  mass      = vIn[7];
}

TRKParticle::~TRKParticle() {}

void TRKParticle::SetPosMom(vector6 posmomIn)
{
  posmom[!beforeindex].SetPosMom(posmomIn);
}

void TRKParticle::SetPosMom(vector3 posIn, vector3 momIn)
{
  posmom[!beforeindex].SetPos(posIn);
  posmom[!beforeindex].SetMom(momIn);
}

std::ostream& operator<< (std::ostream &out, const TRKParticle &part)
{
  out << "E: " << part.E() << " M: " << part.M() << " Chrg: " << part.Charge() << std::endl;
  out << "Before: " << part.PosMom() << " After: " << part.PosMomAfter();
  return out;
}
