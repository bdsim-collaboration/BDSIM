#include "BDSHistogram.hh"
#include "BDSDebug.hh"
#include <vector>
#include <cfloat>
#include <iostream>
#include <iomanip>
#include <string>
#include <utility>
#include "globals.hh"

BDSBin::BDSBin(G4double inXmin, G4double inXmax)
{
  total =  0.0;
  xmin  = inXmin;
  xmax  = inXmax;
  xmean = (xmax + xmin)/2.0;
#ifdef BDSDEBUG
  G4cout << "New bin -> xmin: " << std::setw(7) << xmin 
	 << " m, xmax:  "          << std::setw(7) << xmax 
	 << " m, xmean: "          << std::setw(7) << xmean 
	 << " m" << G4endl;
#endif
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

BDSHistogram1D::BDSHistogram1D(G4double xmin, G4double xmax, G4int nbins)
{
  // Generate bins
  // reserve size for speed optimisation
  bins.reserve(nbins+2);
  // 1st bins is underflow bin
  bins.push_back(new BDSBin(DBL_MIN,xmin));

  // caculate binwidth
  binwidth = (xmax - xmin) / (G4double)nbins;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ 
	 << " S min : "      << xmin 
	 << " m, S max : "   << xmax 
	 << " m, nbins : " << nbins 
	 << "Bin width: "  << binwidth 
	 << " m" << G4endl;
#endif
  G4double localmin, localmax;
  localmin = xmin;
  localmax = xmin + binwidth;
  BDSBin* tempbin = NULL;
  for (G4int i = 0; i < nbins; ++i)
    {
      tempbin = new BDSBin(localmin,localmax);
      bins.push_back(tempbin);
      localmin += binwidth;
      localmax += binwidth;
    }
  // last bin is overflow bin
  bins.push_back(new BDSBin(xmax,DBL_MAX));
}

void BDSHistogram1D::Empty()
{
  for (std::vector<BDSBin*>::iterator i = bins.begin(); i != bins.end(); ++i)
    {(*i)->Empty();}
}

std::vector<BDSBin*> BDSHistogram1D::GetBins()const
{
  return bins;
}

std::vector<G4double> BDSHistogram1D::GetBinTotals()const
{
  std::vector<G4double> result;
  // note first and last bins are under and overflow respectively
  for (std::vector<BDSBin*>::const_iterator i = bins.begin()++; i != --bins.end(); ++i)
    {result.push_back((*i)->GetValue());}
  return result;
}

std::vector<std::pair<G4double, G4double> > BDSHistogram1D::GetBinValues()const
{
  std::vector<std::pair<G4double ,G4double> > result;
  // note first and last bins are under and overflow respectively
  for (std::vector<BDSBin*>::const_iterator i = bins.begin()++; i != --bins.end(); ++i)
    {result.push_back( (*i)->GetXMeanAndTotal() );}
  return result;
}

std::pair<G4double,G4double> BDSHistogram1D::GetUnderOverFlowBins()const
{
  std::pair<G4double,G4double> extrabins = std::make_pair(bins.front()->GetValue(),bins.back()->GetValue());
  return extrabins;
}

void BDSHistogram1D::PrintBins()const
{
  G4cout << G4endl;
  for (std::vector<BDSBin*>::const_iterator i = bins.begin(); i != bins.end(); ++i)
    {G4cout << (*i)->GetValue() << G4endl;}
}

void BDSHistogram1D::Fill(G4double x)
{
  //iterate through vector and check if x in bin range
  //if so append it to total
#ifdef BDSDEBUG
  G4cout << "Hitogram1D > Fill with " << x << G4endl;
#endif
  for (std::vector<BDSBin*>::iterator i = bins.begin(); i != bins.end(); ++i)
    {
      if ((*i)->InRange(x))
	{
	  (*(*i))+=1.0; 
	  break;
	}
    }
}

void BDSHistogram1D::Fill(G4double x, G4double weight)
{
  //iterate through vector and check if x in bin range
  //if so append it to total
  for (std::vector<BDSBin*>::iterator i = bins.begin(); i != bins.end(); ++i)
    {
      if ((*i)->InRange(x))
	{
	  (*(*i)) += (1.0*weight);
	  break;
	}
    }
}

BDSHistogram1D::~BDSHistogram1D()
{
  //must clear the bins from the heap
#ifdef BDSDEBUG
  G4cout << "BDSHistorgram1D > emptying bins" << G4endl;
#endif
  for (std::vector<BDSBin*>::iterator i = bins.begin(); i != bins.end(); ++i)
    {delete *i;}
}

std::ostream& operator<< (std::ostream &out, const BDSHistogram1D &hist)
{
  return out << "### FirstBinLeft = " << hist.GetBins()[1]->xmin 
	     << " LastBinLeft = " << (*(hist.GetBins().rbegin()++))->xmin 
	     << " NBins = " << hist.GetBins().size();
}

