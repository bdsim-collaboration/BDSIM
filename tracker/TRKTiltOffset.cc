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
