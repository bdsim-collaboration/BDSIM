/* 
Beam Delivery Simulation (BDSIM) Copyright (C) Royal Holloway, 
University of London 2001 - 2018.

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
