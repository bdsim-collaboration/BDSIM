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

////////////////////////////////////////////////////////////////////////////////////////////

BDSHistogram1D::BDSHistogram1D(G4double xmin, G4double xmax, G4int nbins, G4String nameIn, G4String titleIn):
  name(nameIn),title(titleIn)
{
  //underflow bin
  underflow = new BDSBin(DBL_MIN,xmin);
  
  // reserve size for speed optimisation
  bins.reserve(nbins);

  // caculate binwidth
  binwidth = (xmax - xmin) / (G4double)nbins;
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
  name(nameIn),title(titleIn)
{
  // reserve size for speed optimisation
  bins.reserve(binEdges.size()-1); // -1 (for extra edge)
  
  // prepare iterators
  std::vector<double>::iterator iter, end;
  iter = binEdges.begin();
  end = binEdges.end();
  
  //underflow bin
  underflow = new BDSBin(DBL_MIN,*iter);
  
  BDSBin* tempbin = NULL;
  for (iter = binEdges.begin(); iter != end--; ++iter)
    {
      tempbin = new BDSBin(*iter,*(iter+1));
      bins.push_back(tempbin);
    }
  // overflow bin
  overflow = new BDSBin(binEdges.back(),DBL_MAX);

  //initialise iterators
  first();
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

std::vector<G4double> BDSHistogram1D::GetBinValues()const
{
  std::vector<G4double> result;
  for (std::vector<BDSBin*>::const_iterator i = bins.begin(); i != bins.end(); ++i)
    {result.push_back((*i)->GetValue());}
  return result;
}

std::vector<std::pair<G4double, G4double> > BDSHistogram1D::GetBinXMeansAndTotals()const
{
  std::vector<std::pair<G4double ,G4double> > result;
  // note first and last bins are under and overflow respectively
  for (std::vector<BDSBin*>::const_iterator i = bins.begin(); i != bins.end(); ++i)
    {result.push_back( (*i)->GetXMeanAndTotal() );}
  return result;
}

std::pair<G4double,G4double> BDSHistogram1D::GetUnderOverFlowBinValues()const
{
  return std::make_pair(bins.front()->GetValue(),bins.back()->GetValue());
}

std::pair<BDSBin*, BDSBin*> BDSHistogram1D::GetUnderOverFlowBins() const
{
  return std::make_pair(underflow,overflow);
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

void BDSHistogram1D::Fill(G4double x)
{
  Fill(x,1.0); // fill with weigth = 1
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
  return out << "### FirstBinLeft = " << hist.GetBins()[1]->xmin 
	     << " LastBinLeft = " << (*(hist.GetBins().rbegin()++))->xmin 
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
  return ((_iterBins != bins.end()) && (std::next(_iterBins) == bins.end()));
}

G4bool BDSHistogram1D::isDone()
{
  return (_iterBins == bins.end());
}

void BDSHistogram1D::next()
{
  _iterBins++;
}
