#include "BDSBin.hh"

#include "globals.hh"

#include <iostream>
#include <utility>

BDSBin::BDSBin(G4double inXmin, G4double inXmax)
{
  total =  0.0;
  sumWeightsSquared = 0.0;
  xmin  = inXmin;
  xmax  = inXmax;
  xmean = (xmax + xmin)/2.0;
  /* // this creates a lot of output!
#ifdef BDSDEBUG
  G4cout << "New bin -> xmin: " << std::setw(7) << xmin 
	 << " m, xmax:  "          << std::setw(7) << xmax 
	 << " m, xmean: "          << std::setw(7) << xmean 
	 << " m" << G4endl;
#endif
  */
}

BDSBin BDSBin::operator+= (const G4double& weight)
{
  total += weight;
  sumWeightsSquared += weight*weight;
  return *this;
}

bool BDSBin::InRange(G4double x)
{
  if ( x >= xmin && x < xmax)
    {return true;}
  else
    {return false;}
}

std::pair<G4double, G4double> BDSBin::GetXMeanAndTotal()
{
  return std::make_pair(xmean,total);
}

std::ostream& operator<< (std::ostream &out, BDSBin const &bin)
{
  return out << "(" << bin.xmin << " , " << bin.xmax << ") : " << bin.total;
}
