#include "TRKParticle.hh"
#include "vector6.hh"

TRKParticle::TRKParticle(vector6 coordsIn, double pIn, double massIn, int chargeIn, int eventIDIn):
  posmom(coordsIn),posmombefore(coordsIn),p(pIn),pbefore(pIn),
  mass(massIn),charge(chargeIn),eventID(eventIDIn)
{}

TRKParticle::TRKParticle(double vIn[], int chargeIn, int eventIDIn):charge(chargeIn),eventID(eventIDIn)
{
  //x,y,s,xp,yp,sp,momentum,mass,charge
  posmom = vector6(vector3(vIn[0],vIn[1],vIn[2]),vector3(vIn[3],vIn[4],vIn[5]));
  posmombefore = posmom;
  p            = vIn[6];
  pbefore      = p;
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
  out << "P: " << part.P() << " M: " << part.M() << " Chrg: " << part.Charge() << std::endl;
  out << "Before: " << part.PosMomBefore() << " After: " << part.PosMom();
  return out;
}
