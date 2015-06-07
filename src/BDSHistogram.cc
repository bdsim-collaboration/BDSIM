#include "BDSHistogram.hh"
#include "BDSDebug.hh"
#include <vector>
#include <cfloat>
#include <iostream>
#include <string>
#include <utility>
#include "globals.hh"

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

BDSHistogram1D::BDSHistogram1D(G4double xmin, G4double xmax, G4int nbins, G4String nameIn, G4String titleIn):
  name(nameIn),title(titleIn),entries(0)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "name: " << nameIn << ", title: " << titleIn << G4endl;
  G4cout << __METHOD_NAME__ << "xmin: " << xmin << ", xmax: " << xmax << ", nbins: " << nbins << G4endl;
#endif
  //underflow bin
  underflow = new BDSBin(DBL_MIN,xmin);
  
  // reserve size for speed optimisation
  bins.reserve(nbins);

  // calculate binwidth
  G4double binwidth = (xmax - xmin) / (G4double)nbins;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ 
	 << " S min : "      << xmin 
	 << " m, S max : "   << xmax 
	 << " m, nbins : " << nbins 
	 << " Bin width: "  << binwidth 
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
  // overflow bin
  overflow = new BDSBin(xmax,DBL_MAX);

  //initialise iterators
  first();
}

BDSHistogram1D::BDSHistogram1D(std::vector<double> binEdges, G4String nameIn, G4String titleIn):
  name(nameIn),title(titleIn),entries(0)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "name: " << nameIn << ", title: " << titleIn << G4endl;
  G4cout << __METHOD_NAME__ << "xmin: " << binEdges.front() << ", xmax: " << binEdges.back() << ", nbins: " << binEdges.size() << G4endl;
#endif
  // reserve size for speed optimisation
  bins.reserve(binEdges.size()-1); // -1 (for extra edge)
  
  // prepare iterators
  std::vector<double>::iterator iter, end;
  iter = binEdges.begin();
  end = binEdges.end();
  
  //underflow bin
  underflow = new BDSBin(DBL_MIN,*iter);
  
  BDSBin* tempbin    = NULL;
  G4double binstart  = 0;
  G4double binfinish = 0;
  if (binEdges.size() >= 2)
    {
      for (iter = binEdges.begin(); iter != (end-1); ++iter)
	{
	  binstart  = *iter;
	  binfinish = *(iter+1);
	  if ((binfinish - binstart) > 1e-6)
	    { //only add a bin if it's a finite width
	      tempbin = new BDSBin(*iter,*(iter+1));
	      bins.push_back(tempbin);
	    }
	}
    }
  else if (binEdges.size() == 1)
    {
      // default 1x 1m bin
      tempbin = new BDSBin(binEdges.front(),binEdges.front()+1.0);
      bins.push_back(tempbin);
    }
  // else just underflow and overflow
  // overflow bin
  overflow = new BDSBin(binEdges.back(),DBL_MAX);

  //initialise iterators
  first();
}

G4String BDSHistogram1D::GetName() const
{ return name;}

G4String BDSHistogram1D::GetTitle() const
{ return title;}

void BDSHistogram1D::Empty()
{
  for (std::vector<BDSBin*>::iterator i = bins.begin(); i != bins.end(); ++i)
    {(*i)->Empty();}
}

std::vector<BDSBin*> BDSHistogram1D::GetBins()const
{
  return bins;
}

std::vector<G4double> BDSHistogram1D::GetBinValues()const
{
  std::vector<G4double> result;
  if (bins.size() < 1)
    {return result;}
  for (std::vector<BDSBin*>::const_iterator i = bins.begin(); i != bins.end(); ++i)
    {result.push_back((*i)->GetValue());}
  return result;
}

std::vector<std::pair<G4double, G4double> > BDSHistogram1D::GetBinXMeansAndTotals()const
{
  std::vector<std::pair<G4double ,G4double> > result;
  if (bins.size() < 1)
    {return result;}
  for (std::vector<BDSBin*>::const_iterator i = bins.begin(); i != bins.end(); ++i)
    {result.push_back( (*i)->GetXMeanAndTotal() );}
  return result;
}

std::vector<G4double> BDSHistogram1D::GetBinLowerEdges() const
{
  std::vector<G4double> result;
  if (bins.size() < 1)
    {return result;}
  for (std::vector<BDSBin*>::const_iterator i = bins.begin(); i != bins.end(); ++i)
    {result.push_back( (*i)->GetLowerEdge() );}
  return result;
}

void BDSHistogram1D::PrintBins()const
{
  G4cout << G4endl;
  G4cout << "Name: " << name << G4endl;
  G4cout << "Underflow: " << *underflow << G4endl;
  G4cout << "Overflow:  " << *overflow  << G4endl;
  for (std::vector<BDSBin*>::const_iterator i = bins.begin(); i != bins.end(); ++i)
    {G4cout << **i << G4endl;}
}

std::pair<BDSBin*, BDSBin*> BDSHistogram1D::GetUnderOverFlowBins() const
{
  return std::make_pair(underflow,overflow);
}

std::pair<G4double,G4double> BDSHistogram1D::GetUnderOverFlowBinValues()const
{
  return std::make_pair(bins.front()->GetValue(),bins.back()->GetValue());
}

BDSBin* BDSHistogram1D::GetUnderflowBin() const
{return underflow;}

BDSBin* BDSHistogram1D::GetOverflowBin() const
{return overflow;}

BDSBin* BDSHistogram1D::GetFirstBin() const
{return bins.front();}

BDSBin* BDSHistogram1D::GetLastBin() const
{return bins.back();}

size_t BDSHistogram1D::GetNBins() const
{return bins.size();}

G4int BDSHistogram1D::GetNEntries() const
{return entries;}

void BDSHistogram1D::Fill(G4double x)
{
  Fill(x,1.0); // fill with weigth = 1
  entries++;
}

void BDSHistogram1D::Fill(G4double x, G4double weight)
{
  //iterate through vector and check if x in bin range
  if (underflow->InRange(x))
    {(*underflow)+=1; return;}
  if (overflow->InRange(x))
    {(*overflow)+=1; return;}
  for (std::vector<BDSBin*>::iterator i = bins.begin(); i != bins.end(); ++i)
    {
      if ((*i)->InRange(x))
	{ (*(*i)) += weight; break;}
    }
  entries++;
}

BDSHistogram1D::~BDSHistogram1D()
{
  //must clear the bins from the heap
#ifdef BDSDEBUG
  G4cout << "BDSHistorgram1D > emptying bins" << G4endl;
#endif
  for (std::vector<BDSBin*>::iterator i = bins.begin(); i != bins.end(); ++i)
    {delete *i;}
  delete underflow;
  delete overflow;
}

std::ostream& operator<< (std::ostream &out, BDSHistogram1D const &hist)
{
  return out << "### FirstBinLeft = " << hist.GetFirstBin()->GetLowerEdge() 
	     << " LastBinLeft = " << hist.GetLastBin()->GetLowerEdge()
	     << " NBins = " << hist.GetBins().size();
}

BDSBin* BDSHistogram1D::currentBin()
{
  return *_iterBins;
}

void BDSHistogram1D::first()
{
  _iterBins = bins.begin();
}

G4bool BDSHistogram1D::isLastBin()
{
  // size safe evalutation of whether we're at the last item
  // can be done with std::next but prefer not to use c++11
  return ((_iterBins != bins.end()) && (_iterBins + 1 == bins.end()));
}

G4bool BDSHistogram1D::isDone()
{
  return (_iterBins == bins.end());
}

void BDSHistogram1D::next()
{
  _iterBins++;
}
