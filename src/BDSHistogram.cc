#include "BDSBin.hh"
#include "BDSDebug.hh"
#include "BDSHistogram.hh"

#include <vector>
#include <cfloat>
#include <iostream>
#include <string>
#include <utility>
#include "globals.hh"

BDSHistogram1D::BDSHistogram1D(G4double xminIn, G4double xmaxIn, G4int nbinsIn, G4String nameIn, G4String titleIn, G4String xlabelIn, G4String ylabelIn):
  name(nameIn),title(titleIn),xlabel(xlabelIn),ylabel(ylabelIn),entries(0),xmin(xminIn),xmax(xmaxIn),nbins(nbinsIn)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "name: " << nameIn << ", title: " << titleIn << G4endl;
  G4cout << __METHOD_NAME__ << "xmin: " << xmin << ", xmax: " << xmax << ", nbins: " << nbins << G4endl;
#endif
  // test to see we have at least 1 bin
  if (nbins < 1)
    {
      G4cerr << __METHOD_NAME__ << "must have at least 1 bin - nbins: " << nbins << G4endl;
      exit(1);
    }
  if (xmax <= xmin)
    {
      G4cerr << __METHOD_NAME__ << "xmax must be greater than xmin: xmax = "
	     << xmax << ", xmin = " << xmin << G4endl;
      exit(1);
    }
  
  //underflow bin
  underflow = new BDSBin(DBL_MIN,xmin);
  // this constructor generates equidistant bins
  equidistantBins = true;
  // reserve size for speed optimisation
  bins.reserve(nbins);

  // calculate binwidth
  binwidth = (xmax - xmin) / (G4double)nbins;
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ 
	 << " S min : "    << xmin 
	 << " m, S max : " << xmax 
	 << " m, nbins : " << nbins 
	 << " Bin width: " << binwidth 
	 << " m" << G4endl;
#endif
  G4double localmin, localmax;
  localmin = xmin;
  localmax = xmin + binwidth;
  BDSBin* tempbin = nullptr;
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

BDSHistogram1D::BDSHistogram1D(std::vector<double> binEdges, G4String nameIn, G4String titleIn, G4String xlabelIn, G4String ylabelIn):
  name(nameIn),title(titleIn),xlabel(xlabelIn),ylabel(ylabelIn),entries(0)
{
#ifdef BDSDEBUG
  G4cout << __METHOD_NAME__ << "name: " << nameIn << ", title: " << titleIn << G4endl;
  G4cout << __METHOD_NAME__ << "xmin: " << binEdges.front() << ", xmax: " << binEdges.back() << ", nbins: " << binEdges.size() << G4endl;
#endif
  nbins = binEdges.size()-1; // -1 (for extra edge)
  // reserve size for speed optimisation
  bins.reserve(nbins);
  
  // prepare iterators
  std::vector<double>::iterator iter, end;
  iter = binEdges.begin();
  end = binEdges.end();

  // this constructor generates equidistant bins
  equidistantBins = false;
 
  //underflow bin
  xmin = *iter;
  underflow = new BDSBin(DBL_MIN,xmin);

  BDSBin* tempbin    = nullptr;
  if (binEdges.size() >= 2)
    {
      G4double binstart = binEdges.front();
      for (iter = binEdges.begin(); iter != (end-1); ++iter)
	{
	  G4double binfinish = *(iter+1);
	  if ((binfinish - binstart) > 1e-6)
	    { //only add a bin if it's a finite width
	      tempbin = new BDSBin(binstart,binfinish);
	      bins.push_back(tempbin);
	      binstart = binfinish;
	    }
	  // else do not update binstart to prevent empty space in histogram
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
  xmax = binEdges.back();
  overflow = new BDSBin(xmax,DBL_MAX);

  // set number of bins
  nbins = bins.size();
  // calculate average binwidth
  binwidth = (xmax - xmin) / (G4double)nbins;
  
  //initialise iterators
  first();
}

G4String BDSHistogram1D::GetName() const
{ return name;}

G4String BDSHistogram1D::GetTitle() const
{ return title;}

G4String BDSHistogram1D::GetXLabel() const
{ return xlabel;}

G4String BDSHistogram1D::GetYLabel() const
{ return ylabel;}

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
{return nbins;}

G4int BDSHistogram1D::GetNEntries() const
{return entries;}

G4int BDSHistogram1D::GetBinNumber(G4double x) const
{
  G4int binNumber = nbins; // overflow bin
  if (equidistantBins)
    {
      // floor since vector position starts at 0
      binNumber = std::floor( (x - xmin) / binwidth );
      // set maxima
      if (binNumber < 0) binNumber = -1;
      if (binNumber > nbins) binNumber = nbins;
    }
  else
    {
      // TODO: binary search
      //iterate through vector and check if x in bin range
      if (underflow->InRange(x))
	{binNumber = -1;}
      else if (overflow->InRange(x))
	{binNumber = nbins;}
      else
	{
	  for (int i = 0; i<nbins; ++i)
	    {
	      if (bins[i]->InRange(x))
		{binNumber = i; break;}
	    }
	}
    }
  return binNumber;
}

BDSBin* BDSHistogram1D::GetBin(G4int binNumber) const
{
  if (binNumber < 0) return underflow;
  else if (binNumber >= nbins) return overflow;
  return bins[binNumber];
}

BDSBin* BDSHistogram1D::GetBin(G4double x) const
{
  G4int binNr = GetBinNumber(x);
  return GetBin(binNr);
}
  
void BDSHistogram1D::Fill(G4double x, G4double weight)
{
  BDSBin* bin = GetBin(x);
  *bin += weight;

  entries++;
}

void BDSHistogram1D::Fill(std::pair<G4double,G4double> range, G4double weight)
{
  G4double lower = range.first;
  G4double upper = range.second;

  // swap if in reverse order
  if (upper < lower) {
    std::swap(lower,upper);
  }

  G4int binNrLower = GetBinNumber(lower);
  BDSBin* binLower = GetBin(binNrLower);
  // check if upper end in same bin and if so fill
  // this is done instead of directly looking up upperbin and comparing bins
  // since presumably faster than bin lookup so only a little wasteful if not true and faster if true
  if (binLower->InRange(upper))
    {
      *binLower += weight;
    }
  else
    {
      G4int binNrUpper = GetBinNumber(upper);
      BDSBin* binUpper = GetBin(binNrUpper);
      // fill each bin with the weight of bin-distance / distance
      G4double distance = upper - lower;
      // fill lower and upper bin first
      G4double binLength = (binLower->GetUpperEdge() - lower);
      *binLower += weight * binLength / distance;

      binLength = (upper - binUpper->GetLowerEdge());
      *binUpper += weight * binLength / distance;

      // rest of bins: always in bins vector and fill over full width
      for (G4int i = binNrLower+1; i<binNrUpper; i++) {
	*bins[i] += weight * bins[i]->GetLength() / distance;
      }
    }
  
  entries++;
}

BDSHistogram1D::~BDSHistogram1D()
{
  //must clear the bins from the heap
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
