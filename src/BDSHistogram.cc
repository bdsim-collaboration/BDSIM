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
  xmax = binEdges.back();
  overflow = new BDSBin(xmax,DBL_MAX);

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
{return bins.size();}

G4int BDSHistogram1D::GetNEntries() const
{return entries;}

void BDSHistogram1D::Fill(G4double x, G4double weight)
{
  if (equidistantBins)
    {
      // floor since vector position starts at 0
      int binNr = std::floor( (x - xmin) / binwidth );
      if (binNr < 0) (*underflow) += weight;
      else if (binNr >= nbins) (*overflow) += weight;
      else {
	(*bins[binNr])+= weight;
      }
    }
  else
    {
      // TODO: binary search
      //iterate through vector and check if x in bin range
      if (underflow->InRange(x))
	{(*underflow)+=1;}
      if (overflow->InRange(x))
	{(*overflow)+=1;}
      for (std::vector<BDSBin*>::iterator i = bins.begin(); i != bins.end(); ++i)
	{
	  if ((*i)->InRange(x))
	    { (*(*i)) += weight; break;}
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
