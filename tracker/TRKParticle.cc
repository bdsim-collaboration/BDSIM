#include "TRKParticle.hh"
#include "vector6.hh"

TRKParticle::TRKParticle():beforeindex(0)
{
  posmom[2] = {vector6(),vector6()};
}

TRKParticle::TRKParticle(vector6 coordsIn, double energyIn, double massIn, int chargeIn):beforeindex(0)
{
  posmom[0] = coordsIn;
  posmom[1] = vector6();
  energy    = energyIn;
  mass      = massIn;
  charge    = chargeIn;
}

TRKParticle::TRKParticle(double vIn[]):beforeindex(0)
{
  //x,y,s,xp,yp,sp,energy,mass,charge
  posmom[0] = vector6(vIn[0],vIn[1],vIn[2],vIn[3],vIn[4],vIn[5]);
  posmom[1] = vector6();
  energy    = vIn[6]
  mass      = vIn[7];
  charge    = (int)vIn[8];
}

TRKParticle::~TRKParticle() {}

vector6 GetBefore()
{
  return posmom[beforeindex];
}

vector6 GetAfter()
{
  return posmom[(int)!(bool)beforeindex];
}

void ConfirmNewCoordinates()
{
  beforeindex = (int)!(bool)beforeindex;
}


std::ostream& operator<< (std::ostream &out, const TRKParticle &part)
{
  return out << (const vector6&)part;
}
