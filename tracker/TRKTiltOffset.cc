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
#include "TRKTiltOffset.hh"

TRKTiltOffset::TRKTiltOffset(double x, double y, double phiIn, double thetaIn, double psiIn):offsetX(x),offsetY(y),phi(phiIn),theta(thetaIn),psi(psiIn)
{}

TRKTiltOffset* TRKTiltOffset::Create(double x, double y, double phi, double theta, double psi)
{
  return new TRKTiltOffset(x,y,phi,theta,psi);
}

TRKTiltOffset* TRKTiltOffset::TRKOffset(double x, double y)
{
  return new TRKTiltOffset(x,y,0,0,0);
}

TRKTiltOffset* TRKTiltOffset::TRKTilt(double phi, double theta, double psi)
{
  return new TRKTiltOffset(0,0,phi,theta,psi);
}

void TRKTiltOffset::AddOffset(double x, double y)
{
  offsetX += x;
  offsetY += y;
}

void TRKTiltOffset::AddTilt(double phiIn, double thetaIn, double psiIn)
{
  phi   += phiIn;
  theta += thetaIn;
  psi   += psiIn;
}

void TRKTiltOffset::SetOffset(double x, double y)
{
  offsetX = x;
  offsetY = y;
}

void TRKTiltOffset::SetTilt(double phiIn, double thetaIn, double psiIn)
{
  phi   = phiIn;
  theta = thetaIn;
  psi   = psiIn;
}
